/**
 * @file LVM.hpp
 * @brief Header file for the LVM (Linear Vector Memory) class
 * 
 * The LVM class provides a circular buffer for storing sensor data with
 * efficient memory management and data access. It's designed to handle
 * large datasets while maintaining constant memory usage through a
 * sliding window approach.
 */

#pragma once

#include "lib.hpp"

/**
 * @class LVM
 * @brief Linear Vector Memory - circular buffer for sensor data
 * 
 * This class implements a circular buffer specifically designed for
 * processing sensor data streams. It provides:
 * - Efficient memory management with automatic overflow handling
 * - Tracking of used/unused data points for drop detection
 * - Iterator support for easy data access
 * - Configurable buffer size for different processing stages
 * 
 * The class is optimized for the drop detection algorithm where
 * different processing stages require different buffer sizes and
 * data access patterns.
 */
class LVM
{
public:
    /**
     * @struct Row
     * @brief Represents a single data point from the sensors
     */
    struct Row
    {
        double time;    // Timestamp of the measurement
        double sensor1; // Signal from ring sensor
        double sensor2; // Signal from dish sensor
        int used;       // Flag indicating if this point is already used in drop detection
    };

    size_t totalUsed; // Total count of used data points

private:
    std::deque<Row> data; // Circular buffer storing the sensor data
    size_t maxSize;       // Maximum size of the buffer (0 = unlimited)

public:
    /**
     * @brief Constructor for LVM buffer
     * @param buffer_size Maximum size of the buffer (0 for unlimited)
     */
    LVM(size_t buffer_size);

    /**
     * @brief Adds sensor data from a string line
     * @param line String containing time, sensor1, sensor2 values
     */
    void addSensorData(const std::string &line);

    /**
     * @brief Adds sensor data from a Row object
     * @param row Row object containing the sensor data
     */
    void addSensorData(Row &row);
    
    /**
     * @brief Marks a range of data points as used
     * @param r1 Start index of the range
     * @param r2 End index of the range (inclusive)
     */
    void setUsed(size_t r1, size_t r2);

    /**
     * @brief Returns a copy of all data as a vector
     * @return Vector containing all data points
     */
    std::vector<Row> get() const;

    /**
     * @brief Returns the current size of the buffer
     * @return Number of data points currently stored
     */
    size_t size() const;

    /**
     * @brief Clears all data from the buffer
     */
    void clear();

    /**
     * @brief Access operator for reading/writing data points
     * @param index Index of the data point
     * @return Reference to the Row at the specified index
     */
    Row &operator[](size_t index);

    /**
     * @brief Access operator for reading data points
     * @param index Index of the data point
     * @return Copy of the Row at the specified index
     */
    Row operator[](size_t index) const;

    // Iterator support for range-based loops
    using iterator = std::deque<Row>::iterator;
    using const_iterator = std::deque<Row>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};
