#pragma once

#include <string>
#include <chrono>
#include "lib.hpp"

namespace utils {
    // Count total lines in a file
    size_t countLines(const std::string& filePath);
} 
