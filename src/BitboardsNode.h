//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_BITBOARDSNODE_H
#define CHESS_ANN_BITBOARDSNODE_H

#include "bitboard.h"
#include "ScoreNode.h"
class BitboardsNode {
 private:
  //ScoreNode* scoreNode;
  bitboard* white;
  bitboard* whitePawns;
  bitboard* whiteQueens;
  bitboard* whiteKings;
  bitboard* whiteRooks;
  bitboard* whiteBishops;
  bitboard* whiteKnights;
  bitboard* black;
  bitboard* blackPawns;
  bitboard* blackQueens;
  bitboard* blackKings;
  bitboard* blackRooks;
  bitboard* blackBishops;
  bitboard* blackKnights;

 public:
  BitboardsNode();
};

#endif //CHESS_ANN_BITBOARDSNODE_H
