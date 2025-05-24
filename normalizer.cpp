#include "normalizer.hpp"

namespace normalizer {

std::vector<LVM::Row> fillHoles(const std::vector<LVM::Row> &data, CLI &cli) {
    if (data.size() < 2) return data;  // Need at least 2 points to detect holes
    
    cli.startProgress("fill_holes", "Filling data holes", data.size());
    std::vector<LVM::Row> filledData;
    filledData.reserve(data.size());  // Reserve space for efficiency
    
    const double expectedInterval = 1.0 / DATA_PER_SECOND;  // 0.0002 seconds
    const double maxExpectedInterval = 2.0 * expectedInterval;  // 0.0004 seconds
    const size_t halfWindow = WINDOW_SIZE / 2;
    size_t numberOfHoles = 0;
    
    // Add first point
    filledData.push_back(data[0]);
    
    for (size_t i = 1; i < data.size(); ++i) {
        double timeDiff = data[i].time - data[i-1].time;
        
        if (timeDiff > maxExpectedInterval) {
            // Calculate number of points to insert
            size_t pointsToInsert = static_cast<size_t>(std::round(timeDiff / expectedInterval)) - 1;
            
            // Calculate averages for interpolation
            double leftAvgSensor1 = 0.0, leftAvgSensor2 = 0.0;
            double rightAvgSensor1 = 0.0, rightAvgSensor2 = 0.0;
            
            // Calculate left average
            size_t leftStart = (i > halfWindow) ? i - halfWindow : 0;
            size_t leftCount = i - leftStart;
            for (size_t j = leftStart; j < i; ++j) {
                leftAvgSensor1 += data[j].sensor1;
                leftAvgSensor2 += data[j].sensor2;
            }
            leftAvgSensor1 /= leftCount;
            leftAvgSensor2 /= leftCount;
            
            // Calculate right average
            size_t rightEnd = std::min(i + halfWindow, data.size());
            size_t rightCount = rightEnd - i;
            for (size_t j = i; j < rightEnd; ++j) {
                rightAvgSensor1 += data[j].sensor1;
                rightAvgSensor2 += data[j].sensor2;
            }
            rightAvgSensor1 /= rightCount;
            rightAvgSensor2 /= rightCount;
            
            // Generate interpolated points
            for (size_t j = 1; j <= pointsToInsert; ++j) {
                double t = static_cast<double>(j) / (pointsToInsert + 1);
                LVM::Row newRow;
                newRow.time = data[i-1].time + j * expectedInterval;
                newRow.sensor1 = leftAvgSensor1 + t * (rightAvgSensor1 - leftAvgSensor1);
                newRow.sensor2 = leftAvgSensor2 + t * (rightAvgSensor2 - leftAvgSensor2);
                filledData.push_back(newRow);
            }
            numberOfHoles++;
        }
        
        filledData.push_back(data[i]);
        cli.updateProgress("fill_holes", i);
    }
    
    cli.finishProgress("fill_holes");
    cli.printSuccess("Filled " + std::to_string(numberOfHoles) + " holes");
    return filledData;
}

std::vector<LVM::Row>
normalizeWithRolling(const std::vector<LVM::Row> &data, CLI &cli)
{
    cli.startProgress("normalize", "Normalizing data", data.size());
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
        cli.updateProgress("normalize", i);
    }
    cli.finishProgress("normalize");
    return normalizedData;
}

}
