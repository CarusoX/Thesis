#include "MaxMinQueue.hpp"

void MaxMinQueue::push(std::pair<double, int> value)
{
    queue.push_back(value);

    // Maintain the maxDeque for maximum values
    while (!maxDeque.empty() && maxDeque.back().first.first < value.first)
    {
        maxDeque.pop_back();
    }
    if (!maxDeque.empty() && maxDeque.back().first.first == value.first) {
        maxDeque.back().second++;
    } else {
        maxDeque.push_back({value, 1});
    }

    // Maintain the minDeque for minimum values
    while (!minDeque.empty() && minDeque.back().first.first > value.first)
    {
        minDeque.pop_back();
    }
    if (!minDeque.empty() && minDeque.back().first.first == value.first) {
        minDeque.back().second++;
    } else {
        minDeque.push_back({value, 1});
    }
}

std::pair<double, int> MaxMinQueue::pop()
{
    if (queue.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    std::pair<double, int> value = queue.front();
    queue.pop_front();

    // Update maxDeque if the popped value is the current maximum
    if (!maxDeque.empty() && value == maxDeque.front().first)
    {
        if(--maxDeque.front().second == 0) {
            maxDeque.pop_front();
        }
    }

    // Update minDeque if the popped value is the current minimum
    if (!minDeque.empty() && value == minDeque.front().first)
    {
        if(--minDeque.front().second == 0) {
            minDeque.pop_front();
        }
    }

    return value;
}

std::pair<double, int> MaxMinQueue::max() const
{
    if (maxDeque.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    return maxDeque.front().first;
}

std::pair<double, int> MaxMinQueue::min() const
{
    if (minDeque.empty())
    {
        throw std::runtime_error("Queue is empty");
    }
    return minDeque.front().first;
}

size_t MaxMinQueue::size() const { return queue.size(); }

bool MaxMinQueue::isEmpty() const { return queue.empty(); }
