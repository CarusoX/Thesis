/**
 * @file Drop.hpp
 * @brief Header file for the Drop class - represents a detected water drop
 * 
 * The Drop class encapsulates all information about a detected water drop,
 * including its electrical signature, physical properties, and analysis results.
 * It provides methods for analyzing drop characteristics and computing various
 * metrics used in drop validation and classification.
 */

#pragma once

#include "constants.hpp"
#include "lib.hpp"
#include "diameter.hpp"

/**
 * @class Drop
 * @brief Represents a detected water drop with all its properties and analysis
 * 
 * This class stores all information about a detected water drop, including:
 * - Raw sensor data and time series
 * - Key analysis points (critical points, starting points, etc.)
 * - Computed physical properties (charge, velocity, diameter)
 * - Validation metrics and penalties
 * - Model fits and statistical measures
 * 
 * The class provides methods for:
 * - Finding key analysis points in the drop signal
 * - Computing physical properties from electrical measurements
 * - Validating drop quality using various criteria
 * - Reading/writing drop data to/from files
 */
class Drop
{
public:
    // === Drop Detection and Analysis Points ===
    bool isPositive; // Whether the drop has positive peaks (true) or negative peaks (false)
    int c1, c2;      // Critical points - the main peaks/valleys in the drop signature
    int u1Original;  // Original starting position of the first sensor (before trimming)
    int u1, u2;      // Starting positions of the drop signal for sensor1 and sensor2
    int p1;          // Middle point of the first sensor signal (ring sensor)
    int p2;          // Tipping point of the second sensor signal (dish sensor)
    
    // === Physical Properties ===
    double q1;       // Electrical charge measured by the ring sensor
    double q2;       // Electrical charge measured by the dish sensor
    double q;        // Average charge (combined measurement)
    double v;        // Drop velocity (m/s)
    double d;        // Drop diameter (mm)
    
    // === Raw Data ===
    std::vector<double> sensor1, sensor2; // Raw sensor data for the drop
    std::vector<double> integralSensor1, integralSensor2; // Integrated sensor data
    std::vector<double> a1, b1; // Model fits for the first pulse (ring sensor)
    std::vector<double> a2;     // Model fit for the second pulse (dish sensor)
    std::vector<double> time;   // Time points for each data sample
    
    // === Quality Assessment Metrics ===
    double sumOfSquaredDiffPenalty1, sumOfSquaredDiffPenalty2; // Penalty for model fit quality
    double chargeDiffPenalty;     // Penalty for charge ratio deviation from expected
    double widthDiffPenalty;      // Penalty for width ratio deviation from expected
    double noisePropPenalty;      // Penalty for noise proportion in the signal
    
    // === Metadata ===
    int id;          // Unique identifier for this drop
    int dataOffset;  // Starting step position in the original data file
    bool valid;      // Whether this drop passed all validation criteria

    // === Constructors ===
    /**
     * @brief Constructor for a detected drop
     * @param isPositive Whether the drop has positive peaks
     * @param c1 Critical point index for sensor1
     * @param c2 Critical point index for sensor2
     */
    Drop(bool isPositive, int c1, int c2);

    /**
     * @brief Default constructor for an empty/invalid drop
     */
    Drop();

    // === Public Analysis Methods ===
    /**
     * @brief Prints debug information about the drop to console
     */
    void debug() const;

    /**
     * @brief Returns the number of data points in the drop
     * @return Size of the drop data
     */
    int size() const;

    /**
     * @brief Finds the middle point of the first sensor signal
     * @return Index of the middle point
     */
    int findSensor1MiddlePoint();

    /**
     * @brief Finds the tipping point of the second sensor signal
     * @return Index of the tipping point
     */
    int findSensor2TippingPoint();

    /**
     * @brief Computes all drop statistics and properties
     * This is the main method that calculates all derived properties
     */
    void computeStats();

    /**
     * @brief Calculates the total penalty score for drop quality assessment
     * @return Combined penalty score (lower is better)
     */
    double penalty() const;

    // === File I/O Methods ===
    /**
     * @brief Reads drops from a file
     * @param file Input file stream
     * @return Vector of Drop objects read from file
     */
    static std::vector<Drop> readFromFile(std::ifstream &file);

    /**
     * @brief Writes drop data to file
     * @param file Output file stream
     * @param withoutIndividualPenalties Whether to exclude individual penalty columns
     */
    void writeToFile(std::ofstream &file, bool withoutIndividualPenalties = false);

private:
    // === Core Computation Methods ===
    /**
     * @brief Computes the integral of sensor data
     */
    void computeIntegral();

    /**
     * @brief Computes the ring sensor charge (q1)
     */
    void computeRingCharge();

    /**
     * @brief Computes the dish sensor charge (q2)
     */
    void computeDishCharge();

    /**
     * @brief Computes the average charge (q)
     */
    void computeAverageCharge();

    /**
     * @brief Applies basic validation filters to the drop
     * @return 1 if valid, 0 if invalid
     */
    int satisfiesBasicFilters();

    /**
     * @brief Computes drop velocity from sensor timing
     */
    void computeVelocity();

    /**
     * @brief Computes drop diameter from velocity using lookup table
     */
    void computeDiameter();

    // === Quality Assessment Methods ===
    /**
     * @brief Computes sum of squared differences penalty for model fit quality
     */
    void computeSumOfSquaredDiffPenalty();

    /**
     * @brief Computes penalty for charge ratio deviation from expected
     */
    void computeChargeDiffPenalty();

    /**
     * @brief Computes penalty for width ratio deviation from expected
     */
    void computeWidthDiffPenalty();

    /**
     * @brief Computes penalty for noise proportion in the signal
     */
    void computeNoisePropPenalty();

    /**
     * @brief Computes theoretical model fits for the drop signals
     */
    void computeModels();
};
