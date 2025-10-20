import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as mpatches

def grafico_complejidades():
    """
    Genera un gráfico con las complejidades algorítmicas mencionadas en la tesis.
    Incluye: O(1), O(log n), O(n), O(n log n), O(n²), O(2^n)
    """
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Definir el rango de n
    n = np.linspace(1, 1000, 10000)
    
    # Definir las funciones de complejidad
    o1 = np.ones_like(n)  # O(1) - Constante
    o_log_n = np.log2(n)  # O(log n) - Logarítmica
    o_n = n  # O(n) - Lineal
    o_n_log_n = n * np.log2(n)  # O(n log n) - Lineal logarítmica
    o_n2 = n**2  # O(n²) - Cuadrática
    o_2n = 2**n  # O(2^n) - Exponencial
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Graficar las funciones
    ax.plot(n, o1, 'b-', linewidth=2.5, label='$O(1)$ - Constante')
    ax.plot(n, o_log_n, 'g-', linewidth=2.5, label='$O(\log n)$ - Logarítmica')
    ax.plot(n, o_n, 'orange', linewidth=2.5, label='$O(n)$ - Lineal')
    ax.plot(n, o_n_log_n, 'r-', linewidth=2.5, label='$O(n \log n)$ - Lineal logarítmica')
    ax.plot(n, o_n2, 'purple', linewidth=2.5, label='$O(n^2)$ - Cuadrática')
    ax.plot(n, o_2n, 'brown', linewidth=2.5, label='$O(2^n)$ - Exponencial')
    
    # Configurar el gráfico
    ax.set_xlabel('Tamaño de entrada (n)', fontweight='bold')
    ax.set_ylabel('Número de operaciones', fontweight='bold')
    ax.set_title('Complejidades Algorítmicas', fontweight='bold', pad=20)
    
    # Configurar los ejes
    ax.set_xlim(1, 1000)
    ax.set_yscale('log')
    ax.set_ylim(0.9, 10000)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper left', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/complejidades_algoritmicas.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de complejidades algorítmicas guardado en: escrito/figures/complejidades_algoritmicas.png")

def grafico_sensores_gota():
    """
    Genera un gráfico de los sensores 1 y 2 del archivo gota.dat
    con líneas horizontales que cruzan los picos de cada sensor.
    """
    # Leer los datos del archivo
    data = np.loadtxt('/Users/uzielluduena/Thesis/new_def/cooking/gota.dat', 
                      skiprows=1, delimiter='\t')
    
    # Extraer las columnas relevantes
    time = data[:, 0]
    sensor1 = data[:, 2]
    sensor2 = data[:, 3]
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Graficar los sensores
    ax.plot(time, sensor1, 'b-', markersize=3, label='Sensor 1', alpha=0.8)
    ax.plot(time, sensor2, 'r-', markersize=3, label='Sensor 2', alpha=0.8)
    
    # Encontrar el máximo entre pares consecutivos
    max_consecutive_sensor1 = np.max(np.minimum(sensor1[:-1], sensor1[1:]))
    max_consecutive_sensor2 = np.max(np.minimum(sensor2[:-1], sensor2[1:]))
    
    # Dibujar líneas horizontales en los picos (máximo entre pares consecutivos)
    ax.axhline(y=max_consecutive_sensor1, color='blue', linestyle='--', linewidth=2, 
               alpha=0.7, label=f'Max Consecutivo Sensor 1: {max_consecutive_sensor1:.4f}')
    ax.axhline(y=max_consecutive_sensor2, color='red', linestyle='--', linewidth=2, 
               alpha=0.7, label=f'Max Consecutivo Sensor 2: {max_consecutive_sensor2:.4f}')
    
    # Agregar líneas de umbrales (basado en la función del código Fortran)
    c1 = 2.00  # umbral superior
    c2 = 0.02  # umbral inferior  
    c3 = 5.0   # exponencial
    ps = 1000  # pasos
    
    # Calcular algunos umbrales intermedios
    k_values = [25, 50, 100, ps//4, ps//2, 3*ps//4, ps]
    umbral_color = 'gray'
    
    for i, k in enumerate(k_values):
        umbral = ((1.0 - 1.0/k)**c3 * (c2 - c1)) / ((1.0 - 1.0/ps)**c3) + c1
        if k == 100:  # Destacar el umbral que encuentra el pulso
            ax.axhline(y=umbral, color='orange', linestyle='-', linewidth=3, 
                       alpha=0.8, label=f'Umbral k={k} (encuentra pulso): {umbral:.3f}')
        else:
            ax.axhline(y=umbral, color=umbral_color, linestyle=':', linewidth=1, 
                       alpha=0.6, label=f'Umbral k={k}: {umbral:.3f}')
    
    # Encontrar las posiciones de los máximos para mostrar la diferencia en puntos
    max_idx_sensor1 = np.argmax(sensor1)
    max_idx_sensor2 = np.argmax(sensor2)
    max_time_sensor1 = time[max_idx_sensor1]
    max_time_sensor2 = time[max_idx_sensor2]
    points_diff = abs(max_idx_sensor2 - max_idx_sensor1)
    
    # Agregar anotación de la diferencia en puntos
    mid_time = (max_time_sensor1 + max_time_sensor2) / 2
    # ax.annotate(f'Δpuntos = {points_diff}', 
    #             xy=(max_time_sensor1, max(max_consecutive_sensor1, max_consecutive_sensor2) * 0.8),
    #             xytext=(max_time_sensor2, min(max_consecutive_sensor1, max_consecutive_sensor2) * 1.1),
    #             arrowprops=dict(arrowstyle='<->', color='black', shrinkA=0, shrinkB=0),
    #             fontsize=12, ha='center', fontweight='bold',
    #             bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7))
    
    # Configurar el gráfico
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud', fontweight='bold')
    ax.set_title('Detección de Pulsos por umbrales', fontweight='bold', pad=20)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/sensores_gota.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print(f"Gráfico de sensores guardado en: escrito/figures/sensores_gota.png")
    print(f"Max Consecutivo Sensor 1: {max_consecutive_sensor1:.4f}")
    print(f"Max Consecutivo Sensor 2: {max_consecutive_sensor2:.4f}")
    print(f"Diferencia en puntos entre máximos: {points_diff}")
    print(f"Índice máximo Sensor 1: {max_idx_sensor1}")
    print(f"Índice máximo Sensor 2: {max_idx_sensor2}")
    print(f"Tiempo máximo Sensor 1: {max_time_sensor1:.6f}s")
    print(f"Tiempo máximo Sensor 2: {max_time_sensor2:.6f}s")

def grafico_gota_analisis():
    """
    Genera un gráfico de la gota con líneas verticales para análisis detallado:
    - c1: máximo consecutivo sensor 1
    - c2: máximo consecutivo sensor 2  
    - u1: inicio del pulso (primer punto donde sensor1 > 0)
    - u2: primer punto donde sensor2 no es < 0
    - p1: punto medio de la señal 1 (ajustar manualmente)
    - p2: quiebre de la integral (ajustar manualmente)
    """
    # Leer los datos del archivo
    data = np.loadtxt('/Users/uzielluduena/Thesis/new_def/cooking/gota.dat', 
                      skiprows=1, delimiter='\t')
    
    # Extraer las columnas relevantes
    time = data[:, 0]
    sensor1 = data[:, 2]
    sensor2 = data[:, 3]
    integral_sensor1 = data[:, 4]
    integral_sensor2 = data[:, 5]
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Graficar los sensores
    ax.plot(time, sensor1, 'b-', linewidth=2, label='Sensor 1', alpha=0.8)
    ax.plot(time, sensor2, 'r-', linewidth=2, label='Sensor 2', alpha=0.8)
    
    # Calcular los puntos de análisis
    # c1: máximo consecutivo sensor 1
    max_consecutive_sensor1 = np.max(np.minimum(sensor1[:-1], sensor1[1:]))
    c1_idx = np.argmax(np.minimum(sensor1[:-1], sensor1[1:]))
    c1_time = time[c1_idx]
    
    # c2: máximo consecutivo sensor 2
    max_consecutive_sensor2 = np.max(np.minimum(sensor2[:-1], sensor2[1:]))
    c2_idx = np.argmax(np.minimum(sensor2[:-1], sensor2[1:]))
    c2_time = time[c2_idx]
    
    u1_time = time[0]
    
    # u2: primer punto donde sensor2 no es < 0 (sensor2 >= 0), buscando hacia atrás desde c2
    u2_idx = np.where(sensor2[:c2_idx] < 0)[0]
    if len(u2_idx) > 0:
        u2_time = time[u2_idx[-1] + 1]  # tomamos el último punto antes de c2
    else:
        u2_time = time[0]  # fallback
    
    # p1: punto medio de la señal 1 (ajustar manualmente - usando el máximo por ahora)
    p1_idx = np.argmax(sensor1)
    p1_time = time[31]
    
    # p2: quiebre de la integral (ajustar manualmente - usando cambio máximo en integral por ahora)
    integral_diff = np.diff(integral_sensor1)
    p2_idx = np.argmax(np.abs(integral_diff)) + 1  # +1 porque diff reduce el tamaño
    p2_time = time[67]
    
    # Dibujar líneas verticales
    ax.axvline(x=c1_time, color='blue', linestyle='--', linewidth=2, 
               alpha=0.8, label=f'c1 (max consecutivo sensor1): {c1_time:.6f}s')
    ax.axvline(x=c2_time, color='red', linestyle='--', linewidth=2, 
               alpha=0.8, label=f'c2 (max consecutivo sensor2): {c2_time:.6f}s')
    ax.axvline(x=u1_time, color='green', linestyle=':', linewidth=2, 
               alpha=0.8, label=f'u1 (inicio pulso sensor1): {u1_time:.6f}s')
    ax.axvline(x=u2_time, color='orange', linestyle=':', linewidth=2, 
               alpha=0.8, label=f'u2 (sensor2 >= 0): {u2_time:.6f}s')
    ax.axvline(x=p1_time, color='purple', linestyle='-', linewidth=2, 
               alpha=0.8, label=f'p1 (punto medio señal1): {p1_time:.6f}s')
    ax.axvline(x=p2_time, color='brown', linestyle='-', linewidth=2, 
               alpha=0.8, label=f'p2 (quiebre integral): {p2_time:.6f}s')
    
    # Configurar el gráfico
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud', fontweight='bold')
    ax.set_title('Análisis Detallado de Gota - Puntos de Referencia', fontweight='bold', pad=20)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/analisis_gota.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print(f"Gráfico de análisis guardado en: escrito/figures/analisis_gota.png")
    print(f"c1 (max consecutivo sensor1): {c1_time:.6f}s")
    print(f"c2 (max consecutivo sensor2): {c2_time:.6f}s")
    print(f"u1 (inicio pulso sensor1): {u1_time:.6f}s")
    print(f"u2 (sensor2 >= 0): {u2_time:.6f}s")
    print(f"p1 (punto medio señal1): {p1_time:.6f}s")
    print(f"p2 (quiebre integral): {p2_time:.6f}s")

def grafico_calculo_carga():
    """
    Genera un gráfico mostrando el cálculo de carga eléctrica mediante integración
    de las señales del anillo y placa.
    """
    # Leer los datos del archivo
    data = np.loadtxt('/Users/uzielluduena/Thesis/new_def/cooking/gota.dat', 
                      skiprows=1, delimiter='\t')
    
    # Extraer las columnas relevantes
    time = data[:, 0]
    sensor1 = data[:, 2]
    sensor2 = data[:, 3]
    integral_sensor1 = data[:, 4]
    integral_sensor2 = data[:, 5]
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))
    
    # Gráfico superior: Señales originales
    ax1.plot(time, sensor1, 'b-', linewidth=2, label='Señal Anillo (sensor1)', alpha=0.8)
    ax1.plot(time, sensor2, 'r-', linewidth=2, label='Señal Placa (sensor2)', alpha=0.8)
    ax1.set_ylabel('Amplitud (V)', fontweight='bold')
    ax1.set_title('Señales Originales de los Sensores', fontweight='bold', pad=20)
    ax1.grid(True, alpha=0.3, linestyle='--')
    ax1.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Gráfico inferior: Integrales (carga eléctrica)
    ax2.plot(time, integral_sensor1, 'b-', linewidth=2, label='Carga Anillo (∫sensor1)', alpha=0.8)
    ax2.plot(time, integral_sensor2, 'r-', linewidth=2, label='Carga Placa (∫sensor2)', alpha=0.8)
    ax2.set_xlabel('Tiempo (s)', fontweight='bold')
    ax2.set_ylabel('Carga Eléctrica (pC)', fontweight='bold')
    ax2.set_title('Cálculo de Carga Eléctrica mediante Integración', fontweight='bold', pad=20)
    ax2.grid(True, alpha=0.3, linestyle='--')
    ax2.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Marcar p1 y p2
    p1_time = time[31]
    p2_time = time[67]
    
    # Marcar líneas verticales para p1 y p2
    ax2.axvline(x=p1_time, color='purple', linestyle='-', linewidth=2,
                alpha=0.8, label=f'p1 (punto medio señal1): {p1_time:.6f}s')
    ax2.axvline(x=p2_time, color='brown', linestyle='-', linewidth=2,
                alpha=0.8, label=f'p2 (quiebre integral): {p2_time:.6f}s')
    
    # Agregar anotaciones de los valores de carga
    q1 = integral_sensor1[31] * 2/5
    q2 = integral_sensor2[67] * 2/5
    
    ax2.annotate(f'q1 = {q1:.2f} pC\n(∫sensor1 × 2/5)', 
                xy=(p1_time, integral_sensor1[31]),
                xytext=(10, 10), textcoords='offset points',
                bbox=dict(boxstyle="round,pad=0.3", facecolor="lightblue", alpha=0.7))
                
    ax2.annotate(f'q2 = {q2:.2f} pC\n(∫sensor2 × 2/5)', 
                xy=(p2_time, integral_sensor2[67]),
                xytext=(10, 30), textcoords='offset points',
                bbox=dict(boxstyle="round,pad=0.3", facecolor="lightcoral", alpha=0.7))
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/calculo_carga_electrica.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de cálculo de carga eléctrica guardado en: escrito/figures/calculo_carga_electrica.png")

def grafico_calculo_velocidad():
    """
    Genera un gráfico mostrando el cálculo de velocidad de caída usando la separación
    entre anillo y placa y el tiempo entre picos.
    """
    # Leer los datos del archivo
    data = np.loadtxt('/Users/uzielluduena/Thesis/new_def/cooking/gota.dat', 
                      skiprows=1, delimiter='\t')
    
    # Extraer las columnas relevantes
    time = data[:, 0]
    sensor1 = data[:, 2]
    sensor2 = data[:, 3]

    p1 = 31
    p2 = 67
    
    # Encontrar los picos de cada sensor
    max_idx_sensor1 = np.argmax(sensor1)
    max_idx_sensor2 = np.argmax(sensor2)
    p1_time = time[p1]
    p2_time = time[p2]
    
    # Calcular la velocidad
    separacion = 0.057  # 5.7 cm en metros
    tiempo_vuelo = abs(p2_time - p1_time)
    velocidad = separacion / tiempo_vuelo if tiempo_vuelo > 0 else 0
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Graficar los sensores
    ax.plot(time, sensor1, 'b-', linewidth=2, label='Anillo (sensor1)', alpha=0.8)
    ax.plot(time, sensor2, 'r-', linewidth=2, label='Placa (sensor2)', alpha=0.8)
    
    # Marcar los picos
    ax.axvline(x=p1_time, color='blue', linestyle='--', linewidth=2, 
               alpha=0.7, label=f'Pico Anillo: {p1_time:.6f}s')
    ax.axvline(x=p2_time, color='red', linestyle='--', linewidth=2, 
               alpha=0.7, label=f'Pico Placa: {p2_time:.6f}s')
    
    # Agregar anotación de la velocidad
    mid_time = (p1_time + p2_time) / 2
    ax.annotate(f'Velocidad = d/Δt\n= {separacion:.3f}m / {tiempo_vuelo:.6f}s\n= {velocidad:.2f} m/s', 
                xy=(mid_time, max(np.max(sensor1), np.max(sensor2)) * 0.8),
                xytext=(mid_time, max(np.max(sensor1), np.max(sensor2)) * 0.9),
                arrowprops=dict(arrowstyle='<->', color='black', lw=3, 
                               mutation_scale=20, shrinkA=0, shrinkB=0),
                fontsize=12, ha='center', fontweight='bold',
                bbox=dict(boxstyle="round,pad=0.4", facecolor="yellow", alpha=0.8))
    
    # Agregar diagrama esquemático
    # ax.annotate('', xy=(0.02, 0.3), xycoords='axes fraction',
    #             xytext=(0.02, 0.7), textcoords='axes fraction',
    #             arrowprops=dict(arrowstyle='<->', color='green', lw=3))
    # ax.text(0.05, 0.5, f'd = {separacion:.3f}m\n(5.7 cm)', 
    #         transform=ax.transAxes, fontsize=10, 
    #         bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgreen", alpha=0.7),
    #         verticalalignment='center')
    
    # Configurar el gráfico
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud (V)', fontweight='bold')
    ax.set_title('Cálculo de Velocidad de Caída usando Separación entre Anillo y Placa', 
                fontweight='bold', pad=20)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/calculo_velocidad_caida.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print(f"Gráfico de cálculo de velocidad guardado en: escrito/figures/calculo_velocidad_caida.png")
    print(f"Velocidad calculada: {velocidad:.2f} m/s")

def grafico_relacion_velocidad_diametro():
    """
    Genera un gráfico mostrando la relación velocidad-diámetro para gotas de lluvia.
    """
    # Leer datos del archivo diameter.hpp
    velocidades = []
    diametros = []
    with open('../diameter.hpp', 'r') as f:
        for line in f:
            if '{' in line and '}' in line:
                # Extraer valores numéricos entre llaves
                nums = line.strip().strip('{},').split(',')
                if len(nums) == 2:
                    velocidades.append(float(nums[0]))
                    diametros.append(float(nums[1]))
    
    velocidades = np.array(velocidades)
    diametros = np.array(diametros)
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Graficar la relación
    ax.plot(velocidades, diametros, 'b-', linewidth=1, 
            label='Relación Velocidad-Diámetro', alpha=0.8)
    
    # Agregar interpolación suave
    # velocidades_smooth = np.linspace(min(velocidades), max(velocidades), 100)
    # diametros_smooth = np.interp(velocidades_smooth, velocidades, diametros)
    # ax.plot(velocidades_smooth, diametros_smooth, 'b--', linewidth=1, alpha=0.6)
    
    # Configurar el gráfico
    ax.set_xlabel('Velocidad Terminal (m/s)', fontweight='bold')
    ax.set_ylabel('Diámetro (mm)', fontweight='bold')
    ax.set_title('Relación Velocidad-Diámetro para Gotas de Lluvia', fontweight='bold', pad=20)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.legend(loc='lower right', frameon=True, fancybox=True, shadow=True)
    
    # Agregar anotaciones
    ax.annotate('Velocidad baja:\nGotas pequeñas', 
                xy=(min(velocidades)+0.1, min(diametros)+0.1), 
                xytext=(min(velocidades)+0.5, min(diametros)+0.5),
                arrowprops=dict(arrowstyle='->', color='red', alpha=0.7),
                fontsize=10, color='red', fontweight='bold',
                bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
    
    ax.annotate('Velocidad alta:\nGotas grandes', 
                xy=(max(velocidades)-0.1, max(diametros)-0.1),
                xytext=(max(velocidades)-1, max(diametros)-0.5),
                arrowprops=dict(arrowstyle='->', color='red', alpha=0.7),
                fontsize=10, color='red', fontweight='bold',
                bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/relacion_velocidad_diametro.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de relación velocidad-diámetro guardado en: escrito/figures/relacion_velocidad_diametro.png")

def grafico_comparacion_modelos():
    """
    Genera un gráfico comparando las señales reales (integrales) con los modelos teóricos
    para los pulsos del anillo y placa.
    """
    # Leer los datos del archivo
    data = np.loadtxt('/Users/uzielluduena/Thesis/new_def/plotter/gota.dat', 
                      skiprows=1, delimiter='\t')
    
    # Extraer las columnas relevantes
    time = data[:, 0]
    integral_sensor1 = data[:, 4]  # Integral real del sensor 1 (anillo)
    integral_sensor2 = data[:, 5]  # Integral real del sensor 2 (placa)
    a1 = data[:, 6]  # Modelo teórico del sensor 1
    a2 = data[:, 7]  # Modelo teórico del sensor 2
    # b1 = data[:, 8]  # Modelo teórico adicional del sensor 1
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))
    
    # Gráfico superior: Comparación Anillo (sensor1)
    ax1.plot(time, integral_sensor1, 'b-', linewidth=2, label='Integral Real (Anillo)', alpha=0.8)
    ax1.plot(time, a1, 'r--', linewidth=2, label='Modelo Teórico a1', alpha=0.8)
    # ax1.plot(time, b1, 'g--', linewidth=2, label='Modelo Teórico b1', alpha=0.8)
    ax1.set_ylabel('Carga Eléctrica (pC)', fontweight='bold')
    ax1.set_title('Comparación: Anillo - Señales Reales vs Modelos Teóricos', fontweight='bold', pad=20)
    ax1.grid(True, alpha=0.3, linestyle='--')
    ax1.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Gráfico inferior: Comparación Placa (sensor2)
    ax2.plot(time, integral_sensor2, 'b-', linewidth=2, label='Integral Real (Placa)', alpha=0.8)
    ax2.plot(time, a2, 'r--', linewidth=2, label='Modelo Teórico a2', alpha=0.8)
    ax2.set_xlabel('Tiempo (s)', fontweight='bold')
    ax2.set_ylabel('Carga Eléctrica (pC)', fontweight='bold')
    ax2.set_title('Comparación: Placa - Señales Reales vs Modelos Teóricos', fontweight='bold', pad=20)
    ax2.grid(True, alpha=0.3, linestyle='--')
    ax2.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Calcular y mostrar métricas de ajuste
    # Error cuadrático medio para cada modelo
    mse_a1 = np.mean((integral_sensor1 - a1)**2)
    # mse_b1 = np.mean((integral_sensor1 - b1)**2)
    mse_a2 = np.mean((integral_sensor2 - a2)**2)
    
    # Coeficiente de correlación
    corr_a1 = np.corrcoef(integral_sensor1, a1)[0, 1]
    # corr_b1 = np.corrcoef(integral_sensor1, b1)[0, 1]
    corr_a2 = np.corrcoef(integral_sensor2, a2)[0, 1]
    
    # Agregar anotaciones con métricas de ajuste
    # ax1.annotate(f'MSE a1: {mse_a1:.4f}\nCorr a1: {corr_a1:.3f}\nMSE b1: {mse_b1:.4f}\nCorr b1: {corr_b1:.3f}', 
    #             xy=(0.02, 0.95), xycoords='axes fraction',
    #             bbox=dict(boxstyle="round,pad=0.3", facecolor="lightblue", alpha=0.7),
    #             fontsize=10, ha='left')
    
    # ax2.annotate(f'MSE a2: {mse_a2:.4f}\nCorr a2: {corr_a2:.3f}', 
    #             xy=(0.02, 0.95), xycoords='axes fraction',
    #             bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgreen", alpha=0.7),
    #             fontsize=10, ha='left')
    
    # Agregar anotación explicativa
    # ax1.annotate('Los modelos teóricos (a1, b1, a2) representan\nla idealización de las señales de carga\nbasada en principios físicos', 
    #             xy=(0.98, 0.05), xycoords='axes fraction',
    #             bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7),
    #             fontsize=10, ha='right')
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/comparacion_modelos_teoricos.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de comparación de modelos teóricos guardado en: escrito/figures/comparacion_modelos_teoricos.png")
    print(f"Métricas de ajuste:")
    print(f"  Anillo - MSE a1: {mse_a1:.4f}, Correlación a1: {corr_a1:.3f}")
    print(f"  Anillo - MSE b1: {mse_b1:.4f}, Correlación b1: {corr_b1:.3f}")
    print(f"  Placa - MSE a2: {mse_a2:.4f}, Correlación a2: {corr_a2:.3f}")

def grafico_sliding_window():
    """
    Genera un gráfico que muestra el algoritmo de sliding window con 7 elementos
    y 3 iteraciones de ventana de tamaño 3, usando cuadrados con números.
    """
    # Generar array de 7 elementos aleatorios
    np.random.seed(42)  # Para reproducibilidad
    array = np.random.randint(1, 10, 7)
    window_size = 3
    
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura con subplots
    fig, axes = plt.subplots(3, 1, figsize=(16, 10))
    
    # Colores para las iteraciones
    window_color = '#2E8B57'  # Verde para elementos en ventana
    outside_color = '#D3D3D3'  # Gris claro para elementos fuera de ventana
    text_color = 'white' if window_color == '#2E8B57' else 'black'
    
    # Calcular las sumas de las ventanas
    sums = []
    for i in range(3):
        start_idx = i
        end_idx = i + window_size
        window_sum = np.sum(array[start_idx:end_idx])
        sums.append(window_sum)
    
    # Función para dibujar cuadrados
    def draw_squares(ax, array, window_start, window_end, iteration_num, sum_value):
        # Posiciones de los cuadrados
        x_positions = np.arange(len(array))
        
        for i, val in enumerate(array):
            # Determinar color según si está en la ventana
            if window_start <= i < window_end:
                color = window_color
                text_color_current = 'white'
            else:
                color = outside_color
                text_color_current = 'black'
            
            # Dibujar cuadrado
            square = plt.Rectangle((i - 0.4, -0.4), 0.8, 0.8, 
                                 facecolor=color, edgecolor='black', linewidth=2)
            ax.add_patch(square)
            
            # Agregar número en el centro
            ax.text(i, 0, str(val), ha='center', va='center', 
                   fontweight='bold', fontsize=16, color=text_color_current)
        
        # Configurar el gráfico
        ax.set_xlim(-0.8, 6.8)
        ax.set_ylim(-0.8, 0.8)
        ax.set_aspect('equal')
        ax.axis('off')
        
        # Agregar título con suma
        ax.set_title(f'Iteración {iteration_num}: Ventana [{window_start}:{window_end}] - Suma = {sum_value}', 
                    fontweight='bold', pad=20)
        
        # Agregar anotación del cálculo para iteraciones 2 y 3
        if iteration_num == 2:
            calc_text = f'Cálculo: {sums[0]} - {array[0]} + {array[3]} = {sum_value}'
        elif iteration_num == 3:
            calc_text = f'Cálculo: {sums[1]} - {array[1]} + {array[4]} = {sum_value}'
        else:
            calc_text = f'Suma inicial: {array[0]} + {array[1]} + {array[2]} = {sum_value}'
        
        ax.text(3, -0.6, calc_text, ha='center', va='center', 
               fontsize=12, fontweight='bold',
               bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7))
    
    # Iteración 1: Ventana [0:3]
    draw_squares(axes[0], array, 0, 3, 1, sums[0])
    
    # Iteración 2: Ventana [1:4]
    draw_squares(axes[1], array, 1, 4, 2, sums[1])
    
    # Iteración 3: Ventana [2:5]
    draw_squares(axes[2], array, 2, 5, 3, sums[2])
    
    # Agregar leyenda
    legend_elements = [
        mpatches.Patch(color=window_color, label='Elemento en ventana'),
        mpatches.Patch(color=outside_color, label='Elemento fuera de ventana')
    ]
    fig.legend(handles=legend_elements, loc='upper right', bbox_to_anchor=(0.98, 0.95))
    
    # Agregar texto explicativo
    # fig.text(0.02, 0.02, 
    #          'Algoritmo Sliding Window: O(n) en lugar de O(n×k)\n'
    #          'En cada iteración: nueva_suma = suma_anterior - elemento_saliente + elemento_entrante',
    #          fontsize=12, bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray", alpha=0.8))
    
    # Ajustar el layout
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.1)
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/sliding_window_algorithm.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de sliding window guardado en: escrito/figures/sliding_window_algorithm.png")
    print(f"Array: {array}")
    print(f"Sumas de las ventanas: {sums}")
    print(f"Verificación manual:")
    print(f"  Iteración 1: {array[0]} + {array[1]} + {array[2]} = {sums[0]}")
    print(f"  Iteración 2: {sums[0]} - {array[0]} + {array[3]} = {sums[1]}")
    print(f"  Iteración 3: {sums[1]} - {array[1]} + {array[4]} = {sums[2]}")

def grafico_minqueue():
    """
    Genera un gráfico que muestra el funcionamiento de la MinQueue paso a paso
    con elementos visuales que representan la queue principal y la deque auxiliar.
    """
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura con subplots
    fig, axes = plt.subplots(7, 1, figsize=(16, 14))
    
    # Datos de ejemplo para la MinQueue
    operations = [
        ('push', 3), ('push', 1), ('push', 4), ('push', 6), ('pop', None), 
        ('push', 5), ('pop', None), ('pop', None)
    ]
    
    # Simular el estado de la MinQueue en cada paso
    queue_states = []
    deque_states = []
    min_values = []
    
    # Simular la MinQueue
    queue = []
    deque = []
    
    for op, value in operations:
        if op == 'push':
            queue.append(value)
            # Mantener deque en orden creciente
            while deque and deque[-1] > value:
                deque.pop()
            deque.append(value)
        elif op == 'pop':
            if queue:
                removed = queue.pop(0)
                if deque and deque[0] == removed:
                    deque.pop(0)
        
        queue_states.append(queue.copy())
        deque_states.append(deque.copy())
        min_values.append(deque[0] if deque else None)
    
    # Función para dibujar la estructura de datos
    def draw_data_structure(ax, queue, deque, min_val, step_num, operation):
        ax.clear()
        
        # Dibujar la queue principal (arriba)
        queue_y = 0.7
        # ax.text(-0.5, queue_y + 0.1, 'Queue:', fontweight='bold', fontsize=10)
        
        for i, val in enumerate(queue):
            # Color diferente para el elemento que se va a procesar
            if step_num < len(operations) and operations[step_num][0] == 'pop' and i == 0:
                # color = '#FF6B6B'  # Rojo para elemento a eliminar
                color = '#4ECDC4'  # Verde para elementos normales
            else:
                color = '#4ECDC4'  # Verde para elementos normales
            
            rect = plt.Rectangle((i - 0.3, queue_y - 0.15), 0.6, 0.3, 
                                facecolor=color, edgecolor='black', linewidth=2)
            ax.add_patch(rect)
            ax.text(i, queue_y, str(val), ha='center', va='center', 
                   fontweight='bold', fontsize=14, color='white')
        
        # Dibujar la deque auxiliar (abajo)
        deque_y = 0.3
        # ax.text(-0.5, deque_y + 0.1, 'Deque:', fontweight='bold', fontsize=10)
        
        for i, val in enumerate(deque):
            color = '#FFD93D'  # Amarillo para elementos en deque
            rect = plt.Rectangle((i - 0.3, deque_y - 0.15), 0.6, 0.3, 
                                facecolor=color, edgecolor='black', linewidth=2)
            ax.add_patch(rect)
            ax.text(i, deque_y, str(val), ha='center', va='center', 
                   fontweight='bold', fontsize=14, color='black')
        
        # Mostrar el mínimo actual (posicionado a la derecha de las cajitas)
        if min_val is not None:
            # La posición x se ajusta a la cantidad máxima de elementos para quedar a la derecha 
            max_elements = max(len(queue), len(deque)) if queue or deque else 1
            ax.text(max_elements + 0.5, 0.5, f'Mínimo: {min_val}', 
                    fontweight='bold', fontsize=12, color='red', va='center', ha='left')
        
        # Configurar el gráfico
        max_elements = max(len(queue), len(deque)) if queue or deque else 1
        ax.set_xlim(-1, max_elements + 1)
        ax.set_ylim(-0.2, 1.0)
        ax.set_aspect('equal')
        ax.axis('off')
        
        # Título con la operación
        op_text = f"{operation[0]}({operation[1]})" if operation[1] is not None else f"{operation[0]}()"
        ax.set_title(f'Paso {step_num + 1}: {op_text}', fontweight='bold', pad=20)
    
    # Mostrar todos los pasos
    for i in range(7):
        if i < len(operations):
            draw_data_structure(axes[i], queue_states[i], deque_states[i], 
                              min_values[i], i, operations[i])
        else:
            axes[i].axis('off')
    
    # Agregar leyenda
    legend_elements = [
        mpatches.Patch(color='#4ECDC4', label='Elemento en Queue'),
        mpatches.Patch(color='#FFD93D', label='Elemento en Deque')
    ]
    fig.legend(handles=legend_elements, loc='upper right', bbox_to_anchor=(0.98, 0.95))
    
    # Eliminar texto explicativo extra
    
    # Ajustar el layout con menos interlineado
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.15, hspace=0.3)
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/minqueue_algorithm.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de MinQueue guardado en: escrito/figures/minqueue_algorithm.png")
    print("Secuencia de operaciones:")
    for i, (op, val) in enumerate(operations):
        print(f"  Paso {i+1}: {op}({val}) - Queue: {queue_states[i]}, Deque: {deque_states[i]}, Mínimo: {min_values[i]}")

def grafico_rellenado_huecos():
    """
    Genera un gráfico que muestra la mejora en el algoritmo de rellenado de huecos:
    - Señal original con un hueco
    - Interpolación puntual (método anterior)
    - Interpolación con promedios de ventanas (método mejorado)
    - Remoción del offset con promedios de 300 puntos
    """
    # Configurar el estilo del gráfico
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.labelsize'] = 14
    plt.rcParams['axes.titlesize'] = 16
    plt.rcParams['xtick.labelsize'] = 12
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['legend.fontsize'] = 12
    
    # Crear datos sintéticos para demostrar el concepto
    np.random.seed(42)
    n_points = 200
    x = np.linspace(0, 10, n_points)
    
    # Crear una señal base con ruido
    signal_base = 2 * np.sin(0.5 * x) + 0.5 * np.sin(2 * x) + 0.1 * np.random.randn(n_points)
    
    # Crear un hueco artificial en el medio
    gap_start = 80
    gap_end = 120
    gap_size = gap_end - gap_start
    
    # Introducir valores anómalos/críticos en los extremos del hueco
    signal_with_anomalies = signal_base.copy()
    signal_with_anomalies[gap_start - 1] = 5.0  # Valor anómalo alto
    signal_with_anomalies[gap_end] = 4.5        # Valor anómalo alto
    
    # Señal con hueco
    signal_with_gap = signal_with_anomalies.copy()
    signal_with_gap[gap_start:gap_end] = np.nan
    
    # Método 1: Interpolación puntual (método anterior)
    # Usar solo los valores inmediatamente antes y después del hueco (valores anómalos)
    left_value = signal_with_anomalies[gap_start - 1]  # 5.0 (anómalo)
    right_value = signal_with_anomalies[gap_end]       # 4.5 (anómalo)
    
    # Interpolación lineal simple
    signal_puntual = signal_with_gap.copy()
    for i in range(gap_start, gap_end):
        alpha = (i - gap_start + 1) / (gap_size + 1)
        signal_puntual[i] = left_value + alpha * (right_value - left_value)
    
    # Método 2: Interpolación con promedios de ventanas (método mejorado)
    window_size = 100  # 1000 puntos como en la tesis, pero reducido para visualización
    
    # Calcular promedios de las ventanas antes y después del hueco
    left_window_start = max(0, gap_start - window_size)
    left_window_end = gap_start
    right_window_start = gap_end
    right_window_end = min(n_points, gap_end + window_size)
    
    # Usar la señal original (sin anomalías) para calcular promedios
    left_avg = np.mean(signal_base[left_window_start:left_window_end])
    right_avg = np.mean(signal_base[right_window_start:right_window_end])
    
    # Interpolación con promedios
    signal_promedios = signal_with_gap.copy()
    for i in range(gap_start, gap_end):
        alpha = (i - gap_start + 1) / (gap_size + 1)
        signal_promedios[i] = left_avg + alpha * (right_avg - left_avg)
    
    # Método 3: Remoción del offset con promedios de 300 puntos
    offset_window = 300  # Ventana para remoción de offset
    signal_offset_removed = signal_promedios.copy()
    
    # Aplicar remoción de offset usando promedios móviles
    for i in range(n_points):
        start_idx = max(0, i - offset_window // 2)
        end_idx = min(n_points, i + offset_window // 2)
        offset_value = np.mean(signal_promedios[start_idx:end_idx])
        signal_offset_removed[i] = signal_promedios[i] - offset_value
    
    # Offsetear la señal a partir del 1
    signal_offset_removed = signal_offset_removed + 1.0
    
    # Crear la figura con subplots (sin el primer gráfico)
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(14, 12))
    
    # Gráfico 1: Interpolación puntual (método anterior) - usando valores anómalos
    # ax1.plot(x, signal_base, 'b-', linewidth=1, label='Señal original', alpha=0.5)
    ax1.plot(x, signal_puntual, 'r-', linewidth=2, label='Interpolación puntual', alpha=0.8)
    
    # Marcar los puntos anómalos usados para interpolación
    ax1.plot(x[gap_start-1], left_value, 'ro', markersize=10, label=f'Valor anómalo izquierdo: {left_value:.1f}')
    ax1.plot(x[gap_end], right_value, 'ro', markersize=10, label=f'Valor anómalo derecho: {right_value:.1f}')
    
    # Marcar el hueco rellenado
    ax1.axvspan(x[gap_start], x[gap_end-1], alpha=0.3, color='red', label='Hueco rellenado')
    
    ax1.set_title('Interpolación puntual (con valores anómalos)', fontweight='bold', pad=20)
    ax1.set_ylabel('Amplitud', fontweight='bold')
    ax1.grid(True, alpha=0.3, linestyle='--')
    ax1.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Gráfico 2: Interpolación con promedios (método mejorado)
    # ax2.plot(x, signal_base, 'b-', linewidth=1, label='Señal original', alpha=0.5)
    ax2.plot(x, signal_promedios, 'g-', linewidth=2, label='Interpolación con promedios', alpha=0.8)
    
    # Marcar las líneas de promedio
    ax2.axhline(y=left_avg, color='green', linestyle='--', linewidth=2, alpha=0.7, 
                label=f'Promedio izquierdo: {left_avg:.3f}')
    ax2.axhline(y=right_avg, color='orange', linestyle='--', linewidth=2, alpha=0.7, 
                label=f'Promedio derecho: {right_avg:.3f}')
    
    # Marcar el hueco rellenado
    ax2.axvspan(x[gap_start], x[gap_end-1], alpha=0.3, color='red', label='Hueco rellenado')
    
    ax2.set_title('Interpolación con promedios de ventanas', fontweight='bold', pad=20)
    ax2.set_ylabel('Amplitud', fontweight='bold')
    ax2.grid(True, alpha=0.3, linestyle='--')
    ax2.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Gráfico 3: Remoción del offset con promedios de 300 puntos
    ax3.plot(x, signal_promedios, 'g-', linewidth=1, label='Remoción con la interpolación puntual', alpha=0.5)
    ax3.plot(x, signal_offset_removed, 'purple', linewidth=2, label='Remoción con promedios de ventanas', alpha=0.8)
    
    # Marcar el hueco rellenado
    ax3.axvspan(x[gap_start], x[gap_end-1], alpha=0.3, color='red', label='Hueco rellenado')
    
    ax3.set_title('Remoción del offset con promedios de 300 puntos', fontweight='bold', pad=20)
    ax3.set_xlabel('Tiempo (s)', fontweight='bold')
    ax3.set_ylabel('Amplitud', fontweight='bold')
    ax3.grid(True, alpha=0.3, linestyle='--')
    ax3.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Agregar texto explicativo
    # fig.text(0.02, 0.02, 
    #          'Mejora en el algoritmo de rellenado de huecos:\n'
    #          '• Mayor robustez estadística al usar promedios de ventanas amplias en lugar de valores anómalos\n'
    #          '• Mejor representación del comportamiento de la señal al evitar distorsiones por valores extremos\n'
    #          '• Reducción de distorsiones en el procesamiento posterior, especialmente en la remoción del offset',
    #          fontsize=11, bbox=dict(boxstyle="round,pad=0.5", facecolor="lightblue", alpha=0.8))
    
    # Ajustar el layout
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.15)
    
    # Guardar la figura
    plt.savefig('/Users/uzielluduena/Thesis/new_def/escrito/figures/mejora_rellenado_huecos.png', 
                dpi=300, bbox_inches='tight', facecolor='white')
    
    # Mostrar el gráfico
    plt.show()
    
    print("Gráfico de mejora en rellenado de huecos guardado en: escrito/figures/mejora_rellenado_huecos.png")
    print(f"Valores anómalos en extremos: izquierdo={left_value:.1f}, derecho={right_value:.1f}")
    print(f"Valores de interpolación con promedios: izquierdo={left_avg:.3f}, derecho={right_avg:.3f}")
    print(f"Tamaño de ventana para promedios: {window_size} puntos")
    print(f"Tamaño de ventana para remoción de offset: {offset_window} puntos")
    print(f"Señal final offseteada a partir de 1.0")

if __name__ == "__main__":
    # grafico_complejidades()
    # grafico_sensores_gota()
    # grafico_gota_analisis()
    # grafico_calculo_carga()
    # grafico_calculo_velocidad()
    # grafico_relacion_velocidad_diametro()
    # grafico_comparacion_modelos()
    # grafico_sliding_window()
    # grafico_rellenado_huecos()
    grafico_minqueue()
