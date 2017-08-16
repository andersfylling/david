#include "david/MoveGeneration.h"
#include "david/bitboard.h"
#include "david/utils/utils.h"
#include <utility>
#include <assert.h>
#include <vector>
#include <algorithm>

namespace david {
namespace movegen {

// ############# MOVE GENERATOR ######################


// default gameState node
static inline ::david::type::gameState_t defaultGameState() {
  ::david::type::gameState_t n;


  // set the new array data
  n.pawns   = {
      ::david::constant::defaultPiecePosition::white::PAWN,
      ::david::constant::defaultPiecePosition::black::PAWN
  };
  n.rooks   = {
      ::david::constant::defaultPiecePosition::white::ROOK,
      ::david::constant::defaultPiecePosition::black::ROOK
  };
  n.queens  = {
      ::david::constant::defaultPiecePosition::white::QUEEN,
      ::david::constant::defaultPiecePosition::black::QUEEN
  };
  n.knights = {
      ::david::constant::defaultPiecePosition::white::KNIGHT,
      ::david::constant::defaultPiecePosition::black::KNIGHT
  };
  n.kings   = {
      ::david::constant::defaultPiecePosition::white::KING,
      ::david::constant::defaultPiecePosition::black::KING
  };
  n.bishops = {
      ::david::constant::defaultPiecePosition::white::BISHOP,
      ::david::constant::defaultPiecePosition::black::BISHOP
  };

  n.piecess = {
      ::david::constant::defaultPiecePosition::white::PIECES,
      ::david::constant::defaultPiecePosition::black::PIECES
  };

  n.combinedPieces = ::david::constant::defaultPiecePosition::PIECES;

  n.blackKingCastling   = true;
  n.blackQueenCastling  = true;
  n.whiteKingCastling   = true;
  n.whiteQueenCastling  = true;
  n.castling = 15; //utils::stringTo8bitArray("00001111");

  n.isWhite = true;

  n.halfMoves = 0;
  n.fullMoves = 1;

  n.possibleSubMoves = 20;


  // take all the data from the new fields and apply them to the old
  int w = 0;
  int b = 1;

  n.WhiteBishop = n.bishops[w];
  n.WhiteKing   = n.kings[w];
  n.WhiteKnight = n.knights[w];
  n.WhitePawn   = n.pawns[w];
  n.WhiteQueen  = n.queens[w];
  n.WhiteRook   = n.rooks[w];

  n.BlackBishop = n.bishops[b];
  n.BlackKing   = n.kings[b];
  n.BlackKnight = n.knights[b];
  n.BlackPawn   = n.pawns[b];
  n.BlackQueen  = n.queens[b];
  n.BlackRook   = n.rooks[b];

  n.whitePieces = n.piecess[w];
  n.blackPieces = n.piecess[b];

  n.pieces = n.combinedPieces;

  return n;
}

const ::david::type::gameState_t DEFAULTCHESSBOARD = defaultGameState();

MoveGenerator::MoveGenerator(const type::gameState_t& gs)
    : constState(gs)
    , state(gs)
    , attacks(0)
    , indexMoveList(0)
{}


MoveGenerator::MoveGenerator()
    : constState(::david::movegen::DEFAULTCHESSBOARD)
    , state(::david::movegen::DEFAULTCHESSBOARD)
    , attacks(0)
    , indexMoveList(0)
{
  // Sets the pList
  pList.WhiteBishop = 2;
  pList.WhitePawn = 8;
  pList.WhiteRook = 2;
  pList.WhiteKnight = 2;
  pList.WhiteQueen = 1;
  pList.WhiteKing = 1;

  pList.BlackBishop = 2;
  pList.BlackPawn = 8;
  pList.BlackRook = 2;
  pList.BlackKnight = 2;
  pList.BlackQueen = 1;
  pList.BlackKing = 1;
}

/**
 * Validate that the gameState is check or not
 */
bool MoveGenerator::isInCheck() {
  generateAttacks(this->constState.isWhite);
  return (this->attacks & this->constState.kings[0]) > 0;
}

/**
 * adds a precalculated vecor to a specific location in the bitboard
 * @param destination - the bit the vector will start at
 * @param dir - direction of the movement
 * @param dist - the distance of travel
 * @return an attackvecor
 */
type::bitboard_t MoveGenerator::add_vector_to_board(type::bitboard_t destination,
                                                             DIRECTION dir,
                                                             type::bitboard_t dist) {
  // Main diagonal
  type::bitboard_t vector;
  if (dir == DIRECTION::NORTH_WEST || dir == DIRECTION::SOUTH_EAST) {
    vector = mainDiagonal[(int) dist - 1];
  } else if (dir == DIRECTION::NORTH_EAST || dir == DIRECTION::SOUTH_WEST) { // Anti diagonal
    vector = antiDiagonal[(int) dist - 1];
  } else if (dir == DIRECTION::NORTH || dir == DIRECTION::SOUTH) {  // Y axis
    vector = yAxis[(int) dist - 1];
  } else {  // X axis
    vector = xAxis[(int) dist - 1];
  }

  return (vector << destination);
}

/**
 * Clears the lists with moves
 */
void MoveGenerator::clearLists() {
  moveList.empty();
  attacks = 0;
}

/**
 * How many moves are there in the moveList?
 * @return basically how many possible moves
 */
int MoveGenerator::numberOfMoves() {
  return (int) moveList.size();
}

/**
 *
 * @param dir - the direction of the movement
 * @param steps - the number of steps to be taken. 0 = infinity
 */

void MoveGenerator::createVectors(type::bitboard_t board, DIRECTION dir, int steps, int loc) {
  type::bitboard_t numberofpieces = utils::nrOfActiveBits(board);
  type::bitboard_t tempBoard, distance, currentBit, adjecent;
  int directionNumber = (int) dir;

  std::pair<type::bitboard_t, type::bitboard_t> pai;

  // Sets the value of direction

  if (steps == 0) {  // steps until end of board
    for (type::bitboard_t i = 0; i < numberofpieces; ++i) { // Loops the pieces in the board
      currentBit = utils::LSB(board);
      tempBoard = 0;

      pai = distanceToEdge(currentBit, dir);
      distance =
          (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important

      if ((dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_EAST || dir == DIRECTION::NORTH_WEST ||
          dir == DIRECTION::WEST) && distance) {
        // Shift the number one direction number above
        adjecent = currentBit + (type::bitboard_t) directionNumber;
        tempBoard = add_vector_to_board(adjecent, dir, distance);

      } else if ((dir == DIRECTION::SOUTH || dir == DIRECTION::SOUTH_EAST || dir == DIRECTION::SOUTH_WEST ||
          dir == DIRECTION::EAST) && distance) {
        // Shift the number to current bit - (distance * direction number);
        adjecent = currentBit + (distance * directionNumber);
        tempBoard = add_vector_to_board(adjecent, dir, distance);
      }

      rawVectors[i + loc] = tempBoard;
      utils::NSB(board);
    }
  } else {
    for (type::bitboard_t i = 0; i < numberofpieces; ++i) {
      tempBoard = 0ULL;
      currentBit = utils::LSB(board);
      pai = distanceToEdge(currentBit, dir);
      distance =
          (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important
      if (distance) {
        utils::flipBitOn(tempBoard, (currentBit + (type::bitboard_t) directionNumber));
      }
      rawVectors[i + loc] = tempBoard;
      utils::NSB(board);
    }
  }
}

/**
 * @param board bitboard containing value of where a bit is
 * @param dir - NORTH EAST SOUTH...
 * @return Returns a pairs consisting of Rows til end, and cols to end
 */
std::pair<type::bitboard_t, type::bitboard_t> MoveGenerator::distanceToEdge(type::bitboard_t board,
                                                                                     DIRECTION dir) {
  std::pair<type::bitboard_t, type::bitboard_t> rp;
  rp.first = 0ULL;
  rp.second = 0ULL;
  switch (dir) {
    case DIRECTION::NORTH:rp.first = (7 - (board / 8));
      rp.second = 7;  // Does not matter
      break;
    case DIRECTION::NORTH_EAST:rp.first = (7 - (board / 8));
      rp.second = (board % 8);
      break;
    case DIRECTION::NORTH_WEST:rp.first = (7 - (board / 8ULL));
      rp.second = (7 - (board % 8ULL));
      break;
    case DIRECTION::SOUTH:rp.first = (board / 8);
      rp.second = 7;
      break;
    case DIRECTION::SOUTH_WEST:rp.first = (board / 8);
      rp.second = (7 - (board % 8));
      break;
    case DIRECTION::SOUTH_EAST:rp.first = (board / 8);
      rp.second = (board % 8);
      break;
    case DIRECTION::EAST:rp.first = 7;
      rp.second = (board % 8);
      break;
    case DIRECTION::WEST:rp.first = 7;
      rp.second = (7 - (board % 8));
      break;
    default:assert("This code is shit" == NULL);
  }
  return rp;
}

/**
 * Generates a blockade as a bitboard using the createVector
 * @param board - a board with one active bit
 * @param dir - direction of the moving piece
 * @return returns a vector
 */
type::bitboard_t MoveGenerator::generateBlock(type::bitboard_t board, DIRECTION dir, bool own) {

  createVectors(board, dir, 0);
  if (own)
    return (board > 0ULL) ? (rawVectors[0] | board) : 0;

  return (board > 0ULL) ? rawVectors[0] : 0;

  return (board > 0) ? rawVectors[0] : 0;
}

/**
 * Get a bitboard of all the white pieces
 * @return - bitboard
 */ type::bitboard_t MoveGenerator::white() {
  return (state.WhitePawn | state.WhiteRook | state.WhiteKnight | state.WhiteKing | state.WhiteBishop
      | state.WhiteQueen);
}

/**
 * Creates a bitboard of all black pieces
 * @return bitboard
 */ type::bitboard_t MoveGenerator::black() {
  return (state.BlackPawn | state.BlackRook | state.BlackKing | state.BlackKnight | state.BlackBishop
      | state.BlackQueen);
}

/**
 * returns if the movement increaces the bitboard value
 * @param dir - the direction of the movement
 * @return returns true if piece is moving in a direction that is positive on the bitboard
 */
bool MoveGenerator::nothernly(DIRECTION dir) {
  if (dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_WEST || dir == DIRECTION::NORTH_EAST
      || dir == DIRECTION::WEST)
    return true;
  return false;
}

/**
 * Simplifies the vector by taking in account the blocks created by other pieces
 * @param vector - bitboard vector
 * @param color - color of the pieces you want to calculate
 * @param dir - direction of the movement
 * @param pawn - a pawn cant attack in some directions. Also treat own as an opponent
 * @return - a much simpler vector that is reasonable to loop through
 */
type::bitboard_t MoveGenerator::reduceVector(type::bitboard_t vector, bitboard::COLOR color,
                                                      DIRECTION dir, bool pawn) {
  type::bitboard_t ownBlock, oponentBlock, oponent, own, ownInter, oppInter;
  ownBlock = 0;
  oponentBlock = 0, oppInter = 0, ownInter = 0;

  if (color == bitboard::COLOR::WHITE) {
    own = this->state.whitePieces;
    oponent = this->state.blackPieces;
  } else {
    own = this->state.blackPieces;
    oponent = this->state.whitePieces;
  }

  if (nothernly(dir)) {
    if (own & vector) utils::flipBitOn(ownInter, utils::LSB(own & vector));
    if (oponent & vector) utils::flipBitOn(oppInter, utils::LSB(oponent & vector));
  } else {
    if (own & vector) utils::flipBitOn(ownInter, utils::MSB(own & vector));
    if (oponent & vector) utils::flipBitOn(oppInter, utils::MSB(oponent & vector));
  }
  ownBlock = generateBlock(ownInter, dir, true);
  oponentBlock = generateBlock(oppInter, dir, pawn);
  //utils::printBoard(ownBlock);
  //utils::printBoard(oponentBlock);

  // result = (vector & ~(own | owB | opB));
  return (vector & ~(ownBlock | oponentBlock));
}

/**
 * Generates basic moves for knights
 * @param board - board of knight pieces
 * @return - array of bitboards
 */

void MoveGenerator::knightMoves(bitboard::COLOR color, bool vector) {
  int boardValue = 0;
  Move inter;

  bitboard_t board, friends, enemy;

  if (color == bitboard::COLOR::WHITE) {
    board = state.WhiteKnight;
    friends = this->state.whitePieces;
    enemy = this->state.blackPieces;
  } else {
    board = state.BlackKnight;
    friends = this->state.blackPieces;
    enemy = this->state.whitePieces;
  }

  if (vector) {
    for (bitboard_t index = utils::LSB(board); index; index = utils::NSB(board), ++boardValue) {
      bitboard_t bb = 0ULL;
      uint8_t row = index / 8; // index is never higher than 63
      uint8_t col = index - (row * 8); // index is never higher than 63

      // Two steps up, one left
      if (row < 6 && col < 7 && !utils::bitAt(friends, index + 17)) {
        utils::flipBitOn(attacks, 17);
      }

      // Two steps up, one right
      if (row < 6 && col > 0 && !utils::bitAt(friends, index + 15)) {
        utils::flipBitOn(attacks, index + 15);
      }

      // One step up, two left
      if (row < 7 && col < 6 && !utils::bitAt(friends, index + 6)) {
        utils::flipBitOn(attacks, index + 6);
      }

      // One step up, two right
      if (row < 7 && col > 1 && !utils::bitAt(friends, index + 10)) {
        utils::flipBitOn(attacks, index + 10);
      }

      // Two steps down, one left
      if (row > 1 && col < 7 && !utils::bitAt(friends, index + (-16 + 1))) {
        utils::flipBitOn(attacks, index - 15);
      }

      // Two steps down, one right
      if (row > 1 && col > 0 && !utils::bitAt(friends, index + (-16 - 1))) {
        utils::flipBitOn(attacks, index - 17);
      }

      // One step down, two left
      if (row > 0 && col < 6 && !utils::bitAt(friends, index + (-8 + 2))) {
        utils::flipBitOn(attacks, index - 6);
      }

      // One step down, two right
      if (row > 0 && col > 1 && !utils::bitAt(friends, index + (-8 - 2))) {
        utils::flipBitOn(attacks, index - 10);
      }
    }
  } else {

    for (bitboard_t index = utils::LSB(board); index; index = utils::NSB(board), ++boardValue) {
      bitboard_t bb = 0ULL;
      uint8_t row = index / 8; // index is never higher than 63
      uint8_t col = index - (row * 8); // index is never higher than 63

      // Two steps up, one left
      if (row < 6 && col < 7 && !utils::bitAt(friends, index + 17)) {
        inter.set(index + 17, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // Two steps up, one right
      if (row < 6 && col > 0 && !utils::bitAt(friends, index + 15)) {
        inter.set(index + 15, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // One step up, two left
      if (row < 7 && col < 6 && !utils::bitAt(friends, index + 10)) {
        inter.set(index + 10, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // One step up, two right
      if (row < 7 && col > 1 && !utils::bitAt(friends, index + (+8 - 2))) {
        inter.set(index + 6, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // Two steps down, one left
      if (row > 1 && col < 7 && !utils::bitAt(friends, index + (-16 + 1))) {
        inter.set(index - 15, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // Two steps down, one right
      if (row > 1 && col > 0 && !utils::bitAt(friends, index + (-16 - 1))) {
        inter.set(index - 17, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // One step down, two left
      if (row > 0 && col < 6 && !utils::bitAt(friends, index + (-8 + 2))) {
        inter.set(index - 6, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }

      // One step down, two right
      if (row > 0 && col > 1 && !utils::bitAt(friends, index + (-8 - 2))) {
        inter.set(index - 10, index, (board & enemy) ? MOVETYPE::CAPTURES : MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());
      }
    }
  }

}

/**
 * sets the current gameState
 * @param st - the gamestate to be set
 */
void MoveGenerator::setGameState(const type::gameState_t& st) {
  state = st;
}

/**
 * This function is made to add a bitboard value
 * @param start
 * @param number
 */
void MoveGenerator::addAttackRaw(int start, int number) {
  for (int i = start; i < number; i++) {
    attacks |= rawVectors[i];
  }
}

void MoveGenerator::addAttack(int start, int number, std::array<type::bitboard_t, 8> &arr) {
  for (int i = start; i < number; i++) {
    attacks |= arr[i];
  }
}

/**
 * Generates moves for pawns, and puts them in a move-vector
 * @param color - color of the side being calculated
 */
void MoveGenerator::pawnMoves(bitboard::COLOR color, bool vector) {
  Move inter, temp;
  type::bitboard_t opp, numpiece, origin, pawns, interTo;

  /*
   * rawVector[i + 0] = vectorY
   * rawVector[i + numpiece] = vectorMd
   * rawVector[i + numpiece * 2] = vectorAd
   */

  type::bitboard_t vecP, vec2, vec3, dpp, passArea;
  int mdIndex, adIndex;


  // Gets correct vectors for pawns
  if (color == bitboard::COLOR::WHITE) {    // Pieces are white
    pawns = state.WhitePawn;
    numpiece = utils::nrOfActiveBits(pawns);
    createVectors(state.WhitePawn, DIRECTION::NORTH, 1);
    createVectors(state.WhitePawn, DIRECTION::NORTH_WEST, 1, (int) numpiece);
    createVectors(state.WhitePawn, DIRECTION::NORTH_EAST, 1, (int) numpiece * 2);
    opp = this->state.blackPieces;
    inter.set(state.lastBlackMove);

  } else {    // Pieces are black
    pawns = state.BlackPawn;
    numpiece = utils::nrOfActiveBits(pawns);
    createVectors(state.BlackPawn, DIRECTION::SOUTH, 1);
    createVectors(state.BlackPawn, DIRECTION::SOUTH_EAST, 1, (int) numpiece);
    createVectors(state.BlackPawn, DIRECTION::SOUTH_WEST, 1, (int) numpiece * 2);
    opp = this->state.whitePieces;
    inter.set(state.lastWhiteMove);
  }
  interTo = 0;
  utils::flipBitOn(interTo, inter.getTo());

  if (vector) {
    addAttackRaw((int) numpiece, (int) numpiece);
    addAttackRaw((int) numpiece * 2, (int) numpiece);
  } else {

    for (type::bitboard_t i = 0; i < numpiece; i++) {  // Loops the pieces / vectors
      vecP = 0, vec2 = 0, vec3 = 0;
      mdIndex = (int) (i + numpiece);
      adIndex = (int) (i + numpiece * 2);
      origin = utils::LSB(pawns);
      //std::cout << origin << std::endl;
      if (color == bitboard::COLOR::WHITE) {
        vecP = reduceVector(rawVectors[i], color, DIRECTION::NORTH, true);
        vec2 = reduceVector(rawVectors[mdIndex], color, DIRECTION::NORTH_WEST);
        vec3 = reduceVector(rawVectors[adIndex], color, DIRECTION::NORTH_EAST);

        if (vecP && (vecP < 16777216ULL)) { // Is double pawn push possible
          dpp = (vecP << 8ULL);
          temp.set(utils::LSB(vecP << 8ULL), origin, MOVETYPE::DOUBLE_PAWN_PUSH);
          if (!(dpp & opp))
            moveList.push_back(temp.getMove());
        }
      } else {
        vecP = reduceVector(rawVectors[i], color, DIRECTION::SOUTH, true);
        vec2 = reduceVector(rawVectors[adIndex], color, DIRECTION::SOUTH_WEST);
        vec3 = reduceVector(rawVectors[mdIndex], color, DIRECTION::SOUTH_EAST);

        if (vecP && (vecP > 549755813888ULL)) { // Is double pawn push possible
          dpp = (vecP >> 8ULL);
          temp.set(utils::LSB(vecP >> 8ULL), origin, MOVETYPE::DOUBLE_PAWN_PUSH);
          if (!(dpp & opp))
            moveList.push_back(temp.getMove());
        }
      }

      // Adds normal movement
      if (vecP) {  // move that cannot hurt anyone
        if (vecP > 36028797018963968ULL || vecP < 32768ULL) { // Pawn to be upgraded
          temp.set(utils::LSB(vecP), origin, MOVETYPE::QUEEN_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vecP), origin, MOVETYPE::BISHOP_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vecP), origin, MOVETYPE::ROOK_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vecP), origin, MOVETYPE::KNIGHT_PROMO);
          moveList.push_back(temp.getMove());
        } else {
          temp.set(utils::LSB(vecP), origin, MOVETYPE::NONTHREAT);
          moveList.push_back(temp.getMove());
        }
      }

      if (vec2 && (vec2 & opp)) {  // can only move if capture
        if (vecP > 36028797018963968ULL || vecP < 32768ULL) { // Pawn to be upgraded
          temp.set(utils::LSB(vec2), origin, MOVETYPE::QUEEN_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec2), origin, MOVETYPE::BISHOP_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec2), origin, MOVETYPE::ROOK_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec2), origin, MOVETYPE::KNIGHT_PROMO);
          moveList.push_back(temp.getMove());
        } else {
          temp.set(utils::LSB(vec2), origin, MOVETYPE::CAPTURES);
          moveList.push_back(temp.getMove());
        }
      }

      if (vec3 && (vec3 & opp)) {  // Can only move if capture
        if (vecP > 36028797018963968ULL || vecP < 32768ULL) { // Pawn to be upgraded
          temp.set(utils::LSB(vec3), origin, MOVETYPE::QUEEN_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec3), origin, MOVETYPE::BISHOP_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec3), origin, MOVETYPE::ROOK_PROMO);
          moveList.push_back(temp.getMove());
          temp.set(utils::LSB(vec3), origin, MOVETYPE::KNIGHT_PROMO);
          moveList.push_back(temp.getMove());
        } else {
          temp.set(utils::LSB(vec3), origin, MOVETYPE::CAPTURES);
          moveList.push_back(temp.getMove());
        }
      }

      // En passant bitches
      passArea = 0;
      utils::flipBitOn(passArea, origin + 1);
      utils::flipBitOn(passArea, origin - 1);
      if (color == bitboard::COLOR::WHITE && inter.getType() == MOVETYPE::DOUBLE_PAWN_PUSH) {

        if (interTo & passArea) {
          temp.set(utils::LSB(interTo << 8), origin, MOVETYPE::ENPASSANT);
          moveList.push_back(temp.getMove());
        }
      } else if (color == bitboard::COLOR::BLACK && inter.getType() == MOVETYPE::DOUBLE_PAWN_PUSH) {
        if (inter.getTo() & passArea) {
          temp.set(utils::LSB(interTo >> 8), origin, MOVETYPE::ENPASSANT);
          moveList.push_back(temp.getMove());
        }
      }
      utils::NSB(pawns);
    }
  }
}
/**
 * Undoes the move of the given pointer
 * @param move
 * @param s
 */
void MoveGenerator::undoMove(type::move_t move) {
  Move inter{move};
  type::bitboard_t *referenceTo, *referenceFrom, to, from;
  to = 0ULL;
  from = 0ULL;
  utils::flipBitOn(to, inter.getTo()); // Makes index
  utils::flipBitOff(from, inter.getFrom()); // makes index

  /*
   * Check if capture, enPassant, rockade
   * if capture.
   */
  // Where was the piece moved to
  if (to & state.WhiteKing)
    referenceTo = &state.WhiteKing;
  else if (to & state.WhiteKnight)
    referenceTo = &state.WhiteKnight;
  else if (to & state.WhiteBishop)
    referenceTo = &state.WhiteBishop;
  else if (to & state.WhiteKing)
    referenceTo = &state.WhiteKing;
  else if (to & state.WhiteQueen)
    referenceTo = &state.WhiteQueen;
  else if (to & state.WhiteRook)
    referenceTo = &state.WhiteRook;
  else if (to & state.WhitePawn)
    referenceTo = &state.WhitePawn;
  else if (to & state.BlackKing)
    referenceTo = &state.BlackKing;
  else if (to & state.BlackKnight)
    referenceTo = &state.BlackKnight;
  else if (to & state.BlackBishop)
    referenceTo = &state.BlackBishop;
  else if (to & state.BlackKing)
    referenceTo = &state.BlackKing;
  else if (to & state.BlackQueen)
    referenceTo = &state.BlackQueen;
  else if (to & state.BlackPawn)
    referenceTo = &state.BlackPawn;
  else
    referenceTo = &state.BlackRook;

  // Where the piece came from
  // The pawn are last in the list. This is to pick up
  if (utils::nrOfActiveBits(state.WhiteQueen) != pList.WhiteQueen) {
    referenceFrom = &state.WhiteQueen;
  } else if (utils::nrOfActiveBits(state.WhiteKing) != pList.WhiteKing) {
    referenceFrom = &state.WhiteKing;
  } else if (utils::nrOfActiveBits(state.WhiteBishop) != pList.WhiteBishop) {
    referenceFrom = &state.WhiteBishop;
  } else if (utils::nrOfActiveBits(state.WhiteKnight) != pList.WhiteKnight) {
    referenceFrom = &state.WhiteKnight;
  } else if (utils::nrOfActiveBits(state.WhiteRook) != pList.WhiteRook) {
    referenceFrom = &state.WhiteRook;
  } else if (utils::nrOfActiveBits(state.WhitePawn) != pList.WhitePawn) {
    referenceFrom = &state.WhitePawn;
  } else if (utils::nrOfActiveBits(state.BlackQueen) != pList.BlackQueen)
    referenceFrom = &state.BlackQueen;
  else if (utils::nrOfActiveBits(state.BlackKing) != pList.BlackKing)
    referenceFrom = &state.BlackKing;
  else if (utils::nrOfActiveBits(state.BlackBishop) != pList.BlackBishop)
    referenceFrom = &state.BlackBishop;
  else if (utils::nrOfActiveBits(state.BlackKnight) != pList.BlackKnight)
    referenceFrom = &state.BlackKnight;
  else if (utils::nrOfActiveBits(state.BlackRook) != pList.BlackRook)
    referenceFrom = &state.BlackRook;
  else
    referenceFrom = &state.BlackPawn;

  if (inter.getMove() == MOVETYPE::QUIET || inter.getType() == MOVETYPE::NONTHREAT
      || inter.getType() == MOVETYPE::DOUBLE_PAWN_PUSH) { // Simple quiet move
    // Find the correct bitboard
    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceTo, inter.getFrom());
  } else if (inter.getType() == MOVETYPE::CAPTURES) {  // If a piece is captured
    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceFrom, inter.getFrom());
  } else if (inter.getType() == MOVETYPE::ENPASSANT) {  // Piece captured by enPassant rule
    if (inter.getTo() & this->state.whitePieces) {
      utils::flipBitOn(state.BlackPawn, inter.getTo() - 8ULL);
    } else {
      utils::flipBitOn(state.BlackPawn, inter.getTo() + 8ULL);
    }
    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceTo, inter.getFrom());

  } else if (inter.getType() == MOVETYPE::CASTLE_K) {

    if (inter.getTo() & this->state.whitePieces) {
      utils::flipBitOff(state.WhiteRook, 2);
      utils::flipBitOn(state.WhiteRook, 0);
    } else { // The move was by black
      utils::flipBitOff(state.WhiteRook, 58);
      utils::flipBitOn(state.WhiteRook, 56);
    }

    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceTo, inter.getFrom());
  } else if (inter.getType() == MOVETYPE::CASTLE_Q) {
    if (inter.getTo() & this->state.whitePieces) {
      utils::flipBitOff(state.WhiteRook, 4);
      utils::flipBitOn(state.WhiteRook, 7);
    } else { // The move was by black
      utils::flipBitOff(state.WhiteRook, 60);
      utils::flipBitOn(state.WhiteRook, 63);
    }

    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceTo, inter.getFrom());
  } else {
    utils::flipBitOff(*referenceTo, inter.getTo());
    utils::flipBitOn(*referenceTo, inter.getFrom());
  }

}

/**
 * Debugging function
 * plots the moves generated to a bitboard
 * @return returns a bitboard of all the possible destinations
 */
type::bitboard_t MoveGenerator::moveToMap() {
  type::bitboard_t res = 0;
//
//  std::vector<bitboard::move_t>::iterator it;
//  Move m;
//  for (it = moveList.begin(); it != moveList.end(); it++) {
//    //std::cout << m.getTo() << std::endl;
//    //m.printMove();
//    m.set(*it);
//    utils::flipBitOn(res, m.getTo());
//  }

  for (const auto me : moveList) {
    Move m{me};
    ::utils::flipBitOn(res, m.getTo());
  }

  return res;
}

/**
 * Prints all possible moves in an human readable format
 */
void MoveGenerator::printMoves() {
  for (const auto me : moveList) {
    Move m{me};
    m.printMove();
  }
}

/**
 * Generates moves by bishops
 * @param color - defines the color to be generated
 */
void MoveGenerator::bishopMoves(bitboard::COLOR color, bool vector) {
  // Loop pieces
  // Loop reduced vectors
  type::bitboard_t pieces = (color == bitboard::COLOR::WHITE) ? state.WhiteBishop : state.BlackBishop;
  getReducedDiagonals(pieces, color);
  type::bitboard_t pcs, origin, to, opp, numVec, numpiece, p;
  opp = (color == bitboard::COLOR::WHITE) ? this->state.blackPieces : this->state.whitePieces;
  numpiece = utils::nrOfActiveBits(pieces);

  Move inter;

  if (vector) {
    addAttack(0, (int) numpiece, vectorDiag);
  } else {

    for (type::bitboard_t i = 0; i < (int) numpiece; i++) {  // Loops the pieces
      pcs = vectorDiag[i];
      origin = utils::LSB(pieces);
      numVec = utils::nrOfActiveBits(pcs);

      for (type::bitboard_t j = 0; j < (int) numVec; ++j) {
        p = 0;
        to = utils::LSB(pcs);
        utils::flipBitOn(p, to);
        if ((p & opp) != 0ULL) {
          inter.set(to, origin, MOVETYPE::CAPTURES);
          moveList.push_back(inter.getMove());
        } else {
          inter.set(to, origin, MOVETYPE::QUIET);
          moveList.push_back(inter.getMove());
        }
        utils::NSB(pcs);
      }
      utils::NSB(pieces);
    }
  }
}

/**
 * Creates movement for rook / castle
 * @param color - color of pieces to be generated
 */
void MoveGenerator::rookMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if (color == bitboard::COLOR::WHITE) {
    opponent = this->state.blackPieces;
    pieces = state.WhiteRook;
  } else {
    opponent = this->state.whitePieces;
    pieces = state.BlackRook;
  }

  getReducedXY(pieces, color, 0);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(0, (int) numpiece, vectorXY);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectorXY[i]);
      origin = utils::LSB(pieces);
      for (int j = 0; j < (int) numvec; ++j) {
        p = 0ULL;
        piece = utils::LSB(vectorXY[i]);
        utils::flipBitOn(p, piece);

        if ((opponent & piece) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorXY[i]);
      }
      utils::NSB(vectorXY[i]);
    }
  }
}

/**
 * Generate pseudo legal moves for the queen
 * @param color
 */
void MoveGenerator::queenMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if (color == bitboard::COLOR::WHITE) {
    opponent = this->state.blackPieces;
    pieces = state.WhiteQueen;
  } else {
    opponent = this->state.whitePieces;
    pieces = state.BlackQueen;
  }

  getReducedXY(pieces, color, 0);
  getReducedDiagonals(pieces, color, 0);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(0, (int) numpiece, vectorXY);
    addAttack(0, (int) numpiece, vectorDiag);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectorDiag[i]);
      origin = utils::LSB(pieces);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorDiag[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorDiag[i]);
      }

      numvec = utils::nrOfActiveBits(vectorXY[i]);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorXY[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorXY[i]);
      }
      utils::NSB(pieces);
    }
  }
}

/**
 * generate moves for the king
 * @param color
 */
void MoveGenerator::kingMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if (color == bitboard::COLOR::WHITE) {
    opponent = this->state.blackPieces;
    pieces = state.WhiteKing;
  } else {
    opponent = this->state.whitePieces;
    pieces = state.BlackKing;
  }

  getReducedXY(pieces, color, 1);
  getReducedDiagonals(pieces, color, 1);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(0, (int) numpiece, vectorXY);
    addAttack(0, (int) numpiece, vectorDiag);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectorDiag[i]);
      origin = utils::LSB(pieces);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorDiag[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorDiag[i]);
      }

      numvec = utils::nrOfActiveBits(vectorXY[i]);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorXY[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorXY[i]);
      }
      utils::NSB(pieces);
    }
  }
}

/**
 * Creates vectors that are pseudo legal
 * @param pieces - bitboard of pieces
 * @param color - color of the pieces
 * @return an array of pseudo legal vectors
 */
void MoveGenerator::getReducedDiagonals(type::bitboard_t pieces, bitboard::COLOR color, int step) {
  type::bitboard_t numPiece, vec;
  numPiece = utils::nrOfActiveBits(pieces);
  createVectors(pieces, DIRECTION::NORTH_WEST, step);
  createVectors(pieces, DIRECTION::NORTH_EAST, step, (int) numPiece);
  createVectors(pieces, DIRECTION::SOUTH_WEST, step, (int) numPiece * 2);
  createVectors(pieces, DIRECTION::SOUTH_EAST, step, (int) numPiece * 3);

  for (type::bitboard_t i = 0ULL; i < numPiece; ++i) {
    vec = 0;

    vec |= reduceVector(rawVectors[i], color, DIRECTION::NORTH_WEST);
    vec |= reduceVector(rawVectors[i + numPiece], color, DIRECTION::NORTH_EAST);
    vec |= reduceVector(rawVectors[i + numPiece * 2], color, DIRECTION::SOUTH_WEST);
    vec |= reduceVector(rawVectors[i + numPiece * 3], color, DIRECTION::SOUTH_EAST);

    vectorDiag[i] = vec;

  }
}
/**
 * Creates pseudo legal vectors
 * @param pieces - bitboard of pieces to get vectors
 * @param color - color of the pieces
 * @return array of pseudo legal vectors
 */
void MoveGenerator::getReducedXY(type::bitboard_t pieces, bitboard::COLOR color, int step) {
  type::bitboard_t numPiece, vec;
  numPiece = utils::nrOfActiveBits(pieces);

  createVectors(pieces, DIRECTION::NORTH, step);
  createVectors(pieces, DIRECTION::SOUTH, step, (int) numPiece);
  createVectors(pieces, DIRECTION::WEST, step, (int) numPiece * 2);
  createVectors(pieces, DIRECTION::EAST, step, (int) numPiece * 3);
  numPiece = utils::nrOfActiveBits(pieces);

  for (type::bitboard_t i = 0ULL; i < numPiece; ++i) {
    vec = 0;

    vec |= reduceVector(rawVectors[i], color, DIRECTION::NORTH);
    vec |= reduceVector(rawVectors[i + numPiece], color, DIRECTION::SOUTH);
    vec |= reduceVector(rawVectors[i + numPiece * 2], color, DIRECTION::WEST);
    vec |= reduceVector(rawVectors[i + numPiece * 3], color, DIRECTION::EAST);
    vectorXY[i] = vec;

  }
}
/**
 * if possible. Castling is added to the movement vector
 * @param color - what side is to be castled
 */
void MoveGenerator::castling(bitboard::COLOR color) {
  // Is castling legal anymore? Check each side independently
  // Check if pieces between castle and king (both sides)
  // - create vector use the AND operator.
  // - see the number of pieces. Should be 1.
  bool queenC, kingC;
  type::bitboard_t origin, pieces, intersects;
  Move inter;

  if (color == bitboard::COLOR::WHITE) {
    queenC = state.whiteQueenCastling;
    kingC = state.whiteKingCastling;
    origin = utils::LSB(state.WhiteKing);
    pieces = state.WhiteKing;
  } else {
    queenC = state.blackQueenCastling;
    kingC = state.blackKingCastling;
    origin = utils::LSB(state.BlackKing);
    pieces = state.BlackKing;
  }

  intersects = this->state.whitePieces;
  intersects |= this->state.blackPieces;

  if (queenC) {
    createVectors(pieces, DIRECTION::WEST, 0);
    if ((utils::nrOfActiveBits(rawVectors[0] & intersects)) == 1ULL) {
      inter.set(utils::LSB(pieces << 2ULL), origin, MOVETYPE::CASTLE_Q);
      moveList.push_back(inter.getMove());
    }
  }

  if (kingC) {
    createVectors(pieces, DIRECTION::EAST, 0);
    if ((utils::nrOfActiveBits(rawVectors[0] & intersects)) == 1ULL) {
      inter.set(utils::LSB(pieces >> 2ULL), origin, MOVETYPE::CASTLE_K);
      moveList.push_back(inter.getMove());
    }
  }
}

/**
 * generates attacks for pieces. The function is for getting opponents
 * attack vectors. This function is to evaluate that a move is legal
 * @param color
 */

void MoveGenerator::generateAttacks(bitboard::COLOR color) {
  attacks = 0ULL;
  pawnMoves(color, true);
  bishopMoves(color, true);
  knightMoves(color, true);
  rookMoves(color, true);
  queenMoves(color, true);
  kingMoves(color, true);
}
void MoveGenerator::generateAttacks(bool forWhite) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;

  bitboard::COLOR color = forWhite ? WHITE : BLACK; // todo rewrite, so that the new structure can be used.

  attacks = 0ULL;
  pawnMoves(color, true);
  bishopMoves(color, true);
  knightMoves(color, true);
  rookMoves(color, true);
  queenMoves(color, true);
  kingMoves(color, true);
}

/**
 * Generates moves for all pieces
 * @param color - you should know by now.
 * @param legacy - used when function is called to generate gameStates
 */
void MoveGenerator::generateMoves(bitboard::COLOR color, bool legacy) {
  updatePlist();
  clearLists();
  pawnMoves(color);
  bishopMoves(color);
  knightMoves(color);
  rookMoves(color);
  queenMoves(color);
  kingMoves(color);
  castling(color);

  Move inter;
  // Remove illegal moves

  if (!legacy) {
    for (int i = (int) moveList.size() - 1; i >= 0; i--) {
      if (!moveIsLegal(moveList[i], color))
        moveList.erase(moveList.begin() + i);
      else {
        inter.set(moveList[i]);
      }
    }
  }
}

/**
 * This is a legacy function. This generates gameStates
 * in the future wi will only generate one gamestate per
 * subtree.
 * @param states - the vector the states will be put into
 */
void MoveGenerator::generateGameStates(std::vector<bitboard::gameState> &states) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;

  bitboard::COLOR c = (state.playerColor == bitboard::COLOR::WHITE) ? bitboard::COLOR::BLACK : bitboard::COLOR::WHITE;
  bool c2 = !this->state.isWhite;

  generateMoves(state.playerColor);

  // just to check how large moveList2 needs to be since now we use an vector
  if (this->moveList.size() >= this->moveList2.size()) {
    std::__throw_runtime_error("need to grow moveList2!!");
  }

  for (int i = (int) moveList.size() - 1; i >= 0; i--) {
    if (moveIsLegal(moveList[i], state.playerColor)) {
      type::gameState_t gs = this->state;
      gs.playerColor = c;
      gs.isWhite = c2;
      applyMove(moveList[i], gs);
      states.push_back(gs);
    }
  }
}


/**
 * Applies the move and tests if it is legal
 * @param m - the move to be applied
 * @param c - the color of the move
 * @return - returns true if legal
 */
bool MoveGenerator::moveIsLegal(bitboard::move_t m, bitboard::COLOR c) {
  using bitboard::bitboard_t;

  //type::bitboard_t res = 0ULL;

  // Code for debugging
  Move inter{m};
  //bitboard_t to, from;
  //to = inter.getTo();
  //from = inter.getFrom();
  type::gameState_t gs = this->constState;
  applyMove(m, gs);

  MoveGenerator moveGen{gs}; // don't call moveIsLegal. No recursion please.

  return !moveGen.isInCheck();
}

/**
 * Applies a move to a given gamestate
 * @param m - move_t
 * @param s - referenced state
 */

void MoveGenerator::applyMove(bitboard::move_t m) {
  applyMove(m, this->state); //bad
}

void MoveGenerator::applyMove(bitboard::move_t m, type::gameState_t& gs) {
  Move inter(m);
  type::bitboard_t origin = 0, board;
  bool capture = false;
  utils::flipBitOn(origin, inter.getFrom());
  bool white = false;

  if ((origin & gs.WhiteKing) != 0ULL) { // Is the king being moved
    gs.whiteKingCastling = false;
    gs.whiteQueenCastling = false;
    white = true;
    utils::flipBitOff(gs.WhiteKing, inter.getFrom());
    utils::flipBitOn(gs.WhiteKing, inter.getTo());

    if (inter.getType() == MOVETYPE::CASTLE_K) {  // King side castle
      utils::flipBitOff(gs.WhiteRook, 0ULL);
      utils::flipBitOn(gs.WhiteRook, 2ULL);
    } else if (inter.getType() == MOVETYPE::CASTLE_Q) { // Queen side castle
      utils::flipBitOff(gs.WhiteRook, 7ULL);
      utils::flipBitOn(gs.WhiteRook, 4ULL);
    }
  } else if ((origin & gs.WhiteKnight) != 0ULL) {
    utils::flipBitOff(gs.WhiteKnight, inter.getFrom());
    utils::flipBitOn(gs.WhiteKnight, inter.getTo());
    white = true;
  } else if ((origin & gs.WhiteBishop) != 0ULL) {
    utils::flipBitOff(gs.WhiteBishop, inter.getFrom());
    utils::flipBitOn(gs.WhiteBishop, inter.getTo());
    white = true;
  } else if ((origin & gs.WhiteQueen) != 0ULL) {
    utils::flipBitOff(gs.WhiteQueen, inter.getFrom());
    utils::flipBitOn(gs.WhiteQueen, inter.getTo());
    white = true;
  } else if ((origin & gs.WhiteRook) != 0ULL) {
    utils::flipBitOff(gs.WhiteRook, inter.getFrom());
    utils::flipBitOn(gs.WhiteRook, inter.getTo());
    white = true;
    if (origin == 128ULL)
      gs.whiteKingCastling = false;
    if (origin == 1ULL)
      gs.whiteQueenCastling = false;
  } else if ((origin & gs.WhitePawn) != 0ULL) {
    white = true;
    utils::flipBitOff(gs.WhitePawn, inter.getFrom());

    if (inter.getType() == MOVETYPE::ENPASSANT) {
      utils::flipBitOff(gs.BlackPawn, inter.getTo() - 8);
    } else if (inter.getType() == MOVETYPE::BISHOP_PROMO) {
      utils::flipBitOn(gs.WhiteBishop, inter.getTo());
    } else if (inter.getType() == MOVETYPE::KNIGHT_PROMO) {
      utils::flipBitOn(gs.WhiteKnight, inter.getTo());
    } else if (inter.getType() == MOVETYPE::QUEEN_PROMO) {
      utils::flipBitOn(gs.WhiteQueen, inter.getTo());
    } else if (inter.getType() == MOVETYPE::ROOK_PROMO) {
      utils::flipBitOn(gs.WhiteRook, inter.getTo());
    } else {
      utils::flipBitOn(gs.WhitePawn, inter.getTo());
    }

    // WELCOME TO THE DARK SIDE
  } else if ((origin & gs.BlackKnight) != 0ULL) {
    utils::flipBitOff(gs.BlackKnight, inter.getFrom());
    utils::flipBitOn(gs.BlackKnight, inter.getTo());
    white = true;

  } else if ((origin & gs.BlackQueen) != 0ULL) {
    utils::flipBitOff(gs.BlackQueen, inter.getFrom());
    utils::flipBitOn(gs.BlackQueen, inter.getTo());
    white = true;

  } else if ((origin & gs.BlackKing) != 0ULL) { // Is the king being moved
    gs.blackKingCastling = false;
    gs.blackQueenCastling = false;
    utils::flipBitOff(gs.BlackKing, inter.getFrom());
    utils::flipBitOn(gs.BlackKing, inter.getTo());

    if (inter.getType() == MOVETYPE::CASTLE_K) {  // King side castle
      utils::flipBitOff(gs.BlackRook, 56ULL);
      utils::flipBitOn(gs.BlackRook, 58ULL);
    } else if (inter.getType() == MOVETYPE::CASTLE_Q) { // Queen side castle
      utils::flipBitOff(gs.BlackRook, 63ULL);
      utils::flipBitOn(gs.BlackRook, 60ULL);
    }
  } else if ((origin & gs.BlackBishop) != 0ULL) {
    utils::flipBitOff(gs.BlackBishop, inter.getFrom());
    utils::flipBitOn(gs.BlackBishop, inter.getTo());

  } else if ((origin & gs.BlackRook) != 0ULL) {
    utils::flipBitOff(gs.BlackRook, inter.getFrom());
    utils::flipBitOn(gs.BlackRook, inter.getTo());

    if (origin == 72057594037927936ULL)
      gs.blackKingCastling = false;
    if (origin == 9223372036854775808ULL)
      gs.blackQueenCastling = false;
  } else if ((origin & gs.BlackPawn) != 0ULL) {
    utils::flipBitOff(gs.BlackPawn, inter.getFrom());
    utils::flipBitOn(gs.BlackPawn, inter.getTo());

    if (inter.getType() == MOVETYPE::ENPASSANT) {
      utils::flipBitOff(gs.WhitePawn, inter.getTo() - 8);
    } else if (inter.getType() == MOVETYPE::BISHOP_PROMO) {
      utils::flipBitOn(gs.BlackBishop, inter.getTo());
    } else if (inter.getType() == MOVETYPE::KNIGHT_PROMO) {
      utils::flipBitOn(gs.BlackKnight, inter.getTo());
    } else if (inter.getType() == MOVETYPE::QUEEN_PROMO) {
      utils::flipBitOn(gs.BlackQueen, inter.getTo());
    } else if (inter.getType() == MOVETYPE::ROOK_PROMO) {
      utils::flipBitOn(gs.BlackRook, inter.getTo());
    } else {
      utils::flipBitOn(gs.BlackPawn, inter.getTo());
    }
  }

  if (white) {
    capturePiece(bitboard::COLOR::BLACK, inter.getTo(), gs);
  } else {
    capturePiece(bitboard::COLOR::WHITE, inter.getTo(), gs);
  }

}

/**
 * In order to unmake a move, it is essential to know
 * how many pieces of each kind that exists.
 * Unmaking moves is again essential to being able to
 * use pointers throughout movegen
 */
void MoveGenerator::updatePlist() {
  // White
  pList.WhiteKing = (int) utils::nrOfActiveBits(state.WhiteKing);
  pList.WhiteQueen = (int) utils::nrOfActiveBits(state.WhiteQueen);
  pList.WhitePawn = (int) utils::nrOfActiveBits(state.WhitePawn);
  pList.WhiteBishop = (int) utils::nrOfActiveBits(state.WhiteBishop);
  pList.WhiteRook = (int) utils::nrOfActiveBits(state.WhiteRook);
  pList.WhiteKnight = (int) utils::nrOfActiveBits(state.WhiteKnight);

  // Black
  pList.BlackKing = (int) utils::nrOfActiveBits(state.BlackKing);
  pList.BlackQueen = (int) utils::nrOfActiveBits(state.BlackQueen);
  pList.BlackPawn = (int) utils::nrOfActiveBits(state.BlackPawn);
  pList.BlackBishop = (int) utils::nrOfActiveBits(state.BlackBishop);
  pList.BlackRook = (int) utils::nrOfActiveBits(state.BlackRook);
  pList.BlackKnight = (int) utils::nrOfActiveBits(state.BlackKnight);
}

/**
 * removes a piece from the board
 * @param color - color of the piece  being removed
 * @param index - index of the field being taken
 * @param st - the gamestate
 */

void MoveGenerator::capturePiece(bitboard::COLOR color, type::bitboard_t index, bitboard::gameState &st) {
  if (color == bitboard::COLOR::WHITE) {
    utils::flipBitOff(st.WhitePawn, index);
    utils::flipBitOff(st.WhiteBishop, index);
    utils::flipBitOff(st.WhiteRook, index);
    utils::flipBitOff(st.WhiteKnight, index);
    utils::flipBitOff(st.WhiteKing, index);
    utils::flipBitOff(st.WhiteQueen, index);
  } else {
    utils::flipBitOff(st.BlackPawn, index);
    utils::flipBitOff(st.BlackBishop, index);
    utils::flipBitOff(st.BlackRook, index);
    utils::flipBitOff(st.BlackKnight, index);
    utils::flipBitOff(st.BlackKing, index);
    utils::flipBitOff(st.BlackQueen, index);
  }
}



/*
 *
 */
// #######################################
// #              Move class             #
// #######################################

/**
 * Sets the move-value to 0
 */

/**
 * Constructor setting the value
 * @param m
 */
Move::Move(const bitboard::move_t m)
  : mv(m)
{}

Move::Move()
  : mv(0)
{}


/**
 *
 * @return the value of a move
 */
bitboard::move_t Move::getMove() { return mv; }

/**
 * Sets the move values
 * @param to - coordinate from
 * @param from - coordinate to
 * @param type - the type of move
 */
void Move::set(type::bitboard_t to, type::bitboard_t from, MOVETYPE type) {
  mv = 0;   // In case object has been used before
  mv |= (bitboard::move_t) type;
  mv |= ((bitboard::move_t) to << 10U);
  mv |= ((bitboard::move_t) from << 4U);
}

void Move::set(bitboard::move_t m) { mv = m; } // a code challenge

/**
 * To get the type out of the number
 * @return return an ENUM
 */
movegen::MOVETYPE Move::getType() {
  movegen::MOVETYPE t = movegen::MOVETYPE::QUIET;
  return ((movegen::MOVETYPE) (mv & 15));
}

/**
 * gets the to from value
 * @return returns the from value in bitboard-form
 */
type::bitboard_t Move::getTo() const {
  return type::bitboard_t((mv >> 10U) & 63);
}

/**
 * gets the to value from the move
 * @return the value as a bitboard
 */ type::bitboard_t Move::getFrom() {
  return type::bitboard_t((mv >> 4U) & 63);
}

/**
 * Prints out information from the move in human readable form
 */
void Move::printMove() {
  std::cout << "MOVE:\t" << getMove() << std::endl;
  std::cout << "From:\t" << getFrom() << std::endl;
  std::cout << "To:  \t" << getTo() << std::endl;
  std::cout << "Type:\t" << (int) getType() << std::endl << std::endl;
}

}  // end of movegen namespace
} // End of david namespace

