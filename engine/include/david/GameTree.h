#pragma once

#include "david/david.h"
#include "david/bitboard.h"
#include <memory>
#include <iomanip>
#include <array>
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
  type::gameState_ptr root; // start of the game
  type::gameState_ptr current; // current game state
  int maxNumberOfNodes; // nodes in memory

  void getNumberOfNodes(type::gameState_ptr node, int &counter);
  void getDepth(type::gameState_ptr node, int &depth);

  type::engineContext_ptr engineContextPtr;

  // transposition table
  //std::array<NodeCache, constant::MAXMOVES>


  std::array<type::gameState_t, (constant::MAXMOVES * /*depth*/20 + 1/*root*/)> tree;
  std::vector<type::gameState_ptr> treeDepths; // index i, will go to depth i at the first node. to simplify deleting.
  void preallocateSearchTree();

  int maxDepth;

 public:
  GameTree(type::engineContext_ptr ctx);
  ~GameTree();
  void setRootNodeFromFEN(const std::string& FEN);
  void reset();
  void setMaxDepth(int depth);
  unsigned int getChildIndex(unsigned int parent, unsigned int child);
  int getGameStateScore(unsigned int index);
  type::gameState_t& getGameState(unsigned int index);
  unsigned int treeIndex(uint8_t depth, uint8_t index);
  //void reset(type::gameState_ptr node);
  //void resetChildren(type::gameState_ptr node);
  //void newRootNode(type::gameState_ptr node);
  //type::gameState_ptr regretNewRootNode();
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  //void generateNodes();
  //type::gameState_ptr generateNode(type::gameState_ptr parent, bitboard::gameState child);
  void generateNode(type::gameState_t& parent, type::gameState_t& n, type::gameState_t child);
  //type::gameState_ptr getCurrentNode();
  int getNumberOfNodes();
  //void generateChildren(type::gameState_ptr node);
  void generateChildren(unsigned int index);
  //void sortChildren(type::gameState_ptr node);
  void sortChildren(type::gameState_t& node);
  type::gameState_ptr getCurrent();
  int getDepth();
  //void printAllScores(type::gameState_ptr root);

};

}
}