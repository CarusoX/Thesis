/**
 * @file DropFinder.hpp
 * @brief Header file for the DropFinder class - drop detection algorithm
 * 
 * The DropFinder class implements the core algorithm for detecting water drops
 * in sensor data. It analyzes electrical signals from two sensors (ring and dish)
 * to identify characteristic drop signatures and extract drop properties.
 */

#pragma once

#include "Drop.hpp"
#include "LVM.hpp"
#include "MaxMinQueue.hpp"
#include "constants.hpp"
#include "lib.hpp"

/**
 * @class DropFinder
 * @brief Main class for detecting and analyzing water drops in sensor data
 * 
 * This class implements a sophisticated algorithm to detect individual water drops
 * from continuous sensor data streams. The algorithm works by:
 * 
 * 1. Analyzing sensor data to find critical points (peaks/valleys)
 * 2. Identifying pairs of critical points that form drop signatures
 * 3. Validating drop candidates using various criteria
 * 4. Extracting drop properties (charge, velocity, diameter)
 * 
 * The detection algorithm is designed to handle both positive and negative
 * drop signatures and uses a sliding window approach with efficient data
 * structures for real-time processing.
 */
class DropFinder
{
public:
    /**
     * @brief Main method to find a drop in the given sensor data
     * 
     * This is the primary interface for drop detection. It processes the
     * sensor data in the LVM buffer and returns a Drop object containing
     * the detected drop information and properties.
     * 
     * @param lvm Reference to LVM buffer containing sensor data
     * @return Drop object with detection results and properties
     */
    Drop findDrop(const LVM &lvm);

private:
    /**
     * @brief Identifies the best drop candidate from sensor data
     * 
     * This method analyzes the sensor data to find critical points that
     * could form a drop signature. It considers both positive and negative
     * drop patterns and returns the best candidate based on signal strength
     * and other criteria.
     * 
     * @param sensor1 Vector of sensor1 (ring) data
     * @param sensor2 Vector of sensor2 (dish) data  
     * @param used Vector marking which data points are already used
     * @return Drop object representing the best candidate (may be invalid)
     */
    Drop getDrop(const std::vector<double> &sensor1,
                 const std::vector<double> &sensor2,
                 const std::vector<int> &used);

    /**
     * @brief Finds the best drop candidate for a specific polarity
     * 
     * This method searches for drop candidates of a specific polarity
     * (positive or negative) and evaluates them to find the strongest
     * signal that meets the detection criteria.
     * 
     * @param sensor1 Vector of sensor1 (ring) data
     * @param sensor2 Vector of sensor2 (dish) data
     * @param used Vector marking which data points are already used
     * @param isPositive Whether to search for positive (true) or negative (false) drops
     * @param criticals Output parameter for critical point indices
     * @param umbral Output parameter for signal threshold value
     */
    void getBestCandidateDrop(const std::vector<double> &sensor1,
                              const std::vector<double> &sensor2,
                              const std::vector<int> &used, bool isPositive,
                              std::pair<int, int> &criticals, double &umbral);

    /**
     * @brief Finds the starting points of a drop signature
     * 
     * Given critical points that form a drop signature, this method
     * traces backwards to find the actual starting points of the drop
     * signal, which are used to define the complete drop region.
     * 
     * @param sensor1 Vector of sensor1 (ring) data
     * @param sensor2 Vector of sensor2 (dish) data
     * @param criticals Pair of critical point indices
     * @param isPositive Whether this is a positive or negative drop
     * @return Pair of starting point indices (u1, u2)
     */
    std::pair<int, int> findStartingPoints(const std::vector<double> &sensor1,
                                           const std::vector<double> &sensor2,
                                           std::pair<int, int> criticals,
                                           bool isPositive);
};
