
#include <iostream>
#include <david/utils/utils.h>
#include <david/utils/gameState.h>
#include "david/MoveGen.h"
#include "david/MoveGenTest.h"
#include "catch.hpp"



TEST_CASE("MoveGen perft [MoveGen]") {
  // https://chessprogramming.wikispaces.com/Perft+Results
  ::david::type::gameState_t gs;

  //::utils::gameState::generateFromFEN(gs, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
  //::utils::gameState::generateFromFEN(gs, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
  ::utils::gameState::generateFromFEN(gs, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  //::utils::gameState::print(gs);

  ::utils::perft_advanced(gs, 1, 7);
}

TEST_CASE("extract legal south attack [MoveGen.extractLegalSouthPath]") {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);

  // rook moves
  {
    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::verticalAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 8) == 72340172838076672);

    p = ::utils::constant::verticalAttackPaths[26];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(26) - 1) & p, 8) == 289360691285196800);

    p = ::utils::constant::horizontalAttackPaths[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 1) == 254);

    p = ::utils::constant::horizontalAttackPaths[35];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(35) - 1) & p, 1) == 1060856922112);
  }

  {
    gs.piecesArr[0][0] = 37065331200;
    gs.piecesArr[0][1] = 1145450631528448;
    ::utils::gameState::generateMergedBoardVersion(gs);

    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::verticalAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 8) == 72340172838076672);

    p = ::utils::constant::verticalAttackPaths[26];
    //mgt.print();
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(26) - 1) & p, 8) == 289360691284934656);

    p = ::utils::constant::horizontalAttackPaths[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 1) == 254);

    p = ::utils::constant::horizontalAttackPaths[35];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(35) - 1) & p, 1) == 1060856922112);
  }

  // bishop moves
  {
    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::diagonalDUAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 7) == 0);

    p = ::utils::constant::diagonalDUAttackPaths[26];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(26) - 1) & p, 7) == 1108102086656);

    p = ::utils::constant::diagonalUDAttackPaths[0];
    bool ok = mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 9) == 9241421688590303744;
    REQUIRE(ok);

    p = ::utils::constant::diagonalUDAttackPaths[35];
    ok = mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(35) - 1) & p, 9) == 4620710809935413504;
    REQUIRE(ok);
  }

  {
    gs.piecesArr[0][0] = 37065331200;
    gs.piecesArr[0][1] = 1145450631528448;
    ::utils::gameState::generateMergedBoardVersion(gs);

    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::diagonalDUAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 7) == 0);

    p = ::utils::constant::diagonalDUAttackPaths[26];
    //mgt.print();
    REQUIRE(mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(26) - 1) & p, 7) == 1108102086656);

    p = ::utils::constant::diagonalUDAttackPaths[0];
    bool ok = mgt.extractLegalSouthPath(p, p & f, h, 0 & p, 9) == 9241421688590303744;
    REQUIRE(ok);

    p = ::utils::constant::diagonalUDAttackPaths[35];
    ok = mgt.extractLegalSouthPath(p, p & f, h, (::utils::indexToBitboard(35) - 1) & p, 9) == 4620710809935413504;
    REQUIRE(ok);
  }


}

TEST_CASE("extract legal north attack [MoveGen.extractLegalNorthPath]") {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);

  // rook moves
  {
    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::verticalAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, 18446744073709551614 & p, 8) == 0);

    p = ::utils::constant::verticalAttackPaths[26];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, ~(::utils::indexToBitboard(26) - 1) & p, 8) == 1130315133486084);

    p = ::utils::constant::horizontalAttackPaths[0];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, 18446744073709551614 & p, 1) == 0);

    p = ::utils::constant::horizontalAttackPaths[35];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, ~(::utils::indexToBitboard(35) - 1) & p, 1) == 1060856922112);
  }

  {
    gs.piecesArr[0][0] = 37065331200;
    gs.piecesArr[0][1] = 1145450631528448;
    ::utils::gameState::generateMergedBoardVersion(gs);

    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    auto p = ::utils::constant::verticalAttackPaths[0];
    auto& h = gs.piecess[1];
    auto& f = gs.piecess[0];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, 18446744073709551614 & p, 8) == 65792);

    p = ::utils::constant::verticalAttackPaths[26];
    //mgt.print();
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, ~(::utils::indexToBitboard(26) - 1) & p, 8) == 1130315133486084);

    p = ::utils::constant::horizontalAttackPaths[0];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, 18446744073709551614 & p, 1) == 0);

    p = ::utils::constant::horizontalAttackPaths[35];
    REQUIRE(mgt.extractLegalNorthPath(p, p & f, h, ~(::utils::indexToBitboard(35) - 1) & p, 1) == 511101108224);
  }

}

TEST_CASE("rook attack [MoveGen.generateRookAttack]") {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);

  {
    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    REQUIRE(mgt.generateRookAttack(20, gs) == 4521260816990208);
    REQUIRE(mgt.generateRookAttack(20, gs, true) == 17661189623808);
  }

  // move h2h4 pawn.
  {
    gs.piecesArr[0][0] = 16842240;
    ::utils::gameState::generateMergedBoardVersion(gs);

    ::david::MoveGen moveGen{gs};
    ::david::MoveGenTest mgt{moveGen};

    REQUIRE(mgt.generateRookAttack(0, gs) == 65792);
    REQUIRE(mgt.generateRookAttack(0, gs, true) == 16843010);
    REQUIRE(mgt.generateRookAttack(26, gs) == 1130319327789056);
    REQUIRE(mgt.generateRookAttack(26, gs, true) == 4419437724672);

  }
}