#include "Drop.hpp"
#include "DropFinder.hpp"
#include "LVM.hpp"
#include "constants.hpp"
#include "file.hpp"

const std::string HISTOGRAM_CHARGE_FILE = "graficos/histograma_carga.dat";
const std::string HISTOGRAM_DIAMETER_FILE = "graficos/histograma_diametro.dat";
const std::string CAR_VS_TIME_FILE = "graficos/carga_vs_tiempo.dat";
const std::string DIAMETER_VS_TIME_FILE = "graficos/diametro_vs_tiempo.dat";
const std::string CAR_VS_DIAMETER_FILE = "graficos/carga_vs_diametro.dat";
const std::string CAR_VS_TIME_PROM30_FILE =
    "graficos/carga_vs_tiempo_prom30.dat";
const std::string DIAMETER_VS_TIME_PROM30_FILE =
    "graficos/diametro_vs_tiempo_prom30.dat";
const std::string CAR_VS_TIME_PROM30_POS_FILE =
    "graficos/carga_vs_tiempo_prom30_pos.dat";
const std::string DIAMETER_VS_TIME_PROM30_POS_FILE =
    "graficos/diametro_vs_tiempo_prom30_pos.dat";
const std::string CAR_VS_TIME_PROM30_NEG_FILE =
    "graficos/carga_vs_tiempo_prom30_neg.dat";
const std::string DIAMETER_VS_TIME_PROM30_NEG_FILE =
    "graficos/diametro_vs_tiempo_prom30_neg.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM02_FILE =
    "graficos/carga_prom_vs_diametro_prom0.2.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM02_POS_FILE =
    "graficos/carga_prom_vs_diametro_prom0.2_pos.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM02_NEG_FILE =
    "graficos/carga_prom_vs_diametro_prom0.2_neg.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM05_FILE =
    "graficos/carga_prom_vs_diametro_prom0.5.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM05_POS_FILE =
    "graficos/carga_prom_vs_diametro_prom0.5_pos.dat";
const std::string CAR_PROM_VS_DIAMETER_PROM05_NEG_FILE =
    "graficos/carga_prom_vs_diametro_prom0.5_neg.dat";
const std::string CAR_VS_TIME_POS_FILE = "graficos/carga_vs_tiempo_pos.dat";
const std::string CAR_VS_TIME_NEG_FILE = "graficos/carga_vs_tiempo_neg.dat";
const std::string DIAMETER_VS_TIME_POS_FILE =
    "graficos/diametro_vs_tiempo_pos.dat";
const std::string DIAMETER_VS_TIME_NEG_FILE =
    "graficos/diametro_vs_tiempo_neg.dat";

void chargeHistogram(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(HISTOGRAM_CHARGE_FILE);

    std::vector<int> histogram(HISTOGRAM_CHARGE_MAX - HISTOGRAM_CHARGE_MIN, 0);

    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            histogram[int(drop.q) - HISTOGRAM_CHARGE_MIN]++;
        }
    }

    for (uint i = 0; i < histogram.size(); i++)
    {
        double charge = double(HISTOGRAM_CHARGE_MIN) + i + 0.5;
        outFile << charge << " " << histogram[i] << std::endl;
    }
}

void diameterHistogram(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(HISTOGRAM_DIAMETER_FILE);

    std::vector<int> histogram(HISTOGRAM_DIAMETER_PRECISION * HISTOGRAM_DIAMETER_MAX, 0);

    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            histogram[int(HISTOGRAM_DIAMETER_PRECISION * drop.d)]++;
        }
    }

    for (uint i = 0; i < histogram.size(); i++)
    {
        double diameter = i / double(HISTOGRAM_DIAMETER_PRECISION) + 0.25;
        outFile << diameter << " " << histogram[i] << std::endl;
    }
}

void averageDiameter(const std::vector<Drop> &drops)
{
    double sum = 0;
    int count = 0;
    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            sum += drop.d;
            count++;
        }
    }
    if (count != 0)
    {
        std::cout << "Diametro promedio: " << sum / count << std::endl;
    }
}

void chargeVsTime(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(CAR_VS_TIME_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            outFile << drop.time[0] << " " << drop.q
                    << std::endl;
        }
    }
}

void diameterVsTime(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(DIAMETER_VS_TIME_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            outFile << drop.time[0] << " " << drop.d
                    << std::endl;
        }
    }
}

void chargeVsDiameter(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(CAR_VS_DIAMETER_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid)
        {
            outFile << drop.d << " " << drop.q << std::endl;
        }
    }
}

void chargeVsTimePos(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(CAR_VS_TIME_POS_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid && drop.q >= 0)
        {
            outFile << drop.time[0] << " " << drop.q
                    << std::endl;
        }
    }
}

void chargeVsTimeNeg(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(CAR_VS_TIME_NEG_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid && drop.q < 0)
        {
            outFile << drop.time[0] << " " << drop.q
                    << std::endl;
        }
    }
}

void diameterVsTimePos(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(DIAMETER_VS_TIME_POS_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid && drop.q >= 0)
        {
            outFile << drop.u1 + drop.p1 / DATA_PER_SECOND << " " << drop.d
                    << std::endl;
        }
    }
}

void diameterVsTimeNeg(const std::vector<Drop> &drops)
{
    auto outFile = openFileWrite(DIAMETER_VS_TIME_NEG_FILE);
    for (const auto &drop : drops)
    {
        if (drop.valid && drop.q < 0)
        {
            outFile << drop.u1 + drop.p1 / DATA_PER_SECOND << " " << drop.d
                    << std::endl;
        }
    }
}

int main()
{
    FAST_IO;
    try
    {
        auto file = openFileRead("drops.dat");

        std::vector<Drop> drops = Drop::readFromFile(file);


        chargeHistogram(drops);
        diameterHistogram(drops);
        averageDiameter(drops);
        chargeVsTime(drops);
        std::cout << drops.size() << " gotas leidas" << std::endl;
        diameterVsTime(drops);
        chargeVsTimePos(drops);
        chargeVsTimeNeg(drops);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}