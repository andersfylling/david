#pragma once

#include "david/bitboard.h"
#include <memory>
#include <iomanip>
#include "david/EngineContext.h"

namespace david {
namespace gameTree {

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
  // previous can be used for en passant
  definitions::gameState_ptr previous;// in case a player regrets their move?
  definitions::gameState_ptr current; // this represent the active game board
  int maxNumberOfNodes; // nodes in memory

  void getNumberOfNodes(definitions::gameState_ptr node, int &counter);
  void getDepth(definitions::gameState_ptr node, int &depth);

  definitions::engineContext_ptr engineContextPtr;

 public:
  GameTree(definitions::engineContext_ptr ctx);
  GameTree(definitions::engineContext_ptr ctx, definitions::gameState_ptr node);
  ~GameTree();
  void reset();
  void reset(definitions::gameState_ptr node);
  void resetChildren(definitions::gameState_ptr node);
  void newRootNode(definitions::gameState_ptr node);
  definitions::gameState_ptr regretNewRootNode();
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  void generateNodes();
  definitions::gameState_ptr generateNode(definitions::gameState_ptr parent, bitboard::gameState child);
  definitions::gameState_ptr getCurrentNode();
  int getNumberOfNodes();
  void generateChildren(definitions::gameState_ptr node);
  void sortChildren(definitions::gameState_ptr node);
  int getDepth();
  void printAllScores(definitions::gameState_ptr root);

};

}
}