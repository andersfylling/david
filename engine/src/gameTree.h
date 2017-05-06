//
// Created by anders on 5/5/17.
//

#ifndef CHESS_ANN_GAMETREE_H
#define CHESS_ANN_GAMETREE_H

#include "chess_ann/bitboard.h"

namespace gameTree {
using ::bitboard::gameState;

class gameTree {
 private:
  gameState* rootNode;

 public:
  gameState* st;
};

}

#endif //CHESS_ANN_GAMETREE_H
