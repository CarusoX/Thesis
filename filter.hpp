#ifndef FILTER_HPP
#define FILTER_HPP

#include <vector>
#include "LVM.hpp"

class Filter {
public:
    static std::vector<LVM::Row> normalize_with_rolling(const std::vector<LVM::Row>& data, size_t window_size);
};

#endif // FILTER_HPP
