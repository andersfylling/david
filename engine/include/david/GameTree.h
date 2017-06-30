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
  type::scoreNode_ptr root; // start of the game
  type::scoreNode_ptr current; // current game state
  int maxNumberOfNodes; // nodes in memory

  void getNumberOfNodes(type::scoreNode_ptr node, int &counter);
  void getDepth(type::scoreNode_ptr node, int &depth);

  type::engineContext_ptr engineContextPtr;

 public:
  GameTree(type::engineContext_ptr ctx);
  GameTree(type::engineContext_ptr ctx, type::gameState_ptr node);
  ~GameTree();
  void reset();
  //void reset(type::gameState_ptr node);
  //void resetChildren(type::gameState_ptr node);
  //void newRootNode(type::gameState_ptr node);
  //type::gameState_ptr regretNewRootNode();
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  //void generateNodes();
  //type::gameState_ptr generateNode(type::gameState_ptr parent, bitboard::gameState child);
  void generateNode(type::scoreNode_ptr parent, type::gameState_t child, int index);
  //type::gameState_ptr getCurrentNode();
  int getNumberOfNodes();
  //void generateChildren(type::gameState_ptr node);
  void generateChildren(type::scoreNode_ptr node);
  //void sortChildren(type::gameState_ptr node);
  void sortChildren(type::scoreNode_ptr node);
  type::scoreNode_ptr getCurrent();
  int getDepth();
  //void printAllScores(type::gameState_ptr root);

};

}
}