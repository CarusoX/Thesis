/**
 * @file normalizer.cpp
 * @brief Implementation of data normalization functions
 * 
 * This file contains the implementation of functions for normalizing
 * sensor data to remove baseline drift and improve drop detection accuracy.
 */

#include "normalizer.hpp"

namespace normalizer {

/**
 * @brief Normalizes sensor data using a rolling window approach
 * 
 * This function implements a rolling window normalization algorithm that:
 * 1. Maintains a sliding window of WINDOW_SIZE data points
 * 2. Calculates the local average for both sensors within the window
 * 3. Subtracts the local average from the center point of the window
 * 4. Outputs normalized data points as the window slides through the data
 * 
 * The algorithm ensures that only data points with a full window of
 * surrounding data are normalized, maintaining data quality at the edges.
 * 
 * @param data Vector of LVM::Row objects containing the raw sensor data
 * @param cli Reference to CLI for progress reporting
 * @return Vector of normalized LVM::Row objects
 */
std::vector<LVM::Row>
normalizeWithRolling(const std::vector<LVM::Row> &data, CLI &cli)
{
    cli.startProgress("normalize", "Normalizing data", data.size());
    
    // Initialize variables for rolling window calculations
    double sumSensor1 = 0.0;
    double sumSensor2 = 0.0;
    std::vector<LVM::Row> normalizedData;
    size_t halfWindow = WINDOW_SIZE / 2;
    size_t actualWindowSize = halfWindow * 2 + 1;

    size_t currentSize = 0;

    for (size_t i = 0; i < data.size(); ++i)
    {
        // Add the current row's values to the running sums
        sumSensor1 += data[i].sensor1;
        sumSensor2 += data[i].sensor2;
        ++currentSize;

        // If we exceed the window size, remove the oldest values
        if (currentSize > actualWindowSize)
        {
            sumSensor1 -= data[i - actualWindowSize].sensor1;
            sumSensor2 -= data[i - actualWindowSize].sensor2;
            --currentSize;
        }

        // Only output normalized data when we have a full window
        if (currentSize == actualWindowSize)
        {
            // Calculate local averages
            double meanSensor1 = sumSensor1 / actualWindowSize;
            double meanSensor2 = sumSensor2 / actualWindowSize;

            // Create a normalized row for the center of the window
            LVM::Row normalizedRow = data[i - halfWindow];
            normalizedRow.sensor1 -= meanSensor1;
            normalizedRow.sensor2 -= meanSensor2;
            normalizedData.push_back(normalizedRow);
        }
        cli.updateProgress("normalize", i);
    }
    cli.finishProgress("normalize");
    return normalizedData;
}

}
