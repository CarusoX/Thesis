import argparse
import subprocess
import sys
import os

# Argument parser setup
parser = argparse.ArgumentParser(description="Clean directories using ./exec")
parser.add_argument("path", help="Path of the original lvm")
parser.add_argument("--max-processes", type=int, default=3, help="Maximum number of concurrent processes")

# Parse arguments
args = parser.parse_args()

# Path of the original lvm
file_path = args.path

# Maximum number of concurrent processes
max_processes = args.max_processes

# List of processes
processes = []


def run_commands_in_parallel(commands):
    for command in commands:
        # If we've reached the maximum number of concurrent processes, wait for one to finish
        if len(processes) >= max_processes:
            # Wait for the first process to finish and remove it from the list
            processes.pop(0).wait()

        # Spawn a process for the command and add it to the list of processes
        processes.append(subprocess.Popen(command))
    
    # Wait for all remaining processes to complete
    for process in processes:
        process.wait()


def check_file_exists(file_path):
    if os.path.isfile(file_path):
        return True
    else:
        return False

def perform_split(file_path):
    print('Performing split...')
    with open(file_path, 'r') as file:
        os.system(f'split {file_path} -C300MB')

def move_chunks_to_folders(prefix):
    print('Moving chunks to folders...')
    index = 1
    chunks = []
    for file in os.listdir():
        if file.startswith('x'):
            chunks.append(file)

    chunks.sort()

    max_digits = len(str(len(chunks)))

    folders = []
    
    for chunk in chunks:

        folder = f'{prefix}-{str(index).zfill(max_digits)}'
        # We create the directory in case it doesn't exist
        if not os.path.exists(folder):
            os.system(f'mkdir {folder}')

        # We move the chunk to the directory
        os.system(f'mv {chunk} {folder}/data.lvm')

        folders.append(folder)

        index += 1

    return folders

def complete_chunks(folders):
    print('Completing chunks...')

    commands = [['../DropFinder/drop_finder', 'fill_gaps', f'{folder}/data.lvm'] for folder in folders]

    run_commands_in_parallel(commands)

def compute_dynamic_avg(folders):
    print('Computing dynamic average...')

    commands = [
        ['../DropFinder/drop_finder',
        'compute_dynamic_avg',
        f'{folder}/data_complete.dat',
        f'{folders[i-1]}/data_complete.dat' if i > 0 else '',
        f'{folders[i+1]}/data_complete.dat' if i < len(folders) - 1 else '' ]
    for i, folder in enumerate(folders)]

    run_commands_in_parallel(commands)

if __name__ == "__main__":

    if not check_file_exists(file_path):
        print("File does not exist")
        exit(2)
    
    perform_split(sys.argv[1])

    [year, month, day] = file_path.split('-')[:3]

    folders = move_chunks_to_folders(f'{year}-{month}-{day}')

    complete_chunks(folders)

    compute_dynamic_avg(folders)

    exit(0)
