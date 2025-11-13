# Programa de Análisis de Gotas

Este programa procesa datos de sensores para detectar y analizar gotas en la señal. Está diseñado para procesar datos de experimentos de gotas cargadas eléctricamente, donde se miden las señales de dos sensores (anillo y plato) para determinar las propiedades físicas de las gotas.

## Requisitos del Sistema

- C++17 o superior
- CMake 3.10 o superior  
- Un compilador de C++ (gcc, clang, o MSVC)
- Python 3.x (para el script de automatización)

## Compilación del Programa

Para compilar todos los componentes:

```bash
make
```

Para limpiar archivos compilados y recompilar:

```bash
make clean
make
```

## Componentes del Programa

El programa está dividido en tres componentes principales que procesan los datos en secuencia:

### 1. Detector de Gotas (`drop_finder`)

**Propósito**: Lee los datos originales, rellena huecos, normaliza la señal y detecta las gotas.

**Funcionamiento**:
- Lee los datos del archivo de entrada (.lvm)
- Interpola valores faltantes y corrige el offset de la señal
- Identifica las gotas presentes en la señal
- Guarda las gotas detectadas en `drops.dat` (incluye una columna `step` con la posición de cada muestra)

**Uso manual**:
```bash
./exec/drop_finder archivo_entrada.lvm
```

### 2. Ordenador de Gotas (`drop_sorter`)

**Propósito**: Ordena las gotas detectadas por su calidad/confiabilidad.

**Funcionamiento**:
- Lee las gotas desde `drops.dat`
- Calcula una métrica de penalización para cada gota
- Ordena las gotas de mejor a peor calidad
- Genera `drops_sorted.dat` con las gotas ordenadas

**Uso manual**:
```bash
./exec/drop_sorter
```

### 3. Generador de Gráficos (`drop_chart`)

**Propósito**: Genera análisis estadísticos y gráficos de las gotas procesadas.

**Funcionamiento**:
- Crea histogramas de carga y diámetro
- Genera gráficos de carga vs. tiempo y diámetro vs. tiempo
- Separa análisis por gotas positivas y negativas
- Calcula promedios y estadísticas
- Guarda todos los archivos en la carpeta `graficos/`

**Uso manual**:
```bash
./exec/drop_chart
```

## Script de Automatización (`run.py`)

El script `run.py` automatiza todo el proceso de análisis, ejecutando los componentes en secuencia y organizando los resultados. **Soporta procesamiento paralelo de múltiples tormentas simultáneamente.**

### Uso Básico

**Procesar un archivo único**:
```bash
python run.py archivo_tormenta.lvm
```

**Procesar múltiples archivos**:
```bash
python run.py archivo1.lvm archivo2.lvm archivo3.lvm
```

**Procesar archivos usando patrones (wildcards)**:
```bash
python run.py archivo_tormenta.lvm     # Procesar un archivo de tormenta
python run.py *.lvm                    # Todos los archivos .lvm
python run.py datos_*.lvm              # Archivos que empiecen con "datos_"
python run.py tormenta_2024_*.lvm      # Archivos de tormentas de 2024
```

### Estructura de Archivos Generados

Después de ejecutar el análisis completo, se genera la siguiente estructura en la misma carpeta que `run.py`:

```
nombre_tormenta/
├── drops.dat            # Gotas detectadas
├── drops_sorted.dat     # Gotas ordenadas por calidad
├── carga_velocidad.dat  # Resumen (step, q1, q2, q, v, diam, penalidad)
└── graficos/            # Gráficos y análisis estadísticos
    ├── histograma_carga.dat
    ├── histograma_diametro.dat
    ├── carga_vs_tiempo.dat
    ├── diametro_vs_tiempo.dat
    └── ... (otros archivos de análisis)
```

El archivo drops.dat contiene las gotas una detras de la otra, identificadas por su `id`. Las columnas son:
  - time --> Tiempo en segundos del dato parti
  - step --> Tiempo convertido a pasos (1 paso ~ 1/5000 segundos)
  - sensor1 --> Señal del sensor 1
  - sensor2 --> Señal del sensor 2
  - integral1 --> Integral de la señal del sensor 1
  - integral2 --> Integral de la señal del sensor 2
  - a1 --> Modelo teorico 1 para la integral del sensor 1
  - a2 --> Modelo teorico 1 para la integral del sensor 2
  - b1 --> Modelo teorico 2 para la integral del sensor 1
  - q1 --> Carga del sensor 1
  - q2 --> Carga del sensor 2
  - v --> Velocidad de la gota
  - diameter --> Diametro de la gota
  - sum_sq1 --> Suma de los cuadrados de las diferencias entre el modelo teorico a1 y la integral del sensor 1
  - sum_sq2 --> Suma de los cuadrados de las diferencias entre el modelo teorico a2 y la integral del sensor 2
  - charge_penalty --> Penalidad de la carga
  - width_penalty --> Penalidad de la anchura
  - noise_penalty --> Penalidad de ruido
  - penalty --> Penalidad total de la gota
  - id --> ID de la gota (para identificar datos de la misma gota)

El archivo drops_sorted.dat contiene las gotas ordenadas por su penalidad total. Las columnas son:
 - time --> Este tiempo se ajusta para que queden ordenadas, no representa nada fisico.
 - step --> TIEMPO ORIGINAL convertido a pasos. Este si representa el tiempo fisico.
 - sensor1 --> Señal del sensor 1
 - sensor2 --> Señal del sensor 2
 - integral1 --> Integral de la señal del sensor 1
 - integral2 --> Integral de la señal del sensor 2
 - a1 --> Modelo teorico 1 para la integral del sensor 1
 - a2 --> Modelo teorico 1 para la integral del sensor 2
 - b1 --> Modelo teorico 2 para la integral del sensor 1
 - q1 --> Carga del sensor 1
 - q2 --> Carga del sensor 2
 - v --> Velocidad de la gota
 - diameter --> Diametro de la gota
 - penalty --> Penalidad total de la gota
 - id --> ID de la gota (es el mismo que en drops.dat)

El archivo carga_velocidad.dat contiene los siguientes datos:

  - La primera fila contiene el total de gotas, el total de steps, el primer step de la primera gota y el primer step de la ultima gota.
  - Las siguientes filas contienen los datos de cada gota en el siguiente orden: step, q1, q2, q, v, diameter, penalty.


### Opciones Avanzadas

**No limpiar ni recompilar antes de ejecutar**:
```bash
python run.py *.lvm --no-clean
```

**Ejecutar desde un paso específico**:
```bash
python run.py *.lvm --from-step 3
python run.py *.lvm --from-step 4   # Generar carga_velocidad.dat (Fortran)
```

Los pasos disponibles son:
- `1`: Ejecutar desde drop_finder
- `2`: Ejecutar desde drop_sorter
- `3`: Ejecutar desde drop_chart
- `4`: Generar carga_velocidad.dat (Fortran)

**Controlar el número de procesos paralelos**:
```bash
python run.py *.lvm --processes 8      # Usar 8 procesos en paralelo
python run.py *.lvm --processes 1      # Procesamiento secuencial
```

**Cambiar el charge multiplier en carga_velocidad.f90**:
El charge multiplier es el factor de multiplicacion de la carga. Generalmente es 1.0 o 5.0. Se puede cambiar directamente en el archivo `carga_velocidad.f90` antes de correr el programa.

El paso 4 ejecuta un programa Fortran que lee `drops.dat`, toma la primera fila de cada gota y genera `carga_velocidad.dat` con las columnas `step q1 q2 q v diam sum_of_penalties`.


## Notas Importantes

- Los archivos de entrada deben estar en formato .lvm
- El programa asume una frecuencia de muestreo de 5000 Hz
- Los resultados se organizan automáticamente en carpetas separadas
- Cada componente puede ejecutarse independientemente si es necesario

