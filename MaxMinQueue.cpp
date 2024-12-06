#include "MaxMinQueue.hpp"

void MaxMinQueue::push(std::pair<double, int> value) {
    queue.push_back(value);

    // Maintain the maxDeque for maximum values
    while (!maxDeque.empty() && maxDeque.back().first < value.first) {
        maxDeque.pop_back();
    }
    maxDeque.push_back(value);

    // Maintain the minDeque for minimum values
    while (!minDeque.empty() && minDeque.back().first > value.first) {
        minDeque.pop_back();
    }
    minDeque.push_back(value);
}

std::pair<double, int> MaxMinQueue::pop() {
    if (queue.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    std::pair<double, int> value = queue.front();
    queue.pop_front();

    // Update maxDeque if the popped value is the current maximum
    if (!maxDeque.empty() && value == maxDeque.front()) {
        maxDeque.pop_front();
    }

    // Update minDeque if the popped value is the current minimum
    if (!minDeque.empty() && value == minDeque.front()) {
        minDeque.pop_front();
    }

    return value;
}

std::pair<double, int> MaxMinQueue::max() const {
    if (maxDeque.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    return maxDeque.front();
}

std::pair<double, int> MaxMinQueue::min() const {
    if (minDeque.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    return minDeque.front();
}

size_t MaxMinQueue::size() const {
    return queue.size();
}

bool MaxMinQueue::isEmpty() const {
    return queue.empty();
}
