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

