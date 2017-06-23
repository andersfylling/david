#include "benchmark/benchmark.h"

namespace {

namespace types {
typedef uint64_t bitboard_t;
}

bool bitAt(uint64_t b, uint8_t i) {
  return (b & (1ULL << i)) != 0;
}

void flipBit(types::bitboard_t &board, types::bitboard_t index) {
  board |= (1LL << index);
}

static void BM_check(benchmark::State &state) {
  uint64_t s = 0LL;
  while (state.KeepRunning()) {
    bitAt(s, 12 + 0LL);
    flipBit(s, 12 + 0LL);
  }
}
BENCHMARK(BM_check);

static void BM_noCheck(benchmark::State &state) {
  uint64_t s = 0LL;
  while (state.KeepRunning()) {
    flipBit(s, 12 + 0LL);
  }
}

BENCHMARK(BM_noCheck);
}