
#include <iostream>
#include <david/utils/utils.h>
#include "david/MoveGen.h"
#include "catch.hpp"

#ifdef MOVEGEN


TEST_CASE("MoveGen perft [MoveGen]") {
  REQUIRE(::utils::perft(5));
}

TEST_CASE("MoveGen generation moves") {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);
  ::david::MoveGen moveGen{gs};

  REQUIRE(gs.isWhite);

  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
  uint16_t len = moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
  ::david::type::gameState_t gs2 = states[1];

  REQUIRE(!gs2.isWhite);

  ::david::MoveGen b{gs2};
  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states2;
  len = b.template generateGameStates<::david::constant::MAXMOVES>(states2);
  ::david::type::gameState_t gs3 = states2[1];

  REQUIRE(gs3.isWhite);

  ::david::MoveGen c{gs3};
  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states3;
  len = c.template generateGameStates<::david::constant::MAXMOVES>(states3);
  ::david::type::gameState_t gs4 = states3[len - 1];

  REQUIRE(!gs4.isWhite);

  ::david::MoveGen d{gs4};
  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states4;
  len = d.template generateGameStates<::david::constant::MAXMOVES>(states4);
  ::david::type::gameState_t gs5 = states4[len - 4];

  REQUIRE(gs5.isWhite);

  ::david::MoveGen e{gs5};
  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states5;
  len = e.template generateGameStates<::david::constant::MAXMOVES>(states5);
  ::david::type::gameState_t gs6 = states5[len - 1];

  REQUIRE(!gs6.isWhite);

  //::utils::printGameState(gs3);

  for (int i = 0; i < len; i++) {
    auto g = states5[i];
    //::utils::printGameState(g);
  }

}

TEST_CASE("diagonal attacks [MoveGen]") {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);

  ::david::MoveGen moveGen{gs};

  auto b = false;

  b = moveGen.test_diagonalAttackPaths(262144) == 18049656063787008;
  REQUIRE(b);

  b = moveGen.test_diagonalAttackPaths(268435456) == 36666685564387328;
  REQUIRE(b);

  /*
  // place the piece on top of an enemy piece. This results in a 0ULL.
  b = moveGen.test_diagonalAttackPaths(18014398509481984) == 11529391036782870528;
  REQUIRE(b);

  b = moveGen.test_diagonalAttackPaths(2305843009213693952) == 22517998136852480;
  REQUIRE(b);
  */

  b = moveGen.test_diagonalAttackPaths(4294967296) == 1128098963914752;
  REQUIRE(b);

  b = moveGen.test_diagonalAttackPaths(2048) == 141017232965632;
  REQUIRE(b);


}
TEST_CASE("knight attacks [MoveGen]") {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);

  ::david::MoveGen moveGen{gs};


  //uint64_t piece = 64;
  //::utils::printBoard(piece);
  //::utils::printBoard(moveGen.test_knightAttackPaths(piece));

}

TEST_CASE("rook attacks [MoveGen]") {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);

  ::david::MoveGen moveGen{gs};

  //moveGen.test_rookAttackPaths();

  //uint64_t piece = 64;
  //::utils::printBoard(piece);
  //::utils::printBoard(moveGen.test_knightAttackPaths(piece));

}


TEST_CASE("queen attacks [MoveGen]") {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);

  ::david::MoveGen moveGen{gs};

  //moveGen.test_bishopAttackPaths();
  //moveGen.test_queenAttackPaths();
  //moveGen.test_rookAttackPaths();

  //uint64_t piece = 64;
  //::utils::printBoard(piece);
  //::utils::printBoard(moveGen.test_knightAttackPaths(piece));

}

#endif