//
// Created by markus on 5/5/17.
//

#include "chess_ann/Search.h"
#include "catch.hpp"
#include "chess_ann/bitboard.h"

std::shared_ptr<::search::Search> searchPtr(new ::search::Search);
std::shared_ptr<::bitboard::gameState> node(new ::bitboard::gameState);





TEST_CASE("Making of relevant objects"){
    REQUIRE(searchPtr->returnMembers());
}

TEST_CASE("Search"){
  REQUIRE_NOTHROW(searchPtr->searchInit(node));

}


