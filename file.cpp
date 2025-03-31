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
