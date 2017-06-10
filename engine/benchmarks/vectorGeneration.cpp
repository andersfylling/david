//
// Created by martin on 6/7/17.
//

#include "benchmark/benchmark.h"
#include "david/bitboard.h"
#include "david/MoveGeneration.h"
#include "david/environment.h"

namespace {
david::bitboard::bitboard_t board = 65280ULL;
static void BM_environment(benchmark::State& state) {
  david::environment::Environment env(david::bitboard::COLOR::WHITE);
  while (state.KeepRunning()) {
    env.getXAxisFromBoard(board);
  }

}

static void BM_environment_diag(benchmark::State& state) {
  david::environment::Environment env(david::bitboard::COLOR::WHITE);
  while (state.KeepRunning()) {
    env.getDiagYAxis(board, david::bitboard::DIRECTION::ANTI_DIAGONAL);
  }

}





static void BM_new_movegen(benchmark::State& state) {
  david::movegen::MoveGenerator gen;
  while(state.KeepRunning()) {
    gen.createVectors(board, david::movegen::DIRECTION::WEST, 0);
  }
}





static void BM_new_movegen_diag(benchmark::State& state) {
  david::movegen::MoveGenerator gen;
  while(state.KeepRunning()) {
    gen.createVectors(board, david::movegen::DIRECTION::NORTH_WEST, 0);
  }
}


BENCHMARK(BM_new_movegen);
BENCHMARK(BM_new_movegen_diag);
BENCHMARK(BM_environment);
BENCHMARK(BM_environment_diag);
}

