#ifndef CLEAN_H
#define CLEAN_H

#include "types.h"

void fill_gaps_on_lvm_file(fs::path lvm_file);

void compute_dynamic_avg_on_dat_file(fs::path dat_file, fs::path dat_file_left, fs::path dat_file_right, uint w = 5000);

#endif