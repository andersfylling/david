#pragma once

#include <array>
#include <memory>
#include <vector>
#include <stdint.h>
#include <math.h>
#include <memory>
#include "david/types.h"

namespace david {
namespace bitboard {

using std::array;
// These values represent the position of the pieces
// BN and WN references to the Knights. This is to
// Differ them from the kings.

//     A    B   C   D   E   F   G   H
// 8:  0    1   2   3   4   5   6   7
// 7:  8    9  10  11  12  13  14  15
// 6: 16   17  18  19  20  21  22  23
// 5: 24   25  26  27  28  29  30  31
// 4: 32   33  34  35  36  37  38  39
// 3: 40   41  42  43  44  45  46  47
// 2: 48   49  50  51  52  53  54  55
// 1: 56   57  58  59  60  61  62  63

// 63 62 61 60 59 58 57 56
// 55 54 53 52 51 50 49 48
// 47 46 45 44 43 42 41 40
// 39 38 37 36 35 34 33 32
// 31 30 29 28 27 26 25 24
// 23 22 21 20 19 18 17 16
// 15 14 13 12 11 10  9  8
//  7  6  5  4  3  2  1  0

enum COLOR { BLACK, WHITE };

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

typedef uint64_t bitboard_t;  // Represents a bitboard_t
typedef uint16_t move_t;      // Representing moves

enum DIRECTION { MAIN_DIAGONAL, ANTI_DIAGONAL, UP, DOWN };

template<std::size_t SIZE>
bitboard_t makeBoardFromArray(const array<int, SIZE> &arr) {
  bitboard_t boardValue = 0;
  for (int i = 0; i < 64; i++) {
    for (const auto &v : arr) {
      if (v == i) {
        boardValue = boardValue | static_cast<bitboard_t>(pow(2.0, static_cast<long double>(i)));
      }
    }
  }
  return boardValue;
}


// Each game state is represented by a struct of
// bitboards. A tree of moves will be made up by
struct gameState {
  type::bitboard_t WhitePawn;
  type::bitboard_t WhiteRook;
  type::bitboard_t WhiteKnight;
  type::bitboard_t WhiteBishop;
  type::bitboard_t WhiteQueen;
  type::bitboard_t WhiteKing;

  type::bitboard_t BlackPawn;
  type::bitboard_t BlackRook;
  type::bitboard_t BlackKnight;
  type::bitboard_t BlackBishop;
  type::bitboard_t BlackQueen;
  type::bitboard_t BlackKing;

  move_t lastWhiteMove;
  move_t lastBlackMove;

  COLOR playerColor = bitboard::COLOR::WHITE;
  std::vector<std::shared_ptr<bitboard::gameState>> children;

  int halfMoves = 0; // number of moves since last capture or pawn moves, otherwise incremented.
  int fullMoves = 1; // starts at 1, increments after every time black moves.

  int score = 0; // board score

  int gameTreeLevel = 0;

  int possibleSubMoves = 0;

  bool blackQueenCastling = true;
  bool blackKingCastling = true;
  bool whiteQueenCastling = true;
  bool whiteKingCastling = true;

  std::weak_ptr<bitboard::gameState> weakParent; //...
};

struct pieceAttack {
  bitboard_t *WhitePawn;
  bitboard_t *WhiteRook;
  bitboard_t *WhiteKnight;
  bitboard_t *WhiteBishop;
  bitboard_t *WhiteQueen;
  bitboard_t *WhiteKing;

  bitboard_t *BlackPawn;
  bitboard_t *BlackRook;
  bitboard_t *BlackKnight;
  bitboard_t *BlackBishop;
  bitboard_t *BlackQueen;
  bitboard_t *BlackKing;
};

} // END Of bitboard
} // End of david