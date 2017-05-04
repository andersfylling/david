#include "include.h"

// Define another benchmark
static void BM_charToIntA(benchmark::State& state) {
  char x = 'E';
  while (state.KeepRunning()) {
    int a = (int) x;
  }
}

BENCHMARK(BM_charToIntA);
static void BM_charToIntB(benchmark::State& state) {
  char x = 'E';
  while (state.KeepRunning()) {
    int b = x;
  }
}
BENCHMARK(BM_charToIntB);

static void BM_charToIntC(benchmark::State& state) {
  char x = 'E';
  while (state.KeepRunning()) {
    int c = x - '0';
  }
}

BENCHMARK(BM_charToIntC);