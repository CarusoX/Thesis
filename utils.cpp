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

} 