#pragma once

#include "LVM.hpp"
#include "constants.hpp"
#include "lib.hpp"

class Filter
{
public:
    static std::vector<LVM::Row>
    normalizeWithRolling(const std::vector<LVM::Row> &data);
};
