/**
 * @file MaxMinQueue.cpp
 * @brief Implementation of the MaxMinQueue class
 * 
 * This file contains the implementation of the efficient min/max queue
 * data structure used in the drop detection algorithm for sliding window
 * operations.
 */

#include "MaxMinQueue.hpp"

/**
 * @brief Add a new value to the queue
 * 
 * This method adds a new value to the queue while maintaining the
 * auxiliary deques for efficient min/max tracking. The algorithm
 * ensures that the maxDeque and minDeque always contain the correct
 * maximum and minimum values respectively.
 * 
 * @param value Pair of (value, index) to add to the queue
 */
void MaxMinQueue::push(std::pair<double, int> value)
{
    // Add to main queue
    queue.push_back(value);

    // Maintain the maxDeque for maximum values
    // Remove elements that are smaller than the new value
    while (!maxDeque.empty() && maxDeque.back().first.first < value.first)
    {
        maxDeque.pop_back();
    }
    // Handle duplicate values by incrementing count
    if (!maxDeque.empty() && maxDeque.back().first.first == value.first) {
        maxDeque.back().second++;
        maxDeque.back().first = value;
    } else {
        maxDeque.push_back({value, 1});
    }

    // Maintain the minDeque for minimum values
    // Remove elements that are larger than the new value
    while (!minDeque.empty() && minDeque.back().first.first > value.first)
    {
        minDeque.pop_back();
    }
    // Handle duplicate values by incrementing count
    if (!minDeque.empty() && minDeque.back().first.first == value.first) {
        minDeque.back().second++;
        minDeque.back().first = value;
    } else {
        minDeque.push_back({value, 1});
    }
}

/**
 * @brief Remove the front element from the queue
 * 
 * This method removes the front element from the queue and updates
 * the auxiliary deques accordingly. It handles duplicate values
 * by decrementing their count rather than immediately removing them.
 * 
 * @return The removed (value, index) pair
 * @throws std::runtime_error if the queue is empty
 */
std::pair<double, int> MaxMinQueue::pop()
{
    if (queue.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    
    // Get the value to be removed
    std::pair<double, int> value = queue.front();
    queue.pop_front();

    // Update maxDeque if the popped value is the current maximum
    if (!maxDeque.empty() && value.first == maxDeque.front().first.first)
    {
        // Decrement count for duplicate values
        if(--maxDeque.front().second == 0) {
            maxDeque.pop_front();
        }
    }

    // Update minDeque if the popped value is the current minimum
    if (!minDeque.empty() && value.first == minDeque.front().first.first)
    {
        // Decrement count for duplicate values
        if(--minDeque.front().second == 0) {
            minDeque.pop_front();
        }
    }

    return value;
}

/**
 * @brief Return the maximum value in the queue
 * 
 * @return Pair of (max_value, index) of the maximum element
 * @throws std::runtime_error if the queue is empty
 */
std::pair<double, int> MaxMinQueue::max() const
{
    if (maxDeque.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    return maxDeque.front().first;
}

/**
 * @brief Return the minimum value in the queue
 * 
 * @return Pair of (min_value, index) of the minimum element
 * @throws std::runtime_error if the queue is empty
 */
std::pair<double, int> MaxMinQueue::min() const
{
    if (minDeque.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    return minDeque.front().first;
}

/**
 * @brief Return the size of the queue
 * @return Number of elements in the queue
 */
size_t MaxMinQueue::size() const { return queue.size(); }

/**
 * @brief Check if the queue is empty
 * @return True if empty, false otherwise
 */
bool MaxMinQueue::isEmpty() const { return queue.empty(); }
