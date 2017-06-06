//
// Created by markus on 5/5/17.
//

#include <david/utils.h>
#include "david/Search.h"
#include "david/definitions.h"
#include "catch.hpp"


david::definitions::engineContext_ptr context = std::make_shared<david::EngineContext>();
david::Search test_search(context);
std::shared_ptr<david::bitboard::gameState> node = std::make_shared<david::bitboard::gameState>();


TEST_CASE("Search creation") {
  david::utils::setDefaultChessLayout(node);

  test_search.setDepth(10);
  REQUIRE(test_search.returnDepth() == 10);
  test_search.setDepth(3);
  REQUIRE(test_search.returnDepth() == 3);

  test_search.setMoveTime(3000);
  REQUIRE(test_search.returnTimeToSearch() == 3000);
  test_search.setMoveTime(1000);
  REQUIRE(test_search.returnTimeToSearch() == 1000);

}

TEST_CASE("Search"){
  //::utils::setDefaultChessLayout(node);
  test_search.setDebug(false);
  test_search.performanceTest(node, 10);
  REQUIRE_NOTHROW(test_search.searchInit(node));
}

/*TEST_CASE("Abort search"){
  ::utils::setDefaultChessLayout(node);
  SECTION("Stopping search") {
    test_search.stopSearch();
  }

  test_search.searchInit(node);
  REQUIRE(test_search.returnScore() == (int)(-INFINITY));
  test_search.iterativeDeepening(node);
  REQUIRE(test_search.returnScore() ==(int)(-INFINITY));
  test_search.negamax(node, (int)(-INFINITY), (int)(INFINITY), 1);
  REQUIRE(test_search.returnScore() == (int)(-INFINITY));
}*/

TEST_CASE("Search completed?"){
  david::utils::setDefaultChessLayout(node);
  test_search.searchInit(node);
  REQUIRE(test_search.returnComplete() == true);
}

TEST_CASE("Search returning optimal node?"){
  context->testing = true;
  test_search.setDebug(true);

  //
  //Simulere iterative deepening
  //Nivå = 2
  //

  auto node1 = std::make_shared<david::bitboard::gameState>(); node1->score = -(int)INFINITY;
  auto node2 = std::make_shared<david::bitboard::gameState>(); node2->score = 34;
  auto node3 = std::make_shared<david::bitboard::gameState>(); node3->score = 32;
  auto node4 = std::make_shared<david::bitboard::gameState>(); node4->score = 12;
  node1->children.push_back(node2);
  node1->children.push_back(node3);
  node1->children.push_back(node4);
  test_search.setDepth(2);
  test_search.searchInit(node1);
  REQUIRE(test_search.returnScore() == -12);

  //
  //Nivå 3
  //
  node2->score = 3;
  node3->score = 6;
  node4->score = 5;
  auto node5 = std::make_shared<david::bitboard::gameState>(); node5->score = 5;
  auto node6 = std::make_shared<david::bitboard::gameState>(); node6->score = 3;
  node2->children.push_back(node5);
  node2->children.push_back(node6);

  auto node7 = std::make_shared<david::bitboard::gameState>(); node7->score = 6;
  auto node8 = std::make_shared<david::bitboard::gameState>(); node8->score = 7;
  node3->children.push_back(node7);
  node3->children.push_back(node8);

  auto node9 = std::make_shared<david::bitboard::gameState>(); node9->score = 5;
  auto node10 = std::make_shared<david::bitboard::gameState>(); node10->score = 8;
  node4->children.push_back(node9);
  node4->children.push_back(node10);
  test_search.setDepth(3);
  test_search.searchInit(node1);
  REQUIRE(test_search.returnScore() == 6);

  //
  //Nivå 4
  //
  /*node5->score = -(int)INFINITY;
  node6->score = -(int)INFINITY;
  node7->score = -(int)INFINITY;
  node8->score = -(int)INFINITY;
  node9->score = -(int)INFINITY;
  node10->score = -(int)INFINITY;*/
  auto node11 = std::make_shared<david::bitboard::gameState>(); node11->score = 5;
  auto node12 = std::make_shared<david::bitboard::gameState>(); node12->score = 4;
  node5->children.push_back(node11);
  node5->children.push_back(node12);

  auto node13 = std::make_shared<david::bitboard::gameState>(); node13->score = 3;
  node6->children.push_back(node13);

  auto node14 = std::make_shared<david::bitboard::gameState>(); node14->score = 6;
  auto node15 = std::make_shared<david::bitboard::gameState>(); node15->score = 6;
  node7->children.push_back(node14);
  node7->children.push_back(node15);

  auto node16 = std::make_shared<david::bitboard::gameState>(); node16->score = 7;
  node8->children.push_back(node16);

  auto node17 = std::make_shared<david::bitboard::gameState>(); node17->score = 5;
  node9->children.push_back(node17);

  auto node18 = std::make_shared<david::bitboard::gameState>(); node18->score = 8;
  auto node19 = std::make_shared<david::bitboard::gameState>(); node19->score = 6;
  node10->children.push_back(node18);
  node10->children.push_back(node19);

  test_search.setDepth(4);
  test_search.searchInit(node1);
  REQUIRE(test_search.returnScore() == 6);

  //
  //Nivå 5
  //
  auto node20 = std::make_shared<david::bitboard::gameState>(); node20->score = 5;
  auto node21 = std::make_shared<david::bitboard::gameState>(); node21->score = 6;
  node11->children.push_back(node20);
  node11->children.push_back(node21);

  auto node22 = std::make_shared<david::bitboard::gameState>(); node22->score = 7;
  auto node23 = std::make_shared<david::bitboard::gameState>(); node23->score = 4;
  auto node24 = std::make_shared<david::bitboard::gameState>(); node24->score = 5;
  node12->children.push_back(node22);
  node12->children.push_back(node23);
  node12->children.push_back(node24);

  auto node25 = std::make_shared<david::bitboard::gameState>(); node25->score = 3;
  node13->children.push_back(node25);

  auto node26 = std::make_shared<david::bitboard::gameState>(); node26->score = 11;
  node14->children.push_back(node26);

  auto node27 = std::make_shared<david::bitboard::gameState>(); node27->score = 6;
  auto node28 = std::make_shared<david::bitboard::gameState>(); node28->score = 9;
  node15->children.push_back(node27);
  node15->children.push_back(node28);

  auto node29 = std::make_shared<david::bitboard::gameState>(); node29->score = 7;
  node16->children.push_back(node29);

  auto node30 = std::make_shared<david::bitboard::gameState>(); node30->score = 5;
  node17->children.push_back(node30);

  auto node31 = std::make_shared<david::bitboard::gameState>(); node31->score = 9;
  auto node32 = std::make_shared<david::bitboard::gameState>(); node32->score = 8;
  node18->children.push_back(node31);
  node18->children.push_back(node32);

  auto node33 = std::make_shared<david::bitboard::gameState>(); node33->score = 6;
  node19->children.push_back(node33);
  test_search.setDepth(5);
  test_search.searchInit(node1);
  REQUIRE(test_search.returnScore() == 7);
}