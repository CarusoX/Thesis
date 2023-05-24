#ifndef TYPES_H
#define TYPES_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using ull = unsigned long long;

#define NaN std::numeric_limits<double>::quiet_NaN()

struct LVM_DATUM {
    ull time;
    std::vector<double> var;
};

struct DAT_DATUM {
    std::vector<double> var;
};

#endif