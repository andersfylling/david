//
// Created by lolos on 05.04.2017.
//

#include "environment.h"
#include "./bitboard.h"
#include <string>
#include <bitset>
#include <iostream>
#include <math.h>
#include <array>
#include <algorithm>

namespace environment {

using std::string;
using std::cout;
using ::bitboard::bitboard_t;
using ::bitboard::WPc;
using ::bitboard::WCc;
using ::bitboard::WNc;
using ::bitboard::WRc;
using ::bitboard::WQc;
using ::bitboard::WKc;
using ::bitboard::BPc;
using ::bitboard::BCc;
using ::bitboard::BRc;
using ::bitboard::BNc;
using ::bitboard::BQc;
using ::bitboard::BKc;
using ::bitboard::COLOR;
using ::bitboard::makeBoardFromArray;

void Environment::setGameState(gameState st) {
  state = st;
}

void Environment::printBitboards() {
  std::cout << state.BlackBishop << "\n";
  std::cout << state.BlackKing << "\n";
  std::cout << state.BlackKnight << "\n";
  std::cout << state.BlackPawn << "\n";
  std::cout << state.BlackQueen << "\n";
  std::cout << state.BlackRook << "\n";
  std::cout << state.WhiteBishop << "\n";
  std::cout << state.WhiteQueen << "\n";
  std::cout << state.WhiteKnight << "\n";
  std::cout << state.WhitePawn << "\n";
  std::cout << state.WhiteRook << "\n";
  std::cout << state.WhiteKing << "\n";
}

void Environment::printBoard(bitboard_t board) {
  string bits = std::bitset<64>(board).to_string();
  cout << "\n  ";
  for (int i = 'A'; i < 'A'+8; i++)
    cout << "  " << (char) i << " ";
  cout << std::endl;
  cout << "  +---+---+---+---+---+---+---+---+\n";
  for (int i = 0; i < 8; i++) {
    cout << i+1 << " | ";
    for (int j = 0; j < 8; j++) {
      cout << bits.at((i * 8) + j) << " | ";
    }
    cout << '\n';
    cout << "  +---+---+---+---+---+---+---+---+\n";
  }
  cout << '\n';
}

bitboard_t *Environment::getXAxisFromBoard(bitboard_t board, bool limit, int lock) {
  // Either limit is 0 or 1
  // Must find every piece that is to be able to move.

  bitboard_t * boards;  // Stores the boards to be returned
  boards = new bitboard_t[numberOfPieces(board)+1];
  int boardsIndex = 0;

  bool xPlus, xMinus;
  xPlus = false; xMinus = false;
  switch (lock) {
    case 1:
      xPlus = true;
      break;

    case 2:
      xMinus = true;
      break;

    default:
      ; // No action
  }


  if(limit) {  // Logic for moving a piece one step
    for (bitboard_t index = LSB(board); index; index = NSB(board), boardsIndex++) {
      bitboard_t temp = 0;
      bitboard_t rowMod = (index /  8);

      if((index+1) / 8 == rowMod)
        temp |= 1LL << (index+1);

      if((index-1) / 8 == rowMod)
        temp |= 1LL << (index-1);

      boards[boardsIndex] = temp;
    }
  } else {  // Logic for making an attack vector
    for (bitboard_t index = LSB(board); index; index = NSB(board), boardsIndex++) {
      // Make map with all possible moves on x-axis.
      // Within boundaries of board width
      bitboard_t temp = 0;
      bitboard_t i = index+1;
      bitboard_t rowMod = (i / 8);

      // Gets adds vector for right side of piece
      while (!xPlus && i / 8 == rowMod) {
        flipBit(temp, i);
        i++;
      }

      i = index-1;
      while (!xMinus && i / 8 == rowMod) {
        flipBit(temp, i);
        i--;
      }

      boards[boardsIndex] = temp;
    }
  }
  return boards;
}

int Environment::numberOfPieces(bitboard_t board) {
  // Needs linux alternative.
  // Fastest way of getting nuber of active bits.
  // Uses special CPU fuctionality

#ifdef __GNUG__
  return __builtin_popcountll(board);
#endif

}

bitboard_t *Environment::getDiagYAxis(bitboard_t board, DIRECTION dir, bool limit, int lock) {
  int dirValue;
  bitboard_t * boards;  // Stores the boards to be returned
  boards = new bitboard_t[numberOfPieces(board)+1];
  int boardsIndex = 0;

  switch (dir) {
    case DIRECTION::MAIN_DIAGONAL:
      dirValue = 9;
      break;
    case DIRECTION::ANTI_DIAGONAL:
      dirValue = 7;
      break;
    default:
      dirValue = 8;
      break;
  }

  bool xPlus, xMinus;
  xPlus = false; xMinus = false;
  switch (lock) {
    case 2:
      xPlus = true;
      break;

    case 1:
      xMinus = true;
      break;

    default:
      ; // No action
  }


  if (limit) {
    for (bitboard_t index = LSB(board); index; index = NSB(board), boardsIndex++) {
      bitboard_t temp = 0;

      if((index-dirValue) >= 0 && !xMinus && ((index-dirValue)/8) == (index/8)-1) // Activate bit over
        temp |= 1LL << index-(dirValue);

      if((index+dirValue) <= 63 && !xPlus && ((index+dirValue)/8) == (index/8)+1) // Activate bit under
        temp |= 1LL << (index+dirValue);

      boards[boardsIndex] = temp;
    }
  } else {
    for (bitboard_t index = LSB(board); index; index = NSB(board), boardsIndex++) {
      // Make map with all possible moves on x-axis.
      // Within boundaries of board width
      bitboard_t temp = 0;
      bitboard_t row = index/8;
      bitboard_t i = index+dirValue;


      // Gets adds vector for right side of piece
      while (!xPlus && i/8 == (row+1) && i < 64) {
        flipBit(temp, i);
        row = i/8;
        i += dirValue;
      }

      i = index - dirValue;
      row = index/8;

      while (!xMinus && i/8 == (row-1) && i < 64) {
        flipBit(temp, i);
        row = i / 8;
        i-=dirValue;
      }
      boards[boardsIndex] = temp;
    }
  }
  return boards;
}

bitboard_t *Environment::knightMovement(bitboard_t board) {
  int boardValue = 0;
  bitboard_t * boards;
  boards = new bitboard_t[numberOfPieces(board)+1];
  for (bitboard_t index = LSB(board); index; index = NSB(board), ++boardValue) {
    bitboard_t temp = 0;
    bitboard_t row = index/8;

    // Two steps up, one left
    if (row-2 == (index-15)/8 && index - 15 < 64) {
      flipBit(temp, index-15);
    }

    // Two steps up, one right
    else if (row-2 == (index-17)/8 && index-17 < 64) {
      flipBit(temp, index-17);
    }

    // One step up, two left
    else if (row-1 == (index-10)/8 && index-10 < 64) {
      flipBit(temp, index-10);
    }

    // One step up, two right
    else if (row-1 == (index-6)/8 && index-6 < 64) {
      flipBit(temp, index-6);
    }

    // Two steps down, one left
    else if (row+2 == (index+15)/8 && index+15 < 64) {
      flipBit(temp, index+15);
    }

    // Two steps down, one right
    else if (row+2 == (index+17)/8 && index+17 < 64) {
      flipBit(temp, index+17);
    }

    // One step down, two left
    else if (row+1 == (index+6)/8 && index+6 < 64) {
      flipBit(temp, index+6);
    }

    // One step down, two right
    else if (row+1 == (index+10)/8 && index+10 < 64) {
      flipBit(temp, index+10);
    }
    boards[boardValue] = temp;
  }
  return boards;
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
bitboard_t Environment::LSB(bitboard_t board) {
  return (board != 0LL) ? __builtin_ffsll(board)-1 : 0LL;
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
bitboard_t Environment::NSB(bitboard_t &board) {
  board &= ~(1LL << LSB(board));
  return LSB(board);
}

bitboard_t Environment::MSB(bitboard_t board) {
  return 63 - __builtin_clzll(board);
}

// Turns on bit
void Environment::flipBit(bitboard_t &board, bitboard_t index) {
  board |= (1LL << index);
}

bitboard_t Environment::whitePieces() {
  return (state.WhitePawn | state.WhiteRook | state.WhiteKnight | state.WhiteKing | state.WhiteBishop | state.WhiteQueen);
}

bitboard_t Environment::blackPieces() {
  return (state.BlackPawn | state.BlackRook | state.BlackKing | state.BlackKnight | state.BlackBishop | state.BlackQueen);
}

Environment::Environment(COLOR color) {
  whiteCastling = true;
  blackCastling = true;
  moves = 0;
  hostColor = color;

  state.WhitePawn = makeBoardFromArray(WPc);
  state.WhiteRook = makeBoardFromArray(WCc);
  state.WhiteKnight = makeBoardFromArray(WNc);
  state.WhiteBishop = makeBoardFromArray(WRc);
  state.WhiteQueen = makeBoardFromArray(WQc);
  state.WhiteKing = makeBoardFromArray(WKc);

  state.BlackPawn = makeBoardFromArray(BPc);
  state.BlackRook = makeBoardFromArray(BCc);
  state.BlackBishop = makeBoardFromArray(BRc);
  state.BlackKnight = makeBoardFromArray(BNc);
  state.BlackQueen = makeBoardFromArray(BQc);
  state.BlackKing = makeBoardFromArray(BKc);

  // IF COLOR WHITE
  // INITIATE AUTO MOVES

}

bitboard_t *Environment::pawnMoves(COLOR color) {
  bitboard_t opponent = (color == COLOR::WHITE) ? whitePieces() : blackPieces();
  bitboard_t own = (color == COLOR::WHITE) ? blackPieces() : whitePieces();
  DIRECTION dir = (color == COLOR::WHITE) ? DIRECTION::UP : DIRECTION::DOWN;

  // Generates attack-vectors for pawns
  bitboard_t * bits = (color == COLOR::WHITE) ? getDiagYAxis(state.WhitePawn, DIRECTION::UP, true, 1) : getDiagYAxis(state.BlackPawn, DIRECTION::UP, true, 2);

  // Adds possibility for double moves
  for (bitboard_t i = 0; i < numberOfPieces((color == COLOR::WHITE) ? state.WhitePawn : state.BlackPawn); i++) {
    if(bits[i] < 65536LL && bits[i] > 128LL) {
      flipBit(bits[i], LSB(bits[i] + 8));
    } else if (bits[i] < 72057594037927936LL &&  bits[i] > 1099511627776LL) {
      flipBit(bits[i], LSB(bits[i]-8));
    }

    // We now have forward movement. Needs a attack, but that logic is different with pawns.
    bits[i] &= ~(generateBlock(bits[i], dir, own) | own);  // Removes collision with own pieces
    bits[i] &= ~(generateBlock(bits[i], dir, opponent)); // Removes collision with oponent pieces

    if (COLOR::WHITE == color) {
      bitboard_t index = 0LL;
      index |= (1LL << LSB(bits[i]));
      bits[i] |= *(getDiagYAxis(index, DIRECTION::MAIN_DIAGONAL, true, 1))&opponent;
      bits[i] |= *(getDiagYAxis(index, DIRECTION::ANTI_DIAGONAL, true, 1))&opponent;
    } else {
      bitboard_t index = 0LL;
      index |= (1LL << MSB(bits[i]));
      bits[i] |= *(getDiagYAxis(index, DIRECTION::MAIN_DIAGONAL, true, 1))&opponent;
      bits[i] |= *(getDiagYAxis(index, DIRECTION::ANTI_DIAGONAL, true, 1))&opponent;
    }

  }
  return bits;
  // Generates pseudo legal moves Needs a check for king in attack vector
}

bitboard_t Environment::generateBlock(bitboard_t vector, DIRECTION dir, bitboard_t opponent) {
  bitboard_t blockade = 0LL;
  bitboard_t block = vector & opponent;
  switch (dir) {
    case DIRECTION::DOWN : {
      if(bitboard_t index = LSB(block)) {
        for (bitboard_t i = 0; i < index-1; i++) {
          blockade |= (1LL << i);
        }
      }
      break;
    }
    case DIRECTION::UP : {
      if (bitboard_t index = LSB(block)) {
        for (bitboard_t i = index+1; i < 64; i++) {
          blockade |= (1LL << i);
        }
      }
      break;
    }
    default:
      blockade = 1LL;
  }
  return blockade;
}

bitboard_t *Environment::knightMove(COLOR color) {
  bitboard_t own = (color == COLOR::WHITE) ? whitePieces() : blackPieces();
  bitboard_t * bits = knightMovement((COLOR::WHITE == color) ? state.WhiteKnight : state.BlackKnight);

  // Cannot move on top of own pieces
  for (int i = 0;  i < numberOfPieces((color == COLOR::WHITE) ? state.WhiteKnight : state.BlackKnight); i++) {
    bits[i] &= ~own;
  }

  return bits;
}

bitboard_t Environment::KingMove(COLOR color) {
  bitboard_t movement = 0LL;

  bitboard_t own, board;
  if (color == COLOR::WHITE) {
    own = whitePieces();
    board = state.WhiteKing;
  } else {
    own = blackPieces();
    board = state.BlackKing;
  }

  // Add the possible fields to move to
  movement |= *getXAxisFromBoard(board, true);    // X-axis (both directions)
  movement |= *getDiagYAxis(board, DIRECTION::UP, true); // Y-AXIS (both directions)
  movement |= *getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, true); // MAIN DIAGONAL
  movement |= *getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, true); // ANTI DIAGONAL

  // Subtract moves into own
  movement &= ~own;

  return movement;
}

bitboard_t Environment::QueenMove(COLOR color) {
  bitboard_t movement = 0;
  bitboard_t own, opponent, board;
  if (color == COLOR::WHITE) {
    own = whitePieces();
    board = state.WhiteQueen;
    opponent = blackPieces();
  } else {
    own = blackPieces();
    board = state.BlackQueen;
    opponent = whitePieces();
  }

  movement |= reduceVector(*getXAxisFromBoard(board, false, 2), opponent, own, DIRECTION::DOWN);
  movement |= reduceVector(*getXAxisFromBoard(board, false, 1), opponent, own, DIRECTION::UP);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::UP, false, 1), opponent, own, DIRECTION::UP);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::UP, false, 2), opponent, own, DIRECTION::DOWN);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 2), opponent, own, DIRECTION::DOWN);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 1), opponent, own, DIRECTION::UP);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 2), opponent, own, DIRECTION::DOWN);
  movement |= reduceVector(*getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 1), opponent, own, DIRECTION::UP);


  return movement;

}

bitboard_t Environment::reduceVector(bitboard_t vector, bitboard_t opponent, bitboard_t own, DIRECTION dir) {
  bitboard_t result, owB, opB;
  owB = generateBlock(vector, dir, own);
  opB = generateBlock(vector, dir, opponent);
  result = (vector & ~(own | owB | opB));

  //result = vector & ~(generateBlocK(vector, dir, own) | own | generateBlocK(vector, dir, opponent));

  return result;
}

/**
 * Converts indexes such as "E6" into an integer index for bitboard_t.
 *
 * @param chessIndex String such as "E6"
 * @return integer bitboard_t index, -1 on error.
 */
int Environment::chessIndexToBitboardIndex(std::string chessIndex) {
  int index = -1;

  // Is always 2 in size "alpha" + "numeric" => "H1"
  if (chessIndex.length() != 2) {
    return index;
  }

  // make sure it's uppercase
  std::transform(chessIndex.begin(), chessIndex.end(), chessIndex.begin(), ::toupper);

  // store first and second char
  const char& a = chessIndex.front();
  const char& b = chessIndex.back();

  char cPos[] = {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}; // matches index value
  int first = 0;
  int second = b - 48 - 1;
  for (int i = 0; i < 8; i++) {
    if (a == cPos[i]) {
      first = i;
      break;
    }
  }

  index = second * 8 + first;

  return index;
}

}// end namespace