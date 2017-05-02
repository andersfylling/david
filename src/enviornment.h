//
// Created by lolos on 05.04.2017.
//

#ifndef CHESS_ANN_ENVIORNMENT_H
#define CHESS_ANN_ENVIORNMENT_H

#include "./bitboard.h"
#include "./search.h"
#include <array>


class Enviornment {
 private:
  int moves;    // Number of moves for performance measuring
  gameState state;  // Current gamestate
  bool pawnStepWhite[8];
  bool pawnStepBlack[8];
  // If the king has been moved, or been set in check.
  // Castling is not allowed. These values are then set to false
  // This is an important strategic factor in chess
  bool whiteCastling;
  bool blackCastling;
  COLOR hostColor;





 public:
  Enviornment(COLOR color);
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
  bitboard * knightMovement(bitboard board);

  bitboard whitePieces(); // Returns all white pieces
  bitboard blackPieces(); // Returns all black pieces

  // Mainly for finding completely legal moves
  bitboard combinedAttacks(); // All attacked pieces of opposing color

  bitboard * pawnMoves(COLOR color);



  int numberOfPieces(bitboard board);     // For generating right sized arrays
  bitboard LSB (bitboard board);          // Gets least signifigant bit
  bitboard NSB (bitboard & board);        // Gets next sigifigant bit
  void flipBit(bitboard &board, bitboard index);           // Flips a bit in a board

};

#endif //CHESS_ANN_ENVIORNMENT_H
