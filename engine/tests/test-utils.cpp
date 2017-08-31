#include <iostream>
#include <david/utils/utils.h>
#include "catch.hpp"

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
// windows code goes here
#endif


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

TEST_CASE("pawn attacks") {
  const auto paths = ::utils::compilePawnAttacks();

  for (int i = 0; i < 64; i++) {
    //::utils::printBoard(paths[i], i);
  }
}

TEST_CASE("index to bitboard") {
  REQUIRE(::utils::indexToBitboard(0) == 1ULL);
  REQUIRE(::utils::indexToBitboard(1) == 2ULL);
}

TEST_CASE("full path") {
#ifdef __linux__

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    fprintf(stdout, "Current working dir: %s\n", cwd);
  else
    perror("getcwd() error");
#elif _WIN32
#endif
}

TEST_CASE ("chess index parser") {
  REQUIRE(::utils::chessIndexToArrayIndex("e1") == 3);
  REQUIRE(::utils::chessIndexToArrayIndex("d5") == 36);

  ::david::type::gameState_t gs1;
  ::utils::setDefaultChessLayout(gs1);

  ::david::type::gameState_t gs2;


  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 6; j++) {
      gs2.piecesArr[j][i] = gs1.piecesArr[j][(i + 1) % 2];
    }
  }

  gs2.piecesArr[0][1] = 4243200;
  ::utils::generateMergedBoardVersion(gs2);

  REQUIRE(::utils::getEGN(gs1, gs2) == "b2b3");

//  ::david::type::gameState_t gs;
//  ::utils::setDefaultChessLayout(gs);
//  gs.piecesArr[0][0] = 21020160;
//  ::utils::generateMergedBoardVersion(gs);
//
//  ::utils::printGameState(gs);
}