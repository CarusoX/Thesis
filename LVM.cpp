#include "LVM.hpp"

LVM::LVM(size_t buffer_size) : maxSize(buffer_size) {}

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
    data.pop_front(); // Remove the oldest element
  }
  data.push_back({time, sensor1, sensor2, 0});
}

void LVM::keepLast(size_t size)
{
  while (data.size() > size)
  {
    data.pop_front();
  }
}

void LVM::setUsed(size_t r1, size_t r2)
{
  if (r2 >= data.size())
  {
    throw std::out_of_range("Index out of range");
  }
  for (size_t i = r1; i <= r2; ++i)
  {
    data[i].used = 1;
  }
}

std::vector<LVM::Row> LVM::get() const
{
  return std::vector<Row>(data.begin(), data.end());
}

size_t LVM::size() const
{
  return data.size();
}
