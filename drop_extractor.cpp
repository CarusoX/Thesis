#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <deque>
#include "LVM.hpp"
#include "filter.hpp"
#include "Drop.hpp"

const size_t DROP_SIZE = 400;
const size_t WINDOW_SIZE = 5000;

// Define a function to write drop data to a file
void write_drop_to_file(const std::vector<LVM::Row>& data, int start, int end, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::app); // Open in append mode

    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
        return;
    }

    outFile << std::fixed << std::setprecision(6); // Format numbers to 6 decimal places
    for (int i = start; i < end; ++i) {
        outFile << data[i].time << "\t" 
                << data[i].sensor1 << "\t" 
                << data[i].sensor2 << "\n";
    }

    outFile.close();
}

void perform(const std::string& fifo_path) {
    if (!std::filesystem::exists(fifo_path)) {
        throw std::invalid_argument(
            "El archivo FIFO '" + fifo_path + "' no existe. Usa 'mkfifo' para crearlo."
        );
    }

    std::cout << "Leyendo del FIFO: " << fifo_path << std::endl;

    LVM lvm(2 * WINDOW_SIZE);
    Drop drop(DROP_SIZE, 100); // Configure Drop object
    // Drop drop(DROP_SIZE, 100); // TODO: configure this if needed
    size_t gotas = 0;

    // std::ifstream fifo(fifo_path);
    std::ifstream fifo(fifo_path);
    if (!fifo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo FIFO.");
    }

    std::string line;
    while (true) {
        if (!std::getline(fifo, line)) {
            // Esperar más datos si no hay líneas disponibles
            exit(0);
            continue;
        }

        lvm.add_sensor_data(line); // Agrega datos al buffer

        if(lvm.size() >= WINDOW_SIZE * 2) {
          std::vector<LVM::Row> normalized_data = Filter::normalize_with_rolling(lvm.get(), WINDOW_SIZE); // Computar rolling average

          std::vector<LVM::Row> find_data(
                normalized_data.end() - DROP_SIZE * 2,
                normalized_data.end()
            );

          // Check if all "Used" entries are 0
          bool all_used_zero = std::all_of(find_data.begin(), find_data.end(), [](const LVM::Row& row) {
              return row.used == 0;
          });

          if (!all_used_zero) {
              continue;
          }

          // Find drop in the last data
          auto dropData = drop.findDrop(find_data);
          int drop_index = dropData.u1;

          size_t offset = normalized_data.size() - DROP_SIZE * 2;

          if (drop_index == -1) {
              lvm.set_used(offset, offset + DROP_SIZE);
              continue;
          }
          dropData.print();

          // Extract the rows corresponding to the drop
          int start = offset + drop_index;
          int end = offset + drop_index + DROP_SIZE;

          std::cout << "Gotas: " << ++gotas << std::endl;
          lvm.set_drop(start, end);
          // Write the drop data to a file
          std::string drop_filename = "drops.dat";
          write_drop_to_file(find_data, drop_index, drop_index + DROP_SIZE, drop_filename);
        }

    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta al archivo FIFO>" << std::endl;
        return 1;
    }

    try {
        perform(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
