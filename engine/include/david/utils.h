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
int stoi(std::string v);
int stoi(const char c);

bool bitAt(uint64_t b, uint8_t i);

std::string getAbsoluteProjectPath();
bool fileExists(const std::string &file);

fann_type *convertGameStateToInputs(type::gameState_ptr node, bitboard::COLOR color);

std::string generateFen(type::gameState_ptr node);
bool isHalfMove(type::gameState_ptr parent, type::gameState_ptr child);
void setDefaultChessLayout(type::gameState_ptr node);

void printGameState(type::gameState_ptr gs);
};

}