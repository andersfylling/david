//
// Created by anders on 5/5/17.
//

#pragma once
#ifndef CHESS_ANN_UTILS_H
#define CHESS_ANN_UTILS_H

#include <iostream>
// universal
#include <iostream>
#include <climits>
#include "engineInformation.h"
#include "chess_ann/GameTree.h"

namespace utils {
int stoi(std::string v);
int stoi(const char c);
bool bitAt(uint64_t b, uint8_t i);
std::string getAbsoluteProjectPath();
bool isHalfMove(::gameTree::nodePtr parent, ::gameTree::nodePtr child);
std::string generateFen(::gameTree::nodePtr node);
void setDefaultChessLayout(::gameTree::nodePtr node);
};

#endif //CHESS_ANN_UTILS_H

