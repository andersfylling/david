#include <iostream>
#include "catch.hpp"
#include "david/bitboard.h"
#include "david/environment.h"
#include <david/GameTree.h>
#include <david/utils.h>

david::type::gameState_t testStruct; // White Queen in the middle. Rest is normal


using david::bitboard::bitboard_t;
using david::bitboard::WPc;
using david::bitboard::WCc;
using david::bitboard::WNc;
using david::bitboard::WRc;
using david::bitboard::WQc;
using david::bitboard::WKc;
using david::bitboard::BPc;
using david::bitboard::BCc;
using david::bitboard::BRc;
using david::bitboard::BNc;
using david::bitboard::BQc;
using david::bitboard::BKc;
using david::bitboard::COLOR;
using david::bitboard::makeBoardFromArray;
using david::bitboard::DIRECTION;
using david::environment::NSB;
using david::environment::LSB;
using david::environment::flipBit;
using david::bitboard::move_t;


bitboard_t WP = makeBoardFromArray(WPc);
bitboard_t WC = makeBoardFromArray(WCc);
bitboard_t WN = makeBoardFromArray(WNc);
bitboard_t WR = makeBoardFromArray(WRc);
bitboard_t WK = makeBoardFromArray(WKc);
bitboard_t WQ = makeBoardFromArray(WQc);

bitboard_t BC = makeBoardFromArray(BCc);
bitboard_t BR = makeBoardFromArray(BRc);
bitboard_t BN = makeBoardFromArray(BNc);
bitboard_t BQ = makeBoardFromArray(BQc);
bitboard_t BK = makeBoardFromArray(BKc);
bitboard_t BP = makeBoardFromArray(BPc);

david::environment::Environment test(COLOR::WHITE);
bitboard_t *bits;

TEST_CASE ("Num-Pieces") {
  // One piece activated
  //std::cout << test.numberOfPieces(WQ) << std::endl;
  // Two pieces
  //std::cout << test.numberOfPieces(WC) << std::endl;
  // All pieces
}

TEST_CASE ("Castling") {
  testStruct.BlackBishop = 2594073385365405696ULL;
  //testStruct.BlackBishop = 0;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  //testStruct.BlackKnight = 0;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  //testStruct.BlackQueen = 0;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 36;
  //testStruct.WhiteBishop = 0;
  testStruct.WhiteKnight = 66;
  //testStruct.WhiteKnight = 0;
  testStruct.WhitePawn = 65280;
  testStruct.WhiteQueen = 8;
  //testStruct.WhiteQueen = 0;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 129;

  test.setGameState(testStruct);
  //test.printBoard(test.whitePieces() | test.blackPieces());
  test.generateAttacks();

  // Tests were done with std::cout variables used
  // various places in canColorCastleK/Q functions

}

TEST_CASE("Flip-bit") {
  bitboard_t temp = 0ULL;
  bitboard_t control = 0ULL;

  // Tests setting bits opposed to using slow power function
  for (bitboard_t i = 0ULL; i < 64ULL; i++) {
    temp = 0ULL;
    flipBit(temp, i);
    control = static_cast<unsigned long long>(pow(2.0, static_cast<float>(i)));
    REQUIRE(temp == control);
  }

  // Tests with several selected bits
  temp = 0ULL;
  control = 0ULL;
  for (bitboard_t i = 0ULL; i < 64ULL; i++) {
    flipBit(temp, i);
    control += static_cast<unsigned long long>(pow(2.0, static_cast<float>(i)));
    REQUIRE(temp == control);
  }
}


TEST_CASE("White pawn movement") {
  bitboard_t WP = makeBoardFromArray(WPc);
  bits = test.getDiagYAxis(WP, DIRECTION::UP, true, 1);
  REQUIRE(bits[0] == 65536LL);
  delete [] bits;

  bits = test.getDiagYAxis(WP, DIRECTION::MAIN_DIAGONAL, true, 1);
  REQUIRE(bits[0] == 131072LL);
  delete [] bits;

  bits = test.getDiagYAxis(WP, DIRECTION::ANTI_DIAGONAL, true, 1);
  REQUIRE(bits[0] == 0LL);
  delete [] bits;
}

TEST_CASE("Black pawn movement") {
  bitboard_t BP = makeBoardFromArray(BPc);
  bitboard_t cor = 1099511627776;
  bits = test.getDiagYAxis(BP, DIRECTION::UP, true, 2);
  REQUIRE(bits[0] == cor);
  delete [] bits;

  bits = test.getDiagYAxis(BP, DIRECTION::MAIN_DIAGONAL, true, 2);
  REQUIRE(bits[0] == 0LL);
  delete [] bits;

  cor = 2199023255552;
  bits = test.getDiagYAxis(BP, DIRECTION::ANTI_DIAGONAL, true, 2);
  REQUIRE(bits[0] == cor);
  delete [] bits;
}


TEST_CASE("LSB MSB NSB") {
  bitboard_t testBrett = 0ULL;

  // Tests that LSB can find the correct index
  for (bitboard_t i = 0; i < 64; i++) {
    testBrett = 0ULL;
    flipBit(testBrett, i);
    REQUIRE(LSB(testBrett) == i);
  }

  for (bitboard_t i = 0; i < 64; i++)
    flipBit(testBrett, i);

  // Tests that NSB works with all possible fields
  for (bitboard_t i = 0; i < 63; i++) {
    REQUIRE(NSB(testBrett) == i+1);
  }
}


TEST_CASE("White knight movement") {
  bitboard_t cor = 1099511627776;
  bits = test.getDiagYAxis(BP, DIRECTION::UP, true, 2);
  REQUIRE(bits[0] == cor);
  delete [] bits;

  bits = test.getDiagYAxis(BP, DIRECTION::MAIN_DIAGONAL, true, 2);
  REQUIRE(bits[0] == 0LL);
  delete [] bits;

  cor = 2199023255552;
  bits = test.getDiagYAxis(BP, DIRECTION::ANTI_DIAGONAL, true, 2);
  REQUIRE(bits[0] == cor);
  delete [] bits;
}

TEST_CASE("X-AXIS movement") {
  bitboard_t * bits;
  bits = test.getXAxisFromBoard(WC, 0, 1);
  REQUIRE(bits[0] == 0ULL);
  REQUIRE(bits[1] == 127ULL);
  bits = test.getXAxisFromBoard(WC, 0, 2);
  REQUIRE(bits[0] == 254ULL);
  REQUIRE(bits[1] == 0ULL);


  bits = test.getXAxisFromBoard(WC, 1, 1);
  REQUIRE(bits[0] == 0ULL);
  REQUIRE(bits[1] == 0ULL);
  bits = test.getXAxisFromBoard(WC, 1, 2);
  REQUIRE(bits[0] == 2ULL);
  REQUIRE(bits[1] == 64ULL);




}

TEST_CASE("YDiagGeneration") {
  // Seriously the greates test. Seriously. Made a great deal. GREAT #TRUMP
  bitboard_t queen = 34359738368;     // Queen in the middle of the board
  bits = test.getDiagYAxis(queen, DIRECTION::MAIN_DIAGONAL, false, 2);

  bitboard_t cor = 578721382704613376;
  bits = test.getDiagYAxis(WQ, DIRECTION::UP);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 2048;
  bits = test.getDiagYAxis(WQ, DIRECTION::UP, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 4096;
  bits = test.getDiagYAxis(WQ, DIRECTION::MAIN_DIAGONAL, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 1024;
  bits = test.getDiagYAxis(WQ, DIRECTION::ANTI_DIAGONAL, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 550831656960;
  bits = test.getDiagYAxis(WQ, DIRECTION::MAIN_DIAGONAL);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 16909312;
  bits = test.getDiagYAxis(WQ, DIRECTION::ANTI_DIAGONAL);
  REQUIRE(bits[0] == cor);

  delete [] bits;



}


TEST_CASE("Queen move BLOCK") {
  //test.printBitboards();
  testStruct.BlackBishop = 2594073385365405696ULL;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 36;
  testStruct.WhiteQueen = 34359738368;
  testStruct.WhiteKnight = 66;
  testStruct.WhitePawn = 65280;
  // testStruct.WhiteQueen = 8;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 129;

  test.setGameState(testStruct);

  //test.printBitboards();
  //test.printBoard(test.whitePieces() | test.blackPieces());

  bitboard_t cor = 11853796676861952;
  //test.printBoard(test.QueenMove(WHITE));
  bitboard_t qm = test.QueenMove(COLOR::WHITE)[0];
  REQUIRE(cor == qm);
}

TEST_CASE ("REDUCE VECTOR") {
  bitboard_t board = test.reduceVector(*test.getDiagYAxis(34359738368LL, DIRECTION::UP, false, 1), test.blackPieces(), test.whitePieces(), DIRECTION::UP);
  REQUIRE(board == 2260595906707456ULL);

  board = test.reduceVector(*test.getDiagYAxis(34359738368LL, DIRECTION::UP, false, 2), test.blackPieces(), test.whitePieces(), DIRECTION::DOWN);
  REQUIRE(board == 134742016ULL);
}


TEST_CASE("Bishop MOVEMENT") {
  testStruct.BlackBishop = 2594073385365405696ULL;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 524292;
  //testStruct.WhiteBishop = 36;
  //testStruct.WhiteQueen = 34359738368;
  testStruct.WhiteKnight = 66;
  testStruct.WhitePawn = 65280;
  testStruct.WhiteQueen = 8;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 129;

  test.setGameState(testStruct);

  //test.printBoard(testStruct.WhiteBishop);
  bitboard_t t1 = test.BishopMove(COLOR::WHITE)[0];
  bitboard_t t2 = test.BishopMove(COLOR::WHITE)[1];



  REQUIRE(t1 == 0);
  REQUIRE(t2 == 36100411639201792ULL);
}


TEST_CASE ("Rook move") {
  testStruct.BlackBishop = 2594073385365405696ULL;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 36;
  //testStruct.WhiteQueen = 34359738368;
  testStruct.WhiteKnight = 66;
  testStruct.WhitePawn = 65280;
  testStruct.WhiteQueen = 8;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 34359738369;
  //testStruct.WhiteRook = 129;

  /*//test.printBoard(testStruct.WhiteRook);
  bitboard_t b1 = test.RookMove(COLOR::WHITE)[0];
  bitboard_t b2 = test.RookMove(COLOR::WHITE)[1];
  test.printBoard(b1);
  test.printBoard(b2);

  //REQUIRE(b1 == 0ULL);
  //REQUIRE(b2 == 2261107679428608);*/

  bitboard_t opponent, own;
  //test.printBoard(testStruct.WhiteRook);
  opponent = test.blackPieces();
  own = test.whitePieces();
  //test.printBoard(own);

  bitboard_t movement = test.reduceVector(test.getXAxisFromBoard(testStruct.WhiteRook, false, 2)[1], opponent, own, DIRECTION::UP);

  //movement |= test.reduceVector(test.getXAxisFromBoard(testStruct.WhiteRook, false, 1)[1], opponent, own, DIRECTION::DOWN);
  //movement |= test.reduceVector(test.getDiagYAxis(testStruct.WhiteRook, DIRECTION::UP, false, 1)[1], opponent, own, DIRECTION::UP);
  //movement |= test.reduceVector(test.getDiagYAxis(testStruct.WhiteRook, DIRECTION::UP, false, 2)[1], opponent, own, DIRECTION::DOWN);
  //test.printBoard(movement);
  david::environment::Environment test2(COLOR::WHITE);
  test2.setGameState(testStruct);
  bitboard_t * b = test2.RookMove(COLOR::WHITE);
  REQUIRE(b[1] == 2261656898371584ULL);


}

TEST_CASE("Change chess index into an bitboard array index; E6 => 43") {
  REQUIRE(test.chessIndexToArrayIndex("H1") == 0);
  REQUIRE(test.chessIndexToArrayIndex("H2") == 8);
  REQUIRE(test.chessIndexToArrayIndex("H3") == 16);
  REQUIRE(test.chessIndexToArrayIndex("H4") == 24);
  REQUIRE(test.chessIndexToArrayIndex("H5") == 32);
  REQUIRE(test.chessIndexToArrayIndex("H6") == 40);
  REQUIRE(test.chessIndexToArrayIndex("H7") == 48);
  REQUIRE(test.chessIndexToArrayIndex("H8") == 56);

  REQUIRE(test.chessIndexToArrayIndex("G1") == 1);
  REQUIRE(test.chessIndexToArrayIndex("G2") == 9);
  REQUIRE(test.chessIndexToArrayIndex("G3") == 17);
  REQUIRE(test.chessIndexToArrayIndex("G4") == 25);
  REQUIRE(test.chessIndexToArrayIndex("G5") == 33);
  REQUIRE(test.chessIndexToArrayIndex("G6") == 41);
  REQUIRE(test.chessIndexToArrayIndex("G7") == 49);
  REQUIRE(test.chessIndexToArrayIndex("G8") == 57);
}

TEST_CASE("Create a bitboard using a string index as E6 for index") {
  bitboard_t board = test.chessIndexToBitboard("e6");
  REQUIRE(board == 8796093022208ULL);
}

TEST_CASE("Validate fen string from gameState node") {
  //::bitboard::gameState node; // 0 state for every board
  //node.WhitePawn = 0;

  //auto fen = test.fen(&node, true);
  //std::cout << ::stockfishMock::evaluate("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1") << std::endl;

  //auto fen = test.fen(n_p, false);

  //std::cout << fen << std::endl;
}

TEST_CASE("Combined attacks") {
  bitboard_t t = test.combinedWhiteAttacks();
  //test.printBoard(t);

  bitboard_t  tt = test.combinedBlackAttacks();
  //test.printBoard(tt);
}

TEST_CASE("Creating moves") {
  david::move::Move m;

  move_t mt = m.setGetValue(35ULL, 36ULL, 15);
  move_t hm1 = m.setGetValue(1ULL, 16ULL, 4);
  //m.printMoveString();
  move_t hm11 = m.setGetValue(1ULL, 18ULL, 4);
  //m.printMoveString();
  move_t hm2 = m.setGetValue(6ULL, 21ULL, 4);
  //m.printMoveString();
  move_t hm22 = m.setGetValue(6ULL, 23ULL, 4);
  //m.printMoveString();




  //m.printMoveString();
  REQUIRE(mt == 36431U);
}

TEST_CASE("Validate that fen strings are parsed correctly", "[Environment.generateBoardFromFen]") {
  david::environment::Environment env(david::bitboard::COLOR::BLACK);

  david::type::gameState_t node;
  david::utils::generateBoardFromFen(node, "r1bqkbnr/pppppppp/n7/8/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 2");
  env.setGameState(node);

  REQUIRE((env.whitePieces() | env.blackPieces()) == 18444210833279025149ULL);
}

TEST_CASE("King and Queen segfault test") {
  bitboard_t * bits;
  bits = test.KingMove(COLOR::WHITE);
  bits = test.QueenMove(COLOR::WHITE);
}

TEST_CASE("Move-Generation") {
  //test.printBoard(*test.knightMove(COLOR::BLACK));
  test.generateMoves(COLOR::WHITE);

}



TEST_CASE("Knight-Movement finish") {
  bitboard_t h1 = *test.knightMove(COLOR::WHITE);
  REQUIRE(h1 == 327680ULL);
  david::environment::Environment t2(COLOR::BLACK);
}

TEST_CASE("Pawn move"){
  testStruct.BlackBishop = 2594073385365405696ULL;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 524292;
  testStruct.WhiteBishop = 36;
  testStruct.WhiteQueen = 34359738368;
  testStruct.WhiteKnight = 66;
  testStruct.WhitePawn = 65280;
  testStruct.WhiteQueen = 8;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 129;

  test.setGameState(testStruct);

  bitboard_t t1 = test.pawnMoves(COLOR::WHITE)[0];
  bitboard_t t2 = test.pawnMoves(COLOR::WHITE)[1];

  REQUIRE(t1 ==16842752);
  REQUIRE(t2 == 33685504);
}

TEST_CASE("King move"){
  testStruct.BlackBishop = 2594073385365405696ULL;
  testStruct.BlackKing = 1152921504606846976ULL;
  testStruct.BlackKnight = 4755801206503243776ULL;
  testStruct.BlackPawn = 71776119061217280ULL;
  testStruct.BlackQueen = 576460752303423488ULL;
  testStruct.BlackRook = 9295429630892703744ULL;

  testStruct.WhiteBishop = 524292;
  testStruct.WhiteBishop = 36;
  testStruct.WhiteQueen = 34359738368;
  testStruct.WhiteKnight = 66;
  testStruct.WhitePawn = 65280;
  testStruct.WhiteQueen = 8;
  testStruct.WhiteKing = 16;
  testStruct.WhiteRook = 129;

  test.setGameState(testStruct);

  //test.printBoard(testStruct.WhiteBishop);
  bitboard_t fail = test.KingMove(COLOR::WHITE)[0];



  //Must fail because of neighbouring pieces
  REQUIRE_FALSE(fail == 14376);

  // King in the middle of board to test free movement
  testStruct.WhiteKing = 34359738368ULL;
  test.setGameState(testStruct);
  bitboard_t success = test.KingMove(COLOR::WHITE)[0];
  REQUIRE(success == 30872694685696ULL);
}

TEST_CASE("Queen move"){
  //Free movement
  testStruct.WhiteQueen = 34359738368ULL;
  testStruct.WhiteKing = 16;
  test.setGameState(testStruct);

  bitboard_t freemovement = test.QueenMove(COLOR::WHITE)[0];
  REQUIRE(freemovement == 11853796676861952ULL);

  //Fail movement in start position
  testStruct.WhiteQueen = 8;
  test.setGameState(testStruct);
  bitboard_t failMovement = test.QueenMove(COLOR::WHITE)[0];
  REQUIRE_FALSE(failMovement == 7188);
}

TEST_CASE("Pawn attack"){
  SECTION("Fail head on"){
      testStruct.WhiteQueen = 8; //Place white queen back to start from previous test_case
      testStruct.BlackPawn = 1310720;
      testStruct.WhitePawn = 2048;

      test.setGameState(testStruct);
      bitboard_t failHeadOn = test.pawnMoves(COLOR::WHITE)[4];
      REQUIRE_FALSE(failHeadOn == 1835008);

  }
  SECTION("Succeed main diagonal up and anti diagonal up"){
    bitboard_t successAttack = test.pawnMoves(COLOR::WHITE)[0];
    REQUIRE(successAttack == 136052736);
  }
  SECTION("Fail behind"){
    testStruct.BlackPawn = 458752;
    testStruct.WhitePawn = 33554432;
    test.setGameState(testStruct);

    bitboard_t failOnBackTrack = test.pawnMoves(COLOR::WHITE)[1];
    REQUIRE_FALSE(failOnBackTrack == 33554432);
  }
}

TEST_CASE("Knight movement"){
  SECTION("Free move on board"){
    testStruct.BlackBishop = 0;
    testStruct.BlackKing = 0;
    testStruct.BlackKnight = 0;
    testStruct.BlackPawn = 0;
    testStruct.BlackQueen = 0;
    testStruct.BlackRook = 0;

    testStruct.WhiteBishop = 0;
    testStruct.WhiteBishop = 0;
    testStruct.WhiteQueen = 0;
    testStruct.WhiteKnight = 268435456;
    testStruct.WhitePawn = 0;
    testStruct.WhiteQueen = 0;
    testStruct.WhiteKing = 0;
    testStruct.WhiteRook = 0;
    test.setGameState(testStruct);

    bitboard_t knightFreeMove = test.knightMove(COLOR::WHITE)[0];
    REQUIRE(knightFreeMove == 44272527353856ULL);
  }
  SECTION("Knight movement with targets"){
    testStruct.BlackBishop = 2594073385365405696ULL;
    testStruct.BlackKing = 1152921504606846976ULL;
    testStruct.BlackKnight = 4755801206503243776ULL;
    testStruct.BlackPawn = 71776119061217280ULL;
    testStruct.BlackQueen = 576460752303423488ULL;
    testStruct.BlackRook = 9295429630892703744ULL;

    testStruct.WhiteBishop = 524292;
    testStruct.WhiteBishop = 36;
    testStruct.WhiteQueen = 34359738368;
    testStruct.WhiteKnight = 8796093022208ULL;
    testStruct.WhitePawn = 65280;
    testStruct.WhiteQueen = 8;
    testStruct.WhiteKing = 16;
    testStruct.WhiteRook = 129;
    test.setGameState(testStruct);

    bitboard_t withTargets = test.knightMove(COLOR::WHITE)[0];
    REQUIRE(withTargets == 1450722176331153408ULL);
  }
}

TEST_CASE ("Check if move is set with capture") {
  move_t t1 = 6516U;
  move_t t2 = 6484U;
  move_t t3 = 1316U;

  david::move::Move m;
  move_t t4 = m.setGetValue(2ULL, 3ULL, 4);

  int to = m.getTo();
  int from = m.getFrom();


  REQUIRE(to == 2);
  REQUIRE(from == 3);

  REQUIRE(m.captures());

  m.setGetValue(2ULL, 3ULL, 0);

  REQUIRE_FALSE(m.captures());

}


TEST_CASE("test if bitisset can see if index is set") {
  using david::environment::bitIsSet;
  bitboard_t opponent = test.blackPieces();
  bitboard_t a = test.whitePieces();
  bitboard_t  b = *test.getDiagYAxis(524288ULL, DIRECTION::ANTI_DIAGONAL, true, 1) & 335544320ULL;

  //test.printBoard(b);
  //test.printBoard(a);
  REQUIRE(bitIsSet(a, 0ULL));
  REQUIRE_FALSE(bitIsSet(a, 35ULL));



}


TEST_CASE("Game initiation and generation of trees") {
  david::environment::Environment lastTest(COLOR::WHITE);

  std::vector <david::bitboard::gameState> tt;
  lastTest.computeGameStates(tt);
  //std::cout << tt.size() << std::endl;

  std::vector<david::bitboard::gameState>::iterator it;

  for (it = tt.begin(); it != tt.end(); it++) {
    //test.printBoard(it->WhiteKing | it->WhiteKnight | it->WhiteRook | it->WhitePawn | it->WhiteQueen | it->WhiteBishop);
  }

  //lastTest.printBoard(lastTest.combinedWhiteAttacks());
}

