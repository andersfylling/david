#include <iostream>
#include <david/utils.h>
#include "david/GameTree.h"
#include "catch.hpp"


TEST_CASE( "Generate nodes", "[GameTree.generateNodes]" ) {

//  auto context = std::make_shared<david::EngineContext>();
//  david::type::gameState_ptr root = std::make_shared<david::bitboard::gameState>();
//  david::utils::setDefaultChessLayout(root);
//
//  david::gameTree::GameTree gt(context, root);
//
// // SECTION("Verifying that only one node exists when a GameTree is initiated") {
//    REQUIRE(gt.getNumberOfNodes() == 1);
// // }
//
//
//  gt.setMaxNumberOfNodes(10);
//  gt.generateNodes();
// // SECTION("Setting max nr of nodes to 10 and generate them") {
//
//    REQUIRE(gt.getNumberOfNodes() == 10);
//
//
// // }




}

//// gui to engine
//TEST_CASE( "Generate nodes", "[GameTree.generateNodes]" ) {
//  using gameTree::nodePtr;
//  using bitboard::gameState;
//
//
//  nodePtr root = std::make_shared<gameState>();
//  ::utils::setDefaultChessLayout(root);
//
//  gameTree::GameTree gt(root);
//
//  SECTION("Verifying that only one node exists when a GameTree is initiated") {
//    REQUIRE(gt.getNumberOfNodes() == 1);
//  }
//
//
//  gt.setMaxNumberOfNodes(10);
//  gt.generateNodes();
//  SECTION("Setting max nr of nodes to 10 and generating them") {
//
//    REQUIRE(gt.getNumberOfNodes() == 10);
//  }
//
//  std::cout << "asdasd" << std::endl;
//
//
//  SECTION("Setting max nr of nodes to 100 and generating them") {
//    gt.setMaxNumberOfNodes(100);
//    gt.generateNodes();
//
//    REQUIRE(gt.getNumberOfNodes() == 100);
//  }
//
//  SECTION("Setting max nr of nodes to 1000 and generating them") {
//    gt.setMaxNumberOfNodes(1000);
//    gt.generateNodes();
//
//    REQUIRE(gt.getNumberOfNodes() == 1000);
//  }
//
//  SECTION("Removing all nodes from game tree") {
//    gt.reset();
//    REQUIRE(gt.getNumberOfNodes() == 0);
//  }
//
//  // gt.setMaxNumberOfNodes(100000000); // When you feel too happy and need some sadness in life
//  // gt.generateNodes();
//  // REQUIRE(gt.getNumberOfNodes() == 100000101);
//
//}

