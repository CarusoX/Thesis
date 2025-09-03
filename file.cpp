#include "file.hpp"

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

std::ofstream openFileWrite(const std::string &filePath)
{
    std::filesystem::path path(filePath);

    if (!std::filesystem::exists(path))
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
    
    // Memory map the file
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

