/**
 * @file drop_finder.cpp
 * @brief Main application for detecting and analyzing water drops from sensor data
 * 
 * This program processes sensor data from a drop measurement instrument to:
 * 1. Read raw sensor data from files
 * 2. Fill gaps in the data using interpolation
 * 3. Normalize the data to remove baseline drift
 * 4. Detect individual drops using peak detection algorithms
 * 5. Analyze drop properties (charge, velocity, diameter)
 * 6. Output results to a data file
 * 
 * The instrument measures electrical signals from two sensors (ring and dish)
 * as charged water drops pass through them, allowing calculation of drop
 * properties based on the electrical signatures.
 */

#include "DropFinder.hpp"
#include "Drop.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "normalizer.hpp"
#include "file.hpp"
#include "cli.hpp"

/**
 * @brief Reads sensor data from a file and loads it into the LVM buffer
 * 
 * This function uses memory mapping for efficient file reading and parses
 * the data line by line. Each line should contain three values:
 * - time: timestamp of the measurement
 * - sensor1: signal from the ring sensor
 * - sensor2: signal from the dish sensor
 * 
 * @param lvm Reference to the LVM buffer to store the data
 * @param cli Reference to CLI for progress reporting
 * @param filePath Path to the input data file
 */
void read(LVM &lvm, CLI &cli, const std::string &filePath)
{
    // Read entire file using memory mapping for efficiency
    std::string fileContents = readFileContents(filePath);
    
    cli.startProgress("read", "Reading data", fileContents.size());
    size_t currentPosition = 0;

    // Parse the file line by line
    for(size_t i = 0; i < fileContents.size(); i++) {
      // Check for end of line or end of file
      if(fileContents[i] == '\n' || i == fileContents.size() - 1) {
        // Extract the current line and add to LVM buffer
        lvm.addSensorData(fileContents.substr(currentPosition, i - currentPosition));
        currentPosition = i + 1;
        cli.updateProgress("read", currentPosition);
      }
    }
    
    cli.finishProgress("read");
}

/**
 * @brief Fills gaps in the sensor data using linear interpolation
 * 
 * This function detects missing data points (gaps larger than expected time intervals)
 * and fills them using linear interpolation between surrounding data points.
 * The interpolation uses a rolling window to calculate average values on both
 * sides of the gap for more accurate filling.
 * 
 * @param lvm Reference to the original LVM data with potential gaps
 * @param cli Reference to CLI for progress reporting
 * @param filledLvm Reference to the output LVM that will contain filled data
 */
void fill(LVM &lvm, CLI &cli, LVM &filledLvm) {
  size_t halfWindow = FILL_WINDOW_SIZE;
  double EXPECTED_TIME_DIFF = static_cast<double>(1) / DATA_PER_SECOND;
  double MAX_TIME_DIFF = 2 * EXPECTED_TIME_DIFF;

  cli.startProgress("fill", "Filling data", lvm.size());

  for(size_t i = 0; i < lvm.size(); i++) {
    // Check if there's a gap in the data (time difference too large)
    if(i > 0 && lvm[i].time - lvm[i - 1].time > MAX_TIME_DIFF) {
      double diff = lvm[i].time - lvm[i - 1].time;
      
      // Calculate how many data points we need to fill the gap
      size_t numberOfLinesToFill = std::round(diff / EXPECTED_TIME_DIFF) - 1;
      
      // Calculate average values from left and right windows around the gap
      double leftAvgSensor1 = 0, leftAvgSensor2 = 0;
      double rightAvgSensor1 = 0, rightAvgSensor2 = 0;
      size_t leftCount = 0, rightCount = 0;

      // Calculate left window average (before the gap)
      for(size_t j = i < halfWindow ? 0 : i - halfWindow; j < i; j++) {
        leftAvgSensor1 += lvm[j].sensor1;
        leftAvgSensor2 += lvm[j].sensor2;
        leftCount++;
      }

      // Calculate right window average (after the gap)
      for(size_t j = i + 1; j < std::min(lvm.size(), i + halfWindow); j++) {
        rightAvgSensor1 += lvm[j].sensor1;
        rightAvgSensor2 += lvm[j].sensor2;
        rightCount++;
      }

      // Compute the averages
      leftAvgSensor1 /= leftCount;
      leftAvgSensor2 /= leftCount;
      rightAvgSensor1 /= rightCount;
      rightAvgSensor2 /= rightCount;

      // Fill the gap using linear interpolation
      for(size_t j = 0; j < numberOfLinesToFill; j++) {
        double t = lvm[i - 1].time + (j + 1) * EXPECTED_TIME_DIFF;
        // Linear interpolation between left and right averages
        double sensor1 = leftAvgSensor1 + (rightAvgSensor1 - leftAvgSensor1) * (t - lvm[i - 1].time) / diff;
        double sensor2 = leftAvgSensor2 + (rightAvgSensor2 - leftAvgSensor2) * (t - lvm[i - 1].time) / diff;
        LVM::Row filledRow = LVM::Row{t, sensor1, sensor2, 0};
        filledLvm.addSensorData(filledRow);
      }
    }
    // Add the original data point
    filledLvm.addSensorData(lvm[i]);
    cli.updateProgress("fill", i);
  }
  cli.finishProgress("fill");
}

/**
 * @brief Removes baseline drift from sensor data using rolling window normalization
 * 
 * This function normalizes the sensor data to remove baseline drift and offset
 * variations that can occur in long-term measurements. It uses a rolling window
 * approach to calculate local averages and subtract them from the data.
 * 
 * @param lvm Reference to the input LVM data with potential baseline drift
 * @param cli Reference to CLI for progress reporting
 * @param offsetLvm Reference to the output LVM with normalized data
 */
void remove_offset(LVM &lvm, CLI &cli, LVM &offsetLvm) {
  // Normalize the data using rolling window approach
  std::vector<LVM::Row> normalizedData = normalizer::normalizeWithRolling(lvm.get(), cli);
  
  // Copy normalized data to output LVM
  for(LVM::Row &row : normalizedData) {
    offsetLvm.addSensorData(row);
  }
}

/**
 * @brief Detects and analyzes individual drops in the sensor data
 * 
 * This is the core function that processes the normalized sensor data to detect
 * individual water drops. It uses a sliding window approach with a DropFinder
 * to identify drop signatures in the data. Each detected drop is analyzed for
 * its properties (charge, velocity, diameter) and written to the output file.
 * 
 * The algorithm:
 * 1. Maintains a sliding window of 2*DROP_SIZE data points
 * 2. Uses DropFinder to detect drop signatures in the window
 * 3. Validates detected drops using various filters
 * 4. Marks used data points to avoid double-counting
 * 5. Writes valid drops to the output file
 * 
 * @param lvm Reference to the normalized sensor data
 * @param cli Reference to CLI for progress reporting
 * @param findLvm Reference to the sliding window buffer for drop detection
 * @param outFile Reference to the output file stream for writing results
 */
void find_drops(LVM &lvm, CLI &cli, LVM &findLvm, std::ofstream &outFile) {
  cli.startProgress("find_drops", "Finding drops", lvm.size());
  DropFinder dropFinder;
  size_t gotas = 0; // Counter for detected drops
  
  for(size_t i = 0; i < lvm.size(); i++) {
    // Add current data point to the sliding window
    findLvm.addSensorData(lvm[i]);
    cli.updateProgress("find_drops", i);
    
    // Process when we have enough data in the window (2*DROP_SIZE)
    if(findLvm.size() == DROP_SIZE * 2) {
      // Skip if too much data is already marked as used
      if(findLvm.totalUsed > NN) {
        continue;
      }
      
      Drop drop;
      do {
        // Try to find a drop in the current window
        drop = dropFinder.findDrop(findLvm);
        
        if(drop.c1 == -1) {
          // No peak found, exit the detection loop
          break;
        }
        
        if(!drop.valid) {
          // Drop found but failed validation - mark peaks as used to avoid re-detection
          findLvm.setUsed(drop.u1Original + drop.c1, drop.u1Original + drop.c1 + 1);
          findLvm.setUsed(drop.u1Original + drop.c2, drop.u1Original + drop.c2 + 1);
          continue;
        }
        
        // Valid drop found - mark the entire drop region as used
        findLvm.setUsed(drop.u1Original, drop.u1Original + drop.size() - 1);
        drop.id = ++gotas; // Assign unique ID
        drop.dataOffset = static_cast<int>(i - findLvm.size() + 1 + drop.u1Original);
        drop.writeToFile(outFile); // Write to output file
      } while(true);

      // Mark the first half of the window as used to advance the sliding window
      findLvm.setUsed(0, DROP_SIZE - 1);
    }
  }
  cli.finishProgress("find_drops");
}

/**
 * @brief Main processing pipeline for drop detection and analysis
 * 
 * This function orchestrates the complete drop detection pipeline:
 * 1. Read raw sensor data from file
 * 2. Fill gaps in the data using interpolation
 * 3. Normalize data to remove baseline drift
 * 4. Detect and analyze individual drops
 * 5. Write results to output file
 * 
 * The function manages memory efficiently by clearing intermediate buffers
 * after each processing step to minimize memory usage.
 * 
 * @param filePath Path to the input sensor data file
 * @param outPath Path to the output file for drop analysis results
 */
void perform(const std::string &filePath, const std::string &outPath)
{
    CLI cli;
    
    // Initialize LVM buffers for different processing stages
    LVM lvm(size_t(-1));           // Original data buffer (unlimited size)
    LVM filledLvm(size_t(-1));     // Data with gaps filled (unlimited size)
    LVM offsetLvm(size_t(-1));     // Normalized data (unlimited size)
    LVM findLvm(2 * DROP_SIZE);    // Sliding window for drop detection (fixed size)
    
    // Step 1: Read raw sensor data from file
    read(lvm, cli, filePath);

    // Step 2: Fill gaps in the data using interpolation
    fill(lvm, cli, filledLvm);
    lvm.clear(); // Free memory from original data

    // Step 3: Normalize data to remove baseline drift
    remove_offset(filledLvm, cli, offsetLvm);
    filledLvm.clear(); // Free memory from filled data

    // Step 4: Detect drops and write results
    auto outFile = openFileWrite(outPath);
    Drop::writeHeader(outFile); // Write column headers
    find_drops(offsetLvm, cli, findLvm, outFile);
}

/**
 * @brief Main entry point for the drop finder application
 * 
 * This is the main function that handles command-line arguments and orchestrates
 * the drop detection process. It expects one command-line argument (input file path)
 * and outputs results to "drops.dat" in the current directory.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Exit code: 0 for success, 1 for error
 */
int main(int argc, char *argv[])
{
    FAST_IO; // Enable fast I/O for better performance
    
    // Check for required command-line arguments
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file path>" << std::endl;
        return 1;
    }

    // Set output file path (default: "drops.dat" in current directory)
    std::string outPath = "drops.dat";

    try
    {
        // Run the main processing pipeline
        perform(argv[1], outPath);
    }
    catch (const std::exception &e)
    {
        // Handle and report any errors during processing
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
