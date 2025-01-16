#ifndef DROP_DATA_HPP
#define DROP_DATA_HPP

#include "constants.hpp"
#include "lib.hpp"

class Drop
{
public:
    bool isPositive; // picos positivos
    int c1, c2;      // puntos criticos
    int u1, u2;      // posiciones iniciales
    int p1;          // punto medio del primer sensor
    int p2;          // punto de quiebre de la integral del segundo sensor
    double q1;       // carga del anillo
    double q2;       // carga del plato
    double q;        // carga promedio
    double v;        // velocidad
    std::vector<double> sensor1, sensor2; // datos de los sensores
    std::vector<double> integralSensor1,
        integralSensor2;        // integral de los sensores
    std::vector<double> a1, b1; // modelos del primer pulso
    std::vector<double> a2;     // modelo del segundo pulso
    std::vector<double> time;   // tiempos de cada punto
    double sumOfSquaredDiffPenalty1,
        sumOfSquaredDiffPenalty2; // penalizacion de suma de diferencias al
                                  // cuadrado
    double chargeDiffPenalty;     // penalizacion de la diff entre (q1 / q2) y
                                  // PROP_CHARGE
    double widthDiffPenalty;      // penalizacion de la diff entre (p1 / p2) y
                                  // PROP_WIDTH
    double noisePropPenalty; // penalizacion de la porcion de la señal que esta
                             // dentro de [-MINIMUM_CHARGE, MINIMUM_CHARGE]
    int id;                  // ID de la gota
    bool valid;

    Drop(bool isPositive, int c1, int c2);

    Drop();

    void debug() const;

    int size() const;

    void computeStats();

    double penalty() const;

    std::vector<Drop> static readFromFile(std::ifstream &file);

    void writeToFile(std::ofstream &file, bool withoutIndividualPenalties = false);

private:
    int findSensor1MiddlePoint();

    int findSensor2TippingPoint();

    void computeIntegral();

    void computeRingCharge();

    void computeDishCharge();

    void computeAverageCharge();

    int satisfiesBasicFilters();

    void computeVelocity();

    void computeSumOfSquaredDiffPenalty();

    void computeChargeDiffPenalty();

    void computeWidthDiffPenalty();

    void computeNoisePropPenalty();

    void computeModels();
};

#endif