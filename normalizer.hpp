#pragma once

#include "LVM.hpp"
#include "constants.hpp"
#include "lib.hpp"
#include "cli.hpp"

namespace normalizer {

    std::vector<LVM::Row> normalizeWithRolling(const std::vector<LVM::Row> &data, CLI &cli);
}
