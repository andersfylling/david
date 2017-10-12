#include <david/utils/gameState.h>
#include <david/MoveGen.h>
#include "benchmark/benchmark.h"


// Define another benchmark
static void BM_reuseA(benchmark::State& state) {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);


  while (state.KeepRunning()) {
    ::david::MoveGen mg{gs};
    ::std::array<::david::type::gameState_t, 256> gameStates{};
    mg.template generateGameStates(gameStates);
  }
}

BENCHMARK(BM_reuseA);
static void BM_reuseB(benchmark::State& state) {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);
  ::david::MoveGen mg{gs};


  while (state.KeepRunning()) {
    ::std::array<::david::type::gameState_t, 256> gameStates{};
    mg.setGameState(gs);
    mg.template generateGameStates(gameStates);
  }
}
BENCHMARK(BM_reuseB);
