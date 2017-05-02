//
// Created by lolos on 05.04.2017.
//

#ifndef CHESS_ANN_ENVIORNMENT_H
#define CHESS_ANN_ENVIORNMENT_H

#include "./bitboard.h"
#include <array>

enum COMPASS {NORTH, SOUTH, EAST, WEST, NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST};

class Environment {
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
  void printBoard(bitboard board);  // A damn sexy board representation


  // The limit makes a divide between sliding and moving
  // piece types. Pond and king from rook and queen
  // They return arrays. Most bitboards consists of more than one
  // piece.
  // main diagonal are from the top left corner to the bottom right
  // the antiDiagonal are from the top right corner to the bottom left
  // LOCK values 0 == no lock. 1 == WHITE-LIMIT 2 == BLACK-LIMIT
  // For X axis

  bitboard * getXAxisFromBoard(bitboard board, bool limit = 0, int lock = 0);
  bitboard * getDiagYAxis(bitboard board, DIRECTION dir, bool limit = false, int lock = 0);
  bitboard generateBlock(bitboard vector, DIRECTION dir, bitboard opponent);
  bitboard * knightMovement(bitboard board);

  bitboard whitePieces(); // Returns all white pieces
  bitboard blackPieces(); // Returns all black pieces

  // Mainly for finding completely legal moves
  bitboard combinedAttacks(); // All attacked pieces of opposing color

  bitboard * pawnMoves(COLOR color);
  bitboard * knightMove(COLOR color);
  bitboard KingMove(COLOR color);

  // Generate a bitboard based on a chess position: E6



  int numberOfPieces(bitboard board);     // For generating right sized arrays
  bitboard LSB (bitboard board);          // Gets least significant bit
  bitboard MSB (bitboard board);
  bitboard NSB (bitboard & board);        // Gets next significant bit
  void flipBit(bitboard &board, bitboard index);           // Flips a bit in a board

};

#endif //CHESS_ANN_ENVIORNMENT_H
