#pragma once


#include "david/types.h"
#include "david/david.h"

#include "fann/floatfann.h"

#include <iostream>
#include <vector>
#include <climits>

namespace david {
namespace utils {

// converters
inline namespace convert {
int stoi(const std::string v);
constexpr int ctoi(const char c) {
  return c == ' ' ? 0 : c - '0';
}
}

type::bitboard_t  LSB(type::bitboard_t board);
type::bitboard_t  NSB(type::bitboard_t &board);
type::bitboard_t  MSB(type::bitboard_t board);
type::bitboard_t  NSB_r(type::bitboard_t &board);
void              flipBit(type::bitboard_t &board, type::bitboard_t index);
void              flipBitOff(type::bitboard_t &board, type::bitboard_t index);
int               nrOfActiveBits(const type::bitboard_t b);
bool              bitAt(const uint64_t b, const uint8_t i);

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

template<std::size_t SIZE>
void addPieceBoardIndexToVector(std::vector<float>& store, std::array<type::bitboard_t, SIZE>& pieces, uint8_t nr);

inline namespace neuralnet {
std::vector<float> convertGameStateToVectorInputs(const type::gameState_t &node);
fann_type* convertVectorInputsToFannType(const std::vector<float> &inputs, unsigned long size);
fann_type* boardToFannInputs(const type::gameState_t& node);
}

std::string generateFen(type::gameState_ptr node);
void generateBoardFromFen(type::gameState_t& gs, const std::string& fen);
bool isHalfMove(type::gameState_t& parent, type::gameState_t& child);
void setDefaultChessLayout(type::gameState_t& node);
type::bitboard_t chessIndexToBitboard(const std::string& chessIndex);
uint8_t chessIndexToArrayIndex(const std::string& chessIndex);
void affectGameStateByEGNMove(type::gameState_t& gs, const std::string& EGN);
void movePiece(type::bitboard_t& board, uint8_t orig, uint8_t dest);

// Everything printing related
// utils::print
// utils::cout
// utils::print::cout
inline namespace print {
inline namespace cout {
void printGameState(const type::gameState_t &gs);
void printBoard(type::bitboard_t board);
}
}

void flipBitOff(type::bitboard_t &board, type::bitboard_t index);
void flipBitOn(type::bitboard_t &board, type::bitboard_t index);

void yellDeprecated(const std::string info);

bool perft();
bool perft(const int limit);
uint64_t perft(const int depth, const type::gameState_t& gs);
const std::string getEGN(const type::gameState_t& first, const type::gameState_t& second);
void getEGN(const type::gameState_t& first, const type::gameState_t& second, std::string& EGN);

};

}