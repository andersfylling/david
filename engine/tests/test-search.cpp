//
// Created by markus on 5/5/17.
//

#include "chess_ann/Search.h"
#include "catch.hpp"

::search::Search test_search;
std::shared_ptr<::bitboard::gameState> node = std::make_shared<::bitboard::gameState>();


TEST_CASE("Search creation") {
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
  test_search.setDebug(false);
  test_search.performanceTest(node, 200);
  REQUIRE_NOTHROW(test_search.searchInit(node));
}

TEST_CASE("Abort search"){
  test_search.stopSearch();
  test_search.searchInit(node);
  REQUIRE(test_search.returnScore() == (int)(-INFINITY));
  test_search.iterativeDeepening(node);
  REQUIRE(test_search.returnScore() ==(int)(-INFINITY));
  test_search.negamax(node, (int)(-INFINITY), (int)(INFINITY), 1);
  REQUIRE(test_search.returnScore() == (int)(-INFINITY));
}

TEST_CASE("Search completed?"){
  test_search.searchInit(node);
  REQUIRE(test_search.returnComplete() == true);
}




