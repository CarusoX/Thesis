#ifndef DROP_HPP
#define DROP_HPP

#include <vector>
#include <iostream>
#include <cmath>
#include "LVM.hpp"
#include "MaxMinQueue.hpp"
#include "DropData.hpp"

class Drop {
public:

    Drop(int dropSize, int nn);

    DropData findDrop(const std::vector<LVM::Row>& data);

private:
    int dropSize;
    int nn;
    double threshold; // c2

    void getBestCandidateDrop(const std::vector<double>& sensor1,
                               const std::vector<double>& sensor2,
                               const std::vector<int>& used,
                               bool isPositive,
                               std::pair<int, int>& criticals,
                               double& umbral);

    std::pair<int, int> findStartingPoints(const std::vector<double>& sensor1,const std::vector<double>& sensor2, std::pair<int,int> criticals, bool isPositive);

    int findSensor1MiddlePoint(const std::vector<double> &sensor, bool isPositive);

    int findSensor2TippingPoint(const std::vector<double> &sensor, int u, bool isPositive);
};

#endif // DROP_HPP
