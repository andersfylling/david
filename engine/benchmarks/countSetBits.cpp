#include "benchmark/benchmark.h"

constexpr uint8_t LSB(const uint64_t board) {
#ifdef __linux__
  return (board > 0) ? __builtin_ffsl(board) - 1 : 0;
#else
  for (int i = 0; i < 64; i++) {
    if (::utils::bitAt(board, i)) {
      return i;
    }
  }
#endif
}
constexpr bool bitAt(const uint64_t b, const uint8_t i) {
  return (b & (1ULL << i)) != 0;
}

// Define another benchmark
static void BM_countSetBitsA(benchmark::State& state) {
  const uint64_t n = 0b100100000000010000101010100000000000100001000000000010010000001;
  while (state.KeepRunning()) {
#ifdef __linux__
    __builtin_popcountll(n);
#else
    // lol
  uint8_t counter = 0;
  for (uint8_t i = 0; i < 64; i++) {
    if (bitAt(n, i)) {
      counter += 1;
    }
  }
#endif
  }
}

BENCHMARK(BM_countSetBitsA);
static void BM_countSetBitsB(benchmark::State& state) {
  const uint64_t n = 0b100100000000010000101010100000000000100001000000000010010000001;
  while (state.KeepRunning()) {
    uint8_t counter = 0;
    for (uint8_t i = 0; i < 64; i++) {
      if (bitAt(n, i)) {
        counter += 1;
      }
    }
  }
}
BENCHMARK(BM_countSetBitsB);
