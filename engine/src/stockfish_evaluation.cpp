
#include <chess_ann/bitboard.h>
#include "stockfish/evaluate.h"
#include "stockfish/types.h"
#include "stockfish/position.h"

namespace stockfish_wrapper {
using Eval::evaluate;

int valueToInt(Value val) {
  int score = 0;


  // update score based on Value struct data.
  score = static_cast<int>(val);

  return score;
}
void nodeToPosition(::bitboard::gameState& n, Position& pos) {

  //pos.set(); //set board setup based on FEN string

}

// ScoreNode pointer, or reference
int getScoreFromNode(::bitboard::gameState& node) {
  Position pos;
  nodeToPosition(node, pos);

  auto val = evaluate(pos);
  return valueToInt(val);
}
}