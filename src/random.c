//Adapted from Chromiums V8 JS Engine. All rights to them. see https://github.com/v8/v8/blob/main/src/base/utils/random-number-generator.cc

#include "random.h"

static UINT64 state0_;
static UINT64 state1_;

static void XorShift128(UINT64* state0, UINT64* state1) {
    UINT64 s1 = *state0;
    UINT64 s0 = *state1;
    *state0 = s0;
    s1 ^= s1 << 23;
    s1 ^= s1 >> 17;
    s1 ^= s0;
    s1 ^= s0 >> 26;
    *state1 = s1;
}

static UINT64 MurmurHash3(UINT64 h) {
  h ^= h >> 33;
  h *= (UINT64)0xFF51AFD7ED558CCDULL;
  h ^= h >> 33;
  h *= (UINT64)0xC4CEB9FE1A85EC53ULL;
  h ^= h >> 33;
  return h;
}


void random_set_seed(UINT64 seed) {
  state0_ = MurmurHash3(seed);
  state1_ = MurmurHash3(~state0_);
}

INT64 random_next_int64() {
  XorShift128(&state0_, &state1_);
  return (INT64)(state0_ + state1_);
}

UINT64 random_next_uint64(){
  XorShift128(&state0_, &state1_);
  return state0_ + state1_;
}


UINT64 random_next_range_uint64(UINT64 fromIncluded, UINT64 toExcluded){
  return fromIncluded + (random_next_uint64() % (toExcluded-fromIncluded));
}

INT64 random_next_range_int64(INT64 fromIncluded, INT64 toExcluded){
  return fromIncluded + (INT64) (random_next_uint64() % (toExcluded-fromIncluded));
}