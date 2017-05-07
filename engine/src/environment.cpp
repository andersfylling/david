//
// Created by lolos on 05.04.2017.
//

#include "chess_ann/environment.h"
#include "chess_ann/environment.h"
#include "chess_ann/bitboard.h"
#include <string>
#include <bitset>
#include <iostream>
#include <math.h>
#include <array>
#include <algorithm>
#include <chess_ann/utils.h>
#include <ibase.h>
#include <chess_ann/GameTree.h>
#include <map>

namespace environment {

using std::string;
using std::cout;
using ::bitboard::bitboard_t;
using ::bitboard::gameState;
using ::bitboard::DIRECTION;
using ::bitboard::COLOR;
using ::bitboard::pieceAttack;

void Environment::setGameState(std::shared_ptr<::bitboard::gameState> st) {
  state = (*st); // dereferrence
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
  for (int i = 'A'; i < 'A' + 8; i++)
    cout << "  " << (char) i << " ";
  cout << std::endl;
  cout << "  +---+---+---+---+---+---+---+---+\n";
  for (int i = 0; i < 8; i++) {
    cout << i + 1 << " | ";
    for (int j = 0; j < 8; j++) {
      cout << bits.at((i * 8) + j) << " | ";
    }
    cout << '\n';
    cout << "  +---+---+---+---+---+---+---+---+\n";
  }
  cout << '\n';
}

bitboard_t *Environment::getXAxisFromBoard(bitboard_t board, bool limit, int lock) {
  // Enten limit 0 eller 1
  // Må finne alle brikker som skal kunne flyttes
  bitboard_t temp;

  bitboard_t *boards;  // Stores the boards to be returned
  bitboard_t numPiece = numberOfPieces(board);
  bitboard_t index;
  boards = new bitboard_t[numPiece];

  bool xPlus, xMinus;
  xPlus = false;
  xMinus = false;
  switch (lock) {
    case 1:xPlus = true;
      break;

    case 2:xMinus = true;
      break;

    default:; // No action
  }

  if (limit) {  // Logic for moving a piece one step
    for (bitboard_t i = 0; i < numPiece; i++) {
      temp = 0;
      index = LSB(board);
      bitboard_t rowMod = (index / 8);

      if ((index + 1) / 8 == rowMod && !xPlus && index < 64ULL)
        temp |= 1LL << (index + 1);

      if ((index - 1) / 8 == rowMod && !xPlus && index < 64ULL)
        temp |= 1LL << (index - 1);

      boards[i] = temp;
      NSB(board);
    }
  } else {  // Logic for making an attack vector

    for (bitboard_t index = 0; index < numPiece; index++) {
      // Make map with all possible moves on x-axis.
      // Within boundaries of board width
      temp = 0;
      bitboard_t i = LSB(board);
      bitboard_t rowMod = (i / 8);
      i++;

      // Gets adds vector for right side of piece
      while (!xPlus && i / 8 == rowMod) {
        flipBit(temp, i);
        i++;
      }

      i = LSB(board)-1ULL;
      while (!xMinus && i / 8 == rowMod && i < 64ULL) {
        flipBit(temp, i);
        i--;
      }

      boards[index] = temp;
      NSB(board);
    }
  }
  return boards;
}

bitboard_t Environment::numberOfPieces(bitboard_t board) {
  // Needs linux alternative.
  // Fastest way of getting nuber of active bits.
  // Uses special CPU fuctionality

#ifdef __GNUG__
  return __builtin_popcountll(board);
#endif

}

bitboard_t *Environment::getDiagYAxis(bitboard_t board, DIRECTION dir, bool limit, int lock) {
  int dirValue;
  bitboard_t *boards;  // Stores the boards to be returned
  boards = new bitboard_t[numberOfPieces(board)];
  bitboard_t numPiece = numberOfPieces(board);


  switch (dir) {
    case DIRECTION::MAIN_DIAGONAL:dirValue = 9;
      break;
    case DIRECTION::ANTI_DIAGONAL:dirValue = 7;
      break;
    default:dirValue = 8;
      break;
  }

  bool xPlus, xMinus;
  xPlus = false;
  xMinus = false;
  switch (lock) {
    case 2:xPlus = true;
      break;

    case 1:xMinus = true;
      break;

    default:; // No action
  }

  if (limit) {
    bitboard_t index;
    for (bitboard_t i = 0; i < numPiece; i++) {
      bitboard_t temp = 0;
      index = LSB(board);

      if ((index - dirValue) >= 0 && !xMinus && ((index - dirValue) / 8) == (index / 8) - 1) // Activate bit over
        temp |= 1LL << index - (dirValue);

      if ((index + dirValue) <= 63 && !xPlus && ((index + dirValue) / 8) == (index / 8) + 1) // Activate bit under
        temp |= 1LL << (index + dirValue);

      boards[i] = temp;
      NSB(board);
    }
  } else {
    for (bitboard_t index = 0; index < numPiece; index++) {
      // Make map with all possible moves on x-axis.
      // Within boundaries of board width
      bitboard_t temp = 0;
      bitboard_t i = LSB(board);
      bitboard_t row = i / 8;
      i += dirValue;


      // Gets adds vector for right side of piece
      while (!xPlus && i / 8 == (row + 1) && i < 64ULL) {
        flipBit(temp, i);
        row = i / 8;
        i += dirValue;
      }

      i = LSB(board);
      row = i/8;
      i -= dirValue;

      while (!xMinus && i / 8 == (row - 1) && i < 64ULL) {
        flipBit(temp, i);
        row = i / 8ULL;
        i -= dirValue;
      }
      boards[index] = temp;
      NSB(board);
    }
  }
  return boards;
}

bitboard_t *Environment::knightMovement(bitboard_t board) {
  int boardValue = 0;
  bitboard_t *boards;
  boards = new bitboard_t[numberOfPieces(board) + 1];
  for (bitboard_t index = LSB(board); index; index = NSB(board), ++boardValue) {
    bitboard_t temp = 0;
    bitboard_t row = index / 8;

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


bitboard_t Environment::whitePieces() {
  return (state.WhitePawn | state.WhiteRook | state.WhiteKnight | state.WhiteKing | state.WhiteBishop
      | state.WhiteQueen);
}

bitboard_t Environment::blackPieces() {
  return (state.BlackPawn | state.BlackRook | state.BlackKing | state.BlackKnight | state.BlackBishop
      | state.BlackQueen);
}

Environment::Environment(COLOR color) {
  whiteCastling = true;
  blackCastling = true;
  moves = 0;
  hostColor = color;

  state.BlackBishop = 2594073385365405696ULL;
  state.BlackKing = 1152921504606846976ULL;
  state.BlackKnight = 4755801206503243776ULL;
  state.BlackPawn = 71776119061217280ULL;
  state.BlackQueen = 576460752303423488ULL;
  state.BlackRook = 9295429630892703744ULL;

  state.WhiteBishop = 36;
  state.WhiteKnight = 66;
  state.WhitePawn = 65280;
  state.WhiteQueen = 8;
  state.WhiteKing = 16;
  state.WhiteRook = 129;

  generateAttacks();

  // IF COLOR WHITE
  // INITIATE AUTO MOVES

}

bitboard_t *Environment::pawnMoves(COLOR color) {
  bitboard_t opponent = (color == COLOR::WHITE) ? whitePieces() : blackPieces();
  bitboard_t own = (color == COLOR::WHITE) ? blackPieces() : whitePieces();
  DIRECTION dir = (color == COLOR::WHITE) ? DIRECTION::UP : DIRECTION::DOWN;

  // Generates attack-vectors for pawns
  bitboard_t *bits =
      (color == COLOR::WHITE) ? getDiagYAxis(state.WhitePawn, DIRECTION::UP, true, 1) : getDiagYAxis(state.BlackPawn,
                                                                                                     DIRECTION::UP,
                                                                                                     true,
                                                                                                     2);

  // Adds possibility for double moves
  for (bitboard_t i = 0; i < numberOfPieces((color == COLOR::WHITE) ? state.WhitePawn : state.BlackPawn); i++) {
    if (bits[i] < 65536LL && bits[i] > 128LL) {
      flipBit(bits[i], LSB(bits[i] + 8));
    } else if (bits[i] < 72057594037927936LL && bits[i] > 1099511627776LL) {
      flipBit(bits[i], LSB(bits[i] - 8));
    }

    // We now have forward movement. Needs a attack, but that logic is different with pawns.
    bits[i] &= ~(generateBlock(bits[i], dir, own) | own);  // Removes collision with own pieces
    bits[i] &= ~(generateBlock(bits[i], dir, opponent)); // Removes collision with oponent pieces

    if (COLOR::WHITE == color) {
      bitboard_t index = 0LL;
      index |= (1LL << LSB(bits[i]));
      bits[i] |= *(getDiagYAxis(index, DIRECTION::MAIN_DIAGONAL, true, 1)) & opponent;
      bits[i] |= *(getDiagYAxis(index, DIRECTION::ANTI_DIAGONAL, true, 1)) & opponent;
    } else {
      bitboard_t index = 0LL;
      index |= (1LL << MSB(bits[i]));
      bits[i] |= *(getDiagYAxis(index, DIRECTION::MAIN_DIAGONAL, true, 1)) & opponent;
      bits[i] |= *(getDiagYAxis(index, DIRECTION::ANTI_DIAGONAL, true, 1)) & opponent;
    }

  }
  return bits;
  // Generates pseudo legal moves Needs a check for king in attack vector
}

bitboard_t Environment::generateBlock(bitboard_t vector, DIRECTION dir, bitboard_t oponent) {
  bitboard_t blockade = 0LL;
  bitboard_t block = vector & oponent;
  switch (dir) {
    case DIRECTION::DOWN : {
      if (bitboard_t index = LSB(block)) {
        for (bitboard_t i = 0; i < index - 1; i++) {
          blockade |= (1LL << i);
        }
      }
      break;
    }
    case DIRECTION::UP : {
      if (bitboard_t index = LSB(block)) {
        for (bitboard_t i = index + 1; i < 64; i++) {
          blockade |= (1LL << i);
        }
      }
      break;
    }
    default:blockade = 1LL;
  }
  return blockade;
}

bitboard_t *Environment::knightMove(COLOR color) {
  bitboard_t own = (color == COLOR::WHITE) ? whitePieces() : blackPieces();
  bitboard_t *bits = knightMovement((COLOR::WHITE == color) ? state.WhiteKnight : state.BlackKnight);

  // Cannot move on top of own pieces
  for (int i = 0; i < numberOfPieces((color == COLOR::WHITE) ? state.WhiteKnight : state.BlackKnight); i++) {
    bits[i] &= ~own;
  }

  return bits;
}

bitboard_t * Environment::KingMove(COLOR color) {
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
  bitboard_t * bits = new bitboard_t;
  bits = &movement;
  return bits;
}

bitboard_t * Environment::QueenMove(COLOR color) {
  bitboard_t movement = 0;
  bitboard_t own, opponent, board;
  bitboard_t * bits;

  if (color == COLOR::WHITE) {
    own = whitePieces();
    board = state.WhiteQueen;
    opponent = blackPieces();
  } else {
    own = blackPieces();
    board = state.BlackQueen;
    opponent = whitePieces();
  }

  bits = new bitboard_t[numberOfPieces(board)];

  for (int i = 0; i < numberOfPieces(board); i++) {
    movement |= reduceVector(getXAxisFromBoard(board, false, 2)[i], opponent, own, DIRECTION::DOWN);
    movement |= reduceVector(getXAxisFromBoard(board, false, 1)[i], opponent, own, DIRECTION::UP);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::UP, false, 1)[i], opponent, own, DIRECTION::UP);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::UP, false, 2)[i], opponent, own, DIRECTION::DOWN);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 2)[i], opponent, own, DIRECTION::DOWN);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 1)[i], opponent, own, DIRECTION::UP);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 2)[i], opponent, own, DIRECTION::DOWN);
    movement |= reduceVector(getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 1)[i], opponent, own, DIRECTION::UP);
    bits[i] = movement;
  }

  return bits;

}

bitboard_t Environment::reduceVector(bitboard_t vector, bitboard_t opponent, bitboard_t own, DIRECTION dir) {
  bitboard_t result, owB, opB;
  owB = generateBlock(vector, dir, own);
  opB = generateBlock(vector, dir, opponent);
  result = (vector & ~(own | owB | opB));

  //result = vector & ~(generateBlock(vector, dir, own) | own | generateBlock(vector, dir, opponent));

  return result;
}

bitboard_t * Environment::BishopMove(COLOR color) {
  bitboard_t * bits;

  bitboard_t own, opponent, board;

  if (color == COLOR::WHITE) {
    own = whitePieces();
    board = state.WhiteBishop;
    opponent = blackPieces();
  } else {
    own = blackPieces();
    board = state.BlackBishop;
    opponent = whitePieces();
  }

  bits = new bitboard_t[numberOfPieces(board)];

  for (int i = 0; i < numberOfPieces(board); i++) {
    bits[i] = 0;
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 2)[i], opponent, own, DIRECTION::DOWN);
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::MAIN_DIAGONAL, false, 1)[i], opponent, own, DIRECTION::UP);
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 2)[i], opponent, own, DIRECTION::DOWN);
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::ANTI_DIAGONAL, false, 1)[i], opponent, own, DIRECTION::UP);
  }

  return bits;
}

bitboard_t * Environment::RookMove(COLOR color) {
  bitboard_t * bits;

  bitboard_t own, opponent, board;

  if (color == COLOR::WHITE) {
    own = whitePieces();
    board = state.WhiteRook;
    opponent = blackPieces();
  } else {
    own = blackPieces();
    board = state.BlackRook;
    opponent = whitePieces();
  }

  bits = new bitboard_t[numberOfPieces(board)];

  for (int i = 0; i < numberOfPieces(board); i++) {
    bits[i] = 0;
    bits[i] |= reduceVector(getXAxisFromBoard(board, false, 2)[i], opponent, own, DIRECTION::UP);
    bits[i] |= reduceVector(getXAxisFromBoard(board, false, 1)[i], opponent, own, DIRECTION::DOWN);
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::UP, false, 1)[i], opponent, own, DIRECTION::UP);
    bits[i] |= reduceVector(getDiagYAxis(board, DIRECTION::UP, false, 2)[i], opponent, own, DIRECTION::DOWN);
  }

  return bits;

}

/**
 * Converts indexes such as "E6" into an integer index: H1 == 0.
 *
 * @param chessIndex String such as "E6"
 * @return integer bitboard_t index, -1 on error.
 */
int Environment::chessIndexToArrayIndex(std::string chessIndex) {
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

  if (index > 64 || index < 0) {
    return -1;
  }

  return index;
}

/**
 * Takes a chess position such as "E6" and creates a bitboard with a on bit at that given index.
 *
 * @param chessIndex String such as "E6"
 * @return bitboard with a active bit in given position
 */
bitboard_t Environment::chessIndexToBitboard(std::string chessIndex) {
  bitboard_t board = 0ULL;
  flipBit(board, intToUint64(chessIndexToArrayIndex(chessIndex)));

  return board;
}
uint64_t Environment::intToUint64(int i) {
  return (uint64_t)i;
}

/**
 * A FEN record contains six fields. The separator between fields is a space. The fields are:

    1. Piece placement (from white's perspective). Each rank is described, starting with rank 8 and ending with rank 1; within each rank, the contents of each square are described from file "a" through file "h". Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K").[1] White pieces are designated using upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8 (the number of empty squares), and "/" separates ranks.

    2. Active color. "w" means White moves next, "b" means Black.

    3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters: "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside), and/or "q" (Black can castle queenside).

    4. En passant target square in algebraic notation. If there's no en passant target square, this is "-". If a pawn has just made a two-square move, this is the position "behind" the pawn. This is recorded regardless of whether there is a pawn in position to make an en passant capture.[2]

    5. Halfmove clock: This is the number of halfmoves since the last capture or pawn advance. This is used to determine if a draw can be claimed under the fifty-move rule.

    6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.

 * @param node
 * @param whiteMovesNext
 * @return
 */
std::string Environment::fen(gameState* node, bool whiteMovesNext) {


  std::array<bitboard_t, 12> boards = {
      node->BlackBishop,
      node->BlackKing,
      node->BlackKnight,
      node->BlackPawn,
      node->BlackQueen,
      node->BlackRook,

      node->WhiteBishop,
      node->WhiteKing,
      node->WhiteKnight,
      node->WhitePawn,
      node->WhiteQueen,
      node->WhiteRook
  };

  std::array<char, 12> symbols = {
      'b',
      'k',
      'n',
      'p',
      'q',
      'r',

      'B',
      'K',
      'N',
      'P',
      'Q',
      'R'
  };

  char spaces[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8'};

  int spacing = 0;
  int pos = 1;
  std::string fen = "";

  for (uint8_t i = 0; i < 64; i++) {
    if (pos == 9) {
      pos = 1;
      fen += '/';
    }


    char p = ' ';
    for (uint8_t j = 0; j < 12; j++) {
      if (::utils::bitAt(boards[j], i)) {
        p = symbols[j];
        break;
      }
    }

    if (p == ' ') {
      spacing += 1;
    }

    if (pos == 8 || p != ' ') {
      fen += spaces[spacing];
      spacing = 0;
    }

    if (p != ' ') {
      fen += p;
    }

    pos += 1;
  }

  // who is to move next
  fen += whiteMovesNext ? " w" : " b";

  // missing castling verification support
  fen += " -";

  // missing passant target verification support
  fen += " -";

  // missing halfmove verification support
  fen += " " + std::to_string(node->halfMoves);

  // missing fullmove verification support
  fen += " " + std::to_string(node->fullMoves);

  return fen;
}

void Environment::setFen(std::string fen) {

}

/**
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param fen std::string, must be correctly formatted (!)
 * @return new shared_ptr of gameState
 */
std::shared_ptr<::bitboard::gameState> Environment::generateBoardFromFen(const std::string fen) {
  gameTree::nodePtr node = std::make_shared<::bitboard::gameState>();

  std::map<const char, ::bitboard::bitboard_t&> links = {
      {'b', node->BlackBishop},
      {'k', node->BlackKing},
      {'n', node->BlackKnight},
      {'p', node->BlackPawn},
      {'q', node->BlackQueen},
      {'r', node->BlackRook},

      {'B', node->WhiteBishop},
      {'K', node->WhiteKing},
      {'N', node->WhiteKnight},
      {'P', node->WhitePawn},
      {'Q', node->WhiteQueen},
      {'R', node->WhiteRook}
  };

  bitboard_t index = 0;
  for (const auto c : fen) {
    if (index == 64) {
      break;
    }
    if (c == '/') {
      continue;
    }

    // check if the char is a piece type
    if (links.count(c) > 0) {
      // it's a piece type
      flipBit(links.at(c), index); // set bit at correct index on correct board
      index += 1;
    }
    else {
      // assumption: it's a number
      // update index with this number
      index += ::utils::stoi(c);
    }
  }

  return node;
}


void Environment::generateAttacks() {
  attacks.WhitePawn = pawnMoves(COLOR::WHITE);
  attacks.WhiteBishop = BishopMove(COLOR::WHITE);
  attacks.WhiteQueen = QueenMove(COLOR::WHITE);
  attacks.WhiteKing = KingMove(COLOR::WHITE);
  attacks.WhiteRook = RookMove(COLOR::WHITE);
  attacks.WhiteKnight = knightMove(COLOR::WHITE);

  attacks.BlackPawn = pawnMoves(COLOR::BLACK);
  attacks.BlackBishop = BishopMove(COLOR::BLACK);
  attacks.BlackQueen = QueenMove(COLOR::BLACK);
  attacks.BlackKing = KingMove(COLOR::BLACK);
  attacks.BlackRook = RookMove(COLOR::BLACK);
  attacks.BlackKnight = knightMove(COLOR::BLACK);
}

bitboard_t Environment::combinedBlackAttacks() {
  bitboard_t comb = 0;
  for (bitboard_t i = 0; i < numberOfPieces(state.BlackBishop); i++) {
    comb |= attacks.BlackBishop[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.BlackKnight); i++) {
    comb |= attacks.BlackKnight[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.BlackQueen); i++) {
    comb |= attacks.BlackQueen[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.BlackKing); i++) {
    comb |= attacks.BlackKing[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.BlackRook); i++) {
    comb |= attacks.BlackRook[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.BlackPawn); i++) {
    comb |= attacks.BlackPawn[i];
  }

  return comb;
}

bitboard_t Environment::combinedWhiteAttacks() {
  bitboard_t comb = 0;
  for (bitboard_t i = 0; i < numberOfPieces(state.WhiteBishop); i++) {
    comb |= attacks.WhiteBishop[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.WhiteKnight); i++) {
    comb |= attacks.BlackKnight[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.WhiteQueen); i++) {
    comb |= attacks.WhiteQueen[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.WhiteKing); i++) {
    comb |= attacks.WhiteKing[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.WhiteRook); i++) {
    comb |= attacks.WhiteRook[i];
  }

  for (bitboard_t i = 0; i < numberOfPieces(state.WhitePawn); i++) {
    comb |= attacks.WhitePawn[i];
  }

  return comb;
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
bitboard_t LSB(bitboard_t board) {
  return (board != 0LL) ? __builtin_ffsll(board) - 1 : 0LL;
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
bitboard_t NSB(bitboard_t &board) {
  board &= ~(1LL << LSB(board));
  return LSB(board);
}

bitboard_t MSB(bitboard_t board) {
  return 63 - __builtin_clzll(board);
}

bitboard_t NSB_r(bitboard_t & board) {
  board &= ~(1LL << MSB(board));
  return MSB(board);
}

// Turns on bit
void flipBit(bitboard_t &board, bitboard_t index) {
  board |= (1LL << index);
}

void flipOff(bitboard_t &board, bitboard_t index) {
  board &= (0LL << index);
}




}// end namespace


namespace move {

using ::bitboard::move_t;
using ::bitboard::bitboard_t;
using std::bitset;
using std::string;


Move::Move() {
  // This function will never be used
  // The function will disappear when development getting better
  mv = 0U;

}

void Move::printMoveString() {
  string bits = std::bitset<16>(mv).to_string();
  for (int i = 0; i < 16; i++) {
    std::cout << bits.at(i) << " ";
  }
  std::cout << '\n';
}

move_t Move::number() {
  return mv;
}

Move::Move(int to, int from, int flags) {
  mv = 0U;
  mv |= (to << 5);
}

} // End of move