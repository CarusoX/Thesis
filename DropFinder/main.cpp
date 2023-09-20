
#include <iostream>
#include <string>
#include <vector>

#include "clean.h"
#include "drop_finder.h"
#include "utils.h"

void fill_gaps(const std::string &lvm_file) {
    std::cout << "Filling gaps in file: " << lvm_file << "...\n";
    fill_gaps_on_lvm_file(lvm_file);
}

void compute_dynamic_avg(const std::string& lvm_file, const std::string& lvm_file_left, const std::string& lvm_file_right) {
    std::cout << "Computing dynamic avg in file: " << lvm_file << "...\n";
    compute_dynamic_avg_on_lvm_file(lvm_file, lvm_file_left, lvm_file_right);
}

void drop_finder(const std::string &lvm_file) {
    std::cout << "Running drop_finder function...\n";
    drop_finder_on_lvm_file(lvm_file);
}

void analysis() {
    std::cout << "Running analysis function...\n";
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Error: No arguments provided.\n";
        exit(1);
    }

    std::string command = argv[1];

    if (command == "fill_gaps") {
        if (argc < 3) {
            std::cerr << "Error: No file path provided for fill_gaps command.\n";
            exit(1);
        }
        std::string lvm_file = argv[2];
        fill_gaps(lvm_file);
    }
    else if (command == "compute_dynamic_avg") {
        if (argc < 5) {
            std::cerr << "Error: No file paths provided for compute_dynamic_avg command.\n";
            exit(1);
        }
        std::string lvm_file = argv[2];
        std::string lvm_file_left = argv[3];
        std::string lvm_file_right = argv[4];
        compute_dynamic_avg(lvm_file, lvm_file_left, lvm_file_right);
    }
    else if (command == "drop_finder") {
        if (argc < 3) {
            std::cerr << "Error: No file path provided for fill_gaps command.\n";
            exit(1);
        }
        std::string lvm_file = argv[2];
        drop_finder(lvm_file);
    }
    else {
        std::cerr << "Error: Invalid command.\n";
        exit(1);
    }

    return 0;
}