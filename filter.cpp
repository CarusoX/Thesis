#include "filter.hpp"

std::vector<LVM::Row>
Filter::normalizeWithRolling(const std::vector<LVM::Row> &data)
{
    double sumSensor1 = 0.0;
    double sumSensor2 = 0.0;
    std::vector<LVM::Row> normalizedData;
    size_t halfWindow = WINDOW_SIZE / 2;
    size_t actualWindowSize = halfWindow * 2 + 1;

    size_t currentSize = 0;

    for (size_t i = 0; i < data.size(); ++i)
    {
        // Add the current row's values to the sums
        sumSensor1 += data[i].sensor1;
        sumSensor2 += data[i].sensor2;
        ++currentSize;

        // If we exceed the window size, remove the oldest values
        if (currentSize > actualWindowSize)
        {
            sumSensor1 -= data[i - actualWindowSize].sensor1;
            sumSensor2 -= data[i - actualWindowSize].sensor2;
            --currentSize;
        }

        if (currentSize == actualWindowSize)
        {
            double meanSensor1 = sumSensor1 / actualWindowSize;
            double meanSensor2 = sumSensor2 / actualWindowSize;

            // Create a normalized row for the center of the window
            LVM::Row normalizedRow = data[i - halfWindow];
            normalizedRow.sensor1 -= meanSensor1;
            normalizedRow.sensor2 -= meanSensor2;
            normalizedData.push_back(normalizedRow);
        }
    }

    return normalizedData;
}
