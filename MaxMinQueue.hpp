#pragma once

#include "lib.hpp"

class MaxMinQueue
{
private:
    std::deque<std::pair<double, int>> queue;
    std::deque<std::pair<std::pair<double, int>, int>> maxDeque;
    std::deque<std::pair<std::pair<double, int>, int>> minDeque;

public:
    // Add a new value to the queue
    void push(std::pair<double, int> value);

    // Remove the front element from the queue
    std::pair<double, int> pop();

    // Return the maximum value in the queue
    std::pair<double, int> max() const;

    // Return the minimum value in the queue
    std::pair<double, int> min() const;

    // Return the size of the queue
    size_t size() const;

    // Check if the queue is empty
    bool isEmpty() const;
};
