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

El programa está dividido en cinco componentes principales que procesan los datos en secuencia:

### 1. Rellenador de Datos (`drop_filler`)

**Propósito**: Rellena los huecos en los datos de los sensores mediante interpolación.

**Funcionamiento**: 
- Lee los datos originales del archivo de entrada (.lvm)
- Detecta gaps en la serie temporal donde faltan mediciones
- Interpola los valores faltantes usando promedios de ventanas deslizantes
- Genera un archivo `drops_filled.dat` con los datos completos

**Uso manual**:
```bash
./exec/drop_filler archivo_entrada.lvm
```

### 2. Promediador (`drop_average`)

**Propósito**: Aplica un promedio móvil a los datos para suavizar la señal y reducir el ruido.

**Funcionamiento**:
- Lee los datos rellenados desde `drops_filled.dat`
- Aplica normalización con ventana deslizante
- Genera un archivo `drops_average.dat` con los datos suavizados

**Uso manual**:
```bash
./exec/drop_average
```

### 3. Extractor de Gotas (`drop_extractor`)

**Propósito**: Detecta y extrae las gotas individuales de los datos procesados.

**Funcionamiento**:
- Analiza los datos suavizados buscando patrones característicos de gotas
- Utiliza un algoritmo especializado (DropFinder) para identificar eventos de gotas
- Calcula las propiedades de cada gota (carga, diámetro, tiempo, etc.)
- Guarda las gotas detectadas en `drops.dat`

**Uso manual**:
```bash
./exec/drop_extractor
```

### 4. Ordenador de Gotas (`drop_sorter`)

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

### 5. Generador de Gráficos (`drop_chart`)

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
python run.py *.lvm                    # Todos los archivos .lvm
python run.py datos_*.lvm              # Archivos que empiecen con "datos_"
python run.py tormenta_2024_*.lvm      # Archivos de tormentas de 2024
```

### Opciones Avanzadas

**Limpiar y recompilar antes de ejecutar**:
```bash
python run.py *.lvm --clean
```

**Ejecutar desde un paso específico**:
```bash
python run.py *.lvm --from-step 3
```

**Controlar el número de procesos paralelos**:
```bash
python run.py *.lvm --processes 8      # Usar 8 procesos en paralelo
python run.py *.lvm --processes 1      # Procesamiento secuencial
```

Los pasos disponibles son:
- `1`: Ejecutar desde drop_filler
- `2`: Ejecutar desde drop_average  
- `3`: Ejecutar desde drop_extractor
- `4`: Ejecutar desde drop_sorter
- `5`: Ejecutar desde drop_chart

### Procesamiento Paralelo

El script utiliza **multiprocessing** para procesar múltiples archivos simultáneamente:

- **Número de procesos por defecto**: 4
- **Máximo recomendado**: Número de CPUs disponibles en el sistema
- **Ventajas**: Reduce significativamente el tiempo total de procesamiento
- **Limitaciones**: Cada proceso consume memoria y recursos del sistema

**Configuración automática**:
- Si se especifican más procesos que CPUs disponibles, se ajusta automáticamente
- Para archivos únicos, se ejecuta directamente sin overhead de multiprocessing
- Cada proceso trabaja en su propia carpeta independiente

### Comportamiento del Script

1. **Compilación**: Compila automáticamente todos los programas (una sola vez)
2. **Expansión de patrones**: Convierte wildcards en listas de archivos
3. **Validación**: Verifica que los archivos existan
4. **Organización**: Crea una carpeta separada para cada archivo
5. **Ejecución paralela**: Distribuye archivos entre los procesos disponibles
6. **Control de dependencias**: Solo ejecuta un paso si es necesario
7. **Reporte de resultados**: Muestra resumen de éxitos y fallos

### Estructura de Archivos Generados

Después de ejecutar el análisis completo, se genera la siguiente estructura:

```
nombre_archivo/
├── drops_filled.dat      # Datos con huecos rellenados
├── drops_average.dat     # Datos suavizados  
├── drops.dat            # Gotas detectadas
├── drops_sorted.dat     # Gotas ordenadas por calidad
└── graficos/            # Gráficos y análisis estadísticos
    ├── histograma_carga.dat
    ├── histograma_diametro.dat
    ├── carga_vs_tiempo.dat
    ├── diametro_vs_tiempo.dat
    └── ... (otros archivos de análisis)
```

## Parámetros de Configuración

Los parámetros principales del análisis se encuentran en `constants.hpp`:

- `DROP_SIZE`: Tamaño máximo de una gota (400 puntos)
- `WINDOW_SIZE`: Tamaño de ventana para promedio móvil (5000 puntos)
- `DATA_PER_SECOND`: Frecuencia de muestreo (5000 Hz)
- `MINIMUM_THRESHOLD`: Umbral mínimo de detección (0.02)
- `MINIMUM_CHARGE`: Carga mínima requerida (0.2)

## Ejemplo de Uso Completo

```bash
# Análisis de un archivo único
python run.py datos_tormenta_2024.lvm

# Análisis de múltiples archivos específicos
python run.py tormenta1.lvm tormenta2.lvm tormenta3.lvm

# Análisis masivo con wildcards
python run.py datos_*.lvm

# Procesamiento intensivo con 8 procesos paralelos
python run.py *.lvm --processes 8

# Recompilar y analizar desde el paso 2 con 6 procesos
python run.py datos_*.lvm --clean --from-step 2 --processes 6

# Solo generar gráficos para todos los archivos procesados
python run.py *.lvm --from-step 5

# Procesamiento secuencial (un archivo a la vez)
python run.py *.lvm --processes 1
```

### Ejemplos de Salida

**Procesamiento de un archivo único** (output completo):
```
Compilando programas...
Archivos a procesar: 1
  - tormenta_001.lvm
Procesando archivo único...
[12345] Iniciando análisis de tormenta_001.lvm
Creando carpeta tormenta_001
Ejecutando drop_filler
...
[12345] Análisis completado para tormenta_001.lvm
Éxito: tormenta_001.lvm
```

**Procesamiento de múltiples archivos** (output limpio con barra de progreso):
```
Compilando programas...
Archivos a procesar: 5
  - tormenta_001.lvm
  - tormenta_002.lvm
  - tormenta_003.lvm
  - tormenta_004.lvm
  - tormenta_005.lvm
Procesando 5 archivos en paralelo (máximo 4 procesos)...
Nota: Output detallado suprimido durante procesamiento paralelo

Progreso: [████████████████████████████████████████████████████] 5/5 (100.0%)

=== RESUMEN DE PROCESAMIENTO ===
Éxito: tormenta_001.lvm
Éxito: tormenta_002.lvm
Error: tormenta_003.lvm - File not found
Éxito: tormenta_004.lvm
Éxito: tormenta_005.lvm

Total: 5 archivos
Exitosos: 4
Fallidos: 1
```

### Manejo de Output

El script maneja el output de manera inteligente:

- **Archivo único**: Muestra todo el output detallado para facilitar el debugging
- **Múltiples archivos**: Suprime el output detallado para evitar ruido, mostrando:
  - Lista de archivos a procesar
  - Barra de progreso en tiempo real
  - Resumen final de resultados

Esto hace que el procesamiento en paralelo sea mucho más limpio y fácil de seguir, especialmente cuando se procesan muchos archivos simultáneamente.

## Notas Importantes

- Los archivos de entrada deben estar en formato .lvm
- El programa asume una frecuencia de muestreo de 5000 Hz
- Los resultados se organizan automáticamente en carpetas separadas
- Cada componente puede ejecutarse independientemente si es necesario

