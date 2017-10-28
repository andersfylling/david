
#include <iostream>
#include <david/utils/utils.h>
#include "david/MoveGen.h"
#include "david/MoveGenTest.h"
#include "catch.hpp"



TEST_CASE("MoveGen perft [MoveGen]") {
  // https://chessprogramming.wikispaces.com/Perft+Results
  // see test-main.cpp

  //std::cout << "FEN::";

  //const std::string FEN = "r3k2r/8/8/8/8/8/8/6K1 b k";
  //::utils::gameState::print(FEN);
  //::utils::perft_advanced(3, FEN, 1);
  //std::exit(1);


  //::utils::perft_egn(1, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

  //::utils::perft_advanced(6, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
  //::utils::perft_advanced(4, "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - 0 1");
  //::utils::perft_advanced(3, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

  //::utils::perft_advanced(5, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1);
  //::utils::perft_advanced(2, "r3k2r/p1ppqpb1/bn2pnp1/3P4/1pN1P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");

  //std::exit(1);
  //::utils::perft(5);

//  ::david::type::gameState_t gs;
//  ::utils::gameState::generateFromFEN(gs, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
//
//  ::david::MoveGen moveGen{gs};
//
//  std::array<::david::type::gameState_t, 256> gameStates{};
//  auto end = moveGen.template generateGameStates(gameStates);
//
//  // 0 - 44
//  //int ii = 19;
//  //::utils::gameState::print(gameStates[ii]);
//  //moveGen.setGameState(gameStates[ii]);
//  //auto end = moveGen.template generateGameStates(gameStates);
//  //if (end == 11) { std::exit(0); }
//  //std::cout << std::to_string(end) << std::endl << std::flush;
//  for (unsigned int i = 0; i < end; i++) {
//    moveGen.setGameState(gameStates[i]);
//    std::array<::david::type::gameState_t, 256> gameStates2{};
//    auto len = moveGen.template generateGameStates(gameStates2);
//    if (len == 11) { continue; }
//    ::utils::gameState::print(gameStates[i]);
//    std::cout << std::to_string(i) << std::endl << std::flush;
//
//    //std::cout << ::utils::gameState::getEGN(gs, gameStates[i]) << "\n";
//  }
//
//  std::exit(1);
  //::utils::perft(7, "Q1B1n3/2N5/1k4p1/R2qr2R/b7/4P1K1/3b3B/2r5 w - - 0 1");
  //std::exit(1);

//  ::david::type::gameState_t gs;
//  std::string FEN = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
//  ::utils::gameState::generateFromFEN(gs, FEN);
//
//  std::cout << ::utils::gameState::generateFen(gs) << std::endl;
//  std::cout << FEN << std::endl;
//  if (::utils::gameState::generateFen(gs) != FEN) {
//    std::cerr << "Incorrect FEN generation" << std::endl;
//  }
//
//  ::david::MoveGen moveGen{gs};
//
//  std::array<::david::type::gameState_t, 256> gameStates{};
//  auto end = moveGen.template generateGameStates(gameStates);
//
//  int sum = 0;
//  int sum2 = 0;
//  for (unsigned int i = 0; i < end; i++) {
//    const std::string fen = ::utils::gameState::generateFen(gameStates[i]);
//
//
//    int perft = ::utils::perft_silent(1, fen);
//    int perft2 = ::utils::perft(1, gameStates[i]);
//    std::cout << std::to_string(i) << ": " << fen << ", perft: " << std::to_string(perft) << "\n";
//
//    if (perft != perft2) {
//      ::david::type::gameState_t gs2;
//      ::utils::gameState::generateFromFEN(gs2, fen);
//      gs2.castling = 0;
//    }
//
//    sum += perft;
//    sum2 += perft2;
//  }
//  std::exit(1);
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