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
class TreeGen {
 private:
  // previous can be used for en passant
  int maxNumberOfNodes; // nodes in memory
  std::array<int, (constant::MAXDEPTH + 1)> depthIndexes;

  void getNumberOfNodes(type::gameState_ptr node, int &counter);
  void getDepth(type::gameState_ptr node, int &depth);

  type::engineContext_ptr engineContextPtr;

  // transposition table
  //std::array<NodeCache, constant::MAXMOVES>

  //std::vector<type::gameState_t> history;
  std::array<type::gameState_t, (constant::MAXMOVES * constant::MAXDEPTH + /*root*/1)> tree;
  int maxDepth;

  // keep track of game history
  std::string startposFEN;
  std::vector<std::string> history;

  // Creates EGN moves for each possible move after root node. 1 - 256.
  std::array<std::string, constant::MAXMOVES> EGNMoves; // calculate this after best move, dont waste time.
  int nrOfEGNMoves; // holds the number of moves generated after root to reduce loop check

 public:
  TreeGen(type::engineContext_ptr ctx);
  TreeGen();
  ~TreeGen();
  void setRootNode(const type::gameState_t& gs);
  void updateRootNodeTo(int index);
  void setRootNodeFromFEN(const std::string& FEN);
  void reset();
  void setMaxDepth(int depth);
  unsigned int getChildIndex(unsigned int parent, unsigned int child);
  int getGameStateScore(unsigned int index);
  type::gameState_t& getGameState(unsigned int index);
  unsigned int treeIndex(uint8_t depth, uint8_t index);
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  void generateNode(type::gameState_t& parent, type::gameState_t& n, type::gameState_t child);
  int getNumberOfNodes();
  void generateChildren(unsigned int index);
  void sortChildren(type::gameState_t& node);
  type::gameState_ptr getCurrent();
  int getDepth();

  // generate EGN moves for root node
  void generateEGNMoves();

  // change root node based on EGN
  void applyEGNMove(const std::string& EGN);

  // sync engine's board track with GUI
  //void syncGameRecord(); // should this take an array of strings, or just the uci param for moves?

};

}
}