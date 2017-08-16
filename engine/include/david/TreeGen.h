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
  // Context class
  type::engineContext_ptr engineContextPtr;

  // transposition table
  //std::array<NodeCache, constant::MAXMOVES>

  // Game tree
  std::array<type::gameState_t, (constant::MAXMOVES * constant::MAXDEPTH + /*root*/1)> tree;
  int maxDepth;

  // Table to faster access child index of a parent node
  std::array<int, (constant::MAXDEPTH + 1)> depthIndexes;

  // keep track of game history
  std::string startposFEN;
  std::array<std::string, 300> history; // number of MAX moves in a game
  int historyIndex;

  // Creates EGN moves for each possible move after root node. 1 - 256.
  std::array<std::string, constant::MAXMOVES> EGNMoves; // calculate this after best move, dont waste time.
  int nrOfEGNMoves; // holds the number of moves generated after root to reduce loop check

 public:

  // Constructors
  TreeGen(type::engineContext_ptr ctx);
  TreeGen();

  // Destructor
  ~TreeGen();

  // public methods
  int /*************/ getGameStateScore(const unsigned int index) const;
  int /*************/ getDepth() const;
  void /************/ setRootNode(const type::gameState_t& gs);
  void /************/ updateRootNodeTo(const int index);
  void /************/ setRootNodeFromFEN(const std::string& FEN);
  void /************/ reset();
  void /************/ setMaxDepth(const int depth);
  void /************/ generateNode(const type::gameState_t& p, type::gameState_t& n, const type::gameState_t c);
  uint16_t /********/ generateChildren(const unsigned int index);
  unsigned int /****/ getChildIndex(const unsigned int parent, const unsigned int child) const;
  unsigned int /****/ treeIndex(const uint8_t depth, const uint8_t index) const;
  type::gameState_t&  getGameState(const unsigned int index) const;
  type::gameState_t   getGameStateCopy(const unsigned int index) const;

  // generate EGN moves for root node
  void generateEGNMoves();

  // change root node based on EGN
  void applyEGNMove(const std::string& EGN);

  // sync engine's board track with GUI
  //void syncGameRecord(); // should this take an array of strings, or just the uci param for moves?

};

}
}