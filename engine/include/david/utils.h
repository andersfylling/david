#pragma once


#include "david/types.h"
#include "david/david.h"

#include "fann/floatfann.h"

#include <iostream>
#include <vector>
#include <climits>

namespace david {
namespace utils {
int stoi(std::string v);
int stoi(const char c);

type::bitboard_t  LSB(type::bitboard_t board);
type::bitboard_t  NSB(type::bitboard_t &board);
type::bitboard_t  MSB(type::bitboard_t board);
type::bitboard_t  NSB_r(type::bitboard_t &board);
void              flipBit(type::bitboard_t &board, type::bitboard_t index);
void              flipBitOff(type::bitboard_t &board, type::bitboard_t index);
bool              bitAt(uint64_t b, uint8_t i);

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

std::vector<float> convertGameStateToInputs(type::gameState_ptr node);
fann_type* convertInputsToFannType (std::vector<float> inputs, unsigned long size);
fann_type* boardToFannInputs (type::gameState_ptr node);

std::string generateFen(type::gameState_ptr node);
type::gameState_ptr generateBoardFromFen(const std::string fen);
bool isHalfMove(type::gameState_ptr parent, type::gameState_ptr child);
void setDefaultChessLayout(type::gameState_ptr node);

void printGameState(definitions::gameState_ptr gs);


bitboard::bitboard_t numberOfPieces(bitboard::bitboard_t board);     // For generating right sized arrays
void printBoard(bitboard::bitboard_t board);  // A damn sexy board representation

bitboard::bitboard_t  LSB(bitboard::bitboard_t board);
bitboard::bitboard_t  NSB(bitboard::bitboard_t &board);
bitboard::bitboard_t MSB(bitboard::bitboard_t board);
bitboard::bitboard_t NSB_r(bitboard::bitboard_t &board);

bool bitIsSet(bitboard::bitboard_t board, bitboard::bitboard_t index);
void flipBitOff(bitboard::bitboard_t &board, bitboard::bitboard_t index);
void flipBitOn(bitboard::bitboard_t &board, bitboard::bitboard_t index);

};

}