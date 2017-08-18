#pragma once


#include "david/types.h"
#include "david/david.h"

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

uint8_t LSB(::david::type::bitboard_t board);
::david::type::bitboard_t  NSB(::david::type::bitboard_t &board);
::david::type::bitboard_t  MSB(::david::type::bitboard_t board);
::david::type::bitboard_t  NSB_r(::david::type::bitboard_t &board);
void              flipBit(::david::type::bitboard_t &board, ::david::type::bitboard_t index);
void              flipBitOff(::david::type::bitboard_t &board, ::david::type::bitboard_t index);
int               nrOfActiveBits(const ::david::type::bitboard_t b);
bool              bitAt(const uint64_t b, const uint8_t i);

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

template<std::size_t SIZE>
void addPieceBoardIndexToVector(std::vector<float>& store, std::array<::david::type::bitboard_t, SIZE>& pieces, uint8_t nr);

inline namespace neuralnet {
std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(const ::david::type::gameState_t &node);
}

std::string generateFen(const ::david::type::gameState_t& node);
void generateBoardFromFen(::david::type::gameState_t& gs, const std::string& fen);
bool isHalfMove(const ::david::type::gameState_t& parent, const ::david::type::gameState_t& child);
void setDefaultChessLayout(::david::type::gameState_t& node);
::david::type::bitboard_t chessIndexToBitboard(const std::string& chessIndex);
uint8_t chessIndexToArrayIndex(const std::string& chessIndex);
void affectGameStateByEGNMove(::david::type::gameState_t& gs, const std::string& EGN);
void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest);

// Everything printing related
// utils::print
// utils::cout
// utils::print::cout
inline namespace print {
inline namespace cout {
void printGameState(const ::david::type::gameState_t &gs);
void printBoard(::david::type::bitboard_t board);
}
}

void flipBitOff(::david::type::bitboard_t &board, ::david::type::bitboard_t index);
void flipBitOn(::david::type::bitboard_t &board, ::david::type::bitboard_t index);

void yellDeprecated(const std::string info);

bool perft();
bool perft(const int limit);
uint64_t perft(const int depth, const ::david::type::gameState_t& gs);
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


} // utils