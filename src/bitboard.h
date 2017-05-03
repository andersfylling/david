//
// Created by lolos on 05.04.2017.
//

#ifndef CHESS_ANN_BITBOARD_H
#define CHESS_ANN_BITBOARD_H

#include <array>
#include <stdint.h>
#include <math.h>

using std::array;
// These values represent the position of the pieces
// BN and WN references to the Knights. This is to
// Differ them from the kings.

//   A    B   C   D   E   F   G   H
// 8: 0    1   2   3   4   5   6   7
// 7: 8    9   10  11  12  13  14  15
// 6: 16   17  18  19  20  21  22  23
// 5: 24   25  26  27  28  29  30  31
// 4: 32   33  34  35  36  37  38  39
// 3: 40   41  42  43  44  45  46  47
// 2: 48   49  50  51  52  53  54  55
// 1: 56   57  58  59  60  61  62  63

enum COLOR {BLACK, WHITE};

// These values are used when generating bitboards
// Replace the whole struct for testing specific situations
const array<int, 8> BPc = {48, 49, 50, 51, 52, 53, 54, 55};
const array<int, 8> WPc = {8, 9, 10, 11, 12, 13, 14, 15};
const array<int, 2> WCc = {0, 7};
const array<int, 2> BCc = {56, 63};
const array<int, 2> BNc = {57, 62};
const array<int, 2> WNc = {1, 6};
const array<int, 2> BRc = {58, 61};
const array<int, 2> WRc = {2, 5};
const array<int, 1> WQc = {3};
const array<int, 1> BQc = {59};
const array<int, 1> WKc = {4};
const array<int, 1> BKc = {60};

// Why do I use arrays?
// They are AWESOME

typedef uint64_t bitboard;  // Represents a bitboard

enum DIRECTION {MAIN_DIAGONAL, ANTI_DIAGONAL, UP, DOWN};

template <std::size_t SIZE>
bitboard makeBoardFromArray(const array <int, SIZE > &arr) {
  bitboard boardValue = 0;
  for (int i = 0; i < 64; i++) {
    for (const auto &v : arr) {
      if (v == i) {
        boardValue = boardValue|static_cast<bitboard>(pow(2.0, static_cast<long double>(i)));
      }
    }
  }
  return boardValue;
}

// Piece types. Sliding includes queen, king, rook and tower
// Needed for move generation
enum pieceType{PAWN, KNIGHT, NORMAL};


// Each game state is represented by a struct of
// bitboards. A tree of moves will be made up by
struct gameState {
  bitboard WhitePawn;
  bitboard WhiteRook;
  bitboard WhiteKnight;
  bitboard WhiteBishop;
  bitboard WhiteQueen;
  bitboard WhiteKing;

  bitboard BlackPawn;
  bitboard BlackRook;
  bitboard BlackKnight;
  bitboard BlackBishop;
  bitboard BlackQueen;
  bitboard BlackKing;
};

struct stateNode {
  int value;
  gameState * state;
  // Some children pointer
};

#endif  //CHESS_ANN_BITBOARD_H
