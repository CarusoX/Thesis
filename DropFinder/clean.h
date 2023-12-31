#ifndef CLEAN_H
#define CLEAN_H

#include "types.h"

void fill_gaps_on_lvm_file(fs::path lvm_file);

void compute_dynamic_avg_on_lvm_file(fs::path lvm_file, fs::path lvm_file_left, fs::path lvm_file_right, uint w = 5000);

#endif