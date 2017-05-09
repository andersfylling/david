//
// Created by lolos on 05.04.2017.
//

#include "chess_ann/environment.h"
#include "chess_ann/bitboard.h"
#include <string>
#include <bitset>
#include <iostream>
#include <math.h>
#include <array>
#include <algorithm>
#include <chess_ann/utils.h>
//#include <ibase.h>
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
using ::bitboard::move_t;

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
    if (row-2 == (index-17)/8 && index-17 < 64) {
      flipBit(temp, index-17);
    }

    // One step up, two left
    if (row-1 == (index-10)/8 && index-10 < 64) {
      flipBit(temp, index-10);
    }

    // One step up, two right
    if (row-1 == (index-6)/8 && index-6 < 64) {
      flipBit(temp, index-6);
    }

    // Two steps down, one left
    if (row+2 == (index+15)/8 && index+15 < 64) {
      flipBit(temp, index+15);
    }

    // Two steps down, one right
    if (row+2 == (index+17)/8 && index+17 < 64) {
      flipBit(temp, index+17);
    }

    // One step down, two left
    if (row+1 == (index+6)/8 && index+6 < 64) {
      flipBit(temp, index+6);
    }

    // One step down, two right
    if (row+1 == (index+10)/8 && index+10 < 64) {
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
  moves = 0;
  hostColor = color;

  state.BlackBishop = 2594073385365405696ULL;
  state.BlackKing = 576460752303423488ULL;
  state.BlackKnight = 4755801206503243776ULL;
  state.BlackPawn = 71776119061217280ULL;
  state.BlackQueen = 1152921504606846976ULL;
  state.BlackRook = 9295429630892703744ULL;

  state.WhiteBishop = 36;
  state.WhiteKnight = 66;
  state.WhitePawn = 65280;
  state.WhiteQueen = 8;
  state.WhiteKing = 16;
  state.WhiteRook = 129;

  state.blackKingCastling = true;
  state.blackQueenCastling = true;
  state.whiteKingCastling  = true;
  state.whiteQueenCastling = true;

  generateAttacks();
  //computeGameStates();
  //generateMoves(COLOR::WHITE);
  currentMoveColor = COLOR::WHITE;


  // IF COLOR WHITE
  // INITIATE AUTO MOVES

}


// ############ LEVEL 2 FUNCTIONS ##################

bitboard_t *Environment::pawnMoves(COLOR color) {
  bitboard_t opponent = (color != COLOR::WHITE) ? whitePieces() : blackPieces();
  bitboard_t own = (color != COLOR::WHITE) ? blackPieces() : whitePieces();
  DIRECTION dir = (color == COLOR::WHITE) ? DIRECTION::UP : DIRECTION::DOWN;

  // Generates attack-vectors for pawns
  bitboard_t * bits = (color == COLOR::WHITE) ? getDiagYAxis(state.WhitePawn, DIRECTION::UP, true, 1) : getDiagYAxis(state.BlackPawn, DIRECTION::UP, true, 2);

  // Adds possibility for double moves

  for (bitboard_t i = 0; i < numberOfPieces((color == COLOR::WHITE) ? state.WhitePawn : state.BlackPawn); i++) {
    //std::cout << bits[i] << std::endl;
    if (bits[i] < 16777216ULL && bits[i] > 32768ULL) {
      flipBit(bits[i], LSB(bits[i] )+8);
    } else if (bits[i] < 72057594037927936ULL && bits[i] > 1099511627775ULL) {
      flipBit(bits[i], LSB(bits[i])-8);
    }
    //std::cout << bits[i] << std::endl;

    // We now have forward movement. Needs a attack, but that logic is different with pawns.
    bits[i] = reduceVector(bits[i], opponent, own, DIRECTION::UP);

    if (COLOR::WHITE == color) {
      bitboard_t index = 0ULL;
      index |= (1ULL << LSB(bits[i]));
      bits[i] |= *(getXAxisFromBoard(index, true, 1)) & opponent;
      bits[i] |= *(getXAxisFromBoard(index, true, 2)) & opponent;
    } else {
      bitboard_t index = 0ULL;
      index |= (1ULL << MSB(bits[i]));
      bits[i] |= *(getXAxisFromBoard(index, true, 2)) & opponent;
      bits[i] |= *(getXAxisFromBoard(index, true, 1)) & opponent;
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
  bits[0] = movement;
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

void Environment::canWhiteCastleK() { // King startpos is 4
  bitboard_t all = whitePieces() | blackPieces();
  bitboard_t blackAttacks = combinedBlackAttacks();

  // No pieces between king and chosen rook
  if (!::utils::bitAt(all, 5) &&
      !::utils::bitAt(all, 6)) {
    // king is not in, passes through, or passes into check
    if (!::utils::bitAt(blackAttacks, 4) &&
        !::utils::bitAt(blackAttacks, 5) &&
        !::utils::bitAt(blackAttacks, 6)) {
      move::Move mo;
      using ::bitboard::move_t;
      move_t tempMove;
      // to pos 6, from startpos 4, code castle Kingside
      tempMove = mo.setGetValue(6, 4, 2);
      // Add move to moveList
      moveList.push_back(tempMove);
    }
  }
}

void Environment::canWhiteCastleQ() { // King startpos is 4
  bitboard_t all = whitePieces() | blackPieces();
  bitboard_t blackAttacks = combinedBlackAttacks();

  // No pieces between king and chosen rook
  if (!::utils::bitAt(all, 3) &&
      !::utils::bitAt(all, 2) &&
      !::utils::bitAt(all, 1)) {
    // king is not in, passes through, or passes into check
    if (!::utils::bitAt(blackAttacks, 4) &&
        !::utils::bitAt(blackAttacks, 3) &&
        !::utils::bitAt(blackAttacks, 2)) {
      move::Move mo;
      using ::bitboard::move_t;
      move_t tempMove;
      // to pos 2, from startpos 4, code castle Queenside
      tempMove = mo.setGetValue(2, 4, 3);
      // Add move to moveList
      moveList.push_back(tempMove);
    }
  }
}

void Environment::canBlackCastleK() { // King startpos is 60
  bitboard_t all = whitePieces() | blackPieces();
  bitboard_t whiteAttacks = combinedWhiteAttacks();

  std::cout << "canBlackCastleK's whiteattacks:\n";
  printBoard(whiteAttacks);

  // No pieces between king and chosen rook
  if (!::utils::bitAt(all, 61) &&
      !::utils::bitAt(all, 62)) {
    std::cout << "canBlackCastleK ingen imellom\n";
    // king is not in, passes through, or passes into check
    if (!::utils::bitAt(whiteAttacks, 60) &&
        !::utils::bitAt(whiteAttacks, 61) &&
        !::utils::bitAt(whiteAttacks, 62)) {
      move::Move mo;
      using ::bitboard::move_t;
      move_t tempMove;
      // to pos 62, from startpos 60, code castle Kingside
      tempMove = mo.setGetValue(62, 60, 2);
      // Add move to moveList
      moveList.push_back(tempMove);
    }
  }
}

void Environment::canBlackCastleQ() { // King startpos is 60
  bitboard_t all = whitePieces() | blackPieces();
  bitboard_t whiteAttacks = combinedWhiteAttacks();

  // No pieces between king and chosen rook
  if (!::utils::bitAt(all, 59) &&
      !::utils::bitAt(all, 58) &&
      !::utils::bitAt(all, 57)) {
    // king is not in, passes through, or passes into checkk
    if (!::utils::bitAt(whiteAttacks, 60) &&
        !::utils::bitAt(whiteAttacks, 59) &&
        !::utils::bitAt(whiteAttacks, 58)) {
      move::Move mo;
      using ::bitboard::move_t;
      move_t tempMove;
      // to pos 58, from startpos 60, code castle Queenside
      tempMove = mo.setGetValue(58, 60, 3);
      // Add move to moveList
      moveList.push_back(tempMove);
    }
  }
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
// Checks if the pit is present in opposing color's bitboards
bool Environment::moveIsCapture(bitboard_t bit, COLOR color) {
  if (color == COLOR::WHITE) {
    return bitIsSet(blackPieces(), LSB(bit));
  } else {
    return bitIsSet(whitePieces(), LSB(bit));
  }
}

void Environment::generateMove(bitboard_t st, bitboard_t *attack, COLOR color) {
  bitboard_t tempBoard, tempAttack, index, indexTo;
  move::Move mo;
  using ::bitboard::move_t;
  int flag;
  move_t tempMove;
  tempBoard = st;
  index = LSB(tempBoard);

  for (bitboard_t i = 0; i < numberOfPieces(st); i++) {
    tempAttack = attack[i];
    indexTo = LSB(tempAttack);
    for (int j = 0; j < numberOfPieces(attack[i]); j++) {

      flag = (moveIsCapture(indexTo, color)) ? 4 : 0;
      tempMove = mo.setGetValue(indexTo, index, flag);
      if(tempMove > 0U)
        moveList.push_back(tempMove);

      indexTo = NSB(tempAttack);
    }
    index = NSB(tempBoard);
  }
}

void Environment::generateMoves(COLOR color) {
  // Loop a piece attack-vector
  // See if it is a capture
  // Make the move
  // Add to vector
  // Repeat for all
  if (color == COLOR::WHITE) {
    generateMove(state.WhitePawn, attacks.WhitePawn, color);
    generateMove(state.WhiteBishop, attacks.WhiteBishop, color);
    generateMove(state.WhiteRook, attacks.WhiteRook, color);
    generateMove(state.WhiteKnight, attacks.WhiteKnight, color);
    generateMove(state.WhiteKing, attacks.WhiteKing, color);
    generateMove(state.WhiteQueen, attacks.WhiteQueen, color);
  } else {
    generateMove(state.BlackPawn, attacks.BlackPawn, color);
    generateMove(state.BlackBishop, attacks.BlackBishop, color);
    generateMove(state.BlackRook, attacks.BlackRook, color);
    generateMove(state.BlackKnight, attacks.BlackKnight, color);
    generateMove(state.BlackKing, attacks.BlackKing, color);
    generateMove(state.BlackQueen, attacks.BlackQueen, color);
  }
  std::cout << moveList.size() << std::endl;
  //std::cout << moveList.size() << std::endl;

}

void Environment::capturePiece(COLOR opponent, bitboard_t index, gameState &st) {
  if (opponent == COLOR::WHITE) {
    flipOff(st.WhitePawn, index);
    flipOff(st.WhiteBishop, index);
    flipOff(st.WhiteRook, index);
    flipOff(st.WhiteKnight, index);
    flipOff(st.WhiteKing, index);
    flipOff(st.WhiteQueen, index);
  } else {
    flipOff(st.BlackPawn, index);
    flipOff(st.BlackBishop, index);
    flipOff(st.BlackRook, index);
    flipOff(st.BlackKnight, index);
    flipOff(st.BlackKing, index);
    flipOff(st.BlackQueen, index);
  }
}

gameState Environment::movePiece(COLOR own, bitboard_t to, bitboard_t from) {
  gameState returnState = state;
  // A move on the white pieces are being made

  if(own == COLOR::WHITE) {
    if (bitIsSet(returnState.WhitePawn, from)) {    // WHITE PAWN MOVE SET
      flipOff(returnState.WhitePawn, from);
      flipBit(returnState.WhitePawn, to);
    } else if (bitIsSet(returnState.WhiteBishop, from)) {   // WHITE BISHOP
      flipOff(returnState.WhiteBishop, from);
      flipBit(returnState.WhiteBishop, to);
    } else if (bitIsSet(returnState.WhiteRook, from)) {   // WHITE ROOK
      flipOff(returnState.WhiteRook, from);
      flipBit(returnState.WhiteRook, to);
      if (from == 0ULL)
        returnState.whiteKingCastling = false;
      else if(from == 128ULL)
        returnState.whiteQueenCastling = false;
    } else if (bitIsSet(returnState.WhiteKing, from)) {   // WHITE KING
      flipOff(returnState.WhiteKing, from);
      flipBit(returnState.WhiteKing, to);
      returnState.whiteKingCastling = false;
      returnState.whiteQueenCastling = false;
    } else if (bitIsSet(returnState.WhiteQueen, from)) {  // WHITE QUEEN
      flipOff(returnState.WhiteQueen, from);
      flipBit(returnState.WhiteQueen, to);
    } else if (bitIsSet(returnState.WhiteKnight, from)) {
      flipOff(returnState.WhiteKnight, from);
      flipBit(returnState.WhiteKnight, to);
    }
  } else {
    if (bitIsSet(returnState.BlackPawn, from)) {    // Black PAWN MOVE SET
      flipOff(returnState.BlackPawn, from);
      flipBit(returnState.BlackPawn, to);
    } else if (bitIsSet(returnState.BlackBishop, from)) {   // Black BISHOP
      flipOff(returnState.BlackBishop, from);
      flipBit(returnState.BlackBishop, to);
    } else if (bitIsSet(returnState.BlackRook, from)) {   // Black ROOK
      flipOff(returnState.BlackRook, from);
      flipBit(returnState.BlackRook, to);
      if (from == 72057594037927936ULL)
        returnState.blackKingCastling = false;
      else if(from == 9223372036854775808ULL)
        returnState.blackQueenCastling = false;
    } else if (bitIsSet(returnState.BlackKing, from)) {   // Black KING
      flipOff(returnState.BlackKing, from);
      flipBit(returnState.BlackKing, to);
      returnState.blackKingCastling = false;
      returnState.blackQueenCastling = false;
    } else if (bitIsSet(returnState.BlackQueen, from)) {  // Black QUEEN
      flipOff(returnState.BlackQueen, from);
      flipBit(returnState.BlackQueen, to);
    } else if (bitIsSet(returnState.BlackKnight, from)) {
      flipOff(returnState.BlackKnight, from);
      flipBit(returnState.BlackKnight, to);
    }
  }
  //printBoard(returnState.WhitePawn | returnState.WhiteKnight);
  return returnState;
}


void Environment::computeGameStates(std::vector<gameState>& states) {
  // Select a move
  // Move the piece
  // remove eventual capture
  // See if legal
  // If legal-generate node
  generateMoves(currentMoveColor);
  gameState g;
  ::move::Move moveInter;

  while (!moveList.empty()) {
    moveInter.set(moveList[moveList.size()]);     // Gets the last item
    moveList.pop_back();
    g = movePiece(currentMoveColor, intToUint64(moveInter.getTo()), intToUint64(moveInter.getFrom()));
    if (moveInter.captures()) {
      COLOR opc = (currentMoveColor == COLOR::WHITE) ? COLOR::BLACK : COLOR::WHITE;
      bitboard_t tempB = intToUint64(moveInter.getTo());
      capturePiece(opc, tempB, g);
    }

    if(legal(g)) { // Should be legal
      states.push_back(g);
    }
  }
}

bool Environment::legal(gameState p) {
  gameState temp = state;
  state = p;
  generateAttacks();
  if(currentMoveColor == COLOR::WHITE) {
    if(state.WhiteKing & combinedBlackAttacks())
      return false;
  } else {
    if(state.BlackKing & combinedWhiteAttacks())
      return false;
  }
  state = temp;
  return true;
}

bitboard_t Environment::initiate() {
  //printBoard(combinedWhiteAttacks());
  //computeGameStates();



}

gameState Environment::getGameState() {
  return this->state;
}

/* ################ END OF ENVIRONMENT FUNCTIONS ################ */

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

// YEAH tell that bit to flipp off!!!
// Nobody wants you bit... NOBODY WANTS YOU
void flipOff(bitboard_t &board, bitboard_t index) {
  board &= ~(1ULL << index);
}

bool bitIsSet(bitboard_t board, bitboard_t index) {
  return (board & (1ULL << index)) ? true : false;
}


}// end namespace


namespace move {

using ::bitboard::move_t;
using ::bitboard::bitboard_t;
using std::bitset;
using std::string;



void Move::printMoveString(move_t m) {

  string bits = std::bitset<16>(m).to_string();
  for (int i = 0; i < 16; i++) {
    if (i == 6 || i == 12)
      std::cout << " | ";
    std::cout << bits.at(i) << " ";
  }
  std::cout << '\n';
}

void Move::printOwn() {

  string bits = std::bitset<16>(mv).to_string();
  for (int i = 0; i < 16; i++) {
    if (i == 6 || i == 12)
      std::cout << " | ";
    std::cout << bits.at(i) << " ";
  }
  std::cout << '\n';
}

move_t Move::setGetValue(bitboard_t to, bitboard_t from, int flags) {
  // USES A SIMPLE TO/FROM format
  // | 6 bit TO | 6 bit FROM | 4 BITS FOR FLAGS |
  mv = 0U;
  mv |= (static_cast<move_t>(to) << 10U);   // Adds to, to the front of the FILE
  mv |= (static_cast<move_t>(from) << 4U);  // Adds from behind to
  mv |= (flags << 0);
  return mv;
}

bool bitIsSet(bitboard_t board, bitboard_t index) {
  return (board & (1 << index)) ? true : false;
}

bool Move::captures() {
  if (mv & (1U << 2)) {
    return true;
  }
  return false;
}

int Move::getTo() {
  return (mv >> 10) & 63;
}

int Move::getFrom() {
  return (mv >> 4) & 63;
}
Move::Move(move_t m) {
  mv = m;
}

Move::Move() {
  mv = 0U;
}

void Move::set(move_t m) {
  mv = m;
}

} // End of move