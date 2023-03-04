//Adapted from Chromiums V8 JS Engine. All rights to them. see https://github.com/v8/v8/blob/main/src/base/utils/random-number-generator.cc

#include "efi.h"

void random_set_seed(UINT64);

UINT64 random_next_uint64();

INT64 random_next_int64();

UINT64 random_next_range_uint64(UINT64 fromIncluded, UINT64 toExcluded);

INT64 random_next_range_int64(INT64 fromIncluded, INT64 toExcluded);