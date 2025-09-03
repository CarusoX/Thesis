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

void write(std::ofstream &outFile, LVM::Row row) {
  outFile << std::fixed << std::setprecision(6) 
          << row.time << " " << row.sensor1 << " " << row.sensor2 << "\n";
}

void perform(const std::string &filePath, const std::string &outPath)
{
    CLI cli;
    LVM lvm(size_t(-1));
    read(lvm, cli, filePath);
    auto outFile = openFileWrite(outPath);
    size_t halfWindow = FILL_WINDOW_SIZE;
    double EXPECTED_TIME_DIFF = static_cast<double>(1) / DATA_PER_SECOND;
    double MAX_TIME_DIFF = 2 * EXPECTED_TIME_DIFF;

    cli.startProgress("fill", "Filling data", lvm.size());

    for(size_t i = 0; i < lvm.size(); i++) {
      if(i > 0 && lvm[i].time - lvm[i - 1].time > MAX_TIME_DIFF) {
        double diff = lvm[i].time - lvm[i - 1].time;
        // We have a hole, fill it and start from scratch
        size_t numberOfLinesToFill = std::round(diff / EXPECTED_TIME_DIFF) - 1;
        double leftAvgSensor1 = 0, leftAvgSensor2 = 0;
        double rightAvgSensor1 = 0, rightAvgSensor2 = 0;
        size_t leftCount = 0, rightCount = 0;

        for(size_t j = i < halfWindow ? 0 : i - halfWindow; j < i; j++) {
          leftAvgSensor1 += lvm[j].sensor1;
          leftAvgSensor2 += lvm[j].sensor2;
          leftCount++;
        }

        for(size_t j = i + 1; j < std::min(lvm.size(), i + halfWindow); j++) {
          rightAvgSensor1 += lvm[j].sensor1;
          rightAvgSensor2 += lvm[j].sensor2;
          rightCount++;
        }

        leftAvgSensor1 /= leftCount;
        leftAvgSensor2 /= leftCount;
        rightAvgSensor1 /= rightCount;
        rightAvgSensor2 /= rightCount;

        for(size_t j = 0; j < numberOfLinesToFill; j++) {
          double t = lvm[i - 1].time + (j + 1) * EXPECTED_TIME_DIFF;
          double sensor1 = leftAvgSensor1 + (rightAvgSensor1 - leftAvgSensor1) * (t - lvm[i - 1].time) / diff;
          double sensor2 = leftAvgSensor2 + (rightAvgSensor2 - leftAvgSensor2) * (t - lvm[i - 1].time) / diff;
          write(outFile, LVM::Row{t, sensor1, sensor2, 0});
        }
      }
      write(outFile, lvm[i]);
      cli.updateProgress("fill", i);
    }
    cli.finishProgress("fill");

    cli.printSuccess("Processing complete!");
}

int main(int argc, char *argv[])
{
    FAST_IO;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file path>" << std::endl;
        return 1;
    }

    std::filesystem::path outPath = std::filesystem::current_path() / "drops_filled.dat";

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
