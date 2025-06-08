#include "MaxMinQueue.hpp"

void MaxMinQueue::push(std::pair<double, int> value)
{
    queue.push_back(value);

    size_t maxEqualCount = 0;
    size_t minEqualCount = 0;
    // Maintain the maxDeque for maximum values
    while (!maxDeque.empty() && maxDeque.back().first.first <= value.first)
    {
        if (maxDeque.back().first.first == value.first)
        {
            maxEqualCount++;
        }
        maxDeque.pop_back();
    }
    maxDeque.push_back({value, maxEqualCount});

    // Maintain the minDeque for minimum values
    while (!minDeque.empty() && minDeque.back().first.first >= value.first)
    {
        if (minDeque.back().first.first == value.first)
        {
            minEqualCount++;
        }
        minDeque.pop_back();
    }
    minDeque.push_back({value, minEqualCount});
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
