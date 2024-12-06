#ifndef DROP_DATA_HPP
#define DROP_DATA_HPP

#include <iostream>

class Drop
{
public:
    int isPositive; // picos positivos
    int c1, c2;     // puntos criticos
    int u1, u2;     // posiciones iniciales
    int p1;         // punto medio del primer sensor
    int p2;         // punto de quiebre de la integral del segundo sensor

    Drop(bool isPositive, int c1, int c2, int u1 = -1, int u2 = -1, int p1 = -1, int p2 = -1)
        : isPositive(isPositive), c1(c1), c2(c2), u1(u1), u2(u2), p1(p1), p2(p2) {}

    Drop() : c1(-1), c2(-1), u1(-1), u2(-1), p1(-1), p2(-1) {}

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
};

#endif