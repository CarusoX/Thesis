/**
 * @file MaxMinQueue.hpp
 * @brief Header file for the MaxMinQueue class - efficient min/max queue
 * 
 * The MaxMinQueue class provides an efficient data structure for maintaining
 * a sliding window of values while being able to quickly retrieve the
 * minimum and maximum values in the window. This is essential for the
 * drop detection algorithm's sliding window operations.
 */

#pragma once

#include "lib.hpp"

/**
 * @class MaxMinQueue
 * @brief Efficient queue with O(1) min/max operations
 * 
 * This class implements a deque-based data structure that maintains
 * a sliding window of values and provides constant-time access to
 * both the minimum and maximum values in the window. It uses two
 * auxiliary deques to track the min/max values efficiently.
 * 
 * The data structure is optimized for the drop detection algorithm
 * where we need to find the best (min/max) sensor values within
 * a sliding window of fixed size.
 */
class MaxMinQueue
{
private:
    std::deque<std::pair<double, int>> queue; // Main queue storing (value, index) pairs
    std::deque<std::pair<std::pair<double, int>, int>> maxDeque; // Deque for tracking maximum values
    std::deque<std::pair<std::pair<double, int>, int>> minDeque; // Deque for tracking minimum values

public:
    /**
     * @brief Add a new value to the queue
     * @param value Pair of (value, index) to add
     */
    void push(std::pair<double, int> value);

    /**
     * @brief Remove the front element from the queue
     * @return The removed (value, index) pair
     */
    std::pair<double, int> pop();

    /**
     * @brief Return the maximum value in the queue
     * @return Pair of (max_value, index) of the maximum element
     */
    std::pair<double, int> max() const;

    /**
     * @brief Return the minimum value in the queue
     * @return Pair of (min_value, index) of the minimum element
     */
    std::pair<double, int> min() const;

    /**
     * @brief Return the size of the queue
     * @return Number of elements in the queue
     */
    size_t size() const;

    /**
     * @brief Check if the queue is empty
     * @return True if empty, false otherwise
     */
    bool isEmpty() const;
};
