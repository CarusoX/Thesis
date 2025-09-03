#include "lib.hpp"

std::ifstream openFileRead(const std::string &filePath);
std::ofstream openFileWrite(const std::string &filePath);

std::string readFileContents(const std::string &filePath);