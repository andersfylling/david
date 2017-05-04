//
// Created by lolos on 05.04.2017.
//

#ifndef CHESS_ANN_ENVIORNMENT_H
#define CHESS_ANN_ENVIORNMENT_H

#include "./bitboard.h"
#include <array>

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
  Enviornment(COLOR color);
  void printBoard(bitboard board);  // A damn sexy board representation
  void setGameState(gameState st);  // Setting the gamestate for testing
  void printBitboards();            // Prints number values of all 12 boards


  // The limit makes a divide between sliding and moving
  // piece types. Pond and king from rook and queen
  // They return arrays. Most bitboards consists of more than one
  // piece.
  // main diagonal are from the top left corner to the bottom right
  // the antiDiagonal are from the top right corner to the bottom left
  // LOCK values 0 == no lock. 1 == WHITE-LIMIT 2 == BLACK-LIMIT
  // For X axis

  // LEVEL 0 of moveGen - The ugly bitflipping and CPU stuff
  int numberOfPieces(bitboard board);     // For generating right sized arrays
  bitboard LSB (bitboard board);          // Gets least signifigant bit
  bitboard MSB (bitboard board);
  bitboard NSB (bitboard & board);        // Gets next sigifigant bit
  void flipBit(bitboard &board, bitboard index);           // Flips a bit in a board

  // LEVEL 1 of moveGen - Basic attack vectors
  bitboard * getXAxisFromBoard(bitboard board, bool limit = 0, int lock = 0);
  bitboard * getDiagYAxis(bitboard board, DIRECTION dir, bool limit = false, int lock = 0);
  bitboard generateBlocK(bitboard vector, DIRECTION dir, bitboard oponent);
  bitboard * knightMovement(bitboard board);


  // LEVEL 2 of moveGen - Intermediate logic
  bitboard whitePieces(); // Returns all white pieces
  bitboard blackPieces(); // Returns all black pieces

  // Funksjonen skal returnere et bitboard som trekker fra motstandere og egne riktig
  // Får tilbake et bitboard som er modifisert
  bitboard reduceVector(bitboard vector, bitboard opponent, bitboard own, DIRECTION dir);

  bitboard * pawnMoves(COLOR color);
  bitboard * knightMove(COLOR color);
  bitboard KingMove(COLOR color);
  bitboard QueenMove(COLOR color);
  bitboard * BishopMove(COLOR color);
  bitboard * RookMove(COLOR color);


  // LEVEL 3 of moveGen - Advanced game logic
  bitboard combinedAttacks(); // All attacked pieces of opposing color
  // Move rockade
  bool legal(gameState p);
  bool checkMate();
  bool draw();



  // LEVEL 4 of moveGen - tree generation


  // INTERPRETATION
  void applyMove();

};

#endif //CHESS_ANN_ENVIORNMENT_H
