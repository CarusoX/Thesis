#ifndef FILTER_HPP
#define FILTER_HPP

#include "LVM.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>

class Filter
{
public:
    static std::vector<LVM::Row>
    normalizeWithRolling(const std::vector<LVM::Row> &data);
};

#endif // FILTER_HPP
