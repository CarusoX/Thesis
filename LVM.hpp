#ifndef LVM_HPP
#define LVM_HPP

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

    Row *begin();

    Row *end();

    const Row *begin() const;

    const Row *end() const;
};

#endif // LVM_HPP
