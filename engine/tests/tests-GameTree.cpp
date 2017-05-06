#include <iostream>
#include "catch.hpp"
#include "chess_ann/GameTree.h"

// gui to engine
TEST_CASE( "Generate nodes", "[GameTree.generateNodes]" ) {
  using gameTree::GameTree;
  using gameTree::nodePtr;
  using bitboard::gameState;

  nodePtr root = std::make_shared<gameState>();

  GameTree gt(root);

  REQUIRE(gt.getNumberOfNodes() == 1);

  gt.generateNodes();

  REQUIRE(gt.getNumberOfNodes() == 100);

  gt.setMaxNumberOfNodes(20000);
  gt.generateNodes();

  REQUIRE(gt.getNumberOfNodes() == 20000);

  gt.reset();
  REQUIRE(gt.getNumberOfNodes() == 0);

  // gt.setMaxNumberOfNodes(100000000); // When you feel too happy and need some sadness in life
  // gt.generateNodes();
  // REQUIRE(gt.getNumberOfNodes() == 100000101);

}