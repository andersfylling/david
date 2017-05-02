//
// Created by anders on 5/2/17.
//

namespace stockfish_wrapper {
#include "../lib/Stockfish/src/evaluate.h"
#include "../lib/Stockfish/src/types.h"
#include "../lib/Stockfish/src/position.h"


int valueToInt(Value val) {
  int score = 0;


  // update score based on Value struct data.


  return score;
}
Position nodeToPosition() {
  Position pos;

  return pos;
}

// Node pointer, or reference
int getScoreFromNode(/* node pointe ror reference here */) {
  auto pos = nodeToPosition(/* node pointe ror reference here */);
  auto val = Eval::evaluate(pos);
  return valueToInt(val);
}
}