/**
 * @file file.cpp
 * @brief Implementation of file I/O utility functions
 * 
 * This file contains the implementation of efficient file operations
 * including memory-mapped file reading and standard file I/O with
 * proper error handling and directory creation.
 */

#include "file.hpp"

/**
 * @brief Opens a file for reading with error checking
 * @param filePath Path to the file to open
 * @return Input file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::ifstream openFileRead(const std::string &filePath)
{
    std::filesystem::path path(filePath);
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("No se pudo abrir el archivo: " + filePath);
    }
    return file;
}

/**
 * @brief Opens a file for writing with error checking and directory creation
 * @param filePath Path to the file to open
 * @return Output file stream
 * @throws std::runtime_error if file cannot be opened
 */
std::ofstream openFileWrite(const std::string &filePath)
{
    std::filesystem::path path(filePath);

    // Create parent directories if they don't exist
    if (!std::filesystem::exists(path) && !path.parent_path().empty())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path, std::ofstream::out | std::ofstream::trunc);
    if (!file.is_open())
    {
        throw std::runtime_error("No se pudo abrir el archivo: " + filePath);
    }
    return file;
}

/**
 * @brief Reads entire file contents using memory mapping for efficiency
 * 
 * This function uses memory mapping to efficiently read large files
 * without loading them entirely into memory. It's particularly useful
 * for processing large sensor data files.
 * 
 * @param filePath Path to the file to read
 * @return String containing the entire file contents
 * @throws std::runtime_error if file cannot be read
 */
std::string readFileContents(const std::string &filePath)
{
    // Open file descriptor
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filePath);
    }
    
    // Get file size
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        close(fd);
        throw std::runtime_error("No se pudo obtener información del archivo: " + filePath);
    }
    
    size_t fileSize = static_cast<size_t>(fileStat.st_size);
    
    // Handle empty files
    if (fileSize == 0) {
        close(fd);
        return std::string();
    }
    
    // Memory map the file for efficient reading
    void* mappedMemory = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    
    // Close file descriptor (mmap keeps its own reference)
    close(fd);
    
    if (mappedMemory == MAP_FAILED) {
        throw std::runtime_error("No se pudo hacer memory mapping del archivo: " + filePath);
    }
    
    // Copy the mapped memory to a string
    std::string contents(static_cast<const char*>(mappedMemory), fileSize);
    
    // Unmap the memory
    munmap(mappedMemory, fileSize);
    
    return contents;
}

