#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <cmath>

// Tamaño maximo de una gota
constexpr int DROP_SIZE = 300;

// Tamaño de la ventana deslizante
constexpr int WINDOW_SIZE = 5000;

// Datos por segundo
constexpr int DATA_PER_SECOND = 5000;

// Umbral minimo (valor absoluto)
constexpr double MINIMUM_THRESHOLD = 0.02;

// Número máximo de puntos para buscar puntos en el segundo sensor que superen el umbral
// desde el punto critico del primer sensor
constexpr int NN = 100;

// Proporcion entre la carga del anillo y del plato
constexpr double PROP_CHARGE = 0.87;

// Minima carga electrica que debe tener la gota en el plato
constexpr double MINIMUM_CHARGE = 0.2;

// Separacion del anillo y plato (5.7cm)
constexpr double RING_DISH_SEP = 0.057;

// Proporcion entre el ancho entre del anillo y la olla
constexpr double PROP_WIDTH = 0.79;

#endif // CONSTANTS_HPP
