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

std::vector<float> convertGameStateToInputs(const type::gameState_t& node);
fann_type* convertInputsToFannType (const std::vector<float>& inputs, unsigned long size);
fann_type* boardToFannInputs (const type::gameState_t& node);

std::string generateFen(type::gameState_ptr node);
type::gameState_ptr generateBoardFromFen(const std::string fen);
void generateBoardFromFen(type::gameState_t& gs, const std::string& fen);
bool isHalfMove(type::gameState_t& parent, type::gameState_t& child);
void setDefaultChessLayout(type::gameState_ptr node);
void setDefaultChessLayout(type::gameState_t& node);
type::bitboard_t chessIndexToBitboard(const std::string& chessIndex);
uint8_t chessIndexToArrayIndex(const std::string& chessIndex);

void printGameState(type::gameState_ptr gs);
void printGameState(type::gameState_t& gs);


int nrOfActiveBits(type::bitboard_t b);
void printBoard(type::bitboard_t board);  // A damn sexy board representation

type::bitboard_t  LSB(type::bitboard_t board);
type::bitboard_t  NSB(type::bitboard_t &board);
type::bitboard_t MSB(type::bitboard_t board);
type::bitboard_t NSB_r(type::bitboard_t &board);

void flipBitOff(type::bitboard_t &board, type::bitboard_t index);
void flipBitOn(type::bitboard_t &board, type::bitboard_t index);

void yellDeprecated(const std::string info);

};

}