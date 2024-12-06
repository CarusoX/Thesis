#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <cmath>

// Tamaño maximo de una gota
constexpr size_t DROP_SIZE = 300;

// Tamaño de la ventana deslizante
constexpr size_t WINDOW_SIZE = 5000;

// Datos por segundo
constexpr size_t DATA_PER_SECOND = 5000;

// Umbral minimo (valor absoluto)
constexpr double MINIMUM_THRESHOLD = 0.02;

// Número máximo de puntos para buscar puntos en el segundo sensor que superen el umbral
// desde el punto critico del primer sensor
constexpr size_t NN = 100;

#endif // CONSTANTS_HPP
