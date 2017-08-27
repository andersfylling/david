#pragma once

#include "david/david.h"

#include "david/types.h"
#include "david/bitboard.h"

#include "fann/floatfann.h"

#include <iostream>
#include <vector>
#include <climits>

namespace utils {

// converters
inline namespace convert {
int stoi(const std::string v);
constexpr int ctoi(const char c) {
  return c == ' ' ? 0 : c - '0';
}

void legacyGameStateUpdate(::david::type::gameState_t& n);
}


/**
 * Finds the index of the first active bit, starting from right to left.
 * LSB: Least Significant Bit
 *
 * @param board type::bitboard_t
 * @return index[0, 63], if board is 0, 0 is returned.
 */
constexpr uint8_t LSB(const uint64_t board) {
#ifdef __linux__
  return (board > 0) ? __builtin_ffsl(board) - 1 : 0;
#else
  for (int i = 0; i < 64; i++) {
    if (::utils::bitAt(board, i)) {
      return i;
    }
  }
#endif
}
/**
* Deactivates the smallest active bit, and returns the LSB value.
*
* @param board type::bitboard_t
* @return index[0, 63], if the board is 0, 0 is returned
*/
constexpr uint8_t NSB(uint64_t &board) {
  board &= ~(1ULL << LSB(board));
  return LSB(board);
}
constexpr uint8_t NSB(uint64_t &board, const uint8_t index) {
  board &= ~(1ULL << index);
  return LSB(board);
}

constexpr uint8_t MSB(const uint64_t board) {
#ifdef __linux__
  return 63 - __builtin_clzll(board);
#elif _WIN32
  // windows code goes here
  return 0LL;
#elif _WIN64
  // windows code goes here
  return 0LL;
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return 0LL;
#endif
}
constexpr uint8_t NSB_r(uint64_t &board) {
  board &= ~(1LL << MSB(board));
  return MSB(board);
}

constexpr void flipBit(uint64_t &board, const uint8_t index) {
  board |= (1ULL << index);
}

constexpr uint8_t nrOfActiveBits(const uint64_t b) {
#ifdef __linux__
  return __builtin_popcountll(b);
#elif _WIN32
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#elif _WIN64
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#endif
}
constexpr bool bitAt(const uint64_t b, const uint8_t i) {
  return (b & (1ULL << i)) != 0;
}
constexpr uint64_t activateBit (const uint64_t board, const uint8_t index) {
  return board | (1ULL << index);
}
constexpr uint64_t indexToBitboard (const uint8_t index) {
  return 1ULL << index;
}

constexpr void flipBitOff(uint64_t &board, const uint64_t index) {
  board &= ~(1ULL << index);
}
constexpr uint64_t flipBitOffCopy(const uint64_t board, const uint8_t index) {
  return board & ~(1ULL << index);
}
constexpr void flipBitOn(uint64_t &board, const uint64_t index) {
  board |= (1LL << index);
}
constexpr uint64_t flipBitOnCopy(const uint64_t board, const uint8_t index) {
  return board | (1LL << index);
}

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

template<std::size_t SIZE>
void addPieceBoardIndexToVector(std::vector<float>& store, std::array<::david::type::bitboard_t, SIZE>& pieces, uint8_t nr);

inline namespace neuralnet {
std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(const ::david::type::gameState_t &node);
}

std::string generateFen(const ::david::type::gameState_t& node);
void generateBoardFromFen(::david::type::gameState_t& gs, const std::string& fen);
void setDefaultChessLayout(::david::type::gameState_t& node);
::david::type::bitboard_t chessIndexToBitboard(const std::string& chessIndex);
uint8_t chessIndexToArrayIndex(const std::string& chessIndex);
void affectGameStateByEGNMove(::david::type::gameState_t& gs, const std::string& EGN);
void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest);


// define the params based on what movegen class is used
#ifdef MOVEGEN
inline bool isHalfMove(
    const uint64_t parentActivePieces,
    const uint64_t childActivePieces,
    const uint64_t parentActivePawns,
    const uint64_t parentOtherPawns,
    const uint64_t childActivePawns,
    const uint64_t childOtherPawns
) {
  // This counter is reset after captures or pawn moves, and incremented otherwise.
  // Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  // including castling itself, increment the Halfmove Clock.
  // However, those moves are irreversible in the sense to reverse the same rights -
  // since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

  // check if parent or child is overlapping, AKA. a piece has been captured.
  if ((parentActivePieces & childActivePieces) != 0ULL) {
    return false;
  }

  // check if the ---- pawns has moved
  if ((parentActivePawns | childOtherPawns) != 0ULL) {
    return false;
  }

  // check if !---- pawns has moved
  //if ((parent.WhitePawn | child.WhitePawn) != 0ULL) {
  //  return false;
  //}
  //
  // no rules of half moving were broken
  //return true;
  return (parentOtherPawns | childActivePawns) != 0ULL;
}
#else
bool isHalfMove(const ::david::type::gameState_t& parent, const ::david::type::gameState_t& child);
#endif

// Everything printing related
// utils::print
// utils::cout
// utils::print::cout
inline namespace print {
inline namespace cout {
void printGameState(const ::david::type::gameState_t &gs);
void printBoard(uint64_t board, const int index = -1);
}
}

void generateMergedBoardVersion(::david::type::gameState_t& gs);

void yellDeprecated(const std::string info);

bool perft();
bool perft(const int limit, const uint8_t startDepth = 0);
uint64_t perft(const uint8_t depth, const ::david::type::gameState_t& gs, std::array<int, 6>& results);
const std::string getEGN(const ::david::type::gameState_t& first, const ::david::type::gameState_t& second);
void getEGN(const ::david::type::gameState_t& first, const ::david::type::gameState_t& second, std::string& EGN);


/**
 * Converts a string into an bit array
 *
 * @param desiredByteLook
 * @return
 */
//constexpr uint8_t stringTo8bitArray(const char desiredByteLook[]) {
//  const int len = sizeof(desiredByteLook) / sizeof(desiredByteLook[0]);
//  if (len > 8) {
//    return 0;
//  }
//
//  uint8_t arr = 0;
//
//
//  for (size_t i = 0; i < len; i++) {
//    const char c = desiredByteLook[i];
//
//    if (c == '0') {
//      continue;
//    }
//
//    arr |= (1 << i); // same as flipBitOn
//  }
//
//  return arr;
//}

/**
 * Generate all knight attacks (psuedo) based on index / position
 */
constexpr ::std::array<uint64_t, 64> compileKnightAttacks() {
  ::std::array<uint64_t, 64> attacks{};

  // directions a rook can move
  const std::array<int8_t, 4> directions = {1, -1};

  for (uint8_t i = 0; i < 64; i++) {

    // the different board limits related to directions
    uint8_t row = i / 8;
    uint8_t col = i % 8;
    const std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

    uint64_t result = 0ULL;

    for (uint8_t ii = 0; ii < 2; ii++) {
      const int8_t directionY = directions[ii];
      // up / down
      for (uint8_t y = 1; y < 3; y++) {
        // make sure the knight doesnt go too high or too "low"
        const auto heightPush = row + (directionY * y);
        if (heightPush > 7 || heightPush < 0) {
          continue;
        }

        // how far to the side should the knight move, based on its desired height move
        uint8_t colOffset = 3 - y; // if y == 1, then move 2 pos to the side, y == 2 move 1 pos to the side

        for (uint8_t x = 0; x < 2; x++) {
          const int8_t directionX = directions[x];
          // make sure it doesn't go too far to the sides
          const auto sidePush = col + (directionX * colOffset);
          if (sidePush > 7 || sidePush < 0) {
            continue;
          }

          // add it to the possible moves
          result |= ::utils::indexToBitboard(i + (directionX * colOffset) + (directionY * 8 * y));
        }
      }
    }

    attacks[i] = result;
  }

  return attacks;
};

/**
 * Generate all horizontal attack paths
 */
constexpr ::std::array<uint64_t, 64> compileHorizontalAttacks() {
  ::std::array<uint64_t, 64> attacks{};

  const uint64_t bottom = 255;

  for (uint8_t i = 0; i < 64; i++) {
    attacks[i] = bottom << (8 * (i / 8));
    attacks[i] ^= ::utils::indexToBitboard(i);
  }

  return attacks;
};

/**
 * Generate all vertical attack paths
 */
constexpr ::std::array<uint64_t, 64> compileVerticalAttacks() {
  ::std::array<uint64_t, 64> attacks{};

  const uint64_t right = 72340172838076673;

  for (uint8_t i = 0; i < 64; i++) {
    attacks[i] = right << (i % 8);
    attacks[i] ^= ::utils::indexToBitboard(i);
  }

  return attacks;
};

/**
 * Generate all horizontal/vertical attacks
 */
constexpr ::std::array<uint64_t, 64> compileRookAttacks() {
  ::std::array<uint64_t, 64> attacks{};

  const auto h = ::utils::compileHorizontalAttacks();
  const auto v = ::utils::compileVerticalAttacks();

  for (uint8_t i = 0; i < 64; i++) {
    attacks[i] = h[i] ^ v[i];
  }

  return attacks;
};

constexpr ::std::array<uint64_t, 64> compilePawnAttacks () {
  ::std::array<uint64_t, 64> attacks{};

  for (uint8_t i = 0; i < 64; i++) {
    auto board = 0ULL;
    const auto piece = ::utils::indexToBitboard(i);

    if ((71775015237779198 & piece) > 0) {
      // shift right and up
      board |= piece << (8 - 1);
    }

    if ((35887507618889599 & piece) > 0) {
      // shift up and left
      board |= piece << (8 + 1);
    }

    if ((9187201950435737344 & piece) > 0) {
      // shift down and left
      board |= piece >> (8 - 1);
    }

    if ((18374403900871474688 & piece) > 0) {
      // shift down and right
      board |= piece >> (8 + 1);
    }

    attacks[i] = board;
  }

  return attacks;
};


constexpr ::std::array<uint64_t, 64> compileDUAttacks () {
  ::std::array<uint64_t, 64> attacks{};

  // ->, down to up
  const std::array<uint64_t, 15> duAttackPaths = {
      0,
      4647714815446351872,
      2323998145211531264,
      1161999622361579520,
      580999813328273408,
      290499906672525312,
      145249953336295424,
      72624976668147840, // middle
      283691315109952,
      1108169199648,
      4328785936,
      16909320,
      66052,
      258,
      0
  };

  for (uint8_t i = 0; i < 64; i++) {
    auto board = duAttackPaths[14 - (i % 8) - (i / 8)];
    const auto piece = ::utils::indexToBitboard(i);

    if (board == 0) {
      attacks[i] = 0ULL;
    }
    else {
      attacks[i] = board ^ piece;
    }
  }

  return attacks;
};


constexpr ::std::array<uint64_t, 64> compileUDAttacks () {
  ::std::array<uint64_t, 64> attacks{};

  // ->, up to down
  const std::array<uint64_t, 15> udAttackPaths = {
      0,
      32832,
      8405024,
      2151686160,
      550831656968,
      141012904183812,
      36099303471055874,
      9241421688590303745, // middle diagonal
      4620710844295151872,
      2310355422147575808,
      1155177711073755136,
      577588855528488960,
      288794425616760832,
      144396663052566528,
      0
  };

  for (uint8_t i = 0; i < 64; i++) {
    auto board = udAttackPaths[7 - (i % 8) + (i / 8)];
    const auto piece = ::utils::indexToBitboard(i);

    if (board == 0) {
      attacks[i] = 0ULL;
    }
    else {
      attacks[i] = board ^ piece;
    }
  }

  return attacks;
};




namespace constant {
const ::std::array<uint64_t, 64> knightAttackPaths = ::utils::compileKnightAttacks();

const ::std::array<uint64_t, 64> rookAttackPaths = ::utils::compileRookAttacks();

// horizontal attack paths
const ::std::array<uint64_t, 64> horizontalAttackPaths = ::utils::compileHorizontalAttacks();

// Vertical attack paths
const ::std::array<uint64_t, 64> verticalAttackPaths = ::utils::compileVerticalAttacks();

// diagonal attack paths, from down to up, going left to right
const ::std::array<uint64_t, 64> diagonalDUAttackPaths = ::utils::compileDUAttacks();

// diagonal attack paths, from up to down, going left to right
const ::std::array<uint64_t, 64> diagonalUDAttackPaths = ::utils::compileUDAttacks();

// pawn attacks
const ::std::array<uint64_t, 64> pawnAttackPaths = ::utils::compilePawnAttacks();
}

} // utils