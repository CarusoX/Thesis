#pragma once
#include "lib.hpp"

// Tamaño maximo de una gota
constexpr int DROP_SIZE = 300;

// Tamaño de la ventana deslizante para el promedio
constexpr int WINDOW_SIZE = 5000;

// Datos por segundo
constexpr int DATA_PER_SECOND = 5000;

// Umbral minimo (valor absoluto)
constexpr double MINIMUM_THRESHOLD = 0.02;

// Número máximo de puntos para buscar puntos en el segundo sensor que superen
// el umbral desde el punto critico del primer sensor
constexpr int NN = 100;

// Proporcion entre la carga del anillo y del plato
constexpr double PROP_CHARGE = 0.87;

// Minima carga electrica que debe tener la gota en el plato
constexpr double MINIMUM_CHARGE = 0.2;

// Separacion del anillo y plato (5.7cm)
constexpr double RING_DISH_SEP = 0.057;

// Proporcion entre el ancho entre del anillo y la olla
constexpr double PROP_WIDTH = 0.79;

// Factor de integracion
constexpr double INTEGRATION_FACTOR = 2000;

// Diametros
constexpr double MIN_DIAMETER = 0.175;
constexpr double MAX_DIAMETER = 5.81;

// Histograma de carga
constexpr int HISTOGRAM_CHARGE_MIN = -101;
constexpr int HISTOGRAM_CHARGE_MAX = 101;

// Histograma de diametro
constexpr int HISTOGRAM_DIAMETER_MAX = 10;
constexpr int HISTOGRAM_DIAMETER_PRECISION = 2;
