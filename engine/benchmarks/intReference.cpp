#include "benchmark/benchmark.h"

namespace {

void reference(uint64_t& a) {
  a &= ~(1LL << (a != 0LL) ? __builtin_ffsll(a) - 1 : 0LL);
}
void copy(uint64_t a) {
  a &= ~(1LL << (a != 0LL) ? __builtin_ffsll(a) - 1 : 0LL);
}

static void BM_intReference(benchmark::State &state) {
  uint64_t x = 954832453;
  while (state.KeepRunning()) {
    reference(x);
  }
}
BENCHMARK(BM_intReference);

static void BM_intCopy(benchmark::State &state) {
  uint64_t x = 954832453;
  while (state.KeepRunning()) {
    copy(x);
  }
}

BENCHMARK(BM_intCopy);
}