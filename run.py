import argparse
import os
import multiprocessing
from concurrent.futures import ProcessPoolExecutor
import glob
import sys
from io import StringIO
import contextlib


def create_folder_for_file(file: str, quiet: bool = False):
    folder = file.split("/")[-1].split(".")[0]
    if not quiet:
        print(f"Creando carpeta {folder}")
    os.makedirs(folder, exist_ok=True)
    return folder


def compile_programs(clean: bool):
    try:
        print("Compilando programas...")
        if clean:
            os.system("make clean")
        os.system("make")
    except Exception as e:
        print(f"Error compilando programas: {e}")
        exit(1)


def run_drop_finder(file: str, force: bool, quiet: bool = False):
    if force or not os.path.exists("drops.dat"):
        if not quiet:
            print(f"Ejecutando drop_finder")

        cmd = f"../exec/drop_finder ../{file}"
        if quiet:
            cmd += " > /dev/null 2>&1"
        os.system(cmd)


def run_drop_sorter(force: bool, quiet: bool = False):
    if force or not os.path.exists("drops_sorted.dat"):
        if not quiet:
            print(f"Ejecutando drop_sorter")
        
        cmd = f"../exec/drop_sorter"
        if quiet:
            cmd += " > /dev/null 2>&1"
        os.system(cmd)


def run_drop_charts(force: bool, quiet: bool = False):
    if force or not os.path.exists("graficos"):
        if not quiet:
            print(f"Ejecutando drop_charts")
        
        cmd = f"../exec/drop_chart"
        if quiet:
            cmd += " > /dev/null 2>&1"
        os.system(cmd)


def run_carga_velocidad(force: bool, quiet: bool = False):
    """Ejecuta el programa Fortran que resume drops.dat en carga_velocidad.dat."""
    if force or not os.path.exists("carga_velocidad.dat"):
        if not quiet:
            print(f"Ejecutando carga_velocidad")

        cmd = f"../exec/carga_velocidad"
        if quiet:
            cmd += " > /dev/null 2>&1"
        os.system(cmd)


def analyze_storm(file: str, from_step: int, quiet: bool = False):
    """Analiza una tormenta individual. Esta función se ejecuta en un proceso separado."""
    original_dir = os.getcwd()
    
    try:
        if not quiet:
            print(f"[{os.getpid()}] Iniciando análisis de {file}")
        
        # 1. Creamos la carpeta donde vamos a analizar la tormenta
        folder = create_folder_for_file(file, quiet)
        # 2. Nos paramos dentro de la carpeta creada
        os.chdir(folder)
        # 3. Ejecutamos el programa drop_finder
        run_drop_finder(file, from_step <= 1, quiet)
        # 4. Ejecutamos el programa drop_sorter
        run_drop_sorter(from_step <= 2, quiet)
        # 5. Ejecutamos el programa drop_charts
        run_drop_charts(from_step <= 3, quiet)
        run_carga_velocidad(from_step <= 4, quiet)
        
        if not quiet:
            print(f"[{os.getpid()}] Análisis completado para {file}")
        return f"Éxito: {file}"
        
    except Exception as e:
        if not quiet:
            print(f"[{os.getpid()}] Error procesando {file}: {e}")
        return f"Error: {file} - {e}"
    finally:
        # Volvemos al directorio original
        os.chdir(original_dir)


def process_single_storm(args_tuple):
    """Wrapper function para multiprocessing"""
    file, from_step = args_tuple
    return analyze_storm(file, from_step, quiet=True)


def expand_file_patterns(input_files):
    """Expande patrones de archivos (wildcards) a lista de archivos"""
    expanded_files = []
    for pattern in input_files:
        if '*' in pattern or '?' in pattern:
            # Es un patrón, expandir
            matches = glob.glob(pattern)
            if matches:
                expanded_files.extend(matches)
            else:
                print(f"Advertencia: No se encontraron archivos que coincidan con el patrón '{pattern}'")
        else:
            # Es un archivo específico
            if os.path.exists(pattern):
                expanded_files.append(pattern)
            else:
                print(f"Advertencia: El archivo '{pattern}' no existe")
    
    return expanded_files


def print_progress_bar(completed, total, width=50):
    """Imprime una barra de progreso simple"""
    progress = completed / total
    filled = int(width * progress)
    bar = '█' * filled + '░' * (width - filled)
    percentage = progress * 100
    print(f'\rProgreso: [{bar}] {completed}/{total} ({percentage:.1f}%)', end='', flush=True)


def main(input_files: list, clean: bool, from_step: int, max_processes: int):
    # 1. Compilamos los programas (solo una vez)
    compile_programs(clean)
    
    # 2. Expandir patrones de archivos
    files_to_process = expand_file_patterns(input_files)
    
    if not files_to_process:
        print("Error: No se encontraron archivos para procesar")
        return
    
    print(f"Archivos a procesar: {len(files_to_process)}")
    for file in files_to_process:
        print(f"  - {file}")
    
    # 3. Procesar archivos en paralelo
    if len(files_to_process) == 1:
        # Un solo archivo, procesamiento directo con output completo
        print("Procesando archivo único...")
        result = analyze_storm(files_to_process[0], from_step, quiet=False)
        print(result)
    else:
        # Múltiples archivos, procesamiento en paralelo con output suprimido
        print(f"Procesando {len(files_to_process)} archivos en paralelo (máximo {max_processes} procesos)...")
        print("Nota: Output detallado suprimido durante procesamiento paralelo")
        print()
        
        # Preparar argumentos para cada proceso
        process_args = [(file, from_step) for file in files_to_process]
        
        # Ejecutar en paralelo con seguimiento de progreso
        completed_count = 0
        results = []
        
        with ProcessPoolExecutor(max_workers=max_processes) as executor:
            # Enviar todos los trabajos
            future_to_file = {executor.submit(process_single_storm, args): args[0] 
                             for args in process_args}
            
            # Procesar resultados conforme se completan
            from concurrent.futures import as_completed
            
            print_progress_bar(0, len(files_to_process))
            
            for future in as_completed(future_to_file):
                file = future_to_file[future]
                try:
                    result = future.result()
                    results.append(result)
                    completed_count += 1
                    print_progress_bar(completed_count, len(files_to_process))
                except Exception as e:
                    results.append(f"Error: {file} - {e}")
                    completed_count += 1
                    print_progress_bar(completed_count, len(files_to_process))
        
        print()  # Nueva línea después de la barra de progreso
        
        # Mostrar resultados
        print("\n=== RESUMEN DE PROCESAMIENTO ===")
        successful = 0
        failed = 0
        for result in results:
            print(result)
            if result.startswith("Éxito"):
                successful += 1
            else:
                failed += 1
        
        print(f"\nTotal: {len(results)} archivos")
        print(f"Exitosos: {successful}")
        print(f"Fallidos: {failed}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Analiza una o múltiples tormentas en paralelo",
        epilog="""
Ejemplos de uso:
  python run.py archivo1.lvm                          # Procesar un archivo
  python run.py archivo1.lvm archivo2.lvm             # Procesar múltiples archivos
  python run.py *.lvm                                 # Procesar todos los archivos .lvm
  python run.py datos_*.lvm --processes 8             # Usar 8 procesos en paralelo
  python run.py archivo1.lvm --no-clean --from-step 3    # No limpiar y empezar desde el paso 3
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "input_files", 
        nargs='+',
        help="Ruta(s) al(los) archivo(s) de tormenta. Soporta wildcards (*.lvm)"
    )
    parser.add_argument(
        "--no-clean",
        action="store_false",
        dest="clean",
        help="No limpiar los programas compilados antes de ejecutar (por defecto se limpia)",
    )
    parser.add_argument(
        "--from-step",
        type=int,
        help="Paso desde el que se quiere analizar la tormenta (1=finder, 2=sorter, 3=charts, 4=carga_velocidad)",
        default=1,
        choices=[1, 2, 3, 4],
    )
    parser.add_argument(
        "--processes",
        type=int,
        help="Número máximo de procesos paralelos a usar (default: 4)",
        default=4,
    )
    
    args = parser.parse_args()
    
    # Validar número de procesos
    max_cpu_count = multiprocessing.cpu_count()
    if args.processes > max_cpu_count:
        print(f"Advertencia: Se solicitaron {args.processes} procesos, pero solo hay {max_cpu_count} CPUs disponibles")
        print(f"Se usarán {max_cpu_count} procesos")
        args.processes = max_cpu_count
    
    main(args.input_files, args.clean, args.from_step, args.processes)
