#include <iostream>
#include <david/utils/utils.h>
#include <david/utils/gameState.h>
#include "david/TreeGen.h"
#include "david/ANN/ANN.h"
#include "catch.hpp"


/*
 *
 * TreeGen(type::engineContext_ptr ctx);
 * TreeGen();
 * ~TreeGen();
 * void setRootNode(const type::gameState_t& gs);
  void updateRootNodeTo(int index);
  void setRootNodeFromFEN(const std::string& FEN);
  void reset();
  void setMaxDepth(int depth);
 * unsigned int getChildIndex(unsigned int parent, unsigned int child);
  int getGameStateScore(unsigned int index);
 * type::gameState_t& getGameState(unsigned int index);
  unsigned int treeIndex(uint8_t depth, uint8_t index);
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  void generateNode(type::gameState_t& parent, type::gameState_t& n, type::gameState_t child);
  int getNumberOfNodes();
  void generateChildren(unsigned int index);
  void sortChildren(type::gameState_t& node);
  type::gameState_ptr getCurrent();
  int getDepth();
 */

TEST_CASE("Retrieving a correctly set gameState_t [TreeGen::getGameState]") {
  using ::david::gameTree::TreeGen;
  using ::david::ANN;

  ANN nn{};
  TreeGen tg{nn};
  auto& gs = tg.getGameState(0);

  REQUIRE(gs.combinedPieces == 0ULL);
  REQUIRE(gs.piecess[0] == 0ULL);
  REQUIRE(gs.piecess[1] == 0ULL);
}

TEST_CASE("Setting a gameState_t and verified that it's copied correctly [TreeGen::setGameState]") {
  using ::david::gameTree::TreeGen;
  using ::david::type::gameState_t;
  using ::david::ANN;

  ANN nn{};
  TreeGen tg{nn};
  auto& gs = tg.getGameState(0);


  REQUIRE(gs.combinedPieces == 0ULL);
  REQUIRE(gs.piecess[0] == 0ULL);
  REQUIRE(gs.piecess[1] == 0ULL);

  // set new data
  gameState_t g;
  ::utils::gameState::setDefaultChessLayout(g);
  tg.setRootNode(g);

  // verify data changes
  REQUIRE(gs.piecess[0] == g.piecess[0]);
  REQUIRE(gs.piecess[1] == g.piecess[1]);
  REQUIRE(gs.combinedPieces == g.combinedPieces);
  REQUIRE(gs.kingCastlings[1] == g.kingCastlings[1]);

  // verify that it's copied when changing the root node!
  {
    gameState_t g2;
    g2.kingCastlings[1] = false;
    tg.setRootNode(g2);
  }

  REQUIRE(!gs.kingCastlings[1]);
  REQUIRE(gs.combinedPieces == 0ULL);
  REQUIRE(gs.piecess[0] == 0ULL);
  REQUIRE(gs.piecess[1] == 0ULL);
}

TEST_CASE("Verify the index math [TreeGen::treeIndex]") {
  using ::david::gameTree::TreeGen;
  using ::david::constant::MAXMOVES;
  using ::david::ANN;

  ANN nn{};
  TreeGen tg{nn};

  // parent 0 ... N
  REQUIRE(tg.getChildIndex(0, 0) == 1);
  REQUIRE(tg.getChildIndex(0, 1) == 2);
  REQUIRE(tg.getChildIndex(1, 0) == MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(1, 1) == MAXMOVES + 2);
  REQUIRE(tg.getChildIndex(2, 0) == MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(2, 1) == MAXMOVES + 2);
  REQUIRE(tg.getChildIndex(MAXMOVES + 1, 0) == 2*MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(MAXMOVES + 1, 1) == 2*MAXMOVES + 2);
  REQUIRE(tg.getChildIndex(MAXMOVES + 2, 0) == 2*MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(MAXMOVES + 2, 1) == 2*MAXMOVES + 2);
  REQUIRE(tg.getChildIndex(2*MAXMOVES + 1, 0) == 3*MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(2*MAXMOVES + 1, 1) == 3*MAXMOVES + 2);
  REQUIRE(tg.getChildIndex(2*MAXMOVES + 2, 0) == 3*MAXMOVES + 1);
  REQUIRE(tg.getChildIndex(2*MAXMOVES + 2, 1) == 3*MAXMOVES + 2);
}