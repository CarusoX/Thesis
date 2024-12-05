#ifndef LVM_HPP
#define LVM_HPP

#include <deque>
#include <vector>
#include <stdexcept>
#include <sstream>

class LVM {
public:
    struct Row {
        double time;
        double sensor1;
        double sensor2;
        int used;
        int isDrop;
    };
private:
    std::deque<Row> data;
    size_t max_size;

public:
    
    explicit LVM(size_t buffer_size) : max_size(buffer_size) {}

      void add_sensor_data(const std::string& line) {
        std::istringstream iss(line);
        double time, sensor1, sensor2;
        if (!(iss >> time >> sensor1 >> sensor2)) {
            throw std::invalid_argument("Invalid line format");
        }
        if (data.size() == max_size) {
            data.pop_front(); // Remove the oldest element
        }
        data.push_back({time, sensor1, sensor2, 0, 0});
    }

    void keep_last(size_t size) {
        while (data.size() > size) {
            data.pop_front();
        }
    }

    void set_used(size_t r1, size_t r2) {
        if (r2 >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        for (size_t i = r1; i <= r2; ++i) {
            data[i].used = 1;
        }
    }

    void set_drop(size_t r1, size_t r2) {
        if (r2 >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        for (size_t i = r1; i <= r2; ++i) {
            data[i].used = 1;
            data[i].isDrop = 1;
        }
    }

    std::vector<Row> get() const {
        return std::vector<Row>(data.begin(), data.end());
    }

    size_t size() const {
        return data.size();
    }
};

#endif // LVM_HPP
