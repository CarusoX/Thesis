#include "Drop.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void perform(const std::string &filePath, const std::string &outPath)
{
    if (!std::filesystem::exists(filePath))
    {
        throw std::invalid_argument("El archivo '" + filePath + "' no existe.");
    }

    std::cout << "Leyendo del archivo: " << filePath << std::endl;

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("No se pudo abrir el archivo.");
    }

    std::ofstream outFile(outPath, std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        throw std::runtime_error("No se pudo abrir el archivo.");
    }

    std::vector<Drop> drops = Drop::readFromFile(file);

    std::cout << drops.size() << " gotas leidas" << std::endl;

    // Creamos un arreglo de indices
    std::vector<int> indexes(drops.size());
    for (uint i = 0; i < drops.size(); i++)
    {
        indexes[i] = i;
    }
    // Ordenamos el arreglo de indices basandonos en la penalidad
    std::sort(indexes.begin(), indexes.end(),
              [&](const int &a, const int &b) -> bool
              { return drops[a].penalty() < drops[b].penalty(); });

    int current_time = 0;
    for (int dropIndex : indexes)
    {
        Drop drop = drops[dropIndex];
        for (int i = 0; i < drop.size(); i++)
        {
            drop.time[i] = current_time++;
        }
        drop.writeToFile(outFile);
    }
}

int main()
{
    try
    {
        perform("drops.dat", "drops_sorted.dat");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
