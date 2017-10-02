#pragma once

#include <array>
#include "david/types.h"
#include "david/david.h"

namespace david {
namespace bitboard {

// 63 62 61 60 59 58 57 56
// 55 54 53 52 51 50 49 48
// 47 46 45 44 43 42 41 40
// 39 38 37 36 35 34 33 32
// 31 30 29 28 27 26 25 24
// 23 22 21 20 19 18 17 16
// 15 14 13 12 11 10  9  8
//  7  6  5  4  3  2  1  0


// Each game state is represented by a struct of
// bitboards. A tree of moves will be made up by
struct gameState {
  uint_fast8_t halfMoves = 0; // number of moves since last capture or pawn moves, otherwise incremented.
  uint_fast16_t fullMoves = 1; // starts at 1, increments after every time black moves.

  int score = ::david::constant::boardScore::LOWEST; // board score

  uint_fast8_t gameTreeLevel = 0;
  uint_fast8_t possibleSubMoves = 0; // is used by an iterator since everything is preinitialized, can by uint8

  std::array<type::bitboard_t, 2> piecess = {::david::constant::EMPTYBOARD, ::david::constant::EMPTYBOARD};
  type::bitboard_t combinedPieces         = ::david::constant::EMPTYBOARD;

  // same as version 2, just easier to loop through
  std::array<std::array<type::bitboard_t, 2>, 6> piecesArr = {{0}}; // version 3

  uint_fast8_t enPassant = 0; // if a pawn moves by two blocks, add the passant index here. where the attack piece will stand.
  uint_fast8_t enPassantPawn = 0; // where the pawn stands, will be one up or down from enPassant.

  bool isWhite = true;
  ::std::array<bool, 2> queenCastlings  = {true, true};
  ::std::array<bool, 2> kingCastlings   = {true, true};
  uint_fast8_t castling = 0b00001111;

  bool passant = false;

#ifdef DAVID_TEST
  bool isInCheck = false;
  bool promoted = false;
  bool castled = false;
#endif
};

} // END Of bitboard
} // End of david