#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <deque>
#include "LVM.hpp"
#include "filter.hpp"
#include "Drop.hpp"
#include "DropFinder.hpp"
#include "constants.hpp"

// Define a function to write drop data to a file
void writeDropToFile(Drop &drop, const std::string &filename)
{
    std::ofstream outFile(filename, std::ofstream::out | std::ofstream::trunc);

    if (!outFile.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
        return;
    }

    outFile << std::fixed << std::setprecision(6); // Format numbers to 6 decimal places
    for (int i = 0; i < drop.p2; ++i)
    {
        std::vector<double> row = {
            drop.time[i],
            drop.sensor1[i],
            drop.sensor2[i],
            drop.integralSensor1[i] * INTEGRATION_FACTOR / DATA_PER_SECOND,
            drop.integralSensor2[i] * INTEGRATION_FACTOR / DATA_PER_SECOND,
            drop.a1[i],
            drop.a2[i],
            drop.b1[i],
            drop.q1,
            drop.q2,
            drop.penalty(),
        };

        for (uint j = 0; j < row.size(); j++)
        {
            outFile << row[j] << "\t\n"[j + 1 == row.size()];
        }
    }

    outFile.close();
}

void perform(const std::string &fifoPath)
{
    if (!std::filesystem::exists(fifoPath))
    {
        throw std::invalid_argument(
            "El archivo FIFO '" + fifoPath + "' no existe. Usa 'mkfifo' para crearlo.");
    }

    std::cout << "Leyendo del FIFO: " << fifoPath << std::endl;

    LVM lvm(2 * WINDOW_SIZE);
    DropFinder dropFinder;
    size_t gotas = 0;

    std::ifstream fifo(fifoPath);
    if (!fifo.is_open())
    {
        throw std::runtime_error("No se pudo abrir el archivo FIFO.");
    }

    std::string line;
    while (true)
    {
        if (!std::getline(fifo, line))
        {
            // Esperar más datos si no hay líneas disponibles
            exit(0);
            continue;
        }

        lvm.addSensorData(line); // Agrega datos al buffer

        if (lvm.size() >= WINDOW_SIZE * 2)
        {
            std::vector<LVM::Row> normalizedData = Filter::normalizeWithRolling(lvm.get()); // Computar rolling average

            std::vector<LVM::Row> findData(
                normalizedData.end() - DROP_SIZE * 2,
                normalizedData.end());

            // Check if all "Used" entries are 0
            bool allUsedZAero = std::all_of(findData.begin(), findData.end(), [](const LVM::Row &row)
                                            { return row.used == 0; });

            if (!allUsedZAero)
            {
                continue;
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

                // Write the drop data to a file
                std::cout << "Gotas: " << ++gotas << std::endl;
                drop.debug();
                std::string dropFilename = "drops.dat";
                writeDropToFile(drop, dropFilename);
            } while (true);

            lvm.setUsed(offset, offset + DROP_SIZE);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <ruta al archivo FIFO>" << std::endl;
        return 1;
    }

    try
    {
        perform(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
