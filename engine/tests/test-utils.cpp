#include <iostream>
#include <david/utils/utils.h>
#include "catch.hpp"


//TEST_CASE("Convert a string representing a bit array to an uint8_t [utils::stringTo8bitarray]") {
//  REQUIRE(::utils::stringTo8bitArray("00000000") == 0); // wtf? it worked before lol
//  REQUIRE(::utils::stringTo8bitArray("00000001") == 1);
//  REQUIRE(::utils::stringTo8bitArray("00000010") == 2);
//  REQUIRE(::utils::stringTo8bitArray("00000011") == 3);
//  REQUIRE(::utils::stringTo8bitArray("00000100") == 4);
//  REQUIRE(::utils::stringTo8bitArray("00000101") == 5);
//  REQUIRE(::utils::stringTo8bitArray("00000111") == 6);
//
//  //TODO: convert int to bit array as a string using bitset, and test.
//}


TEST_CASE("vertical attack paths") {
  const auto paths = ::utils::compileVerticalAttacks();

  for (auto path : paths) {
    //::utils::printBoard(path);
  }
}


TEST_CASE("horizontal attack paths") {
  const auto paths = ::utils::compileHorizontalAttacks();

  for (auto path : paths) {
    //::utils::printBoard(path);
  }
}