//
// Created by martin on 6/5/17.
//


#include "david/MoveGeneration.h"
#include "david/bitboard.h"
#include "david/utils/utils.h"
#include <utility>
#include <assert.h>
#include <vector>

namespace david {


// ############# MOVE GENERATOR ######################

movegen::MoveGenerator::MoveGenerator() {
  utils::setDefaultChessLayout(this->state);
  attacks = 0ULL;
}

/**
 * adds a precalculated vecor to a specific location in the bitboard
 * @param destination - the bit the vector will start at
 * @param dir - direction of the movement
 * @param dist - the distance of travel
 * @return an attackvecor
 */
 type::bitboard_t movegen::MoveGenerator::add_vector_to_board(type::bitboard_t  destination, DIRECTION dir,type::bitboard_t  dist) {
  // Main diagonal
  type::bitboard_t vector;
  if (dir == DIRECTION::NORTH_WEST || dir == DIRECTION::SOUTH_EAST) {
    vector = mainDiagonal[(int) dist-1];
  } else if (dir == DIRECTION::NORTH_EAST || dir == DIRECTION::SOUTH_WEST) { // Anti diagonal
    vector = antiDiagonal[(int) dist-1];
  } else if (dir == DIRECTION::NORTH || dir == DIRECTION::SOUTH) {  // Y axis
    vector = yAxis[(int) dist-1];
  } else {  // X axis
    vector = xAxis[(int) dist-1];
  }

  return (vector << destination);
}

/**
 * Clears the lists with moves
 */
void movegen::MoveGenerator::clearLists() {

  //for (int i = (int) moveList.size(); i > 0; --i) {
  //  moveList.pop_back();
  //}

  moveList.clear();
  attacks = constant::EMPTYBOARD;
}

/**
 * How many moves are there in the moveList?
 * @return basically how many possible moves
 */
int movegen::MoveGenerator::numberOfMoves() {
  return (int) moveList.size();
}

/**
 *
 * @param dir - the direction of the movement
 * @param steps - the number of steps to be taken. 0 = infinity
 * @return pointer to an array of bitboards
 */
type::bitboard_t *david::movegen::MoveGenerator::createVectors(type::bitboard_t  board, DIRECTION dir, int steps) {
  int numberofpieces = utils::nrOfActiveBits(board);
  type::bitboard_t tempBoard, distance, currentBit, adjecent;
  int directionNumber = (int) dir;

  std::pair <type::bitboard_t ,type::bitboard_t > pai;

  type::bitboard_t * boards; // Holds returned boards
  boards = new type::bitboard_t [numberofpieces];

  // Sets the value of direction

  if (steps == 0) {  // steps until end of board
    for (type::bitboard_t  i = 0; i < numberofpieces; ++i) { // Loops the pieces in the board
      currentBit = utils::LSB(board);
      tempBoard = 0;

      pai = distanceToEdge(currentBit, dir);
      distance = (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important

      if((dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_EAST || dir == DIRECTION::NORTH_WEST ||
          dir == DIRECTION::WEST) && distance) {
        // Shift the number one direction number above
        adjecent = currentBit + (type::bitboard_t)  directionNumber;
        tempBoard = add_vector_to_board(adjecent, dir, distance);

      } else if ((dir == DIRECTION::SOUTH || dir == DIRECTION::SOUTH_EAST || dir == DIRECTION::SOUTH_WEST ||
          dir == DIRECTION::EAST) && distance){
        // Shift the number to current bit - (distance * direction number);
        adjecent = currentBit + (distance * directionNumber);
        tempBoard = add_vector_to_board(adjecent, dir, distance);
      }

      boards[i] = tempBoard;
      utils::NSB(board);
    }
  } else {
    for (type::bitboard_t  i = 0; i < numberofpieces; ++i) {
      tempBoard = 0ULL;
      currentBit = utils::LSB(board);
      pai = distanceToEdge(currentBit, dir);
      distance = (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important
      if(distance) {
        utils::flipBitOn(tempBoard, (currentBit + (type::bitboard_t)  directionNumber));
      }
      boards[i] = tempBoard;
      utils::NSB(board);
    }
  }
  return boards;
}

/**
 * @param board bitboard containing value of where a bit is
 * @param dir - NORTH EAST SOUTH...
 * @return Returns a pairs consisting of Rows til end, and cols to end
 */
std::pair <type::bitboard_t ,type::bitboard_t > movegen::MoveGenerator::distanceToEdge(type::bitboard_t  board,
                                                                                             DIRECTION dir) {
  std::pair <type::bitboard_t ,type::bitboard_t > rp;

  switch (dir) {
    case DIRECTION::NORTH:
      rp.first = (7-(board/8));
      rp.second = 7;  // Does not matter
      break;
    case DIRECTION::NORTH_EAST:
      rp.first = (7-(board/8));
      rp.second = (board % 8);
      break;
    case DIRECTION::NORTH_WEST:
      rp.first = (7-(board/8ULL));
      rp.second =(7-(board%8ULL));
      break;
    case DIRECTION::SOUTH:
      rp.first = (board/8);
      rp.second = 7;
      break;
    case DIRECTION::SOUTH_WEST:
      rp.first = (board/8);
      rp.second = (7-(board%8));
      break;
    case DIRECTION::SOUTH_EAST:
      rp.first = (board/8);
      rp.second = (board % 8);
      break;
    case DIRECTION::EAST:
      rp.first = 7;
      rp.second = (board % 8);
      break;
    case DIRECTION::WEST:
      rp.first = 7;
      rp.second = (7-(board%8));
      break;
    default:
      assert("This code is shit" == NULL);
  }
  return rp;
}


/**
 * Generates a blockade as a bitboard using the createVector
 * @param board - a board with one active bit
 * @param dir - direction of the moving piece
 * @return returns a vector
 */ type::bitboard_t movegen::MoveGenerator::generateBlock(type::bitboard_t  board, DIRECTION dir, bool own) {
type::bitboard_t res;
  res = *createVectors(board, dir, 0);
  if (own)
    return (board > 0ULL) ? (res | board) : 0;

  return (board > 0ULL) ? res : 0;

  return (board > 0) ? res : 0;
}

/**
 * Get a bitboard of all the white pieces
 * @return - bitboard
 */ type::bitboard_t movegen::MoveGenerator::white() {
  return (state.WhitePawn | state.WhiteRook | state.WhiteKnight | state.WhiteKing | state.WhiteBishop
      | state.WhiteQueen);
}

/**
 * Creates a bitboard of all black pieces
 * @return bitboard
 */ type::bitboard_t movegen::MoveGenerator::black() {
  return (state.BlackPawn | state.BlackRook | state.BlackKing | state.BlackKnight | state.BlackBishop
      | state.BlackQueen);
}

/**
 * returns if the movement increaces the bitboard value
 * @param dir - the direction of the movement
 * @return returns true if piece is moving in a direction that is positive on the bitboard
 */
bool movegen::MoveGenerator::nothernly(DIRECTION dir) {
  return dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_WEST || dir == DIRECTION::NORTH_EAST
      || dir == DIRECTION::WEST;
}


/**
 * Simplifies the vector by taking in account the blocks created by other pieces
 * @param vector - bitboard vector
 * @param color - color of the pieces you want to calculate
 * @param dir - direction of the movement
 * @param pawn - a pawn cant attack in some directions. Also treat own as an opponent
 * @return - a much simpler vector that is reasonable to loop through
 */
type::bitboard_t movegen::MoveGenerator::reduceVector(type::bitboard_t  vector, bitboard::COLOR color,
                                                          DIRECTION dir, bool pawn) {
type::bitboard_t ownBlock, oponentBlock, oponent, own, ownInter, oppInter;
  ownBlock = 0; oponentBlock = 0, oppInter = 0, ownInter = 0;

  if (color == bitboard::COLOR::WHITE) {
    own = white();
    oponent = black();
  } else {
    own = black();
    oponent = white();
  }

  if(nothernly(dir)) {
    if(own & vector) utils::flipBitOn(ownInter, utils::LSB(own & vector));
    if(oponent & vector) utils::flipBitOn(oppInter, utils::LSB(oponent & vector));
  } else {
    if(own & vector) utils::flipBitOn(ownInter, utils::MSB(own & vector));
    if(oponent & vector) utils::flipBitOn(oppInter, utils::MSB(oponent & vector));
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
 type::bitboard_t * movegen::MoveGenerator::rookMovement(type::bitboard_t  board) {
  int boardValue = 0;
  bitboard_t *boards;
  boards = new bitboard_t[utils::nrOfActiveBits(board)]; // + 1];
  //bitboard_t friends = board == this->state.BlackKnight ? this->black() : this->white();
  for (bitboard_t index = utils::LSB(board); index; index = utils::NSB(board), ++boardValue) {
    bitboard_t bb = constant::EMPTYBOARD;
    uint8_t row = index / 8; // index is never higher than 63
    uint8_t col = index - (row * 8); // index is never higher than 63

    // Two steps up, one left
    if (row < 6 && col < 7){ // && !utils::bitAt(friends, index + (+16 + 1))) {
      utils::flipBitOn(bb, index + (+16 + 1));
    }

    // Two steps up, one right
    if (row < 6 && col > 0){ // && !utils::bitAt(friends, index + (+16 - 1))) {
      utils::flipBitOn(bb, index + (+16 - 1));
    }

    // One step up, two left
    if (row < 7 && col < 6){ // && !utils::bitAt(friends, index + (+8 + 2))) {
      utils::flipBitOn(bb, index + (+8 + 2));
    }

    // One step up, two right
    if (row < 7 && col > 1){ // && !utils::bitAt(friends, index + (+8 - 2))) {
      utils::flipBitOn(bb, index + (+8 - 2));
    }

    // Two steps down, one left
    if (row > 1 && col < 7){ //} && !utils::bitAt(friends, index + (-16 + 1))) {
      utils::flipBitOn(bb, index + (-16 + 1));
    }

    // Two steps down, one right
    if (row > 1 && col > 0){ // && !utils::bitAt(friends, index + (-16 - 1))) {
      utils::flipBitOn(bb, index + (-16 - 1));
    }

    // One step down, two left
    if (row > 0 && col < 6){ // && !utils::bitAt(friends, index + (-8 + 2))) {
      utils::flipBitOn(bb, index + (-8 + 2));
    }

    // One step down, two right
    if (row > 0 && col > 1){ // && !utils::bitAt(friends, index + (-8 - 2))) {
      utils::flipBitOn(bb, index + (-8 - 2));
    }
    boards[boardValue] = bb;
  }
  return boards;
}

/**
 * sets the current gameState
 * @param st - the gamestate to be set
 */
//void movegen::MoveGenerator::setGameState(type::gameState_ptr st) {
//  state = (*st); // dereferrence
//}

void movegen::MoveGenerator::setGameState(const type::gameState_t& gs) {
  state = gs; // copy
}

void movegen::MoveGenerator::addAttack(type::bitboard_t  *vectors, int number) {
  for (int i = 0; i < number; i++) {
    attacks |= vectors[i];
  }
  delete [] vectors;
}

/**
 * Generates moves for pawns, and puts them in a move-vector
 * @param color - color of the side being calculated
 */
void movegen::MoveGenerator::pawnMoves(bitboard::COLOR color, bool vector) {
  movegen::Move inter, temp;
  type::bitboard_t * vectorY, * vectorMd,  * vectorAd;
  type::bitboard_t opp, origin, pawns, interTo;
  type::bitboard_t vecP, vec2, vec3, dpp, passArea;

  // Gets correct vectors for pawns
  if (color == bitboard::COLOR::WHITE) {    // Pieces are white
    pawns = state.WhitePawn;
    vectorY = createVectors(state.WhitePawn, DIRECTION::NORTH, 1);
    vectorMd = createVectors(state.WhitePawn, DIRECTION::NORTH_WEST, 1);
    vectorAd = createVectors(state.WhitePawn, DIRECTION::NORTH_EAST, 1);
    opp = black();
    inter.set(state.lastBlackMove);

  } else {    // Pieces are black
    pawns = state.BlackPawn;
    vectorY = createVectors(state.BlackPawn, DIRECTION::SOUTH, 1);
    vectorMd = createVectors(state.BlackPawn, DIRECTION::SOUTH_EAST, 1);
    vectorAd = createVectors(state.BlackPawn, DIRECTION::SOUTH_WEST, 1);
    opp = white();
    inter.set(state.lastWhiteMove);
  }
  interTo = 0;
  utils::flipBitOn(interTo, inter.getTo());

  int numpiece = utils::nrOfActiveBits(pawns);

  if (vector) {
    addAttack(vectorY,  numpiece);
    addAttack(vectorAd, numpiece);
    addAttack(vectorMd, numpiece);
  } else {

    for (type::bitboard_t  i = 0; i < numpiece; i++) {  // Loops the pieces / vectors
      vecP = 0, vec2 = 0, vec3 = 0;
      origin = utils::LSB(pawns);
      //std::cout << origin << std::endl;
      if (color == bitboard::COLOR::WHITE) {
        vecP = reduceVector(vectorY[i], color, DIRECTION::NORTH, true);
        vec2 = reduceVector(vectorMd[i], color, DIRECTION::NORTH_WEST);
        vec3 = reduceVector(vectorAd[i], color, DIRECTION::NORTH_EAST);

        if (vecP && (vecP < 16777216ULL)) { // Is double pawn push possible
          dpp = (vecP << 8ULL);
          temp.set(utils::LSB(vecP << 8ULL), origin, MOVETYPE::DOUBLE_PAWN_PUSH);
          if (!(dpp & opp))
            moveList.push_back(temp.getMove());
        }
      } else {
        vecP = reduceVector(vectorY[i], color, DIRECTION::SOUTH, true);
        vec2 = reduceVector(vectorAd[i], color, DIRECTION::SOUTH_WEST);
        vec3 = reduceVector(vectorMd[i], color, DIRECTION::SOUTH_EAST);

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
    delete [] vectorY;
    delete [] vectorAd;
    delete [] vectorMd;
  }
}

/**
 * Debugging function
 * plots the moves generated to a bitboard
 * @return returns a bitboard of all the possible destinations
 */ type::bitboard_t movegen::MoveGenerator::moveToMap() {
  std::vector<bitboard::move_t>::iterator it;
  movegen::Move m;
  type::bitboard_t res = 0;
  for(it = moveList.begin(); it != moveList.end(); it++) {
    //std::cout << m.getTo() << std::endl;
    //m.printMove();
    m.set(*it);
    utils::flipBitOn(res, m.getTo());
  }
  return res;
}

/**
 * Prints all possible moves in an human readable format
 */
void movegen::MoveGenerator::printMoves() {
  movegen::Move m;
  for(int i = 0; i < (int) moveList.size(); ++i) {
    m.set(moveList[i]);
    m.printMove();
  }
}

/**
 * Generates moves by bishops
 * @param color - defines the color to be generated
 */
void movegen::MoveGenerator::bishopMoves(bitboard::COLOR color, bool vector) {
  // Loop pieces
  // Loop reduced vectors
  type::bitboard_t pieces = (color == bitboard::COLOR::WHITE) ? state.WhiteBishop : state.BlackBishop;
  type::bitboard_t * vects = getReducedDiagonals(pieces, color);
  type::bitboard_t pcs, origin, to, opp, p;
  opp = (color == bitboard::COLOR::WHITE) ? black() : white();
  int numpiece = utils::nrOfActiveBits(pieces), numVec;

  Move inter;

  if (vector) {
    addAttack(vects, numpiece);
  } else {

    for (type::bitboard_t  i = 0; i < numpiece; i++) {  // Loops the pieces
      pcs = vects[i];
      origin = utils::LSB(pieces);
      numVec = utils::nrOfActiveBits(pcs);

      for (type::bitboard_t  j = 0; j < numVec; ++j) {
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
    delete[] vects;
  }
}

/**
 * Generates the movement of the knight and
 * adds the pseudo legal moves to the moveList
 * @param color - self explanitory
 */
void movegen::MoveGenerator::knightMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t * vectors, origin, opp, pieces, vecpiece, own, p;
  Move inter;

  if (color == bitboard::COLOR::WHITE) {
    pieces = state.WhiteKnight;
    opp = black();
    own = white();
  } else {
    pieces = state.BlackKnight;
    opp = white();
    own = black();
  }

  int numpiece = utils::nrOfActiveBits(pieces), numVec;

  vectors = rookMovement(pieces);

  if (vector) {
    addAttack(vectors, numpiece);
  } else {
    for (type::bitboard_t  i = 0; i < numpiece; ++i) {
      origin = utils::LSB(pieces);
      numVec = utils::nrOfActiveBits(vectors[i]);

      for (type::bitboard_t  j = 0; j < (int) numVec; j++) {
        p = 0;
        vecpiece = utils::LSB(vectors[i]);
        utils::flipBitOn(p, vecpiece);
        if ((p & opp) != 0ULL) {
          inter.set(vecpiece, origin, MOVETYPE::CAPTURES);
          moveList.push_back(inter.getMove());
        }

        if ((p & own) == 0ULL) {
          inter.set(vecpiece, origin, MOVETYPE::QUIET);
          moveList.push_back(inter.getMove());
        }

        utils::NSB(vectors[i]);
      }

      utils::NSB(pieces);
    }
    delete[] vectors;
  }
}

/**
 * Creates movement for rook / castle
 * @param color - color of pieces to be generated
 */
void movegen::MoveGenerator::rookMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, * vectors, opponent, p, origin, piece;
  Move inter;
  int numpiece, numvec;

  if(color == bitboard::COLOR::WHITE) {
    opponent = black();
    pieces = state.WhiteRook;
  } else {
    opponent = white();
    pieces = state.BlackRook;
  }

  vectors = getReducedXY(pieces, color, 0);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(vectors, numpiece);
  } else {

    for (int i = 0; i < numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectors[i]);
      origin = utils::LSB(pieces);
      for (int j = 0; j < numvec; ++j) {
        p = 0ULL;
        piece = utils::LSB(vectors[i]);
        utils::flipBitOn(p, piece);

        if ((opponent & piece) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectors[i]);
      }
      utils::NSB(vectors[i]);
    }
    delete[] vectors;
  }
}

/**
 * Generate pseudo legal moves for the queen
 * @param color
 */
void movegen::MoveGenerator::queenMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, * vectorsDiag, *vectorsXY, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if(color == bitboard::COLOR::WHITE) {
    opponent = black();
    pieces = state.WhiteQueen;
  } else {
    opponent = white();
    pieces = state.BlackQueen;
  }

  vectorsXY = getReducedXY(pieces, color, 0);
  vectorsDiag = getReducedDiagonals(pieces, color, 0);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(vectorsXY, (int) numpiece);
    addAttack(vectorsDiag, (int) numpiece);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectorsDiag[i]);
      origin = utils::LSB(pieces);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorsDiag[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorsDiag[i]);
      }

      numvec = utils::nrOfActiveBits(vectorsXY[i]);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorsXY[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorsXY[i]);
      }
      utils::NSB(pieces);
    }
    delete [] vectorsDiag;
    delete [] vectorsXY;
  }
}

/**
 * generate moves for the king
 * @param color
 */
void movegen::MoveGenerator::kingMoves(bitboard::COLOR color, bool vector) {
  type::bitboard_t pieces, * vectorsDiag, *vectorsXY, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if(color == bitboard::COLOR::WHITE) {
    opponent = black();
    pieces = state.WhiteKing;
  } else {
    opponent = white();
    pieces = state.BlackKing;
  }

  vectorsXY = getReducedXY(pieces, color, 1);
  vectorsDiag = getReducedDiagonals(pieces, color, 1);
  numpiece = utils::nrOfActiveBits(pieces);

  if (vector) {
    addAttack(vectorsDiag, (int) numpiece);
    addAttack(vectorsXY, (int) numpiece);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::nrOfActiveBits(vectorsDiag[i]);
      origin = utils::LSB(pieces);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorsDiag[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorsDiag[i]);
      }

      numvec = utils::nrOfActiveBits(vectorsXY[i]);

      for (int j = 0; j < (int) numvec; ++j) {
        piece = utils::LSB(vectorsXY[i]);
        p = 0;
        utils::flipBitOn(p, piece);

        if ((p & opponent) != 0ULL)
          inter.set(piece, origin, MOVETYPE::CAPTURES);
        else
          inter.set(piece, origin, MOVETYPE::QUIET);
        moveList.push_back(inter.getMove());

        utils::NSB(vectorsXY[i]);
      }
      utils::NSB(pieces);
    }
    delete [] vectorsXY;
    delete [] vectorsDiag;
  }
}

/**
 * Creates vectors that are pseudo legal
 * @param pieces - bitboard of pieces
 * @param color - color of the pieces
 * @return an array of pseudo legal vectors
 */type::bitboard_t * movegen::MoveGenerator::getReducedDiagonals(type::bitboard_t  pieces, bitboard::COLOR color, int step) {
type::bitboard_t numPiece, vec;
type::bitboard_t * vectorNW, * vectorNE, * vectorSW, * vectorSE, * ret;
  vectorNW = createVectors(pieces, DIRECTION::NORTH_WEST, step);
  vectorNE = createVectors(pieces, DIRECTION::NORTH_EAST, step);
  vectorSE = createVectors(pieces, DIRECTION::SOUTH_EAST, step);
  vectorSW = createVectors(pieces, DIRECTION::SOUTH_WEST, step);
  numPiece = utils::nrOfActiveBits(pieces);

  ret = new type::bitboard_t [numPiece];

  for (type::bitboard_t  i = 0ULL; i <  numPiece; ++i) {
    vec = 0;

    vec |= reduceVector(vectorNW[i], color, DIRECTION::NORTH_WEST);
    vec |= reduceVector(vectorNE[i], color, DIRECTION::NORTH_EAST);
    vec |= reduceVector(vectorSW[i], color, DIRECTION::SOUTH_WEST);
    vec |= reduceVector(vectorSE[i], color, DIRECTION::SOUTH_EAST);
    ret[i] = vec;

  }
  delete [] vectorNE;
  delete [] vectorNW;
  delete [] vectorSE;
  delete [] vectorSW;

  return ret;
}
/**
 * Creates pseudo legal vectors
 * @param pieces - bitboard of pieces to get vectors
 * @param color - color of the pieces
 * @return array of pseudo legal vectors
 */
 type::bitboard_t * movegen::MoveGenerator::getReducedXY(type::bitboard_t  pieces, bitboard::COLOR color, int step) {
  type::bitboard_t numPiece, vec;
  type::bitboard_t * vectorN, * vectorE, * vectorS, * vectorW, * ret;
  vectorN = createVectors(pieces, DIRECTION::NORTH, step);
  vectorS = createVectors(pieces, DIRECTION::SOUTH, step);
  vectorW = createVectors(pieces, DIRECTION::WEST, step);
  vectorE = createVectors(pieces, DIRECTION::EAST, step);
  numPiece = utils::nrOfActiveBits(pieces);

  ret = new type::bitboard_t [numPiece];

  for (type::bitboard_t  i = 0ULL; i < numPiece; ++i) {
    vec = 0;

    vec |= reduceVector(vectorN[i], color, DIRECTION::NORTH);
    vec |= reduceVector(vectorE[i], color, DIRECTION::EAST);
    vec |= reduceVector(vectorW[i], color, DIRECTION::WEST);
    vec |= reduceVector(vectorS[i], color, DIRECTION::SOUTH);
    ret[i] = vec;

  }
  delete [] vectorN;
  delete [] vectorW;
  delete [] vectorE;
  delete [] vectorS;

  return ret;
}
/**
 * if possible. Castling is added to the movement vector
 * @param color - what side is to be castled
 */
void movegen::MoveGenerator::castling(bitboard::COLOR color){
  // Is castling legal anymore? Check each side independently
  // Check if pieces between castle and king (both sides)
  // - create vector use the AND operator.
  // - see the number of pieces. Should be 1.
  bool queenC, kingC;
  type::bitboard_t origin, vector, pieces, intersects;
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

  intersects = white();
  intersects |= black();

  if(queenC) {
    vector = *createVectors(pieces, DIRECTION::WEST, 0);
    if ((utils::nrOfActiveBits(vector & intersects)) == 1ULL) {
      inter.set(utils::LSB(pieces << 2ULL), origin, MOVETYPE::CASTLE_Q);
      moveList.push_back(inter.getMove());
    }
  }

  if(kingC) {
    vector = *createVectors(pieces, DIRECTION::EAST, 0);
    if ((utils::nrOfActiveBits(vector & intersects)) == 1ULL) {
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

void movegen::MoveGenerator::generateAttacks(bitboard::COLOR color) {
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
void movegen::MoveGenerator::generateMoves(bitboard::COLOR color, bool legacy) {
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
void movegen::MoveGenerator::generateGameStates(std::vector<bitboard::gameState> &states) {
  generateMoves(state.playerColor);

  for (int i = (int) moveList.size() -1; i >= 0; i--) {
    if (moveIsLegal(moveList[i], state.playerColor)) {
      auto tmp = state;
      applyMove(moveList[i], tmp);
      states.push_back(tmp);
    }
  }

  assert(states.size() <= constant::MAXMOVES);
}

/**
 * Applies the move and tests if it is legal
 * @param m - the move to be applied
 * @param c - the color of the move
 * @return - returns true if legal
 */
bool movegen::MoveGenerator::moveIsLegal(bitboard::move_t m, bitboard::COLOR c) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  const bitboard::gameState s = state;
  type::bitboard_t res = 0ULL;

  const auto oldWhite = this->white();
  const auto oldBlack = this->black();
  const auto oldPieces = oldWhite | oldBlack;
  const auto oldWhiteRook = state.WhiteRook;
  const auto oldBlackRook = state.BlackRook;

  applyMove(m, state);

  const auto newWhite = this->white();
  const auto newBlack = this->black();
  const auto newPieces = newWhite | newBlack;

  // check that no friendly pieces are stepped on
  const auto whiteMissStep = c == WHITE && utils::nrOfActiveBits(newWhite ^ oldWhite) > 2;
  const auto blackMissStep = c == BLACK && utils::nrOfActiveBits(newBlack ^ oldBlack) > 2;
  if (whiteMissStep || blackMissStep) {
    return false;
  }

  // Make sure no white are standing on top of any blacks
  // we basicly dont want more pieces on the newly merged board compared to an empty one.
  if ((newWhite & newBlack) > constant::EMPTYBOARD) {
    return false;
  }

  // make sure only one piece has been moved during the game
  // there should only be at most 2 bits affected by a move
  // only one when something has been attacked
  const auto changedBits = utils::nrOfActiveBits(oldPieces ^ newPieces); // looks at the difference
  if (!(changedBits == 1 || changedBits == 2)) { // 1 = attacked a piece, 2 = moved to a new position
    return false;
  }

  // Make sure that this move doesn't put the current player in check
  if (c == bitboard::COLOR::WHITE) {
    generateAttacks(bitboard::COLOR::BLACK);
    res  = (attacks & state.WhiteKing);
  } else {
    generateAttacks(bitboard::COLOR::WHITE);
    res  = (attacks & state.BlackKing);
  }
  if (res != constant::EMPTYBOARD) {
    return false; // Current player just placed himself in check
  }
//
//  // verify that a rook move is legal
//  if (c == WHITE) {
//    if (state.WhiteRook != oldWhiteRook) {
//      // change has taken place
//      //check if its an attack
//      if ((newBlack & oldWhiteRook) == 0) {
//        // not an attack, verify it can go to this position
//        const bitboard_t diff = oldWhiteRook ^ state.WhiteRook;
//
//        // make sure there exists an origin and destination
//        if (utils::nrOfActiveBits(diff) == 2) {
//          const bitboard_t origin = oldWhiteRook & diff;
//          const bitboard_t destination = state.WhiteRook & diff;
//
//          const auto indexO = utils::LSB(origin);
//          const auto indexD = utils::LSB(destination);
//
//          const auto max = std::max(indexD, indexO);
//          const auto min = std::min(indexD, indexO);
//
//          // same row?
//          if (max - min < 8) {
//            for (bitboard_t i = min + 1; i < max; i++) {
//              if (utils::bitAt(oldWhite, i)) {
//                return false;
//              }
//            }
//          }
//          else {
//            for (bitboard_t i = min + 8; i < max; i += 8) {
//              if (utils::bitAt(oldWhite, i)) {
//                return false;
//              }
//            }
//          }
//        }
//      }
//    }
//  }
//  else {
//    if (state.BlackRook != oldBlackRook) {
//      // change has taken place
//      //check if its an attack
//      if ((newWhite & oldBlackRook) == 0) {
//        // not an attack, verify it can go to this position
//        const bitboard_t diff = oldBlackRook ^ state.BlackRook;
//
//        // make sure there exists an origin and destination
//        if (utils::nrOfActiveBits(diff) == 2) {
//          const bitboard_t origin = oldBlackRook & diff;
//          const bitboard_t destination = state.BlackRook & diff;
//
//          const auto indexO = utils::LSB(origin);
//          const auto indexD = utils::LSB(destination);
//
//          const auto max = std::max(indexD, indexO);
//          const auto min = std::min(indexD, indexO);
//
//          // same row?
//          if (max - min < 8) {
//            for (bitboard_t i = min + 1; i < max; i++) {
//              if (utils::bitAt(oldBlack, i)) {
//                return false;
//              }
//            }
//          }
//          else {
//            for (bitboard_t i = min + 8; i < max; i += 8) {
//              if (utils::bitAt(oldBlack, i)) {
//                return false;
//              }
//            }
//          }
//        }
//      }
//    }
//  }


  // undo move
  state = s;


  // all good
  return true;
}


/**
 * Applies a move to a given gamestate
 * @param m - move_t
 * @param s - referenced state
 */

void movegen::MoveGenerator::applyMove(bitboard::move_t m, bitboard::gameState& s) {
  Move inter(m);
  type::bitboard_t origin = 0, board;
  bool capture = false;
  utils::flipBitOn(origin, inter.getFrom());
  bool white = false;

  if ((origin & s.WhiteKing) != 0ULL) { // Is the king being moved
    s.whiteKingCastling = false;
    s.whiteQueenCastling = false;
    white = true;
    utils::flipBitOff(s.WhiteKing, inter.getFrom());
    utils::flipBitOn(s.WhiteKing, inter.getTo());

    if (inter.getType() == MOVETYPE::CASTLE_K) {  // King side castle
      utils::flipBitOff(state.WhiteRook, 0ULL);
      utils::flipBitOn(state.WhiteRook, 2ULL);
    } else if (inter.getType() == MOVETYPE::CASTLE_Q) { // Queen side castle
      utils::flipBitOff(state.WhiteRook, 7ULL);
      utils::flipBitOn(state.WhiteRook, 4ULL);
    }
  } else if ((origin & s.WhiteKnight) != 0ULL) {
    utils::flipBitOff(s.WhiteKnight, inter.getFrom());
    utils::flipBitOn(s.WhiteKnight, inter.getTo());
    white = true;
  } else if ((origin & s.WhiteBishop) != 0ULL) {
    utils::flipBitOff(s.WhiteBishop, inter.getFrom());
    utils::flipBitOn(s.WhiteBishop, inter.getTo());
    white = true;
  } else if ((origin & s.WhiteQueen) != 0ULL) {
    utils::flipBitOff(s.WhiteQueen, inter.getFrom());
    utils::flipBitOn(s.WhiteQueen, inter.getTo());
    white = true;
  }else if ((origin & s.WhiteRook) != 0ULL) {
    utils::flipBitOff(s.WhiteRook, inter.getFrom());
    utils::flipBitOn(s.WhiteRook, inter.getTo());
    white = true;
    if (origin == 128ULL)
      s.whiteKingCastling = false;
    if (origin == 1ULL)
      s.whiteQueenCastling = false;
  } else if ((origin & s.WhitePawn) != 0ULL) {
    white = true;
    utils::flipBitOff(s.WhitePawn, inter.getFrom());


    if (inter.getType() == MOVETYPE::ENPASSANT) {
      utils::flipBitOff(state.BlackPawn, inter.getTo() - 8);
    } else if (inter.getType() == MOVETYPE::BISHOP_PROMO) {
      utils::flipBitOn(state.WhiteBishop, inter.getTo());
    } else if (inter.getType() == MOVETYPE::KNIGHT_PROMO) {
      utils::flipBitOn(state.WhiteKnight, inter.getTo());
    } else if (inter.getType() == MOVETYPE::QUEEN_PROMO) {
      utils::flipBitOn(state.WhiteQueen, inter.getTo());
    } else if (inter.getType() == MOVETYPE::ROOK_PROMO) {
      utils::flipBitOn(state.WhiteRook, inter.getTo());
    } else  {
      utils::flipBitOn(s.WhitePawn, inter.getTo());
    }

    // WELCOME TO THE DARK SIDE
  } else if ((origin & s.BlackKnight) != 0ULL) {
    utils::flipBitOff(s.BlackKnight, inter.getFrom());
    utils::flipBitOn(s.BlackKnight, inter.getTo());
    white = true;

  } else if ((origin & s.BlackQueen) != 0ULL) {
    utils::flipBitOff(s.BlackQueen, inter.getFrom());
    utils::flipBitOn(s.BlackQueen, inter.getTo());
    white = true;

  } else if ((origin & s.BlackKing) != 0ULL) { // Is the king being moved
    s.blackKingCastling = false;
    s.blackQueenCastling = false;
    utils::flipBitOff(s.BlackKing, inter.getFrom());
    utils::flipBitOn(s.BlackKing, inter.getTo());

    if (inter.getType() == MOVETYPE::CASTLE_K) {  // King side castle
      utils::flipBitOff(state.BlackRook, 56ULL);
      utils::flipBitOn(state.BlackRook, 58ULL);
    } else if (inter.getType() == MOVETYPE::CASTLE_Q) { // Queen side castle
      utils::flipBitOff(state.BlackRook, 63ULL);
      utils::flipBitOn(state.BlackRook, 60ULL);
    }
  } else if ((origin & s.BlackBishop) != 0ULL) {
    utils::flipBitOff(s.BlackBishop, inter.getFrom());
    utils::flipBitOn(s.BlackBishop, inter.getTo());

  } else if ((origin & s.BlackRook) != 0ULL) {
    utils::flipBitOff(s.BlackRook, inter.getFrom());
    utils::flipBitOn(s.BlackRook, inter.getTo());

    if (origin == 72057594037927936ULL)
      s.blackKingCastling = false;
    if (origin == 9223372036854775808ULL)
      s.blackQueenCastling = false;
  } else if ((origin & s.BlackPawn) != 0ULL) {
    utils::flipBitOff(s.BlackPawn, inter.getFrom());
    utils::flipBitOn(s.BlackPawn, inter.getTo());

    if (inter.getType() == MOVETYPE::ENPASSANT) {
      utils::flipBitOff(state.WhitePawn, inter.getTo() - 8);
    } else if (inter.getType() == MOVETYPE::BISHOP_PROMO) {
      utils::flipBitOn(state.BlackBishop, inter.getTo());
    } else if (inter.getType() == MOVETYPE::KNIGHT_PROMO) {
      utils::flipBitOn(state.BlackKnight, inter.getTo());
    } else if (inter.getType() == MOVETYPE::QUEEN_PROMO) {
      utils::flipBitOn(state.BlackQueen, inter.getTo());
    } else if (inter.getType() == MOVETYPE::ROOK_PROMO) {
      utils::flipBitOn(state.BlackRook, inter.getTo());
    } else  {
      utils::flipBitOn(s.BlackPawn, inter.getTo());
    }
  }

  if (white) {
    capturePiece(bitboard::COLOR::BLACK, inter.getTo(), s);
  } else {
    capturePiece(bitboard::COLOR::WHITE, inter.getTo(), s);
  }

  // update state data such as pieces, white and black
  type::gameState_t cp = state;
  state = s;

  s.pieces = this->white() | this->black();
  s.blackPieces = this->black();
  s.whitePieces = this->white();

  // undo
  state = cp;
}

/**
 * removes a piece from the board
 * @param color - color of the piece  being removed
 * @param index - index of the field being taken
 * @param st - the gamestate
 */

void movegen::MoveGenerator::capturePiece(bitboard::COLOR color,type::bitboard_t  index, bitboard::gameState &st) {
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

movegen::Move::Move() {
  mv = 0;
}

/**
 * Constructor setting the value
 * @param m
 */
movegen::Move::Move(bitboard::move_t m) { mv  = m; }

/**
 *
 * @return the value of a move
 */
bitboard::move_t movegen::Move::getMove() { return mv; }

/**
 * Sets the move values
 * @param to - coordinate from
 * @param from - coordinate to
 * @param type - the type of move
 */
void movegen::Move::set(type::bitboard_t  to,type::bitboard_t  from, MOVETYPE type) {
  mv = 0;   // In case object has been used before
  mv |= (bitboard::move_t) type;
  mv |= ((bitboard::move_t) to << 10U);
  mv |= ((bitboard::move_t) from << 4U);
}

void movegen::Move::set(bitboard::move_t m) { mv = m; } // a code challenge

/**
 * To get the type out of the number
 * @return return an ENUM
 */
movegen::MOVETYPE movegen::Move::getType() {
  movegen::MOVETYPE t = movegen::MOVETYPE::QUIET;
  return ((movegen::MOVETYPE) (mv&15));
}


/**
 * gets the to from value
 * @return returns the from value in bitboard-form
 */ type::bitboard_t movegen::Move::getTo() {
  return type::bitboard_t  ((mv >> 10U) & 63);
}

/**
 * gets the to value from the move
 * @return the value as a bitboard
 */ type::bitboard_t movegen::Move::getFrom() {
  return type::bitboard_t  ((mv >> 4U) & 63);
}

/**
 * Prints out information from the move in human readable form
 */
void movegen::Move::printMove() {
  std::cout << "From:\t" << getFrom() << std::endl;
  std::cout << "To:  \t" << getTo() << std::endl;
  std::cout << "Type:\t" << (int) getType() << std::endl << std::endl;
}

} // End of david