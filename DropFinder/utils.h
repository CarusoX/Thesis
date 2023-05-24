#ifndef UTILS_H
#define UTILS_H

#include "types.h"

std::vector<LVM_DATUM> read_lvm_file(fs::path lvm_file, uint data_columns = 2);

std::vector<DAT_DATUM> read_dat_file(fs::path dat_file, uint data_columns = 2);

std::vector<DAT_DATUM> read_dat_file_first_k_lines(fs::path dat_file, uint data_columns, uint k);

std::vector<DAT_DATUM> read_dat_file_last_k_lines(fs::path dat_file, uint data_columns, uint k);

void write_dat_file(fs::path dat_file, std::vector<DAT_DATUM> &data);

#endif