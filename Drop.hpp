#ifndef DROP_DATA_HPP
#define DROP_DATA_HPP

#include <iostream>

class Drop
{
public:
    bool isPositive;                      // picos positivos
    int c1, c2;                           // puntos criticos
    int u1, u2;                           // posiciones iniciales
    int p1;                               // punto medio del primer sensor
    int p2;                               // punto de quiebre de la integral del segundo sensor
    int q1;                               // carga del anillo
    int q2;                               // carga del plato
    int q;                                // carga promedio
    std::vector<double> sensor1, sensor2; // Datos de los sensores
    bool valid;

    Drop(bool isPositive, int c1, int c2)
        : isPositive(isPositive), c1(c1), c2(c2), valid(1) {}

    Drop() : c1(-1), c2(-1), valid(0) {}

    void print() const
    {
        std::cout << "Drop: " << std::endl;
        std::cout << "  u1: " << u1 << std::endl;
        std::cout << "  u2: " << u2 << std::endl;
        std::cout << "  c1: " << c1 << std::endl;
        std::cout << "  c2: " << c2 << std::endl;
        std::cout << "  p1: " << p1 << std::endl;
        std::cout << "  p2: " << p2 << std::endl;
    }

    int size() const
    {
        return sensor1.size();
    }
};

#endif