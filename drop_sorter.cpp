#include "Drop.hpp"
#include "file.hpp"

void perform(const std::string &filePath, const std::string &outPath)
{
    auto file = openFileRead(filePath);

    auto outFile = openFileWrite(outPath);

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
        drop.writeToFile(outFile, true);
    }
}

int main()
{
    FAST_IO;
    try
    {
        std::filesystem::path outPath = std::filesystem::current_path() / "drops_sorted.dat";
        perform("drops.dat", outPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
