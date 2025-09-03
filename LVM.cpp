#include "LVM.hpp"

std::tuple<double, double, double> parse3d(const std::string &line)
{
    const char* ptr = line.data();
    const char* end = ptr + line.size();
    
    // Skip leading whitespace
    while (ptr < end && (*ptr == ' ' || *ptr == '\t')) ptr++;
    
    // Parse first double (time)
    double time = 0.0;
    bool negative = false;
    if (ptr < end && *ptr == '-') {
        negative = true;
        ptr++;
    }
    
    while (ptr < end && *ptr >= '0' && *ptr <= '9') {
        time = time * 10.0 + (*ptr - '0');
        ptr++;
    }
    
    if (ptr < end && *ptr == '.') {
        ptr++;
        double fraction = 0.1;
        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
            time += (*ptr - '0') * fraction;
            fraction *= 0.1;
            ptr++;
        }
    }
    
    if (negative) time = -time;
    
    // Skip whitespace
    while (ptr < end && (*ptr == ' ' || *ptr == '\t')) ptr++;
    
    // Parse second double (sensor1)
    double sensor1 = 0.0;
    negative = false;
    if (ptr < end && *ptr == '-') {
        negative = true;
        ptr++;
    }
    
    while (ptr < end && *ptr >= '0' && *ptr <= '9') {
        sensor1 = sensor1 * 10.0 + (*ptr - '0');
        ptr++;
    }
    
    if (ptr < end && *ptr == '.') {
        ptr++;
        double fraction = 0.1;
        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
            sensor1 += (*ptr - '0') * fraction;
            fraction *= 0.1;
            ptr++;
        }
    }
    
    if (negative) sensor1 = -sensor1;
    
    // Skip whitespace
    while (ptr < end && (*ptr == ' ' || *ptr == '\t')) ptr++;
    
    // Parse third double (sensor2)
    double sensor2 = 0.0;
    negative = false;
    if (ptr < end && *ptr == '-') {
        negative = true;
        ptr++;
    }
    
    while (ptr < end && *ptr >= '0' && *ptr <= '9') {
        sensor2 = sensor2 * 10.0 + (*ptr - '0');
        ptr++;
    }
    
    if (ptr < end && *ptr == '.') {
        ptr++;
        double fraction = 0.1;
        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
            sensor2 += (*ptr - '0') * fraction;
            fraction *= 0.1;
            ptr++;
        }
    }
    
    if (negative) sensor2 = -sensor2;
    
    return {time, sensor1, sensor2};
}


LVM::LVM(size_t buffer_size) : totalUsed(0), maxSize(buffer_size) {}

void LVM::addSensorData(const std::string &line)
{
    std::istringstream iss(line);
    double time, sensor1, sensor2;
    if (!(iss >> time >> sensor1 >> sensor2))
    {
        throw std::invalid_argument("Invalid line format");
    }

    if (data.size() == maxSize)
    {
        if (data.front().used == 1)
        {
            totalUsed--;
        }
        data.pop_front(); // Remove the oldest element
    }
    data.push_back({time, sensor1, sensor2, 0});
}

void LVM::addSensorData(Row &row)
{
    if (data.size() == maxSize)
    {
        if (data.front().used == 1)
        {
            totalUsed--;
        }
        data.pop_front();
    }
    data.push_back(row);
}

void LVM::setUsed(size_t r1, size_t r2)
{
    if (r2 >= data.size())
    {
        throw std::out_of_range("Index out of range");
    }
    for (size_t i = r1; i <= r2; ++i)
    {
        if (data.at(i).used == 0)
        {
            data.at(i).used = 1;
            totalUsed++;
        }
    }
}

std::vector<LVM::Row> LVM::get() const
{
    return std::vector<Row>(data.begin(), data.end());
}

size_t LVM::size() const { return data.size(); }

void LVM::clear() { data.clear(); }

LVM::Row &LVM::operator[](size_t index) { return data.at(index); }

LVM::Row LVM::operator[](size_t index) const { return data.at(index); }

LVM::iterator LVM::begin() { return data.begin(); }
LVM::iterator LVM::end() { return data.end(); }
LVM::const_iterator LVM::begin() const { return data.begin(); }
LVM::const_iterator LVM::end() const { return data.end(); }
