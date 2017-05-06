#include "catch.hpp"
#include "chess_ann/GameTree.h"

// gui to engine
TEST_CASE( "Generate 10 nodes", "[GameTree.generateNodes]" ) {
  using gameTree::GameTree;
  using gameTree::nodePtr;
  using bitboard::gameState;

  nodePtr root = std::make_shared<gameState>();

  GameTree gt(root);
}