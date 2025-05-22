#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>

namespace utils {

size_t countLines(const std::string& filePath) {
    std::ifstream file(filePath);
    return std::count(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>(), '\n');
}

ProgressTracker::ProgressTracker(const std::string& filePath)
    : totalItems(countLines(filePath))
    , currentItem(0)
    , lastPercentage(0)
    , startTime(std::chrono::steady_clock::now())
{}

void ProgressTracker::update(size_t currentItem) {
    this->currentItem = currentItem;
    size_t percentage = (currentItem * 100) / totalItems;
    
    if (percentage >= lastPercentage + 5) {
        printProgress();
        lastPercentage = percentage;
    }
}

void ProgressTracker::printProgress() const {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
    auto estimatedTotal = (elapsed * 100) / ((currentItem * 100) / totalItems);
    auto remaining = estimatedTotal - elapsed;

    std::cout << std::fixed << std::setprecision(1)
              << "Progress: " << ((currentItem * 100) / totalItems) << "% "
              << "(" << currentItem << "/" << totalItems << ") "
              << "Elapsed: " << elapsed.count() << "s "
              << "Remaining: " << remaining.count() << "s"
              << std::endl;
}

} 