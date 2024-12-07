#ifndef LVM_HPP
#define LVM_HPP

#include <deque>
#include <vector>
#include <stdexcept>
#include <sstream>

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

private:
    std::deque<Row> data;
    size_t maxSize;

public:
    LVM(size_t buffer_size);

    void addSensorData(const std::string &line);

    void keepLast(size_t size);

    void setUsed(size_t r1, size_t r2);

    void setDrop(size_t r1, size_t r2);

    std::vector<Row> get() const;

    size_t size() const;
};

#endif // LVM_HPP
