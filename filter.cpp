#include "filter.hpp"
#include "constants.hpp"

std::vector<LVM::Row> Filter::normalizeWithRolling(const std::vector<LVM::Row>& data) {
    double sumSensor1 = 0.0;
    double sumSensor2 = 0.0;
    std::vector<LVM::Row> normalizedData;

    size_t currentSize = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        // Add the current row's values to the sums
        sumSensor1 += data[i].sensor1;
        sumSensor2 += data[i].sensor2;
        ++currentSize;

        // If we exceed the window size, remove the oldest values
        if (currentSize > WINDOW_SIZE) {
            sumSensor1 -= data[i - WINDOW_SIZE].sensor1;
            sumSensor2 -= data[i - WINDOW_SIZE].sensor2;
            --currentSize;
        }

        // Compute the rolling mean in O(1)
        double meanSensor1 = sumSensor1 / currentSize;
        double meanSensor2 = sumSensor2 / currentSize;

        // Create a normalized row
        LVM::Row normalizedRow = data[i];
        normalizedRow.sensor1 -= meanSensor1;
        normalizedRow.sensor2 -= meanSensor2;

        // Add the normalized row to the output
        normalizedData.push_back(normalizedRow);
    }

    return normalizedData;
}
