
#include <iostream>
#include <string>
#include <vector>

#include "clean.h"
#include "utils.h"

void fill_gaps(const std::string &lvm_file) {
    std::cout << "Filling gaps in directory: " << lvm_file << "...\n";
    fill_gaps_on_lvm_file(lvm_file);
}

void compute_dynamic_avg(const std::string& dat_file, const std::string& dat_file_left, const std::string& dat_file_right) {
    std::cout << "Computing dynamic avg: " << dat_file << "...\n";
    compute_dynamic_avg_on_dat_file(dat_file, dat_file_left, dat_file_right);
}

void drop_finder() {
    std::cout << "Running drop_finder function...\n";
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
        std::string dat_file = argv[2];
        std::string dat_file_left = argv[3];
        std::string dat_file_right = argv[4];
        compute_dynamic_avg(dat_file, dat_file_left, dat_file_right);
    }
    else if (command == "analysis") {
        analysis();
    }
    else {
        std::cerr << "Error: Invalid command.\n";
        exit(1);
    }

    return 0;
}