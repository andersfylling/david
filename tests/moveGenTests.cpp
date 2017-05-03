//
// Created by Martin Klingenberg on 16.04.2017.
//

#include "../lib/Catch/include/catch.hpp"
#include "../src/bitboard.h"
#include "../src/environment.h"

//
//TEST_CASE("White pawn movement") {
//  using ::bitboard::gameState;
//  using ::bitboard::COLOR;
//  using ::bitboard::bitboard_t;
//  using ::bitboard::DIRECTION;
//  using ::environment::Environment;
//  using ::bitboard::makeBoardFromArray;
//
//  Environment test(COLOR::WHITE);
//  bitboard_t *bits;
//
//  bits = test.getDiagYAxis(::bitboard::WP, DIRECTION::UP, true, 1);
//  REQUIRE(bits[0] == 65536LL);
//  delete[] bits;
//
//  bits = test.getDiagYAxis(::bitboard::WP, DIRECTION::MAIN_DIAGONAL, true, 1);
//  REQUIRE(bits[0] == 131072LL);
//  delete[] bits;
//
//  bits = test.getDiagYAxis(::bitboard::WP, DIRECTION::ANTI_DIAGONAL, true, 1);
//  REQUIRE(bits[0] == 0LL);
//  delete[] bits;
//}
//
//TEST_CASE("Black pawn movement") {
//  using ::bitboard::gameState;
//  using ::bitboard::COLOR;
//  using ::bitboard::bitboard_t;
//  using ::bitboard::DIRECTION;
//  using ::environment::Environment;
//  using ::bitboard::makeBoardFromArray;
//
//  Environment test(COLOR::WHITE);
//  bitboard_t *bits;
//
//  bitboard_t cor = 1099511627776;
//  bits = test.getDiagYAxis(::bitboard::BP, DIRECTION::UP, true, 2);
//  REQUIRE(bits[0] == cor);
//  delete[] bits;
//
//  bits = test.getDiagYAxis(::bitboard::BP, DIRECTION::MAIN_DIAGONAL, true, 2);
//  REQUIRE(bits[0] == 0LL);
//  delete[] bits;
//
//  cor = 2199023255552;
//  bits = test.getDiagYAxis(::bitboard::BP, DIRECTION::ANTI_DIAGONAL, true, 2);
//  REQUIRE(bits[0] == cor);
//  delete[] bits;
//}
//
//TEST_CASE("Black knight movement") {
//  using ::bitboard::gameState;
//  using ::bitboard::COLOR;
//  using ::bitboard::bitboard_t;
//  using ::bitboard::DIRECTION;
//  using ::environment::Environment;
//  using ::bitboard::makeBoardFromArray;
//
//  Environment test(COLOR::WHITE);
//  bitboard_t *bits;
//
//  bitboard_t cor = 1099511627776;
//  test.printBoard(cor);
//}
//
//TEST_CASE("White knight movement") {
//  using ::bitboard::gameState;
//  using ::bitboard::COLOR;
//  using ::bitboard::bitboard_t;
//  using ::bitboard::DIRECTION;
//  using ::environment::Environment;
//  using ::bitboard::makeBoardFromArray;
//
//  Environment test(COLOR::WHITE);
//  bitboard_t *bits;
//}