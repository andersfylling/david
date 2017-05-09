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

  SECTION("Verifying that only one node exists when a GameTree is initiated") {
    REQUIRE(gt.getNumberOfNodes() == 1);
  }


  SECTION("Setting max nr of nodes to 100 and generating them") {
    gt.setMaxNumberOfNodes(100);
    gt.generateNodes();

    REQUIRE(gt.getNumberOfNodes() == 100);
  }

  SECTION("Setting max nr of nodes to 1000 and generating them") {
    gt.setMaxNumberOfNodes(1000);
    gt.generateNodes();

    REQUIRE(gt.getNumberOfNodes() == 1000);
  }

  SECTION("Removing all nodes from game tree") {
    gt.reset();
    REQUIRE(gt.getNumberOfNodes() == 0);
  }

  // gt.setMaxNumberOfNodes(100000000); // When you feel too happy and need some sadness in life
  // gt.generateNodes();
  // REQUIRE(gt.getNumberOfNodes() == 100000101);

}