import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as mpatches
import os

def leer_lvm(archivo, num_puntos=10000, inicio=0):
    """
    Lee un archivo .lvm y retorna los datos de tiempo, sensor1 y sensor2.
    
    Args:
        archivo: Ruta al archivo .lvm
        num_puntos: Número de puntos a leer (default: 10000)
        inicio: Punto de inicio para leer (default: 0)
    
    Returns:
        time, sensor1, sensor2: Arrays numpy con los datos
    """
    time = []
    sensor1 = []
    sensor2 = []
    
    with open(archivo, 'r') as f:
        # Saltar líneas hasta el inicio
        for i in range(inicio):
            line = f.readline()
            if not line:
                break
        
        # Leer num_puntos líneas
        for i in range(num_puntos):
            line = f.readline()
            if not line:
                break
            
            # Parsear la línea (formato: tiempo\tsensor1\tsensor2)
            # También maneja comas como separadores decimales
            line_clean = line.strip().replace(',', '.')
            parts = line_clean.split()
            if len(parts) >= 3:
                try:
                    t = float(parts[0])
                    s1 = float(parts[1])
                    s2 = float(parts[2])
                    time.append(t)
                    sensor1.append(s1)
                    sensor2.append(s2)
                except ValueError:
                    continue
    
    return np.array(time), np.array(sensor1), np.array(sensor2)

def grafico_tormenta_lvm(archivo_lvm, num_puntos=10000, inicio=0, 
                         guardar_path=None, titulo=None, mostrar=True,
                         solo_sensor1=False, solo_sensor2=False):
    """
    Genera un gráfico de los sensores desde un archivo .lvm para la presentación.
    
    Args:
        archivo_lvm: Ruta al archivo .lvm
        num_puntos: Número de puntos a graficar (default: 10000)
        inicio: Punto de inicio para leer (default: 0)
        guardar_path: Ruta donde guardar la figura (default: presentacion/figures/)
        titulo: Título del gráfico (default: automático)
        mostrar: Si mostrar el gráfico (default: True)
        solo_sensor1: Si solo mostrar sensor1 (default: False)
        solo_sensor2: Si solo mostrar sensor2 (default: False)
    """
    # Leer los datos
    print(f"Leyendo {num_puntos} puntos desde el archivo {archivo_lvm}...")
    time, sensor1, sensor2 = leer_lvm(archivo_lvm, num_puntos, inicio)
    
    print(f"Leídos {len(time)} puntos")
    print(f"Rango de tiempo: {time[0]:.6f}s - {time[-1]:.6f}s")
    print(f"Rango sensor1: {np.min(sensor1):.6f} - {np.max(sensor1):.6f}")
    print(f"Rango sensor2: {np.min(sensor2):.6f} - {np.max(sensor2):.6f}")
    
    # Configurar el estilo del gráfico para presentación
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Graficar los sensores según las opciones
    if solo_sensor1:
        ax.plot(time, sensor1, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
    elif solo_sensor2:
        ax.plot(time, sensor2, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    else:
        ax.plot(time, sensor1, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
        ax.plot(time, sensor2, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud (V)', fontweight='bold')
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    if guardar_path is None:
        # Crear nombre de archivo basado en el archivo de entrada
        base_name = os.path.splitext(os.path.basename(archivo_lvm))[0]
        guardar_path = f'/Users/uzielluduena/Thesis/new_def/presentacion/figures/{base_name}_{num_puntos}p.png'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    # Mostrar el gráfico
    if mostrar:
        plt.show()
    else:
        plt.close()

def grafico_array(guardar_path=None, mostrar=False):
    """
    Genera un gráfico visual de un Array mostrando acceso directo por índice.
    """
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 16
    plt.rcParams['axes.labelsize'] = 18
    plt.rcParams['axes.titlesize'] = 20
    
    fig, ax = plt.subplots(figsize=(12, 4))
    ax.axis('off')
    
    # Datos del array
    elementos = [10, 4, 6, 4, 5, 3]
    indices = list(range(len(elementos)))
    
    # Dibujar cajas del array
    for i, (idx, val) in enumerate(zip(indices, elementos)):
        # Caja del elemento
        rect = mpatches.Rectangle((i*1.2, 0), 1, 1, 
                                 linewidth=2, edgecolor='black', 
                                 facecolor='lightblue', alpha=0.7)
        ax.add_patch(rect)
        
        # Índice
        ax.text(i*1.2 + 0.5, -0.3, f'[{idx}]', 
               ha='center', va='top', fontsize=14, fontweight='bold')
        
        # Valor
        ax.text(i*1.2 + 0.5, 0.5, str(val), 
               ha='center', va='center', fontsize=16, fontweight='bold')
    
    # Flecha de acceso directo
    ax.annotate('', xy=(2*1.2 + 0.5, 1.5), xytext=(2*1.2 + 0.5, 2),
               arrowprops=dict(arrowstyle='->', lw=2, color='red'))
    ax.text(2*1.2 + 0.5, 2.2, 'Acceso O(1)', 
           ha='center', fontsize=14, fontweight='bold', color='red')
    
    ax.set_xlim(-0.5, len(elementos)*1.2 - 0.2)
    ax.set_ylim(-0.8, 2.5)
    ax.set_title('Array: Acceso directo por índice', fontweight='bold', pad=20)
    
    plt.tight_layout()
    
    if guardar_path is None:
        guardar_path = '/Users/uzielluduena/Thesis/new_def/presentacion/figures/array.png'
    
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    if mostrar:
        plt.show()
    else:
        plt.close()

def grafico_queue(guardar_path=None, mostrar=False):
    """
    Genera un gráfico visual de una Queue mostrando operaciones FIFO.
    """
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 16
    plt.rcParams['axes.labelsize'] = 18
    plt.rcParams['axes.titlesize'] = 20
    
    fig, ax = plt.subplots(figsize=(12, 4))
    ax.axis('off')
    
    # Elementos de la queue
    elementos = [10, 4, 6]
    
    # Dibujar queue horizontal
    for i, val in enumerate(elementos):
        # Caja del elemento
        rect = mpatches.Rectangle((i*1.2, 0), 1, 1, 
                                 linewidth=2, edgecolor='black', 
                                 facecolor='darkblue', alpha=0.7)
        ax.add_patch(rect)
        
        # Valor
        ax.text(i*1.2 + 0.5, 0.5, str(val), 
               ha='center', va='center', fontsize=16, fontweight='bold', color='white')
    
    # Flecha de push (entrada)
    ax.annotate('', xy=(len(elementos)*1.2, 0.5), xytext=(len(elementos)*1.2 + 0.5, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='green'))
    ax.text(len(elementos)*1.2 + 0.25, 0.8, 'push', 
           ha='center', fontsize=12, fontweight='bold', color='green')
    
    # Flecha de pop (salida)
    ax.annotate('', xy=(-0.2, 0.5), xytext=(-0.7, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='red'))
    ax.text(-0.45, 0.8, 'pop', 
           ha='center', fontsize=12, fontweight='bold', color='red')
    
    # Etiquetas
    ax.text(len(elementos)*1.2 + 0.25, -0.3, 'Entrada', 
           ha='center', fontsize=12, fontweight='bold')
    ax.text(-0.45, -0.3, 'Salida', 
           ha='center', fontsize=12, fontweight='bold')
    
    ax.set_xlim(-1, len(elementos)*1.2 + 1)
    ax.set_ylim(-0.8, 1.5)
    ax.set_title('Queue: FIFO (First In, First Out)', fontweight='bold', pad=20)
    
    plt.tight_layout()
    
    if guardar_path is None:
        guardar_path = '/Users/uzielluduena/Thesis/new_def/presentacion/figures/queue.png'
    
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    if mostrar:
        plt.show()
    else:
        plt.close()

def grafico_deque(guardar_path=None, mostrar=False):
    """
    Genera un gráfico visual de una Deque mostrando operaciones bidireccionales.
    """
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 16
    plt.rcParams['axes.labelsize'] = 18
    plt.rcParams['axes.titlesize'] = 20
    
    fig, ax = plt.subplots(figsize=(12, 4))
    ax.axis('off')
    
    # Elementos de la deque
    elementos = [10, 4, 6, 4]
    
    # Dibujar deque horizontal
    for i, val in enumerate(elementos):
        # Caja del elemento
        rect = mpatches.Rectangle((i*1.2, 0), 1, 1, 
                                 linewidth=2, edgecolor='black', 
                                 facecolor='darkgreen', alpha=0.7)
        ax.add_patch(rect)
        
        # Valor
        ax.text(i*1.2 + 0.5, 0.5, str(val), 
               ha='center', va='center', fontsize=16, fontweight='bold', color='white')
    
    # Flechas de push (ambos extremos)
    # Entrada por la derecha
    ax.annotate('', xy=(len(elementos)*1.2, 0.5), xytext=(len(elementos)*1.2 + 0.5, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='green'))
    ax.text(len(elementos)*1.2 + 0.25, 0.8, 'push_back', 
           ha='center', fontsize=12, fontweight='bold', color='green')
    
    # Entrada por la izquierda
    ax.annotate('', xy=(-0.2, 0.5), xytext=(-0.7, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='green'))
    ax.text(-0.45, 0.8, 'push_front', 
           ha='center', fontsize=12, fontweight='bold', color='green')
    
    # Flechas de pop (ambos extremos)
    # Salida por la derecha
    ax.annotate('', xy=(len(elementos)*1.2 - 0.2, 0.5), xytext=(len(elementos)*1.2 + 0.3, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='red'))
    ax.text(len(elementos)*1.2 + 0.05, -0.3, 'pop_back', 
           ha='center', fontsize=12, fontweight='bold', color='red')
    
    # Salida por la izquierda
    ax.annotate('', xy=(0.2, 0.5), xytext=(-0.5, 0.5),
               arrowprops=dict(arrowstyle='->', lw=2, color='red'))
    ax.text(-0.15, -0.3, 'pop_front', 
           ha='center', fontsize=12, fontweight='bold', color='red')
    
    ax.set_xlim(-1, len(elementos)*1.2 + 1)
    ax.set_ylim(-0.8, 1.5)
    ax.set_title('Deque: Inserción/eliminación por ambos extremos', fontweight='bold', pad=20)
    
    plt.tight_layout()
    
    if guardar_path is None:
        guardar_path = '/Users/uzielluduena/Thesis/new_def/presentacion/figures/deque.png'
    
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    if mostrar:
        plt.show()
    else:
        plt.close()

def leer_dat(archivo):
    """
    Lee un archivo .dat con formato: tiempo\tsensor1\tsensor2
    
    Args:
        archivo: Ruta al archivo .dat
    
    Returns:
        time, sensor1, sensor2: Arrays numpy con los datos
    """
    time = []
    sensor1 = []
    sensor2 = []
    
    with open(archivo, 'r') as f:
        for line in f:
            line_clean = line.strip()
            if not line_clean:
                continue
            
            # Parsear la línea (formato: tiempo\tsensor1\tsensor2)
            # También maneja comas como separadores decimales
            line_clean = line_clean.replace(',', '.')
            parts = line_clean.split()
            if len(parts) >= 3:
                try:
                    t = float(parts[0])
                    s1 = float(parts[1])
                    s2 = float(parts[2])
                    time.append(t)
                    sensor1.append(s1)
                    sensor2.append(s2)
                except ValueError:
                    continue
    
    return np.array(time), np.array(sensor1), np.array(sensor2)

def leer_drops_dat(archivo):
    """
    Lee un archivo .dat de drops con múltiples columnas.
    Extrae las columnas 0 (tiempo), 2 (sensor1) y 3 (sensor2).
    
    Args:
        archivo: Ruta al archivo .dat
    
    Returns:
        time, sensor1, sensor2: Arrays numpy con los datos
    """
    time = []
    sensor1 = []
    sensor2 = []
    
    with open(archivo, 'r') as f:
        for line in f:
            line_clean = line.strip()
            if not line_clean:
                continue
            
            # Parsear la línea (formato: tiempo\tid\tsensor1\tsensor2\t...)
            # También maneja comas como separadores decimales
            line_clean = line_clean.replace(',', '.')
            parts = line_clean.split()
            if len(parts) >= 4:
                try:
                    t = float(parts[0])      # Columna 0: tiempo
                    s1 = float(parts[2])     # Columna 2: sensor1
                    s2 = float(parts[3])     # Columna 3: sensor2
                    time.append(t)
                    sensor1.append(s1)
                    sensor2.append(s2)
                except ValueError:
                    continue
    
    return np.array(time), np.array(sensor1), np.array(sensor2)

def grafico_gota_buena_mala(archivo_good, archivo_bad, guardar_path_good=None, guardar_path_bad=None, mostrar=False):
    """
    Genera dos gráficos separados: uno para gota buena y otro para gota mala.
    
    Args:
        archivo_good: Ruta al archivo .dat con gota buena
        archivo_bad: Ruta al archivo .dat con gota mala
        guardar_path_good: Ruta donde guardar la figura de gota buena (default: presentacion/figures/gota_buena.png)
        guardar_path_bad: Ruta donde guardar la figura de gota mala (default: presentacion/figures/gota_mala.png)
        mostrar: Si mostrar los gráficos (default: False)
    """
    # Leer los datos de ambos archivos
    print(f"Leyendo gota buena desde {archivo_good}...")
    time_good, sensor1_good, sensor2_good = leer_drops_dat(archivo_good)
    
    print(f"Leyendo gota mala desde {archivo_bad}...")
    time_bad, sensor1_bad, sensor2_bad = leer_drops_dat(archivo_bad)
    
    print(f"Gota buena: {len(time_good)} puntos")
    print(f"Gota mala: {len(time_bad)} puntos")
    
    # Normalizar tiempos para que empiecen en 0
    if len(time_good) > 0:
        time_good = time_good - time_good[0]
    if len(time_bad) > 0:
        time_bad = time_bad - time_bad[0]
    
    # Configurar el estilo del gráfico para presentación
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Asegurar que el directorio existe
    base_dir = '/Users/uzielluduena/Thesis/new_def/presentacion/figures'
    os.makedirs(base_dir, exist_ok=True)
    
    # ===== GRÁFICO DE GOTA BUENA =====
    fig1, ax1 = plt.subplots(figsize=(14, 8))
    ax1.plot(time_good, sensor1_good, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
    ax1.plot(time_good, sensor2_good, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    ax1.set_xlabel('Tiempo (s)', fontweight='bold')
    ax1.set_ylabel('Amplitud (V)', fontweight='bold')
    # Sin título
    ax1.grid(True, alpha=0.3, linestyle='--')
    ax1.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    plt.tight_layout()
    
    if guardar_path_good is None:
        guardar_path_good = f'{base_dir}/gota_buena.png'
    
    plt.savefig(guardar_path_good, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico de gota buena guardado en: {guardar_path_good}")
    
    if mostrar:
        plt.show()
    else:
        plt.close(fig1)
    
    # ===== GRÁFICO DE GOTA MALA =====
    fig2, ax2 = plt.subplots(figsize=(14, 8))
    ax2.plot(time_bad, sensor1_bad, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
    ax2.plot(time_bad, sensor2_bad, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    ax2.set_xlabel('Tiempo (s)', fontweight='bold')
    ax2.set_ylabel('Amplitud (V)', fontweight='bold')
    # Sin título
    ax2.grid(True, alpha=0.3, linestyle='--')
    ax2.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    plt.tight_layout()
    
    if guardar_path_bad is None:
        guardar_path_bad = f'{base_dir}/gota_mala.png'
    
    plt.savefig(guardar_path_bad, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico de gota mala guardado en: {guardar_path_bad}")
    
    if mostrar:
        plt.show()
    else:
        plt.close(fig2)

def grafico_dat(archivo_dat, guardar_path=None, titulo=None, mostrar=False,
                solo_sensor1=False, solo_sensor2=False):
    """
    Genera un gráfico de los sensores desde un archivo .dat para la presentación.
    
    Args:
        archivo_dat: Ruta al archivo .dat
        guardar_path: Ruta donde guardar la figura (default: presentacion/figures/)
        titulo: Título del gráfico (default: automático)
        mostrar: Si mostrar el gráfico (default: False)
        solo_sensor1: Si solo mostrar sensor1 (default: False)
        solo_sensor2: Si solo mostrar sensor2 (default: False)
    """
    # Leer los datos
    print(f"Leyendo datos desde el archivo {archivo_dat}...")
    time, sensor1, sensor2 = leer_dat(archivo_dat)
    
    print(f"Leídos {len(time)} puntos")
    if len(time) > 0:
        print(f"Rango de tiempo: {time[0]:.6f}s - {time[-1]:.6f}s")
        print(f"Rango sensor1: {np.min(sensor1):.6f} - {np.max(sensor1):.6f}")
        print(f"Rango sensor2: {np.min(sensor2):.6f} - {np.max(sensor2):.6f}")
    
    # Configurar el estilo del gráfico para presentación
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Graficar los sensores según las opciones
    if solo_sensor1:
        ax.plot(time, sensor1, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
    elif solo_sensor2:
        ax.plot(time, sensor2, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    else:
        ax.plot(time, sensor1, 'b-', linewidth=1.5, label='Sensor 1 (Anillo)', alpha=0.8)
        ax.plot(time, sensor2, 'r-', linewidth=1.5, label='Sensor 2 (Placa)', alpha=0.8)
    
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud (V)', fontweight='bold')
    
    # Agregar título si se proporciona
    if titulo:
        ax.set_title(titulo, fontweight='bold', pad=20)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    if guardar_path is None:
        # Crear nombre de archivo basado en el archivo de entrada
        base_name = os.path.splitext(os.path.basename(archivo_dat))[0]
        guardar_path = f'/Users/uzielluduena/Thesis/new_def/presentacion/figures/{base_name}.png'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    # Mostrar el gráfico
    if mostrar:
        plt.show()
    else:
        plt.close()

def leer_drops_lvm_data(archivo):
    """
    Lee un archivo .dat con múltiples drops separados por "---".
    
    Args:
        archivo: Ruta al archivo .dat con formato drops_lvm_data.dat
    
    Returns:
        drops: Lista de tuplas (time, sensor1, sensor2) para cada drop
    """
    drops = []
    current_drop_time = []
    current_drop_sensor1 = []
    current_drop_sensor2 = []
    
    with open(archivo, 'r') as f:
        for line in f:
            line_clean = line.strip()
            
            # Si encontramos un separador, guardamos el drop actual
            if line_clean == "---":
                if len(current_drop_time) > 0:
                    drops.append((
                        np.array(current_drop_time),
                        np.array(current_drop_sensor1),
                        np.array(current_drop_sensor2)
                    ))
                    current_drop_time = []
                    current_drop_sensor1 = []
                    current_drop_sensor2 = []
                continue
            
            # Parsear la línea (formato: tiempo sensor1 sensor2)
            line_clean = line_clean.replace(',', '.')
            parts = line_clean.split()
            if len(parts) >= 3:
                try:
                    t = float(parts[0])
                    s1 = float(parts[1])
                    s2 = float(parts[2])
                    current_drop_time.append(t)
                    current_drop_sensor1.append(s1)
                    current_drop_sensor2.append(s2)
                except ValueError:
                    continue
    
    # Guardar el último drop si no termina con "---"
    if len(current_drop_time) > 0:
        drops.append((
            np.array(current_drop_time),
            np.array(current_drop_sensor1),
            np.array(current_drop_sensor2)
        ))
    
    return drops

def leer_drops_invalid(archivo):
    """
    Lee un archivo drop_invalid.dat con múltiples drops separados por "--- c1 c2".
    Los valores c1 y c2 corresponden al drop anterior.
    
    Args:
        archivo: Ruta al archivo drop_invalid.dat
    
    Returns:
        drops: Lista de tuplas (time, sensor1, sensor2, c1, c2) para cada drop
               c1 y c2 pueden ser None si no se encontraron
    """
    drops = []
    current_drop_time = []
    current_drop_sensor1 = []
    current_drop_sensor2 = []
    current_c1 = None
    current_c2 = None
    current_c1_original = None
    current_c2_original = None
    
    with open(archivo, 'r') as f:
        for line in f:
            line_clean = line.strip()
            
            # Si encontramos un separador con c1 y c2
            if line_clean.startswith("---"):
                # Extraer c1 y c2 del separador (formato: --- c1 c2 [c1_original c2_original])
                parts = line_clean.split()
                if len(parts) >= 3:
                    try:
                        current_c1 = int(parts[1])
                        current_c2 = int(parts[2])
                        # Si hay 4 valores adicionales, leer c1_original y c2_original
                        if len(parts) >= 5:
                            current_c1_original = int(parts[3])
                            current_c2_original = int(parts[4])
                        else:
                            current_c1_original = None
                            current_c2_original = None
                    except (ValueError, IndexError):
                        current_c1 = None
                        current_c2 = None
                        current_c1_original = None
                        current_c2_original = None
                else:
                    current_c1 = None
                    current_c2 = None
                    current_c1_original = None
                    current_c2_original = None

                # Guardar el drop anterior con sus c1 y c2
                if len(current_drop_time) > 0:
                    drops.append((
                        np.array(current_drop_time),
                        np.array(current_drop_sensor1),
                        np.array(current_drop_sensor2),
                        current_c1,
                        current_c2,
                        current_c1_original,
                        current_c2_original
                    ))
                    current_drop_time = []
                    current_drop_sensor1 = []
                    current_drop_sensor2 = []
                continue
            
            # Parsear la línea (formato: tiempo sensor1 sensor2)
            line_clean = line_clean.replace(',', '.')
            parts = line_clean.split()
            if len(parts) >= 3:
                try:
                    t = float(parts[0])
                    s1 = float(parts[1])
                    s2 = float(parts[2])
                    current_drop_time.append(t)
                    current_drop_sensor1.append(s1)
                    current_drop_sensor2.append(s2)
                except ValueError:
                    continue
    
    # Guardar el último drop si no termina con "---"
    if len(current_drop_time) > 0:
        drops.append((
            np.array(current_drop_time),
            np.array(current_drop_sensor1),
            np.array(current_drop_sensor2),
            current_c1,
            current_c2,
            current_c1_original,
            current_c2_original
        ))
    
    return drops

def grafico_gotas_invalidas(archivo_dat, guardar_path=None, mostrar=False, 
                             inicio_drop=0, fin_drop=None, espacio_entre_drops=0.01):
    """
    Genera un gráfico de todas las gotas inválidas desde un archivo drop_invalid.dat.
    Las gotas se muestran una después de otra, con líneas verticales marcando los cortes.
    Los puntos c1 y c2 se marcan en cada gota.
    
    Args:
        archivo_dat: Ruta al archivo drop_invalid.dat
        guardar_path: Ruta donde guardar la figura (default: presentacion/figures/gotas_invalidas.png)
        mostrar: Si mostrar el gráfico (default: False)
        inicio_drop: Índice del primer drop a graficar (default: 0)
        fin_drop: Índice del último drop a graficar (None = todos) (default: None)
        espacio_entre_drops: Espacio en segundos entre drops consecutivos (default: 0.01)
    """
    # Leer todos los drops
    print(f"Leyendo gotas inválidas desde {archivo_dat}...")
    todos_los_drops = leer_drops_invalid(archivo_dat)
    print(f"Total de gotas inválidas encontradas: {len(todos_los_drops)}")
    
    # Filtrar drops según los índices especificados
    if fin_drop is None:
        drops = todos_los_drops[inicio_drop:]
    else:
        drops = todos_los_drops[inicio_drop:fin_drop]
    
    # Filtrar drops vacíos
    drops = [(t, s1, s2, c1, c2, c11, c22) for t, s1, s2, c1, c2, c11, c22 in drops if len(t) > 0]
    
    if len(drops) == 0:
        print("No se encontraron gotas inválidas válidas para graficar")
        return
    
    print(f"Graficando {len(drops)} gotas inválidas (desde índice {inicio_drop})")
    
    # Configurar el estilo del gráfico para presentación
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(16, 8))
    
    # Procesar y graficar cada drop
    time_offset = 0.0
    separator_positions = []
    
    for i, (time, sensor1, sensor2, c1, c2, c11, c22) in enumerate(drops):
        # Normalizar tiempo del drop para que empiece en 0
        time_normalized = time - time[0]
        duracion_drop = time_normalized[-1] if len(time_normalized) > 0 else 0
        
        # Agregar offset para concatenar con drops anteriores
        time_shifted = time_normalized + time_offset
        
        # Graficar sensores
        ax.plot(time_shifted, sensor1, 'b-', linewidth=1.5, 
                label='Sensor 1 (Anillo)' if i == 0 else '', alpha=0.8)
        ax.plot(time_shifted, sensor2, 'r-', linewidth=1.5, 
                label='Sensor 2 (Placa)' if i == 0 else '', alpha=0.8)
        
        # Marcar c1 y c2 si están disponibles y son válidos
        if c1 is not None and c1 >= 0 and c1 < len(time_shifted):
            ax.plot(time_shifted[c1], sensor1[c1], 'go', markersize=10, 
                   label='c1 (Sensor 1)' if i == 0 else '', zorder=5)
            ax.plot(time_shifted[c1], sensor2[c1], 'go', markersize=10, 
                   zorder=5)
            # Anotar el índice c1
            ax.annotate(f'c1={c1}', xy=(time_shifted[c1], sensor1[c1]), 
                       xytext=(5, 10), textcoords='offset points',
                       fontsize=10, color='green', fontweight='bold',
                       bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.7))
        
        if c2 is not None and c2 >= 0 and c2 < len(time_shifted):
            ax.plot(time_shifted[c2], sensor1[c2], 'mo', markersize=10, 
                   label='c2 (Sensor 2)' if i == 0 else '', zorder=5)
            ax.plot(time_shifted[c2], sensor2[c2], 'mo', markersize=10, 
                   zorder=5)
            # Anotar el índice c2
            ax.annotate(f'c2={c2}', xy=(time_shifted[c2], sensor2[c2]), 
                       xytext=(5, -15), textcoords='offset points',
                       fontsize=10, color='magenta', fontweight='bold',
                       bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.7))
        
        if c11 is not None and c11 >= 0 and c11 < len(time_shifted):
            ax.plot(time_shifted[c11], sensor1[c11], 'go', markersize=10, 
                   label='c1 (Sensor 1)' if i == 0 else '', zorder=5)
            ax.plot(time_shifted[c11], sensor2[c11], 'go', markersize=10, 
                   zorder=5)
            # Anotar el índice c1
            ax.annotate(f'c1={c11}', xy=(time_shifted[c11], sensor1[c11]), 
                       xytext=(5, 10), textcoords='offset points',
                       fontsize=10, color='green', fontweight='bold',
                       bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.7))
        if c22 is not None and c22 >= 0 and c22 < len(time_shifted):
            ax.plot(time_shifted[c22], sensor1[c22], 'mo', markersize=10, 
                   label='c2 (Sensor 2)' if i == 0 else '', zorder=5)
            ax.plot(time_shifted[c22], sensor2[c22], 'mo', markersize=10, 
                   zorder=5)
            # Anotar el índice c2
            ax.annotate(f'c2={c22}', xy=(time_shifted[c22], sensor2[c22]), 
                       xytext=(5, -15), textcoords='offset points',
                       fontsize=10, color='magenta', fontweight='bold',
                       bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.7))

        # Guardar posición del separador (al final de este drop)
        if i < len(drops) - 1:
            separator_positions.append(time_shifted[-1])
        
        # Actualizar offset para el siguiente drop
        time_offset = time_shifted[-1] + espacio_entre_drops
    
    # Dibujar líneas verticales para marcar los cortes entre drops
    if separator_positions:
        for sep_pos in separator_positions:
            ax.axvline(x=sep_pos, color='green', linestyle='--', linewidth=2, 
                      alpha=0.7, label='Separador' if sep_pos == separator_positions[0] else '')
    
    # Configurar etiquetas y título
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud (V)', fontweight='bold')
    ax.set_title(f'Gotas inválidas detectadas ({len(drops)} gotas)', fontweight='bold', pad=20)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    if guardar_path is None:
        guardar_path = '/Users/uzielluduena/Thesis/new_def/presentacion/figures/gotas_invalidas.png'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    # Mostrar el gráfico
    if mostrar:
        plt.show()
    else:
        plt.close()

def grafico_todas_las_gotas(archivo_dat, guardar_path=None, mostrar=False, 
                            inicio_drop=0, fin_drop=None, espacio_entre_drops=0.01):
    """
    Genera un gráfico de todas las gotas desde un archivo drops_lvm_data.dat.
    Las gotas se muestran una después de otra, con líneas verticales marcando los cortes.
    
    Args:
        archivo_dat: Ruta al archivo drops_lvm_data.dat
        guardar_path: Ruta donde guardar la figura (default: presentacion/figures/todas_las_gotas.png)
        mostrar: Si mostrar el gráfico (default: False)
        inicio_drop: Índice del primer drop a graficar (default: 0)
        fin_drop: Índice del último drop a graficar (None = todos) (default: None)
        espacio_entre_drops: Espacio en segundos entre drops consecutivos (default: 0.01)
    """
    # Leer todos los drops
    print(f"Leyendo drops desde {archivo_dat}...")
    todos_los_drops = leer_drops_lvm_data(archivo_dat)
    print(f"Total de drops encontrados: {len(todos_los_drops)}")
    
    # Filtrar drops según los índices especificados
    if fin_drop is None:
        drops = todos_los_drops[inicio_drop:]
    else:
        drops = todos_los_drops[inicio_drop:fin_drop]
    
    # Filtrar drops vacíos
    drops = [(t, s1, s2) for t, s1, s2 in drops if len(t) > 0]
    
    if len(drops) == 0:
        print("No se encontraron drops válidos para graficar")
        return
    
    print(f"Graficando {len(drops)} drops (desde índice {inicio_drop})")
    
    # Configurar el estilo del gráfico para presentación
    plt.style.use('seaborn-v0_8')
    plt.rcParams['font.size'] = 14
    plt.rcParams['axes.labelsize'] = 16
    plt.rcParams['axes.titlesize'] = 18
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['legend.fontsize'] = 14
    
    # Crear la figura
    fig, ax = plt.subplots(figsize=(16, 8))
    
    # Procesar y graficar cada drop
    time_offset = 0.0
    separator_positions = []
    
    for i, (time, sensor1, sensor2) in enumerate(drops):
        # Normalizar tiempo del drop para que empiece en 0
        time_normalized = time - time[0]
        duracion_drop = time_normalized[-1] if len(time_normalized) > 0 else 0
        
        # Agregar offset para concatenar con drops anteriores
        time_shifted = time_normalized + time_offset

        print(len(time_shifted))
        print(len(sensor1))
        print(len(sensor2))
        
        # Graficar sensores
        ax.plot(time_shifted, sensor1, 'b-', linewidth=1.5, 
                label='Sensor 1 (Anillo)' if i == 0 else '', alpha=0.8)
        ax.plot(time_shifted, sensor2, 'r-', linewidth=1.5, 
                label='Sensor 2 (Placa)' if i == 0 else '', alpha=0.8)
        
        # Marcar el punto 400 si existe
        if len(time_shifted) > 400:
            ax.plot(time_shifted[400], sensor1[400], 'go', markersize=10, 
                   label='Punto 400 (Sensor 1)' if i == 0 else '', zorder=5)
            ax.plot(time_shifted[400], sensor2[400], 'mo', markersize=10, 
                   label='Punto 400 (Sensor 2)' if i == 0 else '', zorder=5)
        
        # Guardar posición del separador (al final de este drop)
        if i < len(drops) - 1:
            separator_positions.append(time_shifted[-1])
        
        # Actualizar offset para el siguiente drop
        time_offset = time_shifted[-1] + espacio_entre_drops
    
    # Dibujar líneas verticales para marcar los cortes entre drops
    if separator_positions:
        for sep_pos in separator_positions:
            ax.axvline(x=sep_pos, color='green', linestyle='--', linewidth=2, 
                      alpha=0.7, label='Separador' if sep_pos == separator_positions[0] else '')
    
    # Configurar etiquetas y título
    ax.set_xlabel('Tiempo (s)', fontweight='bold')
    ax.set_ylabel('Amplitud (V)', fontweight='bold')
    ax.set_title(f'Todas las gotas detectadas ({len(drops)} drops)', fontweight='bold', pad=20)
    
    # Agregar grid
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Configurar la leyenda
    ax.legend(loc='upper right', frameon=True, fancybox=True, shadow=True)
    
    # Ajustar el layout
    plt.tight_layout()
    
    # Guardar la figura
    if guardar_path is None:
        guardar_path = '/Users/uzielluduena/Thesis/new_def/presentacion/figures/todas_las_gotas.png'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    plt.savefig(guardar_path, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado en: {guardar_path}")
    
    # Mostrar el gráfico
    if mostrar:
        plt.show()
    else:
        plt.close()

def generar_svg_gota(archivo_dat, indice_drop, guardar_path=None, 
                     ancho=800, alto=400, margen=50, punto_marcar=400):
    """
    Genera un archivo SVG con 2 paths (sensor1 y sensor2) para una gota específica.
    Marca un punto específico en el gráfico.
    
    Args:
        archivo_dat: Ruta al archivo drops_lvm_data.dat
        indice_drop: Índice del drop a graficar (0-based)
        guardar_path: Ruta donde guardar el SVG (default: presentacion/figures/gota_{indice_drop}.svg)
        ancho: Ancho del SVG en píxeles (default: 800)
        alto: Alto del SVG en píxeles (default: 400)
        margen: Margen en píxeles (default: 50)
        punto_marcar: Índice del punto a marcar (default: 400)
    """
    # Leer todos los drops
    print(f"Leyendo drops desde {archivo_dat}...")
    todos_los_drops = leer_drops_lvm_data(archivo_dat)
    
    if indice_drop >= len(todos_los_drops):
        print(f"Error: El índice {indice_drop} está fuera de rango. Total de drops: {len(todos_los_drops)}")
        return
    
    # Obtener el drop específico
    time, sensor1, sensor2 = todos_los_drops[indice_drop]
    
    if len(time) == 0:
        print(f"Error: El drop {indice_drop} está vacío")
        return
    
    print(f"Generando SVG para drop {indice_drop} con {len(time)} puntos")
    
    # Normalizar tiempo para que empiece en 0
    time_normalized = time - time[0]
    
    # Calcular rangos de datos
    min_time = time_normalized[0]
    max_time = time_normalized[-1]
    
    # Buscar máximo y mínimo entre los puntos 200 y 600
    inicio_rango = max(0, min(0, len(time_normalized) - 1))
    fin_rango = min(len(time_normalized), 400)
    
    if fin_rango > inicio_rango:
        # Extraer los datos en el rango de interés
        sensor1_rango = sensor1[inicio_rango:fin_rango]
        sensor2_rango = sensor2[inicio_rango:fin_rango]
        
        # Calcular min y max en ese rango
        min_sensor = min(np.min(sensor1_rango), np.min(sensor2_rango))
        max_sensor = max(np.max(sensor1_rango), np.max(sensor2_rango))
    else:
        # Fallback: usar todo el rango si el rango especificado no es válido
        min_sensor = min(np.min(sensor1), np.min(sensor2))
        max_sensor = max(np.max(sensor1), np.max(sensor2))
    
    # Calcular el rango real de los sensores en el rango de interés
    time_range = max_time - min_time
    sensor_range = max_sensor - min_sensor
    
    # Área de dibujo (sin márgenes) - necesitamos esto para calcular la escala
    draw_width = ancho - 2 * margen
    draw_height = alto - 2 * margen
    
    # Ajustar la escala vertical para que la señal ocupe el 80% del área vertical
    # Queremos que el rango de datos (sensor_range) ocupe el 80% del área de dibujo
    # Para que ocupe el 80%, el rango visual debe ser: sensor_range / 0.8
    sensor_range_visual = sensor_range / 1
    
    # Calcular el centro de los datos
    sensor_center = (min_sensor + max_sensor) / 2
    
    # Posicionar para que el máximo esté cerca del 80% desde abajo (20% desde arriba)
    # Esto significa que queremos más espacio abajo que arriba
    # Si el máximo real está en max_sensor, queremos que esté al 20% desde arriba del rango visual
    # Entonces: max_sensor_visual = max_sensor + (sensor_range_visual - sensor_range) * 0.2
    padding_total = sensor_range_visual - sensor_range
    max_sensor = max_sensor + padding_total * 0.2  # 20% del padding arriba
    min_sensor = max_sensor - sensor_range_visual  # El rango completo hacia abajo
    
    # Agregar un poco de padding horizontal al tiempo
    time_padding = time_range * 0.05
    min_time -= time_padding
    max_time += time_padding
    
    # Función para convertir coordenadas de datos a coordenadas SVG
    def x_to_svg(x):
        return margen + ((x - min_time) / (max_time - min_time)) * draw_width
    
    def y_to_svg(y):
        # Invertir Y porque en SVG Y aumenta hacia abajo
        return margen + draw_height - ((y - min_sensor) / (max_sensor - min_sensor)) * draw_height
    
    # Generar paths
    def generar_path(time_data, sensor_data):
        path_parts = []
        for i in range(len(time_data)):
            x = x_to_svg(time_data[i])
            y = y_to_svg(sensor_data[i])
            if i == 0:
                path_parts.append(f"M {x:.2f} {y:.2f}")
            else:
                path_parts.append(f"L {x:.2f} {y:.2f}")
        return " ".join(path_parts)
    
    path_sensor1 = generar_path(time_normalized, sensor1)
    path_sensor2 = generar_path(time_normalized, sensor2)
    
    # Generar marcador del punto si existe
    marcador_punto = ""
    if punto_marcar < len(time_normalized):
        x_punto = x_to_svg(time_normalized[punto_marcar])
        y_punto_s1 = y_to_svg(sensor1[punto_marcar])
        y_punto_s2 = y_to_svg(sensor2[punto_marcar])
        y_texto = y_punto_s1 - 10
        
        marcador_punto = f'''
        <circle cx="{x_punto:.2f}" cy="{y_punto_s1:.2f}" r="5" fill="green" stroke="darkgreen" stroke-width="2"/>
        <circle cx="{x_punto:.2f}" cy="{y_punto_s2:.2f}" r="5" fill="magenta" stroke="darkmagenta" stroke-width="2"/>
        <text x="{x_punto:.2f}" y="{y_texto:.2f}" text-anchor="middle" font-size="12" fill="green" font-weight="bold">400</text>
        '''
    
    # Generar SVG
    svg_content = f'''<?xml version="1.0" encoding="UTF-8"?>
<svg width="{ancho}" height="{alto}" xmlns="http://www.w3.org/2000/svg">
    <!-- Fondo -->
    <rect width="{ancho}" height="{alto}" fill="white"/>
    
    <!-- Ejes -->
    <line x1="{margen}" y1="{margen}" x2="{margen}" y2="{alto - margen}" 
          stroke="gray" stroke-width="1" opacity="0.5"/>
    <line x1="{margen}" y1="{alto - margen}" x2="{ancho - margen}" y2="{alto - margen}" 
          stroke="gray" stroke-width="1" opacity="0.5"/>
    
    <!-- Paths de los sensores -->
    <path d="{path_sensor1}" fill="none" stroke="blue" stroke-width="2" opacity="0.8"/>
    <path d="{path_sensor2}" fill="none" stroke="red" stroke-width="2" opacity="0.8"/>
    
    <!-- Marcador del punto {punto_marcar} -->
    {marcador_punto}
    
    <!-- Leyenda -->
    <g transform="translate({ancho - 150}, {margen + 20})">
        <line x1="0" y1="0" x2="30" y2="0" stroke="blue" stroke-width="2"/>
        <text x="35" y="5" font-size="12" fill="black">Sensor 1 (Anillo)</text>
        <line x1="0" y1="20" x2="30" y2="20" stroke="red" stroke-width="2"/>
        <text x="35" y="25" font-size="12" fill="black">Sensor 2 (Placa)</text>
    </g>
    
    <!-- Etiquetas de ejes -->
    <text x="{ancho / 2}" y="{alto - 10}" text-anchor="middle" font-size="14" font-weight="bold" fill="black">Tiempo (s)</text>
    <text x="20" y="{alto / 2}" text-anchor="middle" font-size="14" font-weight="bold" fill="black" transform="rotate(-90, 20, {alto / 2})">Amplitud (V)</text>
</svg>'''
    
    # Guardar SVG
    if guardar_path is None:
        guardar_path = f'/Users/uzielluduena/Thesis/new_def/presentacion/figures/gota_{indice_drop}.svg'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    with open(guardar_path, 'w', encoding='utf-8') as f:
        f.write(svg_content)
    
    print(f"SVG guardado en: {guardar_path}")

def generar_svg_gota_invalida(archivo_dat, indice_drop, guardar_path=None, 
                               ancho=800, alto=400, margen=50):
    """
    Genera un archivo SVG con 2 paths (sensor1 y sensor2) para una gota inválida específica.
    Marca los puntos c1, c2, c1_original y c2_original en el gráfico.
    
    Args:
        archivo_dat: Ruta al archivo drop_invalid.dat
        indice_drop: Índice del drop a graficar (0-based)
        guardar_path: Ruta donde guardar el SVG (default: presentacion/figures/gota_invalida_{indice_drop}.svg)
        ancho: Ancho del SVG en píxeles (default: 800)
        alto: Alto del SVG en píxeles (default: 400)
        margen: Margen en píxeles (default: 50)
    """
    # Leer todos los drops inválidos
    print(f"Leyendo gotas inválidas desde {archivo_dat}...")
    todos_los_drops = leer_drops_invalid(archivo_dat)
    
    if indice_drop >= len(todos_los_drops):
        print(f"Error: El índice {indice_drop} está fuera de rango. Total de gotas: {len(todos_los_drops)}")
        return
    
    # Obtener el drop específico
    time, sensor1, sensor2, c1, c2, c1_original, c2_original = todos_los_drops[indice_drop]
    
    if len(time) == 0:
        print(f"Error: La gota {indice_drop} está vacía")
        return
    
    print(f"Generando SVG para gota inválida {indice_drop} con {len(time)} puntos")
    print(f"c1={c1}, c2={c2}, c1_original={c1_original}, c2_original={c2_original}")
    
    # Normalizar tiempo para que empiece en 0
    time_normalized = time - time[0]
    
    # Calcular rangos de datos
    min_time = time_normalized[0]
    max_time = time_normalized[-1]
    
    # Calcular min y max de los sensores
    min_sensor = min(np.min(sensor1), np.min(sensor2))
    max_sensor = max(np.max(sensor1), np.max(sensor2))
    
    # Calcular el rango real de los sensores
    time_range = max_time - min_time
    sensor_range = max_sensor - min_sensor
    
    # Área de dibujo (sin márgenes)
    draw_width = ancho - 2 * margen
    draw_height = alto - 2 * margen
    
    # Ajustar la escala vertical
    sensor_range_visual = sensor_range / 1
    
    # Posicionar para que el máximo esté cerca del 80% desde abajo (20% desde arriba)
    padding_total = sensor_range_visual - sensor_range
    max_sensor_visual = max_sensor + padding_total * 0.2
    min_sensor_visual = max_sensor_visual - sensor_range_visual
    
    # Agregar un poco de padding horizontal al tiempo
    time_padding = time_range * 0.05
    min_time -= time_padding
    max_time += time_padding
    
    # Función para convertir coordenadas de datos a coordenadas SVG
    def x_to_svg(x):
        return margen + ((x - min_time) / (max_time - min_time)) * draw_width
    
    def y_to_svg(y):
        # Invertir Y porque en SVG Y aumenta hacia abajo
        return margen + draw_height - ((y - min_sensor_visual) / (max_sensor_visual - min_sensor_visual)) * draw_height
    
    # Generar paths
    def generar_path(time_data, sensor_data):
        path_parts = []
        for i in range(len(time_data)):
            x = x_to_svg(time_data[i])
            y = y_to_svg(sensor_data[i])
            if i == 0:
                path_parts.append(f"M {x:.2f} {y:.2f}")
            else:
                path_parts.append(f"L {x:.2f} {y:.2f}")
        return " ".join(path_parts)
    
    path_sensor1 = generar_path(time_normalized, sensor1)
    path_sensor2 = generar_path(time_normalized, sensor2)
    
    # Generar marcadores para c1, c2, c1_original, c2_original
    marcadores = []
    
    # Marcador para c1 (verde, círculo relleno)
    if c1 is not None and c1 >= 0 and c1 < len(time_normalized):
        x_c1 = x_to_svg(time_normalized[c1])
        y_c1_s1 = y_to_svg(sensor1[c1])
        y_c1_s2 = y_to_svg(sensor2[c1])
        marcadores.append(f'''
        <circle cx="{x_c1:.2f}" cy="{y_c1_s1:.2f}" r="6" fill="green" stroke="darkgreen" stroke-width="2" opacity="0.8"/>
        <circle cx="{x_c1:.2f}" cy="{y_c1_s2:.2f}" r="6" fill="green" stroke="darkgreen" stroke-width="2" opacity="0.8"/>
        <text x="{x_c1:.2f}" y="{y_c1_s1 - 15:.2f}" text-anchor="middle" font-size="11" fill="green" font-weight="bold">c1={c1}</text>
        ''')
    
    # Marcador para c2 (magenta, círculo relleno)
    if c2 is not None and c2 >= 0 and c2 < len(time_normalized):
        x_c2 = x_to_svg(time_normalized[c2])
        y_c2_s1 = y_to_svg(sensor1[c2])
        y_c2_s2 = y_to_svg(sensor2[c2])
        marcadores.append(f'''
        <circle cx="{x_c2:.2f}" cy="{y_c2_s1:.2f}" r="6" fill="magenta" stroke="darkmagenta" stroke-width="2" opacity="0.8"/>
        <circle cx="{x_c2:.2f}" cy="{y_c2_s2:.2f}" r="6" fill="magenta" stroke="darkmagenta" stroke-width="2" opacity="0.8"/>
        <text x="{x_c2:.2f}" y="{y_c2_s2 + 20:.2f}" text-anchor="middle" font-size="11" fill="magenta" font-weight="bold">c2={c2}</text>
        ''')
    
    # Marcador para c1_original (verde, círculo vacío)
    if c1_original is not None and c1_original >= 0 and c1_original < len(time_normalized):
        x_c1_orig = x_to_svg(time_normalized[c1_original])
        y_c1_orig_s1 = y_to_svg(sensor1[c1_original])
        y_c1_orig_s2 = y_to_svg(sensor2[c1_original])
        marcadores.append(f'''
        <circle cx="{x_c1_orig:.2f}" cy="{y_c1_orig_s1:.2f}" r="6" fill="none" stroke="green" stroke-width="2" stroke-dasharray="3,3" opacity="0.8"/>
        <circle cx="{x_c1_orig:.2f}" cy="{y_c1_orig_s2:.2f}" r="6" fill="none" stroke="green" stroke-width="2" stroke-dasharray="3,3" opacity="0.8"/>
        <text x="{x_c1_orig:.2f}" y="{y_c1_orig_s1 - 30:.2f}" text-anchor="middle" font-size="11" fill="darkgreen" font-weight="bold">c1_orig={c1_original}</text>
        ''')
    
    # Marcador para c2_original (magenta, círculo vacío)
    if c2_original is not None and c2_original >= 0 and c2_original < len(time_normalized):
        x_c2_orig = x_to_svg(time_normalized[c2_original])
        y_c2_orig_s1 = y_to_svg(sensor1[c2_original])
        y_c2_orig_s2 = y_to_svg(sensor2[c2_original])
        marcadores.append(f'''
        <circle cx="{x_c2_orig:.2f}" cy="{y_c2_orig_s1:.2f}" r="6" fill="none" stroke="magenta" stroke-width="2" stroke-dasharray="3,3" opacity="0.8"/>
        <circle cx="{x_c2_orig:.2f}" cy="{y_c2_orig_s2:.2f}" r="6" fill="none" stroke="magenta" stroke-width="2" stroke-dasharray="3,3" opacity="0.8"/>
        <text x="{x_c2_orig:.2f}" y="{y_c2_orig_s2 + 35:.2f}" text-anchor="middle" font-size="11" fill="darkmagenta" font-weight="bold">c2_orig={c2_original}</text>
        ''')
    
    marcadores_str = "".join(marcadores)
    
    # Generar SVG
    svg_content = f'''<?xml version="1.0" encoding="UTF-8"?>
<svg width="{ancho}" height="{alto}" xmlns="http://www.w3.org/2000/svg">
    <!-- Fondo -->
    <rect width="{ancho}" height="{alto}" fill="white"/>
    
    <!-- Ejes -->
    <line x1="{margen}" y1="{margen}" x2="{margen}" y2="{alto - margen}" 
          stroke="gray" stroke-width="1" opacity="0.5"/>
    <line x1="{margen}" y1="{alto - margen}" x2="{ancho - margen}" y2="{alto - margen}" 
          stroke="gray" stroke-width="1" opacity="0.5"/>
    
    <!-- Paths de los sensores -->
    <path d="{path_sensor1}" fill="none" stroke="blue" stroke-width="2" opacity="0.8"/>
    <path d="{path_sensor2}" fill="none" stroke="red" stroke-width="2" opacity="0.8"/>
    
    <!-- Marcadores de puntos críticos -->
    {marcadores_str}
    
    <!-- Leyenda -->
    <g transform="translate({ancho - 180}, {margen + 20})">
        <line x1="0" y1="0" x2="30" y2="0" stroke="blue" stroke-width="2"/>
        <text x="35" y="5" font-size="12" fill="black">Sensor 1 (Anillo)</text>
        <line x1="0" y1="20" x2="30" y2="20" stroke="red" stroke-width="2"/>
        <text x="35" y="25" font-size="12" fill="black">Sensor 2 (Placa)</text>
        <circle cx="15" cy="40" r="5" fill="green" stroke="darkgreen" stroke-width="1"/>
        <text x="35" y="45" font-size="11" fill="black">c1, c2</text>
        <circle cx="15" cy="60" r="5" fill="none" stroke="green" stroke-width="1" stroke-dasharray="2,2"/>
        <text x="35" y="65" font-size="11" fill="black">c1_orig, c2_orig</text>
    </g>
    
    <!-- Etiquetas de ejes -->
    <text x="{ancho / 2}" y="{alto - 10}" text-anchor="middle" font-size="14" font-weight="bold" fill="black">Tiempo (s)</text>
    <text x="20" y="{alto / 2}" text-anchor="middle" font-size="14" font-weight="bold" fill="black" transform="rotate(-90, 20, {alto / 2})">Amplitud (V)</text>
</svg>'''
    
    # Guardar SVG
    if guardar_path is None:
        guardar_path = f'/Users/uzielluduena/Thesis/new_def/presentacion/figures/gota_invalida_{indice_drop}.svg'
    
    # Asegurar que el directorio existe
    os.makedirs(os.path.dirname(guardar_path), exist_ok=True)
    
    with open(guardar_path, 'w', encoding='utf-8') as f:
        f.write(svg_content)
    
    print(f"SVG guardado en: {guardar_path}")

if __name__ == "__main__":
    # Generar gráficos de estructuras de datos
    # grafico_array(mostrar=False)
    # grafico_queue(mostrar=False)
    # grafico_deque(mostrar=False)
    
    # Archivo .dat a graficar
    archivo_dat = '/Users/uzielluduena/Thesis/new_def/23-03-23-02-04-27-casa-A2/drops_falla_velocidad.dat'
    grafico_dat(archivo_dat, mostrar=False)
    
    # Gráfico comparativo de gota buena y mala
    archivo_good = '/Users/uzielluduena/Thesis/new_def/21-11-09-01-10-24/drops_good.dat'
    archivo_bad = '/Users/uzielluduena/Thesis/new_def/21-11-09-01-10-24/drops_bad.dat'
    grafico_gota_buena_mala(archivo_good, archivo_bad, mostrar=False)
    
    # Archivo .lvm a leer (opcional)
    # archivo = '/Users/uzielluduena/Thesis/Tormentas/21-11-09-01-10-24.lvm'
    # grafico_tormenta_lvm(archivo, num_puntos=10000, inicio=0, mostrar=True)
    
    # Gráfico de todas las gotas desde drops_lvm_data.dat
    archivo_drops_lvm = '/Users/uzielluduena/Thesis/new_def/21-11-09-01-10-24/drops_lvm_data.dat'
    # Ejemplo: graficar desde el drop 14 en adelante
    grafico_todas_las_gotas(archivo_drops_lvm, mostrar=False, inicio_drop=0, fin_drop=10)
    
    # Generar SVG de la gota #5 con marcador en el punto 400
    generar_svg_gota(archivo_drops_lvm, indice_drop=2, punto_marcar=400)
    
    # Gráfico de gotas inválidas desde drop_invalid.dat
    archivo_drops_invalid = '/Users/uzielluduena/Thesis/new_def/21-11-09-01-10-24/drop_invalid.dat'
    grafico_gotas_invalidas(archivo_drops_invalid, mostrar=False, inicio_drop=13, fin_drop=14)
    
    # Generar SVG de la gota inválida #13
    generar_svg_gota_invalida(archivo_drops_invalid, indice_drop=13)

