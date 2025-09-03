#include "Drop.hpp"
#include "DropFinder.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "file.hpp"
#include "normalizer.hpp"
#include "utils.hpp"
#include "cli.hpp"

void read(LVM &lvm, CLI &cli, const std::string &filePath)
{
    size_t totalLines = utils::countLines(filePath);
    auto file = openFileRead(filePath);

    cli.startProgress("read", "Reading data", totalLines);
    size_t currentLine = 0;
    std::string line;
    while (std::getline(file, line))
    {
        lvm.addSensorData(line);
        cli.updateProgress("read", ++currentLine);
    }
    cli.finishProgress("read");
}

void write(const std::vector<LVM::Row> &normalizedData, CLI &cli, const std::string &outPath)
{
    auto outFile = openFileWrite(outPath);
    cli.startProgress("write", "Writing results", normalizedData.size());
    size_t currentLine = 0;
    for (const auto& row : normalizedData) {
        outFile << std::fixed << std::setprecision(6) 
                << row.time << " " << row.sensor1 << " " << row.sensor2 << "\n";
        cli.updateProgress("write", ++currentLine);
    }
    cli.finishProgress("write");
}


void perform(const std::string &filePath, const std::string &outPath)
{
    CLI cli;
    LVM lvm(size_t(-1));

    read(lvm, cli, filePath);

    std::vector<LVM::Row> normalizedData = normalizer::normalizeWithRolling(lvm.get(), cli);
    lvm.clear();

    write(normalizedData, cli, outPath);

    cli.printSuccess("Processing complete!");
}

int main()
{
    FAST_IO;
    std::filesystem::path outPath = std::filesystem::current_path() / "drops_average.dat";

    try
    {
        perform("drops_filled.dat", outPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
