#include "Drop.hpp"

Drop::Drop(int dropSize, int nn) : dropSize(dropSize), nn(nn), threshold(0.02) {} // TODO: 0.02 should be a param

DropData Drop::findDrop(const std::vector<LVM::Row> &data)
{
    // Extract sensor1, sensor2, and used into separate vectors
    std::vector<double> sensor1, sensor2;
    std::vector<int> used;

    for (const auto &row : data)
    {
        sensor1.push_back(row.sensor1);
        sensor2.push_back(row.sensor2);
        used.push_back(row.used);
    }

    std::pair<int, int> positive_criticals = {-1, -1};
    std::pair<int, int> negative_criticals = {-1, -1};
    double umbralp = 0.0, umbraln = 0.0;

    getBestCandidateDrop(sensor1, sensor2, used, true, positive_criticals, umbralp);
    getBestCandidateDrop(sensor1, sensor2, used, false, negative_criticals, umbraln);

    std::pair<int, int> criticals = umbralp != 0 && (umbraln == 0 || std::abs(umbralp) > std::abs(umbraln)) ? positive_criticals : negative_criticals;

    double umbral = (criticals == positive_criticals) ? umbralp : umbraln;

    if (umbral != 0)
    {
        bool isPositive = umbral > 0;
        std::pair<int, int> startingPoints = findStartingPoints(sensor1, sensor2, criticals, isPositive);
        std::vector<double> drop_sensor1, drop_sensor2;
        for (int i = 0; i < this->dropSize; i++)
        {
            drop_sensor1.push_back(sensor1[startingPoints.first + i]);
            drop_sensor2.push_back(sensor2[startingPoints.first + i]);
        }
        int sensor1MiddlePoint = findSensor1MiddlePoint(drop_sensor1, isPositive);
        int sensor2TippingPoint = findSensor2TippingPoint(drop_sensor2, startingPoints.second - startingPoints.first, isPositive);
        return DropData(startingPoints.first, startingPoints.second, criticals.first, criticals.second, sensor1MiddlePoint, sensor2TippingPoint);
    }

    return DropData();
}

void Drop::getBestCandidateDrop(const std::vector<double> &sensor1,
                                const std::vector<double> &sensor2,
                                const std::vector<int> &used,
                                bool isPositive,
                                std::pair<int, int> &criticals,
                                double &umbral)
{
    criticals = {-1, -1};
    umbral = 0.0;

    int maxIndex = sensor1.size() - this->dropSize;
    std::vector<double> sensor1Values(sensor1.size() - 1, 0.0);
    std::vector<double> sensor2Values(sensor1.size() - 1, 0.0);

    auto getValueOfSensor = [&](const std::vector<double> &sensor, int i) -> double
    {
        if (used[i] || used[i + 1])
        {
            return 0;
        }
        return isPositive ? std::min(sensor[i], sensor[i + 1]) : std::max(sensor[i], sensor[i + 1]);
    };

    for (int i = 0; i < sensor1.size() - 1; ++i)
    {
        sensor1Values[i] = getValueOfSensor(sensor1, i);
        sensor2Values[i] = getValueOfSensor(sensor2, i);
    }

    MaxMinQueue maxMinQueue;

    for (int i = sensor1.size() - 2; i >= 0; --i)
    {
        maxMinQueue.push({sensor2Values[i], i});
        if (maxMinQueue.size() > nn)
        {
            maxMinQueue.pop();
        }
        if (i < maxIndex)
        {
            std::pair<double, int> sensor2Value = isPositive ? maxMinQueue.max() : maxMinQueue.min();

            double value = isPositive
                               ? std::min(sensor1Values[i], sensor2Value.first)
                               : std::max(sensor1Values[i], sensor2Value.first);

            if (isPositive && value < 0)
                continue;
            if (!isPositive && value > 0)
                continue;

            if (threshold < std::abs(value) && std::abs(umbral) < std::abs(value))
            {
                umbral = value;
                criticals = {i, sensor2Value.second};
            }
        }
    }
}

std::pair<int, int> Drop::findStartingPoints(const std::vector<double> &sensor1, const std::vector<double> &sensor2, std::pair<int, int> criticals, bool isPositive)
{
    int u1 = criticals.first;
    int u2 = criticals.second;

    auto isNotStartingPoint = [&](double x) -> bool
    {
        return isPositive ? x > 0 : x < 0;
    };

    while (u1 > 0 && isNotStartingPoint(sensor1[u1]) && criticals.first - u1 < nn)
    {
        --u1;
    }

    while (u2 > 0 && isNotStartingPoint(sensor2[u2]) && criticals.second - u2 < nn && u1 < u2)
    {
        u2--;
    }

    return {u1, u2};
}

int Drop::findSensor1MiddlePoint(const std::vector<double> &sensor, bool isPositive)
{
    auto isSensorAnulled = [&](double x) -> bool
    {
        return isPositive ? x < 0 : x > 0;
    };

    for (int i = 0; i < sensor.size() - 1; i++)
    {
        if (isSensorAnulled(sensor[i]) && isSensorAnulled(sensor[i + 1]))
        {
            return i;
        }
    }
    return -1;
}

int Drop::findSensor2TippingPoint(const std::vector<double> &sensor, int u, bool isPositive)
{

    auto compare = [&](double x, double y) -> bool
    {
        return isPositive ? x < y : x > y;
    };

    double integral;
    for (int i = u; i < sensor.size(); i++)
    {
        integral = integral - sensor[i];
        if (compare(integral, isPositive ? -0.1 : 0.1))
        {
            double sum = 0;
            for (int j = i; j < sensor.size() - 1; j++)
            {
                sum += sensor[j];
                if (compare(sensor[j + 1], 0.3 * sum / (j - i + 1)))
                {
                    return j;
                }
            }
        }
    }
    return -1;
}