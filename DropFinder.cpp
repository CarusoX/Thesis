/**
 * @file DropFinder.cpp
 * @brief Implementation of the DropFinder class for drop detection algorithms
 * 
 * This file contains the implementation of the core drop detection algorithm
 * that analyzes sensor data to identify individual water drops and extract
 * their properties.
 */

#include "DropFinder.hpp"
#include "constants.hpp"

/**
 * @brief Main drop detection method that processes sensor data and returns a Drop object
 * 
 * This method implements the complete drop detection pipeline:
 * 1. Extracts sensor data from LVM buffer
 * 2. Identifies the best drop candidate using getDrop()
 * 3. Finds the starting points of the drop signature
 * 4. Extracts the drop data and analyzes key points
 * 5. Adjusts drop boundaries based on signal characteristics
 * 6. Computes all drop statistics and properties
 * 
 * @param lvm Reference to LVM buffer containing sensor data
 * @return Drop object with complete analysis results
 */
Drop DropFinder::findDrop(const LVM &lvm)
{
    // Extract time, sensor1, sensor2, and used status into separate vectors
    std::vector<double> time, sensor1, sensor2;
    std::vector<int> used;

    for (const auto &row : lvm)
    {
        time.push_back(row.time);
        sensor1.push_back(row.sensor1);
        sensor2.push_back(row.sensor2);
        used.push_back(row.used);
    }

    // Find the best drop candidate in the data
    Drop drop = getDrop(sensor1, sensor2, used);

    // Return early if no valid drop was found
    if (!drop.valid)
    {
        return drop;
    }

    // Find the actual starting points of the drop signature
    std::tie(drop.u1, drop.u2) = findStartingPoints(
        sensor1, sensor2, {drop.c1, drop.c2}, drop.isPositive);

    // Extract the maximum drop size (4*NN) worth of data
    for (int i = 0; i < DROP_SIZE; i++)
    {
        drop.time.push_back(time[drop.u1 + i]);
        drop.sensor1.push_back(sensor1[drop.u1 + i]);
        drop.sensor2.push_back(sensor2[drop.u1 + i]);
    }

    // Find key analysis points in the drop
    drop.findSensor1MiddlePoint();  // Find middle point of first sensor signal
    drop.findSensor2TippingPoint(); // Find tipping point of second sensor signal

    // Adjust drop boundaries based on signal characteristics
    size_t drop_size = DROP_SIZE;
    if(drop.p2 + NN / 2 < 3 * NN / 2) {
        // Case where second pulse is very narrow
        // Take minimum of 3*NN/2 after start of first pulse
        drop_size = 3 * NN / 2;
    } else if(drop.p2 + NN / 2 <= 4 * NN) {
        // Case where second pulse is very wide
        // Take p2+NN/2 after start of second pulse
        drop_size = drop.p2 + NN / 2;
    } else {
        // Intermediate case, take maximum of 4*NN
        // after start of first pulse
        drop_size = 4*NN;
    }

    // Resize drop data to the calculated size
    drop.time.resize(std::min(drop_size, size_t(DROP_SIZE)));
    drop.sensor1.resize(std::min(drop_size, size_t(DROP_SIZE)));
    drop.sensor2.resize(std::min(drop_size, size_t(DROP_SIZE)));

    // Adjust positions since we trimmed the drop (u1 is now 0)
    drop.c1 -= drop.u1;
    drop.c2 -= drop.u1;
    drop.u2 -= drop.u1;
    drop.p1 = std::min(drop.p1, drop.size() - 1);
    drop.p2 = std::min(drop.p2, drop.size() - 1);
    drop.u1Original = drop.u1; // Store original position for reference
    drop.u1 = 0; // Reset to 0 since we trimmed from the beginning
    drop.computeStats(); // Calculate all drop statistics

    return drop;
}

/**
 * @brief Identifies the best drop candidate from sensor data
 * 
 * This method analyzes the sensor data to find the strongest drop signature
 * by considering both positive and negative drop patterns. It evaluates
 * candidates from both polarities and selects the one with the strongest
 * signal that meets the detection criteria.
 * 
 * @param sensor1 Vector of sensor1 (ring) data
 * @param sensor2 Vector of sensor2 (dish) data
 * @param used Vector marking which data points are already used
 * @return Drop object representing the best candidate (may be invalid)
 */
Drop DropFinder::getDrop(const std::vector<double> &sensor1,
                         const std::vector<double> &sensor2,
                         const std::vector<int> &used)
{
    // Initialize variables for both positive and negative drop candidates
    std::pair<int, int> positiveCriticals = {-1, -1};
    std::pair<int, int> negativeCriticals = {-1, -1};
    double umbralp = 0.0, umbraln = 0.0; // Threshold values for each polarity

    // Search for best positive drop candidate
    getBestCandidateDrop(sensor1, sensor2, used, true, positiveCriticals,
                         umbralp);
    // Search for best negative drop candidate
    getBestCandidateDrop(sensor1, sensor2, used, false, negativeCriticals,
                         umbraln);

    // Choose the candidate with the stronger signal
    std::pair<int, int> criticals =
        umbralp != 0 && (umbraln == 0 || std::abs(umbralp) > std::abs(umbraln))
            ? positiveCriticals
            : negativeCriticals;

    double umbral = (criticals == positiveCriticals) ? umbralp : umbraln;

    // Return empty drop if no valid candidate found
    if (umbral == 0)
    {
        return Drop();
    }
    
    // Return drop with the best candidate's properties
    return Drop(umbral > 0, criticals.first, criticals.second);
}

/**
 * @brief Finds the best drop candidate for a specific polarity
 * 
 * This method implements the core drop detection algorithm by searching for
 * the strongest signal that meets the drop criteria. It uses a sliding window
 * approach with a MaxMinQueue to efficiently find the best candidate points
 * in the sensor data.
 * 
 * The algorithm:
 * 1. Preprocesses sensor data to find local minima/maxima
 * 2. Uses a sliding window to find pairs of critical points
 * 3. Evaluates each pair against detection criteria
 * 4. Returns the strongest valid candidate
 * 
 * @param sensor1 Vector of sensor1 (ring) data
 * @param sensor2 Vector of sensor2 (dish) data
 * @param used Vector marking which data points are already used
 * @param isPositive Whether to search for positive (true) or negative (false) drops
 * @param criticals Output parameter for critical point indices
 * @param umbral Output parameter for signal threshold value
 */
void DropFinder::getBestCandidateDrop(const std::vector<double> &sensor1,
                                      const std::vector<double> &sensor2,
                                      const std::vector<int> &used,
                                      bool isPositive,
                                      std::pair<int, int> &criticals,
                                      double &umbral)
{
    // Initialize output parameters
    criticals = {-1, -1};
    umbral = 0.0;

    // Calculate the maximum index we can search from
    int maxIndex = sensor1.size() - DROP_SIZE;
    
    // Preprocess sensor data to find local extrema
    std::vector<double> sensor1Values(sensor1.size() - 1, 0.0);
    std::vector<double> sensor2Values(sensor1.size() - 1, 0.0);

    // Lambda function to extract sensor values based on polarity and usage
    auto getValueOfSensor = [&](const std::vector<double> &sensor,
                                int i) -> double
    {
        // Skip if either point is already used
        if (used[i] || used[i + 1])
        {
            return 0;
        }
        // For positive drops, find minimum; for negative drops, find maximum
        return isPositive ? std::min(sensor[i], sensor[i + 1])
                          : std::max(sensor[i], sensor[i + 1]);
    };

    // Preprocess all sensor data
    for (int i = 0; i < int(sensor1.size()) - 1; ++i)
    {
        sensor1Values[i] = getValueOfSensor(sensor1, i);
        sensor2Values[i] = getValueOfSensor(sensor2, i);
    }

    // Use MaxMinQueue for efficient sliding window operations
    MaxMinQueue maxMinQueue;

    // Search backwards through the data
    for (int i = sensor1.size() - 2; i >= 0; --i)
    {
        // Add current sensor2 value to the queue
        maxMinQueue.push({sensor2Values[i], i});
        
        // Maintain queue size limit
        if (maxMinQueue.size() > NN)
        {
            maxMinQueue.pop();
        }
        
        // Check for valid drop candidates
        if (i < maxIndex)
        {
            // Get the best sensor2 value from the current window
            std::pair<double, int> sensor2Value =
                isPositive ? maxMinQueue.max() : maxMinQueue.min();

            // Combine sensor1 and sensor2 values based on polarity
            double value = isPositive
                               ? std::min(sensor1Values[i], sensor2Value.first)
                               : std::max(sensor1Values[i], sensor2Value.first);

            // Skip if value doesn't match expected polarity
            if (isPositive && value < 0)
                continue;
            if (!isPositive && value > 0)
                continue;

            // Check if this is the best candidate so far
            if (MINIMUM_THRESHOLD < std::abs(value) &&
                std::abs(umbral) < std::abs(value))
            {
                umbral = value;
                criticals = {i, sensor2Value.second};
            }
        }
    }
}

/**
 * @brief Finds the starting points of a drop signature
 * 
 * Given critical points that form a drop signature, this method traces
 * backwards through the sensor data to find the actual starting points
 * where the drop signal begins. This is important for defining the
 * complete drop region and ensuring accurate analysis.
 * 
 * The method searches backwards from the critical points until it finds
 * points where the signal crosses the baseline (zero) or reaches the
 * maximum search distance (NN).
 * 
 * @param sensor1 Vector of sensor1 (ring) data
 * @param sensor2 Vector of sensor2 (dish) data
 * @param criticals Pair of critical point indices (c1, c2)
 * @param isPositive Whether this is a positive or negative drop
 * @return Pair of starting point indices (u1, u2)
 */
std::pair<int, int>
DropFinder::findStartingPoints(const std::vector<double> &sensor1,
                               const std::vector<double> &sensor2,
                               std::pair<int, int> criticals, bool isPositive)
{
    int u1 = criticals.first;  // Starting point for sensor1
    int u2 = criticals.second; // Starting point for sensor2

    // Lambda to check if a point is not a starting point (still in signal)
    auto isNotStartingPoint = [&](double x) -> bool
    { return isPositive ? x > 0 : x < 0; };

    // Find starting point for sensor1 by tracing backwards
    while (u1 > 0 && isNotStartingPoint(sensor1[u1]) &&
           criticals.first - u1 < NN)
    {
        --u1;
    }

    // Find starting point for sensor2 by tracing backwards
    // Ensure u2 doesn't go before u1 to maintain proper order
    while (u2 > 0 && isNotStartingPoint(sensor2[u2]) &&
           criticals.second - u2 < NN && u1 < u2)
    {
        u2--;
    }

    return {u1, u2};
}
