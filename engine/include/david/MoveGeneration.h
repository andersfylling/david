//
// Created by martin on 6/5/17.
//

#ifndef CHESS_ANN_MOVEGENERATION_H
#define CHESS_ANN_MOVEGENERATION_H
#include "bitboard.h"
#include "types.h"
#include <utility>
#include <vector>
namespace david {
namespace movegen {

/* Directions are in the perspective of white pieces
 *       N
 *   NW  |  NE
 *  W----|----E
 *    SW | SE
 *       S
 *       8
 *    9  |  7
 *  1----|---- -1
 *    -7 | -9
 *      -8
 */


// What types of moves do we have? Pls read below
enum MOVETYPE {QUIET = 0, DOUBLE_PAWN_PUSH = 1, CASTLE_K = 2, CASTLE_Q = 3,
  CAPTURES = 4, ENPASSANT = 5, KNIGHT_PROMO = 8, BISHOP_PROMO = 9,
  ROOK_PROMO = 10, QUEEN_PROMO = 11, NONTHREAT = 12};

enum DIRECTION { NORTH = 8, SOUTH = -8, EAST=-1, WEST=1, NORTH_EAST=7, SOUTH_EAST = -9, SOUTH_WEST = -7, NORTH_WEST = 9};


/*
 * These values should in theory increase speed. These are used instead of loops. Shifting one of these values to
 * a specified location. This will increase computation-time of
 */
const type::bitboard_t mainDiagonal[7] = {1ULL, 513ULL, 262657ULL, 134480385ULL, 68853957121ULL, 35253226045953ULL, 18049651735527937ULL};
const type::bitboard_t antiDiagonal[7] = {1ULL, 129ULL, 16513ULL, 2113665ULL, 270549121ULL, 34630287489ULL, 4432676798593ULL};
const type::bitboard_t xAxis[7] = {1ULL, 3ULL, 7ULL, 15ULL, 31ULL, 63ULL, 127ULL};
const type::bitboard_t yAxis[7] = {1ULL, 257ULL, 65793ULL, 16843009ULL, 4311810305ULL, 1103823438081ULL, 282578800148737ULL};

class MoveGenerator {
 private:
  std::vector<bitboard::move_t> moveList;
  bitboard::gameState state;
  type::bitboard_t attacks;

 public:
  // Constructors
  MoveGenerator();   // ONLY FOR TESTING


  // Utility
  type::bitboard_t white();
  type::bitboard_t black();
  type::bitboard_t moveToMap();
  void clearLists();
  bool nothernly(DIRECTION dir);
  void printMoves();
  int numberOfMoves();

  // Level 1 move basic generation of vectors
  type::bitboard_t * createVectors(type::bitboard_t board, DIRECTION dir, int steps);
  type::bitboard_t add_vector_to_board(type::bitboard_t destination, DIRECTION dir, type::bitboard_t dist);
  std::pair<type::bitboard_t , type::bitboard_t > distanceToEdge(type::bitboard_t board, DIRECTION dir);
  type::bitboard_t generateBlock(type::bitboard_t board, DIRECTION dir, bool own = false);

  // Level 2 higher level function
  type::bitboard_t reduceVector(type::bitboard_t vector, bitboard::COLOR color, DIRECTION dir, bool pawn = false);
  type::bitboard_t * getReducedDiagonals(type::bitboard_t pieces, bitboard::COLOR color, int step = 0);
  type::bitboard_t * getReducedXY(type::bitboard_t pieces, bitboard::COLOR color, int step = 0);
  type::bitboard_t * rookMovement(type::bitboard_t board);

  // Level 3 generation of move_t
  void addAttack(type::bitboard_t * vectors, int number);
  void pawnMoves(bitboard::COLOR color, bool vector = false);
  void bishopMoves(bitboard::COLOR color, bool vector = false);
  void knightMoves(bitboard::COLOR color, bool vector = false);
  void rookMoves(bitboard::COLOR color, bool vector = false);
  void queenMoves(bitboard::COLOR color, bool vector = false);
  void kingMoves(bitboard::COLOR color, bool vector = false);
  void castling(bitboard::COLOR color);


  // Level 4 handling
  void capturePiece(bitboard::COLOR color, type::bitboard_t index, bitboard::gameState &st);
  void setGameState(type::gameState_ptr st);
  void applyMove(bitboard::move_t m, bitboard::gameState & s);
  bool moveIsLegal(bitboard::move_t m, bitboard::COLOR c);
  void generateMoves(bitboard::COLOR color, bool legacy = false);
  void generateAttacks(bitboard::COLOR color);


  /*
   * Legacy functions
   * the following functions is made to generate moves
   * for older versions of the treeGen
   */
  void generateGameStates(std::vector<bitboard::gameState> &states);
  void generateGameStates(std::vector<bitboard::gameState> * states);

};

using type::move_t;
using type::bitboard_t ;

bool bitIsSet(move_t board, move_t index);



// Move class interpritates the 16-bit move values
// And makes them ineracable with human beings
class Move {
 private:
  move_t mv;
 public:

  void printMove();
  Move(); // Setter mv til 0
  Move(type::move_t m);
  void set(type::bitboard_t to, type::bitboard_t from, MOVETYPE type);
  void set(type::move_t m);


  MOVETYPE getType();
  type::bitboard_t getFrom();
  type::bitboard_t getTo();
  bitboard::move_t getMove();
};

};  // End of movegeneration
}; // End of david

#endif //CHESS_ANN_MOVEGENERATION_H