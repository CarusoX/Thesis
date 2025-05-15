#include "Drop.hpp"
#include "DropFinder.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "file.hpp"
#include "filter.hpp"

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

void perform(const std::string &filePath, const std::string &outPath,
             bool realtime)
{
    auto file = openFileRead(filePath);

    // TODO: solve issue non existing file
    auto outFile = openFileWrite(outPath);

    LVM lvm(2 * WINDOW_SIZE);

    size_t gotas = 0;

    std::string line;
    char ch;
    uint start_pos = 0;
    while (true)
    {
        if (!file.get(ch))
        {
            if (!realtime) {
                break;
            }
            // Esperar más datos si no hay mas caracteres disponibles
            // Para esto se cierra el archivo y se abre de nuevo
            // y se posiciona en la posición donde se quedó
            file.close();
            file = openFileRead(filePath);
            file.seekg(start_pos);
            continue;
        }
        start_pos = file.tellg();
        if (ch == '\r')
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
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <ruta al archivo>" << std::endl;
        return 1;
    }

    bool realtime = false;
    if (argc == 3 && std::string(argv[2]) == "--realtime")
    {
        realtime = true;
    }

    try
    {
        perform(argv[1], "drops.dat", realtime);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
