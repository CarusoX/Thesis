#ifndef FILTER_HPP
#define FILTER_HPP

#include <iostream>
#include <vector>
#include "constants.hpp"
#include "LVM.hpp"

class Filter
{
public:
    static std::vector<LVM::Row> normalizeWithRolling(const std::vector<LVM::Row> &data);
};

#endif // FILTER_HPP
