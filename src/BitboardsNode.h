//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_BITBOARDSNODE_H
#define CHESS_ANN_BITBOARDSNODE_H

#include "bitboard.h"

class BitboardsNode {
 private:
  ::bitboard::bitboard_t* white;
  ::bitboard::bitboard_t* whitePawns;
  ::bitboard::bitboard_t* whiteQueens;
  ::bitboard::bitboard_t* whiteKings;
  ::bitboard::bitboard_t* whiteRooks;
  ::bitboard::bitboard_t* whiteBishops;
  ::bitboard::bitboard_t* whiteKnights;
  ::bitboard::bitboard_t* black;
  ::bitboard::bitboard_t* blackPawns;
  ::bitboard::bitboard_t* blackQueens;
  ::bitboard::bitboard_t* blackKings;
  ::bitboard::bitboard_t* blackRooks;
  ::bitboard::bitboard_t* blackBishops;
  ::bitboard::bitboard_t* blackKnights;
  int test;

 public:
  BitboardsNode();
};

#endif //CHESS_ANN_BITBOARDSNODE_H
