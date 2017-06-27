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



enum DIRECTION { NORTH = 8, SOUTH = -8, EAST=-1, WEST=1, NORTH_EAST=7, SOUTH_EAST = -9, SOUTH_WEST = -7, NORTH_WEST = 9};

/*
 * These values should in theory increase speed. These are used instead of loops. Shifting one of these values to
 * a specified location. This will increase computation-time of
 */
const bitboard::bitboard_t mainDiagonal[7] = {1ULL, 513ULL, 262657ULL, 134480385ULL, 68853957121ULL, 35253226045953ULL, 18049651735527937ULL};
const bitboard::bitboard_t antiDiagonal[7] = {1ULL, 129ULL, 16513ULL, 2113665ULL, 270549121ULL, 34630287489ULL, 4432676798593ULL};
const bitboard::bitboard_t xAxis[7] = {1ULL, 3ULL, 7ULL, 15ULL, 31ULL, 63ULL, 127ULL};
const bitboard::bitboard_t yAxis[7] = {1ULL, 257ULL, 65793ULL, 16843009ULL, 4311810305ULL, 1103823438081ULL, 282578800148737ULL};



class MoveGenerator {
 private:
  std::vector<bitboard::move_t> moveList;
  bitboard::gameState state;
  bitboard::bitboard_t attacks;





 public:
  // Constructors
  MoveGenerator();   // ONLY FOR TESTING


  // Utility
  bitboard::bitboard_t white();
  bitboard::bitboard_t black();
  bitboard::bitboard_t moveToMap();
  void clearLists();
  bool nothernly(DIRECTION dir);
  void printMoves();
  int numberOfMoves();

  // Level 1 move basic generation of vectors
  bitboard::bitboard_t * createVectors(bitboard::bitboard_t board, DIRECTION dir, int steps);
  bitboard::bitboard_t add_vector_to_board(bitboard::bitboard_t destination, DIRECTION dir, bitboard::bitboard_t dist);
  std::pair<bitboard::bitboard_t, bitboard::bitboard_t> distanceToEdge(bitboard::bitboard_t board, DIRECTION dir);
  bitboard::bitboard_t generateBlock(bitboard::bitboard_t board, DIRECTION dir, bool own = false);

  // Level 2 higher level function
  bitboard::bitboard_t reduceVector(bitboard::bitboard_t vector, bitboard::COLOR color, DIRECTION dir, bool pawn = false);
  bitboard::bitboard_t * getReducedDiagonals(bitboard::bitboard_t pieces, bitboard::COLOR color, int step = 0);
  bitboard::bitboard_t * getReducedXY(bitboard::bitboard_t pieces, bitboard::COLOR color, int step = 0);
  bitboard::bitboard_t * rookMovement(bitboard::bitboard_t board);

  // Level 3 generation of move_t
  void addAttack(bitboard::bitboard_t * vectors, int number);
  void pawnMoves(bitboard::COLOR color, bool vector = false);
  void bishopMoves(bitboard::COLOR color, bool vector = false);
  void knightMoves(bitboard::COLOR color, bool vector = false);
  void rookMoves(bitboard::COLOR color, bool vector = false);
  void queenMoves(bitboard::COLOR color, bool vector = false);
  void kingMoves(bitboard::COLOR color, bool vector = false);
  void castling(bitboard::COLOR color);


  // Level 4 handling
  void capturePiece(bitboard::COLOR color, bitboard::bitboard_t index, bitboard::gameState &st);
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

};

using bitboard::move_t;
using bitboard::bitboard_t;

bool bitIsSet(move_t board, move_t index);

enum MOVETYPE {QUIET = 0, DOUBLE_PAWN_PUSH = 1, CASTLE_K = 2, CASTLE_Q = 3,
                CAPTURES = 4, ENPASSANT = 5, KNIGHT_PROMO = 8, BISHOP_PROMO = 9,
                ROOK_PROMO = 10, QUEEN_PROMO = 11, NONTHREAT = 12};

// Move class interpritates the 16-bit move values
// And makes them ineracable with human beings
class Move {
 private:
  move_t mv;
 public:

  void printMove();
  Move(); // Setter mv til 0
  Move(bitboard::move_t m);
  void set(bitboard::bitboard_t to, bitboard::bitboard_t from, MOVETYPE type);
  void set(bitboard::move_t m);


  MOVETYPE getType();
  bitboard::bitboard_t getFrom();
  bitboard::bitboard_t getTo();
  bitboard::move_t getMove();
};

};  // End of movegeneration
}; // End of david

#endif //CHESS_ANN_MOVEGENERATION_H
