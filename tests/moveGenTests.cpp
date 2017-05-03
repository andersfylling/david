//
// Created by Martin Klingenberg on 16.04.2017.
//

#include "../lib/Catch/include/catch.hpp"
#include "../src/bitboard.h"
#include "../src/environment.h"



gameState testStruct; // White Queen in the middle. Rest is normal



bitboard WP = makeBoardFromArray(WPc);
bitboard WC = makeBoardFromArray(WCc);
bitboard WN = makeBoardFromArray(WNc);
bitboard WR = makeBoardFromArray(WRc);
bitboard WQ = makeBoardFromArray(WQc);
bitboard WK = makeBoardFromArray(WKc);

bitboard BP = makeBoardFromArray(BPc);
bitboard BC = makeBoardFromArray(BCc);
bitboard BR = makeBoardFromArray(BRc);
bitboard BN = makeBoardFromArray(BNc);
bitboard BQ = makeBoardFromArray(BQc);
bitboard BK = makeBoardFromArray(BKc);

Enviornment test(COLOR::WHITE);
bitboard * bits;
TEST_CASE("White pawn movement") {
  using ::bitboard::gameState;
  using ::bitboard::COLOR;
  using ::bitboard::bitboard_t;
  using ::bitboard::DIRECTION;
  using ::environment::Environment;
  using ::bitboard::makeBoardFromArray;

  Environment test(COLOR::WHITE);
  bitboard_t *bits;
  bitboard_t WP;

  bits = test.getDiagYAxis(WP, DIRECTION::UP, true, 1);
  REQUIRE(bits[0] == 65536LL);
  delete[] bits;

  bits = test.getDiagYAxis(WP, DIRECTION::MAIN_DIAGONAL, true, 1);
  REQUIRE(bits[0] == 131072LL);
  delete[] bits;

  bits = test.getDiagYAxis(WP, DIRECTION::ANTI_DIAGONAL, true, 1);
  REQUIRE(bits[0] == 0LL);
  delete[] bits;
}

TEST_CASE("Black pawn movement") {
  using ::bitboard::gameState;
  using ::bitboard::COLOR;
  using ::bitboard::bitboard_t;
  using ::bitboard::DIRECTION;
  using ::environment::Environment;
  using ::bitboard::makeBoardFromArray;

  Environment test(COLOR::WHITE);
  bitboard_t *bits;
  bitboard_t BP;

  cor = 2199023255552;
  bits = test.getDiagYAxis(BP, DIRECTION::ANTI_DIAGONAL, true, 2);
  REQUIRE(bits[0] == cor);
  delete [] bits;
}


TEST_CASE("White knight movement") {
  bitboard cor = 1099511627776;
  bits = test.getDiagYAxis(BP, DIRECTION::UP, true, 2);
  REQUIRE(bits[0] == cor);
  delete[] bits;

  bits = test.getDiagYAxis(BP, DIRECTION::MAIN_DIAGONAL, true, 2);
  REQUIRE(bits[0] == 0LL);
  delete[] bits;

  cor = 2199023255552;
  bits = test.getDiagYAxis(BP, DIRECTION::ANTI_DIAGONAL, true, 2);
  REQUIRE(bits[0] == cor);
  delete[] bits;
}

TEST_CASE("X-AXIS movement") {
  bitboard  cor = 20;

  bits = test.getXAxisFromBoard(WQ, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 247;
  bits = test.getXAxisFromBoard(WQ);

  REQUIRE(bits[0] == cor);

  delete [] bits;


}

TEST_CASE("YDiagGeneration") {
  bitboard cor = 578721382704613376;
  bits = test.getDiagYAxis(WQ, UP);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 2048;
  bits = test.getDiagYAxis(WQ, UP, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 4096;
  bits = test.getDiagYAxis(WQ, MAIN_DIAGONAL, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 1024;
  bits = test.getDiagYAxis(WQ, ANTI_DIAGONAL, 1);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 550831656960;
  bits = test.getDiagYAxis(WQ, MAIN_DIAGONAL);
  REQUIRE(bits[0] == cor);

  delete [] bits;

  cor = 16909312;
  bits = test.getDiagYAxis(WQ, ANTI_DIAGONAL);
  REQUIRE(bits[0] == cor);

  delete [] bits;
}


TEST_CASE("Queen move BLOCK") {
  //test.printBitboards();
  testStruct.BlackBishop = 2594073385365405696LL;
  testStruct.BlackKing = 1152921504606846976LL;
  testStruct.BlackKnight = 4755801206503243776LL;
  testStruct.BlackPawn = 71776119061217280LL;
  testStruct.BlackQueen = 576460752303423488LL;
  testStruct.BlackRook = 9295429630892703744LL;

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

  bitboard cor = 11853796676861952;
  //test.printBoard(test.QueenMove(WHITE));
  REQUIRE(cor == test.QueenMove(WHITE));
}

TEST_CASE ("REDUCE VECTOR") {
  bitboard board = test.reduceVector(*test.getDiagYAxis(34359738368LL, UP, false, 1), test.blackPieces(), test.whitePieces(), UP);
  REQUIRE(board == 2260595906707456);

  board = test.reduceVector(*test.getDiagYAxis(34359738368LL, UP, false, 2), test.blackPieces(), test.whitePieces(), DOWN);
  REQUIRE(board == 134742016);
}
