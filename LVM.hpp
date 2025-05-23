#pragma once

#include "lib.hpp"

class LVM
{
public:
    struct Row
    {
        double time;
        double sensor1;
        double sensor2;
        int used;
    };

    size_t totalUsed;

private:
    std::deque<Row> data;
    size_t maxSize;

public:
    LVM(size_t buffer_size);

    void addSensorData(const std::string &line);

    void setUsed(size_t r1, size_t r2);

    void setDrop(size_t r1, size_t r2);

    std::vector<Row> get() const;

    size_t size() const;

    Row &operator[](size_t index);

    Row operator[](size_t index) const;

    using iterator = std::deque<Row>::iterator;
    using const_iterator = std::deque<Row>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};
