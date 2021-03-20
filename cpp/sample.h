#ifndef SAMPLE_H
#define SAMPLE_H

#include "board.h"

#include <unordered_set>
#include <unordered_map>

void sample_gen(ARRAY_2D_OUT_M(ULL), cint n);
void sample_pos_gen(ARRAY_2D_OUT_M(ULL), cint n);
void sample_flip_benchmark(ARRAY_2D_OUT_M(ULL), ARRAY_2D_IN_I(ULL));
void sample_gen_select(ARRAY_2D_OUT_M(ULL), cint n, cbool flag_verbose = true);

#endif // SAMPLE_H
