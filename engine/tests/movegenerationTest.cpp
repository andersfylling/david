//
// Created by martin on 6/6/17.
//
#include "catch.hpp"
#include "david/MoveGeneration.h"
#include "david/bitboard.h"
#include <utility>
#include <david/utils.h>

david::movegen::MoveGenerator generator;

TEST_CASE("Test generation of vectors") {
  david::bitboard::bitboard_t Wpawns, Bpawns, Wcastle, Bcastle;
  Wpawns = 65280ULL;
  Bpawns = 71776119061217280ULL;
  david::bitboard::bitboard_t * b;

  b = generator.createVectors(Wpawns, david::movegen::DIRECTION::NORTH, 0);
  //david::utils::printBoard(*b);
  REQUIRE(b[0] == 72340172838076416ULL);
  REQUIRE(b[1] == 144680345676152832ULL);
  REQUIRE(b[7] == 9259542123273781248ULL);

 b = generator.createVectors(Wpawns, david::movegen::DIRECTION::NORTH, 1);
  REQUIRE(b[0] == 65536ULL);
  REQUIRE(b[1] == 131072ULL);
  REQUIRE(b[7] == 8388608ULL);

  b = generator.createVectors(Wpawns, david::movegen::DIRECTION::NORTH_EAST, 1);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 65536ULL);
  REQUIRE(b[7] == 4194304ULL);

  b = generator.createVectors(Wpawns, david::movegen::DIRECTION::NORTH_EAST, 0);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 65536ULL);
  REQUIRE(b[7] == 145249953336262656ULL);

  b = generator.createVectors(Bpawns, david::movegen::DIRECTION::SOUTH_EAST, 0);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 1099511627776ULL);
  REQUIRE(b[7] == 70506452091906ULL);

  b = generator.createVectors(Bpawns, david::movegen::DIRECTION::SOUTH_WEST, 0);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 2216338399296ULL);
  REQUIRE(b[1] == 4432676798592ULL);
  REQUIRE(b[7] == 0ULL);

  b = generator.createVectors(Bpawns, david::movegen::DIRECTION::WEST, 0);
  //david::utils::printBoard(b[0]);

  REQUIRE(b[0] == 71494644084506624ULL);
  REQUIRE(b[1] == 70931694131085312ULL);
  REQUIRE(b[7] == 0ULL);

}

TEST_CASE("DISTANCE TO EDGES") {
  std::pair <david::bitboard::bitboard_t, david::bitboard::bitboard_t> rp;

  // TESTS NORTH EAST MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(17179869184ULL), david::movegen::DIRECTION::NORTH_EAST);
  REQUIRE(rp.first == 3);
  REQUIRE(rp.second == 2);

  // TESTS NORTH MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(137438953472ULL), david::movegen::DIRECTION::NORTH);
  REQUIRE(rp.first == 3);
  REQUIRE(rp.second == 7);

  // NORTH WEST MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(2097152ULL), david::movegen::DIRECTION::NORTH_WEST);
  REQUIRE(rp.first == 5);
  REQUIRE(rp.second == 2);

  rp = generator.distanceToEdge(david::utils::LSB(262144ULL), david::movegen::DIRECTION::SOUTH);
  REQUIRE(rp.first == 2);
  REQUIRE(rp.second == 7);
}

TEST_CASE("ENCODING OF MOVES") {
  david::bitboard::move_t case1, case2, case3;
  case1 = 11572U;    // Pawn moves from 11 to 19 with capture
  case2 = 21443U;  // Piece moves from 20 to 60 with queen castle
  case3 = 50127U;   // Piece from 25 to 30 with max parameters

  david::movegen::Move m1(11572);
  david::movegen::Move m2;
  m2.set(11, 19, david::movegen::MOVETYPE::CAPTURES);
  REQUIRE(m1.getMove() == m2.getMove());

  david::movegen::Move m3(21443);
  m2.set(20, 60, david::movegen::MOVETYPE::CASTLE_Q);

  REQUIRE(m2.getMove() == m3.getMove());
  REQUIRE(m2.getTo() == 20);
  REQUIRE(m2.getFrom() == 60);
  REQUIRE(m2.getType() == david::movegen::MOVETYPE::CASTLE_Q);
}

TEST_CASE ("PAWN MOVEMENT TESTS") {
  generator.pawnMoves(david::bitboard::COLOR::WHITE);
  david::bitboard::bitboard_t attack = generator.moveToMap();
  david::utils::printBoard(attack);
  david::utils::printBoard(generator.white());
}