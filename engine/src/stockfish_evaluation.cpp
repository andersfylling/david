
#include <chess_ann/bitboard.h>
#include <chess_ann/environment.h>
#include <stockfish/thread.h>
#include "stockfish/evaluate.h"
#include "stockfish/types.h"
#include "stockfish/position.h"

namespace stockfish_wrapper {


int valueToInt(Value val) {
  int score = 0;


  // update score based on Value struct data.
  score = static_cast<int>(val);

  return score;
}
void nodeToPosition(::bitboard::gameState* n, Position& pos) {

  StateInfo si;

  //pos.set(); //set board setup based on FEN string
  ::environment::Environment env(::bitboard::COLOR::WHITE);
  Thread th;
  pos.set(::bitboard::startFENPosition, false, &si, &th);

}

// ScoreNode pointer, or reference
int getScoreFromNode(::bitboard::gameState* node) {
  using Eval::evaluate;

  Position pos;
  pos.init();
  nodeToPosition(node, pos);

  auto val = evaluate(pos);
  return valueToInt(val);
}
}