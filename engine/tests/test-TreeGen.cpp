#include <iostream>
#include <david/utils/utils.h>
#include "david/TreeGen.h"
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

#ifndef MOVEGEN
TEST_CASE("Retrieving a correctly set gameState_t [TreeGen::getGameState]") {
  using ::david::gameTree::TreeGen;

  TreeGen tg{}; // testing constructor
  auto& gs = tg.getGameState(0);

  REQUIRE(gs.pieces == 0ULL);
  REQUIRE(gs.blackPieces == 0ULL);
  REQUIRE(gs.whitePieces == 0ULL);
}

TEST_CASE("Setting a gameState_t and verified that it's copied correctly [TreeGen::setGameState]") {
  using ::david::gameTree::TreeGen;
  using ::david::type::gameState_t;
  using ::utils::setDefaultChessLayout;

  TreeGen tg{}; // testing constructor
  auto& gs = tg.getGameState(0);


  REQUIRE(gs.pieces == 0ULL);
  REQUIRE(gs.blackPieces == 0ULL);
  REQUIRE(gs.whitePieces == 0ULL);

  // set new data
  gameState_t g;
  ::utils::setDefaultChessLayout(g);
  tg.setRootNode(g);

  // verify data changes
  REQUIRE(gs.blackPieces == g.blackPieces);
  REQUIRE(gs.whitePieces == g.whitePieces);
  REQUIRE(gs.pieces == g.pieces);
  REQUIRE(gs.blackKingCastling == g.blackKingCastling);

  // verify that it's copied when changing the root node!
  {
    gameState_t g2;
    g2.blackKingCastling = false;
    tg.setRootNode(g2);
  }

  REQUIRE(!gs.blackKingCastling);
  REQUIRE(gs.pieces == 0ULL);
  REQUIRE(gs.blackPieces == 0ULL);
  REQUIRE(gs.whitePieces == 0ULL);
}

TEST_CASE("Verify the index math [TreeGen::treeIndex]") {
  using ::david::gameTree::TreeGen;
  using ::david::constant::MAXMOVES;

  TreeGen tg{}; // testing constructor

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

#endif