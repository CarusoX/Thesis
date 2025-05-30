#pragma once

#include "Drop.hpp"
#include "LVM.hpp"
#include "MaxMinQueue.hpp"
#include "constants.hpp"
#include "lib.hpp"

class DropFinder
{
public:
    Drop findDrop(const LVM &lvm);

private:
    Drop getDrop(const std::vector<double> &sensor1,
                 const std::vector<double> &sensor2,
                 const std::vector<int> &used);

    void getBestCandidateDrop(const std::vector<double> &sensor1,
                              const std::vector<double> &sensor2,
                              const std::vector<int> &used, bool isPositive,
                              std::pair<int, int> &criticals, double &umbral);

    std::pair<int, int> findStartingPoints(const std::vector<double> &sensor1,
                                           const std::vector<double> &sensor2,
                                           std::pair<int, int> criticals,
                                           bool isPositive);
};
