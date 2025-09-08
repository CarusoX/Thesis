#include "DropFinder.hpp"
#include "Drop.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "normalizer.hpp"
#include "file.hpp"
#include "cli.hpp"

void read(LVM &lvm, CLI &cli, const std::string &filePath)
{
    // Read entire file using memory mapping
    std::string fileContents = readFileContents(filePath);
    
    cli.startProgress("read", "Reading data", fileContents.size());
    size_t currentPosition = 0;

    for(size_t i = 0; i < fileContents.size(); i++) {
      if(fileContents[i] == '\n' || i == fileContents.size() - 1) {
        lvm.addSensorData(fileContents.substr(currentPosition, i - currentPosition));
        currentPosition = i + 1;
        cli.updateProgress("read", currentPosition);
      }
    }
    
    cli.finishProgress("read");
}

void fill(LVM &lvm, CLI &cli, LVM &filledLvm) {
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
        LVM::Row filledRow = LVM::Row{t, sensor1, sensor2, 0};
        filledLvm.addSensorData(filledRow);
      }
    }
    filledLvm.addSensorData(lvm[i]);
    cli.updateProgress("fill", i);
  }
  cli.finishProgress("fill");
}

void remove_offset(LVM &lvm, CLI &cli, LVM &offsetLvm) {
  std::vector<LVM::Row> normalizedData = normalizer::normalizeWithRolling(lvm.get(), cli);
  for(LVM::Row &row : normalizedData) {
    offsetLvm.addSensorData(row);
  }
}

void find_drops(LVM &lvm, CLI &cli, LVM &findLvm, std::ofstream &outFile) {
  cli.startProgress("find_drops", "Finding drops", lvm.size());
  DropFinder dropFinder;
  size_t gotas = 0;
  for(size_t i = 0; i < lvm.size(); i++) {
    findLvm.addSensorData(lvm[i]);
    cli.updateProgress("find_drops", i);
    if(findLvm.size() == DROP_SIZE * 2) {
      if(findLvm.totalUsed > NN) {
        continue;
      }
      Drop drop;
      do {
        drop = dropFinder.findDrop(findLvm);
        if(drop.c1 == -1) {
          // No se encontro ningun pico
          break;
        }
        if(!drop.valid) {
          // Marcamos los picos encontrados como usados
          findLvm.setUsed(drop.u1Original + drop.c1, drop.u1Original + drop.c1 + 1);
          findLvm.setUsed(drop.u1Original + drop.c2, drop.u1Original + drop.c2 + 1);
          continue;
        }
        findLvm.setUsed(drop.u1Original, drop.u1Original + drop.size() - 1);
        drop.id = ++gotas;
        drop.dataOffset = static_cast<int>(i - findLvm.size() + 1 + drop.u1Original);
        drop.writeToFile(outFile);
      } while(true);

      findLvm.setUsed(0, DROP_SIZE - 1);
    }
  }
  cli.finishProgress("find_drops");
}

void perform(const std::string &filePath, const std::string &outPath)
{
    CLI cli;
    LVM lvm(size_t(-1));
    LVM filledLvm(size_t(-1));
    LVM offsetLvm(size_t(-1));
    LVM findLvm(2 * DROP_SIZE);
    
    read(lvm, cli, filePath);

    fill(lvm, cli, filledLvm);
    lvm.clear(); // clear the original lvm

    remove_offset(filledLvm, cli, offsetLvm);
    filledLvm.clear(); // clear the filled lvm

    auto outFile = openFileWrite(outPath);
    Drop::writeHeader(outFile);
    find_drops(offsetLvm, cli, findLvm, outFile);
}

int main(int argc, char *argv[])
{
    FAST_IO;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file path>" << std::endl;
        return 1;
    }

    std::filesystem::path outPath = std::filesystem::current_path() / "drops.dat";

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
