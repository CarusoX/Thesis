/**
 * @file normalizer.hpp
 * @brief Header file for data normalization utilities
 * 
 * This file provides functions for normalizing sensor data to remove
 * baseline drift and offset variations that can occur in long-term
 * measurements. The normalization is essential for accurate drop detection.
 */

#pragma once

#include "LVM.hpp"
#include "constants.hpp"
#include "lib.hpp"
#include "cli.hpp"

/**
 * @namespace normalizer
 * @brief Namespace containing data normalization functions
 */
namespace normalizer {

    /**
     * @brief Normalizes sensor data using a rolling window approach
     * 
     * This function removes baseline drift from sensor data by calculating
     * local averages using a rolling window and subtracting them from
     * the original data. This helps eliminate long-term drift and offset
     * variations that can interfere with drop detection.
     * 
     * @param data Vector of LVM::Row objects containing the raw sensor data
     * @param cli Reference to CLI for progress reporting
     * @return Vector of normalized LVM::Row objects
     */
    std::vector<LVM::Row> normalizeWithRolling(const std::vector<LVM::Row> &data, CLI &cli);
}
