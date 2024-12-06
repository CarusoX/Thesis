#ifndef DROP_DATA_HPP
#define DROP_DATA_HPP

#include <iostream>

class Drop {
public:

    int u1, u2; // posiciones iniciales
    int c1, c2; // puntos criticos
    int p1; // punto medio del primer sensor
    int p2; // punto de quiebre de la integral del segundo sensor

    Drop(int u1, int u2, int c1, int c2, int p1, int p2)
        : u1(u1), u2(u2), c1(c1), c2(c2), p1(p1), p2(p2) {}

    Drop() : u1(-1), u2(-1), c1(-1), c2(-1), p1(-1), p2(-1) {}

    void print() const {
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