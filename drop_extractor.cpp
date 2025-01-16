#include "Drop.hpp"
#include "DropFinder.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "filter.hpp"
#include <deque>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void processLine(LVM &lvm, std::string line, size_t &gotas,
                 std::ofstream &outFile)
{
    DropFinder dropFinder;
    lvm.addSensorData(line); // Agrega datos al buffer

    if (lvm.size() >= WINDOW_SIZE * 2)
    {
        std::vector<LVM::Row> normalizedData =
            Filter::normalizeWithRolling(lvm.get()); // Computar rolling average

        std::vector<LVM::Row> findData(normalizedData.end() - DROP_SIZE * 2,
                                       normalizedData.end());

        // Check if all "Used" entries are 0
        bool allUsedAreZero =
            std::all_of(findData.begin(), findData.end(),
                        [](const LVM::Row &row) { return row.used == 0; });

        if (!allUsedAreZero)
        {
            return;
        }

        Drop drop;
        size_t offset = normalizedData.size() - DROP_SIZE * 2;
        do
        {
            drop = dropFinder.findDrop(findData);
            if (!drop.valid)
            {
                break;
            }
            // Marcar la data de la gota como usada
            for (int i = drop.u1; i < drop.u1 + drop.size(); i++)
            {
                findData[i].used = 1;
            }

            // Darle un ID a la gota
            drop.id = ++gotas;

            // Write the drop data to a file
            drop.writeToFile(outFile);

            // Print drop debugging info
            drop.debug();
        } while (true);

        lvm.setUsed(offset, offset + DROP_SIZE);
    }
}

void perform(const std::string &filePath, const std::string &outPath)
{
    if (!std::filesystem::exists(filePath))
    {
        throw std::invalid_argument("El archivo '" + filePath + "' no existe.");
    }

    std::cout << "Leyendo del archivo: " << filePath << std::endl;

    LVM lvm(2 * WINDOW_SIZE);

    size_t gotas = 0;

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

    std::string line;
    char ch;
    while (true)
    {
        if (!file.get(ch))
        {
            // Esperar más datos si no hay mas caracteres disponibles
            continue;
        }
        else if (ch == '\r')
        {
            // ignorar
        }
        else if (ch != '\n')
        {
            line += ch;
        }
        else
        {
            processLine(lvm, line, gotas, outFile);
            line.clear();
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <ruta al archivo>" << std::endl;
        return 1;
    }

    try
    {
        perform(argv[1], "drops.dat");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
