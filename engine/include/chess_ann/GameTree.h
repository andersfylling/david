//
// Created by anders on 5/5/17.
//

#ifndef CHESS_ANN_GAMETREE_H
#define CHESS_ANN_GAMETREE_H

#include "chess_ann/bitboard.h"
#include <memory>

namespace gameTree {
using ::bitboard::gameState;

typedef std::shared_ptr<gameState> nodePtr;


/**
 * This class instance lives through the whole engine lifetime.
 *
 * Used to generate nodes from a gameState node.
 *
 * Number of levels to be generated or nr of nodes can be specified.
 *
 * This class starts always on the leftmost node, generates children and then moves
 * to the neighbour node to his right. If there are no neighbour node at this level,
 * it's either skipped if the parent discovered chess mate or the generating is finished.
 */
class GameTree {
 private:
  nodePtr previous;// in case a player regrets their move?
  nodePtr current; // this represent the active game board
  int maxLevel;
  int maxNumberOfNodes;
  int maxStoredLevels; // highest level - lowest level (leafNode->gameTreeLevel - currentNode->gameTreeLevel)

 public:
  GameTree();
  GameTree(std::shared_ptr<gameState> node);
  void reset();
  void generateChildrenForNode(std::shared_ptr<gameState> node);
  void setMaxLevel(int level);
  void setMaxStoreLevel(int level);
  void setMaxNumberOfNodes(int level);
  bool hasMaxLevel();
  bool hasMaxStoreLevel();
  bool hasMaxNumberOfNodes();
  int getMaxLevel();
  int getMaxStoreLevel();
  int getMaxNumberOfNodes();
  void generateLevels();
  void generateNodes();
  nodePtr generateNode(nodePtr parent);
  nodePtr getCurrentNode();
  int getNumberOfNodes();

};

}

#endif //CHESS_ANN_GAMETREE_H
