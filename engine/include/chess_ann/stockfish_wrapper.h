#ifndef CHESS_ANN_STCOKFISH_WRAPPER_H
#define CHESS_ANN_STCOKFISH_WRAPPER_H

#include <chess_ann/bitboard.h>
#include <chess_ann/environment.h>
#include "stockfish/evaluate.h"
#include "stockfish/types.h"
#include "stockfish/position.h"

namespace stockfish_wrapper {
int valueToInt(Value val);
void nodeToPosition(::bitboard::gameState* n, Position& pos);
int getScoreFromNode(::bitboard::gameState* node);
}

#endif //CHESS_ANN_STCOKFISH_WRAPPER_H
