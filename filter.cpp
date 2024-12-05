#include "filter.hpp"

std::vector<LVM::Row> Filter::normalize_with_rolling(const std::vector<LVM::Row>& data, size_t window_size) {
    double sum_sensor1 = 0.0;
    double sum_sensor2 = 0.0;
    std::vector<LVM::Row> normalized_data;

    size_t current_size = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        // Add the current row's values to the sums
        sum_sensor1 += data[i].sensor1;
        sum_sensor2 += data[i].sensor2;
        ++current_size;

        // If we exceed the window size, remove the oldest values
        if (current_size > window_size) {
            sum_sensor1 -= data[i - window_size].sensor1;
            sum_sensor2 -= data[i - window_size].sensor2;
            --current_size;
        }

        // Compute the rolling mean in O(1)
        double mean_sensor1 = sum_sensor1 / current_size;
        double mean_sensor2 = sum_sensor2 / current_size;

        // Create a normalized row
        LVM::Row normalized_row = data[i];
        normalized_row.sensor1 -= mean_sensor1;
        normalized_row.sensor2 -= mean_sensor2;

        // Add the normalized row to the output
        normalized_data.push_back(normalized_row);
    }

    return normalized_data;
}
