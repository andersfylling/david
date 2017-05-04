//
// Created by Martin Klingenberg on 16.04.2017.
//

#include "../lib/Catch/include/catch.hpp"
#include "../src/bitboard.h"
#include "../src/environment.h"


::environment::gameState testStruct; // White Queen in the middle. Rest is normal


using ::bitboard::bitboard_t;
using ::bitboard::WPc;
using ::bitboard::WCc;
using ::bitboard::WNc;
using ::bitboard::WRc;
using ::bitboard::WQc;
using ::bitboard::WKc;
using ::bitboard::BPc;
using ::bitboard::BCc;
using ::bitboard::BRc;
using ::bitboard::BNc;
using ::bitboard::BQc;
using ::bitboard::BKc;
using ::bitboard::COLOR;
using ::bitboard::makeBoardFromArray;
using ::bitboard::DIRECTION;

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

::environment::Environment test(COLOR::WHITE);
bitboard_t *bits;


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
  bitboard_t  cor = 20;

  bits = test.getXAxisFromBoard(WQ, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 247;
  bits = test.getXAxisFromBoard(WQ);

  REQUIRE(bits[0] == cor);

  delete [] bits;


}

TEST_CASE("YDiagGeneration") {
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
  REQUIRE(cor == test.QueenMove(COLOR::WHITE));
}

TEST_CASE ("REDUCE VECTOR") {
  bitboard_t board = test.reduceVector(*test.getDiagYAxis(34359738368LL, DIRECTION::UP, false, 1), test.blackPieces(), test.whitePieces(), DIRECTION::UP);
  REQUIRE(board == 2260595906707456);

  board = test.reduceVector(*test.getDiagYAxis(34359738368LL, DIRECTION::UP, false, 2), test.blackPieces(), test.whitePieces(), DIRECTION::DOWN);
  REQUIRE(board == 134742016);
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
  REQUIRE(t2 == 36100411639201792);
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

  //test.printBoard(testStruct.WhiteRook);
  bitboard_t b1 = test.RookMove(COLOR::WHITE)[0];
  bitboard_t b2 = test.RookMove(COLOR::WHITE)[1];
  //test.printBoard(b1);

}

TEST_CASE("Change chess index into an bitboard array index; E6 => 43") {
    REQUIRE(test.chessIndexToBitboardIndex("H1") == 0);
  REQUIRE(test.chessIndexToBitboardIndex("H2") == 8);
  REQUIRE(test.chessIndexToBitboardIndex("H3") == 16);
  REQUIRE(test.chessIndexToBitboardIndex("H4") == 24);
  REQUIRE(test.chessIndexToBitboardIndex("H5") == 32);
  REQUIRE(test.chessIndexToBitboardIndex("H6") == 40);
  REQUIRE(test.chessIndexToBitboardIndex("H7") == 48);
  REQUIRE(test.chessIndexToBitboardIndex("H8") == 56);

  REQUIRE(test.chessIndexToBitboardIndex("G1") == 1);
  REQUIRE(test.chessIndexToBitboardIndex("G2") == 9);
  REQUIRE(test.chessIndexToBitboardIndex("G3") == 17);
  REQUIRE(test.chessIndexToBitboardIndex("G4") == 25);
  REQUIRE(test.chessIndexToBitboardIndex("G5") == 33);
  REQUIRE(test.chessIndexToBitboardIndex("G6") == 41);
  REQUIRE(test.chessIndexToBitboardIndex("G7") == 49);
  REQUIRE(test.chessIndexToBitboardIndex("G8") == 57);

}