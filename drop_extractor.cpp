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

    if (lvm.size() == DROP_SIZE * 2)
    {
        if (lvm.totalUsed != 0)
        {
            return;
        }

        Drop drop;
        do
        {
            drop = dropFinder.findDrop(lvm);
            if (!drop.valid)
            {
                break;
            }
            // Marcar la data de la gota como usada
            lvm.setUsed(drop.u1, drop.u1 + drop.size() - 1);

            // Darle un ID a la gota
            drop.id = ++gotas;

            // Write the drop data to a file
            drop.writeToFile(outFile);

            // Print drop debugging info
            drop.debug();
        } while (true);

        lvm.setUsed(0, DROP_SIZE - 1);
    }
}

void perform(const std::string &filePath, const std::string &outPath)
{
    auto file = openFileRead(filePath);

    auto outFile = openFileWrite(outPath);

    LVM lvm(2 * DROP_SIZE);

    std::string line;

    size_t gotas = 0;

    while (std::getline(file, line))
    {
        processLine(lvm, line, gotas, outFile);
    }
}

int main()
{
    std::filesystem::path outPath = std::filesystem::current_path() / "drops.dat";

    try
    {
        perform("drops_average.dat", outPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
