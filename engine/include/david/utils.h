#pragma once


#include "david/bitboard.h"
#include "david/definitions.h"
#include "david/engineInformation.h"

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

fann_type *convertGameStateToInputs(definitions::gameState_ptr node, bitboard::COLOR color);

std::string generateFen(definitions::gameState_ptr node);
bool isHalfMove(definitions::gameState_ptr parent, definitions::gameState_ptr child);
void setDefaultChessLayout(definitions::gameState_ptr node);

void printGameState(definitions::gameState_ptr gs);
};

}