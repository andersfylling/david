//
// Created by markus on 5/5/17.
//

#include "chess_ann/Search.h"
#include "catch.hpp"
#include "chess_ann/bitboard.h"

::search::Search test_search;
std::shared_ptr<::bitboard::gameState> node = std::make_shared<::bitboard::gameState>();





TEST_CASE("Making of relevant objects"){
    REQUIRE(test_search.returnMembers()); //Implies that object exist
}

TEST_CASE("Search"){
  REQUIRE_NOTHROW(test_search.searchInit(node)); // No errors though out search, may be a bit wide

}



