#ifndef UTILS_H
#define UTILS_H

#include "types.h"

std::vector<LVM_DATUM> read_lvm_file(fs::path lvm_file);

std::vector<LVM_DATUM> read_lvm_file_first_k_lines(fs::path lvm_file, uint k);

std::vector<LVM_DATUM> read_lvm_file_last_k_lines(fs::path lvm_file, uint k);

void write_lvm_file(fs::path lvm_file, std::vector<LVM_DATUM> &data);

#endif