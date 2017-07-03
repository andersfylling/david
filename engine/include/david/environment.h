#pragma once

#include "david/bitboard.h"
#include "david/types.h"
#include <array>
#include <vector>


namespace david {
namespace environment {

// deal with bitboard_t dependencies
using bitboard::gameState;
using bitboard::COLOR;
using bitboard::bitboard_t;
using bitboard::DIRECTION;
using bitboard::pieceAttack;
using bitboard::move_t;
using std::vector;

enum COMPASS { NORTH, SOUTH, EAST, WEST, NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST };

bitboard_t LSB(bitboard_t board);          // Gets least signifigant bit
bitboard_t MSB(bitboard_t board);
bitboard_t NSB(bitboard_t &board);        // Gets next sigifigant bit
bitboard_t NSB_t(bitboard_t &board);
bool bitIsSet(bitboard_t, bitboard_t index);

void flipBit(bitboard_t &board, bitboard_t index);           // Flips a bit in a board
void flipOff(bitboard_t &board, bitboard_t index);

class Environment {
 private:
  int moves;    // Number of moves for performance measuring
  gameState state;  // Current gamestate
  pieceAttack attacks;  // Calculated attacks by pieces
  // If the king has been moved, or been set in check.
  // Castling is not allowed. These values are then set to false
  // This is an important strategic factor in chess
  COLOR hostColor;
  vector<move_t> moveList;
  COLOR currentMoveColor;

 public:
  Environment();
  ~Environment();
  Environment(COLOR color);
  void setGameStateColor(COLOR color);
  void printBoard(bitboard_t board);  // A damn sexy board representation
  void setGameState(const type::gameState_t& st);  // Setting the gamestate for testing
  gameState getGameState();
  pieceAttack getAttackState();
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
  bitboard_t numberOfPieces(bitboard_t board);     // For generating right sized arrays



  // LEVEL 1 of moveGen - Basic attack vectors
  bitboard_t *getXAxisFromBoard(bitboard_t board, bool limit = 0, int lock = 0);
  bitboard_t *getDiagYAxis(bitboard_t board, DIRECTION dir, bool limit = false, int lock = 0);
  bitboard_t generateBlock(bitboard_t vector, DIRECTION dir, bitboard_t opponent);
  bitboard_t *knightMovement(bitboard_t board);

  // LEVEL 2 of moveGen - Intermediate logic
  bitboard_t whitePieces(); // Returns all white pieces
  bitboard_t blackPieces(); // Returns all black pieces


  // Funksjonen skal returnere et bitboard som trekker fra motstandere og egne riktig
  // Får tilbake et bitboard som er modifisert
  bitboard_t reduceVector(bitboard_t vector, bitboard_t opponent, bitboard_t own, DIRECTION dir);

  bitboard_t *pawnMoves(COLOR color);
  bitboard_t *knightMove(COLOR color);
  bitboard_t *KingMove(COLOR color);
  bitboard_t *QueenMove(COLOR color);
  bitboard_t *BishopMove(COLOR color);
  bitboard_t *RookMove(COLOR color);

  // Generate a bitboard_t based on a chess position: E6

  // LEVEL 3 of moveGen - Advanced game logic

  // Castling
  void canWhiteCastleK();
  void canWhiteCastleQ();
  void canBlackCastleK();
  void canBlackCastleQ();

  void generateAttacks();  // Sets the attacs-values
  void generateMove(bitboard_t st, bitboard_t *attack, COLOR color);
  void generateMoves(COLOR color);    // Adds moves to move-vector
  bitboard_t combinedBlackAttacks(); // All attacked pieces of black
  bitboard_t combinedWhiteAttacks(); // All attacked pieces of white
  bool moveIsCapture(bitboard_t bit, COLOR color);     // Checks if an attack will capture a piece
  void capturePiece(COLOR opponent, bitboard_t index, gameState &st);
  void computeGameStates(std::vector<gameState> &states);
  gameState movePiece(COLOR own, bitboard_t to, bitboard_t from, int flag);
  // Move rockade1
  bool legal(gameState p);
  bool checkMate();
  bool draw();

  // TEST FUNCTIONS
  vector<bitboard_t> getMoves();
  // LEVEL 4 of moveGen - tree generation
  //gameState getCurrentGameState();


  // INTERPRETATION
  void applyMove();

  // Converters
  int chessIndexToArrayIndex(std::string chessIndex);
  bitboard_t chessIndexToBitboard(std::string chessIndex);
  bitboard_t intToUint64(int i);

  void setFen(std::string fen);
  type::gameState_ptr generateBoardFromFen(const std::string fen);

};
}

namespace move {

using bitboard::move_t;
using bitboard::bitboard_t;

bool bitIsSet(move_t board, move_t index);

// Move class interpritates the 16-bit move values
// And makes them ineracable with human beings
class Move {
 private:
  move_t mv;
 public:
  move_t setGetValue(bitboard_t to, bitboard_t from, int flags);
  void printMoveString(move_t m);
  void printOwn();
  Move(move_t m);
  Move();
  void set(move_t m);

  bool doublePawnPush();
  bool kingCastle();
  bool queenCastle();
  bool captures();
  bool enPassant();
  bool rookPromo();
  bool queenPromo();
  bool knightPromo();
  bool bishopPromo();
  int getTo();
  int getFrom();
};

}
}