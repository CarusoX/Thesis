#pragma once

#include <string>
#include <chrono>
#include "lib.hpp"

namespace utils {
    // Count total lines in a file
    size_t countLines(const std::string& filePath);

    // Progress tracker class
    class ProgressTracker {
    public:
        ProgressTracker(const std::string& filePath);
        void update(size_t currentItem);
        void printProgress() const;

    private:
        size_t totalItems;
        size_t currentItem;
        size_t lastPercentage;
        std::chrono::steady_clock::time_point startTime;
    };
} 