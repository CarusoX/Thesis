#include "Drop.hpp"
#include "DropFinder.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "file.hpp"
#include "filter.hpp"

void perform(const std::string &filePath, const std::string &outPath)
{
    auto file = openFileRead(filePath);
    auto outFile = openFileWrite(outPath);

    LVM lvm(size_t(-1));

    std::string line;
    while (std::getline(file, line))
    {
        lvm.addSensorData(line);
    }

    std::vector<LVM::Row> normalizedData =
        Filter::normalizeWithRolling(lvm.get()); // Compute rolling average

    for (const auto& row : normalizedData) {
        outFile << std::fixed << std::setprecision(6) << row.time << " " << row.sensor1 << " " << row.sensor2 << "\n";
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file path>" << std::endl;
        return 1;
    }

    std::filesystem::path outPath = std::filesystem::current_path() / "drops_average.dat";

    try
    {
        perform(argv[1], outPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
