#ifndef DROP_DATA_HPP
#define DROP_DATA_HPP

#include <iostream>
#include "constants.hpp"

class Drop
{
public:
    bool isPositive;                                      // picos positivos
    int c1, c2;                                           // puntos criticos
    int u1, u2;                                           // posiciones iniciales
    int p1;                                               // punto medio del primer sensor
    int p2;                                               // punto de quiebre de la integral del segundo sensor
    double q1;                                            // carga del anillo
    double q2;                                            // carga del plato
    double q;                                             // carga promedio
    double v;                                             // velocidad
    std::vector<double> sensor1, sensor2;                 // Datos de los sensores
    std::vector<double> integralSensor1, integralSensor2; // Integral de los sensores
    std::vector<double> a1, b1;                           // modelos del primer pulso
    std::vector<double> a2;                               // modelo del segundo pulso
    std::vector<double> time;                             // tiempos de cada punto
    bool valid;

    Drop(bool isPositive, int c1, int c2);

    Drop();

    void print() const;

    int size() const;

    void computeStats();

private:
    int findSensor1MiddlePoint();

    int findSensor2TippingPoint();

    void computeIntegral();

    void computeRingCharge();

    void computeDishCharge();

    void computeAverageCharge();

    int satisfiesBasicFilters();

    void computeVelocity();

    void computeModels();
};

#endif