#include "benchmark/benchmark.h"

static void BM_uint8CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (uint8_t i = 0; i < 64; i += 1) {}
  } // eternal loop: uint8_t i = 0; i < 256; i += 1
}
static void BM_int8CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (int8_t i = 0; i < 64; i += 1) {}
  } // eternal loop: uint8_t i = 0; i < 256; i += 1
}

static void BM_uint16CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (uint16_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_int16CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (int16_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_uint32CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (uint32_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_int32CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (int32_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_uint64CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (uint64_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_int64CounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (int64_t i = 0; i < 64; i += 1) {}
  }
}

static void BM_intCounterInLoop(benchmark::State& state) {
  while (state.KeepRunning()) {
    for (int i = 0; i < 64; i += 1) {}
  }
}



BENCHMARK(BM_uint8CounterInLoop);
BENCHMARK(BM_int8CounterInLoop);
BENCHMARK(BM_uint16CounterInLoop);
BENCHMARK(BM_int16CounterInLoop);
BENCHMARK(BM_uint32CounterInLoop);
BENCHMARK(BM_int32CounterInLoop);
BENCHMARK(BM_uint64CounterInLoop);
BENCHMARK(BM_int64CounterInLoop);
BENCHMARK(BM_intCounterInLoop);