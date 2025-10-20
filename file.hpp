/**
 * @file file.hpp
 * @brief Header file for file I/O utility functions
 * 
 * This file provides utility functions for efficient file operations,
 * including memory-mapped file reading for large datasets and standard
 * file I/O operations with proper error handling.
 */

#include "lib.hpp"

/**
 * @brief Opens a file for reading with error checking
 * @param filePath Path to the file to open
 * @return Input file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::ifstream openFileRead(const std::string &filePath);

/**
 * @brief Opens a file for writing with error checking and directory creation
 * @param filePath Path to the file to open
 * @return Output file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::ofstream openFileWrite(const std::string &filePath);

/**
 * @brief Reads entire file contents using memory mapping for efficiency
 * @param filePath Path to the file to read
 * @return String containing the entire file contents
 * @throws std::runtime_error if file cannot be read
 */
std::string readFileContents(const std::string &filePath);