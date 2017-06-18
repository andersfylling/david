#pragma once


#include "david/bitboard.h"
#include "david/types.h"
#include "david/david.h"

#include "fann/floatfann.h"

#include <iostream>
#include <iostream>
#include <climits>

namespace david {
namespace utils {
constexpr int stoi(std::string v);
constexpr int stoi(const char c);

constexpr bool bitAt(uint64_t b, uint8_t i);

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

std::vector<float> convertGameStateToInputs(type::gameState_ptr node);
fann_type* convertInputsToFannType (std::vector<float> inputs, unsigned long size);
fann_type* boardToFannInputs (type::gameState_ptr node);

std::string generateFen(type::gameState_ptr node);
bool isHalfMove(type::gameState_ptr parent, type::gameState_ptr child);
void setDefaultChessLayout(type::gameState_ptr node);

void printGameState(type::gameState_ptr gs);
};

}