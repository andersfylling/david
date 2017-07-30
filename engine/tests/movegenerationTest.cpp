//
// Created by martin on 6/6/17.
//
#include "catch.hpp"
#include "david/MoveGeneration.h"
#include "david/bitboard.h"
#include <utility>
#include <david/utils.h>

using david::type::bitboard_t;
using david::bitboard::COLOR;
using david::movegen::DIRECTION;
using david::movegen::MOVETYPE;
using david::type::move_t;
using david::movegen::Move;
david::movegen::MoveGenerator generator;


TEST_CASE("Test generation of vectors") {
  david::bitboard::bitboard_t Wpawns, Bpawns, Wcastle, Bcastle, QueenTest;
  Wpawns = 65280ULL;
  Bpawns = 71776119061217280ULL;
  david::bitboard::bitboard_t * b;
  QueenTest = 34359738368ULL;

  b = generator.createVectors(QueenTest, DIRECTION::NORTH, 0);
  REQUIRE(b[0] == 578721348210130944ULL);
  //david::utils::printBoard(*b);

  b = generator.createVectors(QueenTest, DIRECTION::SOUTH, 0);
  REQUIRE(b[0] == 134744072ULL);

  b = generator.createVectors(QueenTest, DIRECTION::SOUTH_EAST, 0);
  REQUIRE(b[0] == 67240192ULL);


  b = generator.createVectors(Wpawns, DIRECTION::NORTH, 0);
  //david::utils::printBoard(*b);
  REQUIRE(b[0] == 72340172838076416ULL);
  REQUIRE(b[1] == 144680345676152832ULL);
  REQUIRE(b[7] == 9259542123273781248ULL);

 b = generator.createVectors(Wpawns, DIRECTION::NORTH, 1);
  REQUIRE(b[0] == 65536ULL);
  REQUIRE(b[1] == 131072ULL);
  REQUIRE(b[7] == 8388608ULL);

  b = generator.createVectors(Wpawns, DIRECTION::NORTH_EAST, 1);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 65536ULL);
  REQUIRE(b[7] == 4194304ULL);

  b = generator.createVectors(Wpawns, DIRECTION::NORTH_EAST, 0);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 65536ULL);
  REQUIRE(b[7] == 145249953336262656ULL);

  b = generator.createVectors(Bpawns, DIRECTION::SOUTH_EAST, 0);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 0ULL);
  REQUIRE(b[1] == 1099511627776ULL);
  REQUIRE(b[7] == 70506452091906ULL);

  b = generator.createVectors(Bpawns, DIRECTION::SOUTH_WEST, 0);
  //david::utils::printBoard(b[6]);

  REQUIRE(b[0] == 2216338399296ULL);
  REQUIRE(b[1] == 4432676798592ULL);
  REQUIRE(b[7] == 0ULL);

  b = generator.createVectors(Bpawns, DIRECTION::WEST, 0);
  //david::utils::printBoard(b[0]);

  REQUIRE(b[0] == 71494644084506624ULL);
  REQUIRE(b[1] == 70931694131085312ULL);
  REQUIRE(b[7] == 0ULL);

}

TEST_CASE("DISTANCE TO EDGES") {
  std::pair <david::bitboard::bitboard_t, david::bitboard::bitboard_t> rp;

  // TESTS NORTH EAST MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(17179869184ULL), DIRECTION::NORTH_EAST);
  REQUIRE(rp.first == 3);
  REQUIRE(rp.second == 2);

  // TESTS NORTH MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(137438953472ULL), DIRECTION::NORTH);
  REQUIRE(rp.first == 3);
  REQUIRE(rp.second == 7);

  // NORTH WEST MOVEMENT
  rp = generator.distanceToEdge(david::utils::LSB(2097152ULL), DIRECTION::NORTH_WEST);
  REQUIRE(rp.first == 5);
  REQUIRE(rp.second == 2);

  rp = generator.distanceToEdge(david::utils::LSB(262144ULL), DIRECTION::SOUTH);
  REQUIRE(rp.first == 2);
  REQUIRE(rp.second == 7);
}

TEST_CASE("ENCODING OF MOVES") {
  david::bitboard::move_t case1, case2, case3;
  case1 = 11572U;    // Pawn moves from 11 to 19 with capture
  case2 = 21443U;  // Piece moves from 20 to 60 with queen castle
  case3 = 50127U;   // Piece from 25 to 30 with max parameters

  Move m1(11572);
  Move m2;
  m2.set(11, 19, MOVETYPE::CAPTURES);
  REQUIRE(m1.getMove() == m2.getMove());

  Move m3(21443);
  m2.set(20, 60, MOVETYPE::CASTLE_Q);

  REQUIRE(m2.getMove() == m3.getMove());
  REQUIRE(m2.getTo() == 20);
  REQUIRE(m2.getFrom() == 60);
  REQUIRE(m2.getType() == MOVETYPE::CASTLE_Q);
}


TEST_CASE("BLOCK AND REDUCE VECTOR") {
  bitboard_t res, queen;

  queen =  34359738368ULL;

  //david::utils::printBoard(*generator.createVectors(queen, DIRECTION::NORTH, 0));

  res = generator.generateBlock(queen, DIRECTION::NORTH, false);
  REQUIRE(res == 578721348210130944ULL);

  res = generator.generateBlock(queen, DIRECTION::NORTH, true);
  REQUIRE(res == 578721382569869312ULL);

  res = generator.reduceVector(578721348210130944ULL, COLOR::WHITE, DIRECTION::NORTH);
  //david::utils::printBoard(res);

  res = generator.reduceVector(4611686018427387904, COLOR::WHITE, DIRECTION::NORTH);
  //david::utils::printBoard(res);

}



TEST_CASE ("PAWN MOVEMENT TESTS") {
  david::type::gameState_t st;
  Move m;
  m.set(35ULL, 51ULL, MOVETYPE::DOUBLE_PAWN_PUSH);
  david::type::gameState_t stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 51509955097788416ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 36;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 18014467296111360ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 129;

  stt.lastBlackMove = m.getMove();

  st = stt;

  generator.pawnMoves(COLOR::WHITE);
  david::bitboard::bitboard_t attack, attack2;

  attack = generator.moveToMap();

  generator.clearLists();

  generator.pawnMoves(COLOR::BLACK);
  attack2 = generator.moveToMap();

  REQUIRE(attack == 4294901760ULL);
  REQUIRE(attack2 == 281470681743360ULL);

  generator.setGameState(st);
  generator.clearLists();

  generator.pawnMoves(COLOR::WHITE);
  //david::utils::printGameState(st);
}

TEST_CASE("getReducedDiagonals") {
  david::bitboard::gameState stt;

  stt.WhiteQueen = 8589934592ULL;
  bitboard_t * res = generator.getReducedDiagonals(stt.WhiteQueen, COLOR::WHITE);
  //david::utils::printBoard(*res);
  REQUIRE(*res == 1155178801995448320ULL);
}

TEST_CASE("getReducedXY") {
  bitboard_t queenMiddle = 34359738368ULL;
  bitboard_t r = *generator.getReducedXY(queenMiddle, COLOR::WHITE);
  REQUIRE(r == 578721378409644032ULL);
}

TEST_CASE("Bishop and Knight moves") {
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 51509955097788416ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 8388612ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 18014467296111360ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 129;
  st = stt;

  generator.setGameState(st);

  //
  //david::utils::printBoard(generator.white() | generator.black());
  generator.clearLists();
  generator.bishopMoves(COLOR::WHITE);

  //david::utils::printBoard(generator.moveToMap());
  REQUIRE(generator.moveToMap() == 290499906664153120ULL);

  generator.clearLists();
  generator.knightMoves(COLOR::WHITE);

  REQUIRE(generator.moveToMap() == 2428928ULL);
  //david::utils::printBoard(generator.moveToMap());
}

TEST_CASE ("Rook Movement") {
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 51509955097788416ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 8388612ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 18014467296111360ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 4294967424ULL;
  st = stt;

  generator.setGameState(st);
  generator.clearLists();
  generator.rookMoves(COLOR::WHITE);

  REQUIRE(generator.moveToMap() == 282634634723328ULL);
  //david::utils::printBoard(generator.moveToMap());
  //david::utils::printGameState(st);
}

TEST_CASE("Queen Movement") {
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 51509955097788416ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 8388612ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 18014467296111360ULL;
  stt.WhiteQueen = 8589934592;
  stt.WhiteKing = 16;
  stt.WhiteRook = 4294967424ULL;
  st = stt;
  generator.setGameState(st);
  generator.clearLists();
  generator.queenMoves(COLOR::WHITE);

  REQUIRE(generator.moveToMap() == 1155744002545418240ULL);
}

TEST_CASE("King movement") {
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 51509955097788416ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 8388612ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 18014467296111360ULL;
  stt.WhiteQueen = 8589934592;
  stt.WhiteKing = 16;
  stt.WhiteRook = 4294967424ULL;
  st = stt;
  generator.setGameState(st);
  generator.clearLists();
  generator.kingMoves(COLOR::WHITE);
  REQUIRE(generator.moveToMap() == 4136ULL);
}

TEST_CASE("Basic generation of moves") {
  // Test if correct number of moves is created
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 2594073385365405696ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 4755801206503243776ULL;
  stt.BlackPawn = 71776119061217280ULL;
  stt.BlackQueen = 1152921504606846976ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 36ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 65280ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 129ULL;
  st = stt;
  generator.setGameState(st);

  generator.clearLists();
  generator.kingMoves(COLOR::WHITE);

  REQUIRE(generator.numberOfMoves() == 0);

  generator.clearLists();

  generator.knightMoves(COLOR::WHITE);
  REQUIRE(generator.numberOfMoves() == 4);

  generator.clearLists();
  generator.pawnMoves(COLOR::WHITE);

  REQUIRE(generator.numberOfMoves() == 16);
}


TEST_CASE("CASTLING") {
  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 0ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 0ULL;
  stt.BlackPawn = 71776119061217280ULL;
  stt.BlackQueen = 0ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 36ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 65280ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 129ULL;
  st = stt;
  generator.setGameState(st);

  generator.clearLists();

  generator.castling(COLOR::BLACK);
  REQUIRE(2449958197289549824ULL == generator.moveToMap());
}

TEST_CASE("LEGAL MOVES") {
  // Test if only moves that put the king out of chess
  // Black queen at 25 putting king into chess
  // should be 3 moves

  david::type::gameState_t st;
  david::bitboard::gameState stt;

  stt.BlackBishop = 0ULL;
  stt.BlackKing = 576460752303423488ULL;
  stt.BlackKnight = 0ULL;
  stt.BlackPawn = 71776119061217280ULL;
  stt.BlackQueen = 33554432ULL;
  stt.BlackRook = 9295429630892703744ULL;

  stt.WhiteBishop = 36ULL;
  stt.WhiteKnight = 66;
  stt.WhitePawn = 402712320ULL;
  stt.WhiteQueen = 8;
  stt.WhiteKing = 16;
  stt.WhiteRook = 129ULL;
  st = stt;
  generator.setGameState(st);

  generator.generateMoves(COLOR::WHITE);
  //david::utils::printBoard(generator.moveToMap());
  //david::utils::printBoard(generator.white());
  REQUIRE(generator.numberOfMoves() == 6);
}

TEST_CASE("PERFT") {
  const int depth = 6;

  REQUIRE(::david::utils::perft(depth));
}