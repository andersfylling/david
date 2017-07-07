#include "benchmark/benchmark.h"
#include "david/types.h"
#include "david/utils.h"
#include <cassert>

namespace {

::david::type::bitboard_t bits = 4760305923896123394; // 9 active bits

static void BM_normalBitArrayLoop(benchmark::State &state) {
  while (state.KeepRunning()) {
    int nr = 0;
    auto n = bits;
    for (int i = 0; i < 64; i++) {
      if (::david::utils::bitAt(n, i)) {
        nr += 1;
      }
    }
    assert(nr == 9);
  }
}

static void BM_bitArrayLoopUsingLSB(benchmark::State &state) {
  while (state.KeepRunning()) {
    int nr = 0;
    auto n = bits;
    do {
      ::david::utils::flipBitOff(n, ::david::utils::LSB(n));
      nr += 1;
    } while(n != 0ULL);
    if (n != 0ULL) {
      std::cerr << n << std::endl;
      assert(n == 0ULL);
    }
  }
}

BENCHMARK(BM_normalBitArrayLoop);
BENCHMARK(BM_bitArrayLoopUsingLSB);
}