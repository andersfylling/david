//
// Created by lolos on 05.04.2017.
//

#ifndef CHESS_ANN_ENVIORNMENT_H
#define CHESS_ANN_ENVIORNMENT_H

#include "./bitboard.h"
#include <array>



namespace environment {

// deal with bitboard_t dependencies
using ::bitboard::gameState;
using ::bitboard::COLOR;
using ::bitboard::bitboard_t;
using ::bitboard::DIRECTION;


enum COMPASS {NORTH, SOUTH, EAST, WEST, NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST};

class Enviornment {
 private:
  int moves;    // Number of moves for performance measuring
  gameState state;  // Current gamestate
  // If the king has been moved, or been set in check.
  // Castling is not allowed. These values are then set to false
  // This is an important strategic factor in chess
  bool whiteCastling;
  bool blackCastling;
  COLOR hostColor;





 public:
  Environment(COLOR color);
  void printBoard(bitboard_t board);  // A damn sexy board representation


  // The limit makes a divide between sliding and moving
  // piece types. Pond and king from rook and queen
  // They return arrays. Most bitboards consists of more than one
  // piece.
  // main diagonal are from the top left corner to the bottom right
  // the antiDiagonal are from the top right corner to the bottom left
  // LOCK values 0 == no lock. 1 == WHITE-LIMIT 2 == BLACK-LIMIT
  // For X axis

  bitboard_t * getXAxisFromBoard(bitboard_t board, bool limit = 0, int lock = 0);
  bitboard_t * getDiagYAxis(bitboard_t board, DIRECTION dir, bool limit = false, int lock = 0);
  bitboard_t generateBlock(bitboard_t vector, DIRECTION dir, bitboard_t opponent);
  bitboard_t * knightMovement(bitboard_t board);

  bitboard_t whitePieces(); // Returns all white pieces
  bitboard_t blackPieces(); // Returns all black pieces

  // Mainly for finding completely legal moves
  bitboard_t combinedAttacks(); // All attacked pieces of opposing color

  bitboard_t * pawnMoves(COLOR color);
  bitboard_t * knightMove(COLOR color);
  bitboard_t * KingMove(COLOR color);

  // Generate a bitboard_t based on a chess position: E6



  int numberOfPieces(bitboard_t board);     // For generating right sized arrays
  bitboard_t LSB (bitboard_t board);          // Gets least significant bit
  bitboard_t MSB (bitboard_t board);
  bitboard_t NSB (bitboard_t & board);        // Gets next significant bit
  void flipBit(bitboard_t &board, bitboard_t index);           // Flips a bit in a board

};
}

#endif //CHESS_ANN_ENVIORNMENT_H
