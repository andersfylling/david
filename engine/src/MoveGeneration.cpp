//
// Created by martin on 6/5/17.
//


#include "david/MoveGeneration.h"
#include "david/bitboard.h"
#include "david/utils.h"
#include <utility>
#include <assert.h>
#include <vector>

namespace david {


// ############# MOVE GENERATOR ######################

movegen::MoveGenerator::MoveGenerator() {
  state.BlackBishop = 2594073385365405696ULL;
  state.BlackKing = 576460752303423488ULL;
  state.BlackKnight = 4755801206503243776ULL;
  state.BlackPawn = 71776119061217280ULL;
  state.BlackQueen = 1152921504606846976ULL;
  state.BlackRook = 9295429630892703744ULL;

  state.WhiteBishop = 36;
  state.WhiteKnight = 66;
  state.WhitePawn = 65280;
  state.WhiteQueen = 16;
  state.WhiteKing = 8;
  state.WhiteRook = 129;

  state.blackKingCastling = true;
  state.blackQueenCastling = true;
  state.whiteKingCastling = true;
  state.whiteQueenCastling = true;

  attacks = 0;
}

/**
 * adds a precalculated vecor to a specific location in the bitboard
 * @param destination - the bit the vector will start at
 * @param dir - direction of the movement
 * @param dist - the distance of travel
 * @return an attackvecor
 */

bitboard::bitboard_t movegen::MoveGenerator::add_vector_to_board(bitboard::bitboard_t destination, DIRECTION dir, bitboard::bitboard_t dist) {
  // Main diagonal
  bitboard::bitboard_t vector;
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

  for (int i = (int) moveList.size(); i > 0; --i) {
    moveList.pop_back();
  }

  attacks = 0;
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

bitboard::bitboard_t *david::movegen::MoveGenerator::createVectors(bitboard::bitboard_t board, DIRECTION dir, int steps) {
  bitboard::bitboard_t numberofpieces = utils::numberOfPieces(board);
  bitboard::bitboard_t tempBoard, distance, currentBit, adjecent;
  int directionNumber = dir;

  std::pair<bitboard::bitboard_t, bitboard::bitboard_t> pai;

  bitboard::bitboard_t * boards; // Holds returned boards
  boards = new bitboard::bitboard_t[numberofpieces];

  // Sets the value of direction

  if (steps == 0) {  // steps until end of board
    for (bitboard::bitboard_t i = 0; i < numberofpieces; ++i) { // Loops the pieces in the board
      currentBit = utils::LSB(board);
      tempBoard = 0;

      pai = distanceToEdge(currentBit, dir);
      distance = (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important

      if((dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_EAST || dir == DIRECTION::NORTH_WEST ||
          dir == DIRECTION::WEST) && distance) {
        // Shift the number one direction number above
        adjecent = currentBit + (bitboard::bitboard_t) directionNumber;
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
    for (bitboard::bitboard_t i = 0; i < numberofpieces; ++i) {
      tempBoard = 0ULL;
      currentBit = utils::LSB(board);
      pai = distanceToEdge(currentBit, dir);
      distance = (pai.first < pai.second) ? pai.first : pai.second; // In case of diagonal the smallest is the most important
      if(distance) {
        utils::flipBitOn(tempBoard, (currentBit+ (bitboard::bitboard_t) directionNumber));
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
std::pair<bitboard::bitboard_t, bitboard::bitboard_t> movegen::MoveGenerator::distanceToEdge(bitboard::bitboard_t board,
                                                                                             DIRECTION dir) {
  std::pair <bitboard::bitboard_t, bitboard::bitboard_t> rp;

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
 */
bitboard::bitboard_t movegen::MoveGenerator::generateBlock(bitboard::bitboard_t board, DIRECTION dir, bool own) {
  bitboard::bitboard_t res;
  /*if(nothernly(dir)) {
    if (own) {
      res = *createVectors(board, dir, 0);
    } else {
      res = *createVectors((board << (bitboard::bitboard_t) dir), dir, 0);
    }
  } else {
    if (own) {
      res = *createVectors(board, dir, 0);
    } else {
      res = *createVectors((board >> dir), dir, 0);
    }
  }*/
  res = *createVectors(board, dir, 0);
  if (own)
    return (board > 0ULL) ? (res | board) : 0;

  return (board > 0ULL) ? res : 0;

  return (board > 0) ? res : 0;
}

/**
 * Get a bitboard of all the white pieces
 * @return - bitboard
 */
bitboard::bitboard_t movegen::MoveGenerator::white() {
  return (state.WhitePawn | state.WhiteRook | state.WhiteKnight | state.WhiteKing | state.WhiteBishop
      | state.WhiteQueen);
}

/**
 * Creates a bitboard of all black pieces
 * @return bitboard
 */
bitboard::bitboard_t movegen::MoveGenerator::black() {
  return (state.BlackPawn | state.BlackRook | state.BlackKing | state.BlackKnight | state.BlackBishop
      | state.BlackQueen);
}

/**
 * returns if the movement increaces the bitboard value
 * @param dir - the direction of the movement
 * @return returns true if piece is moving in a direction that is positive on the bitboard
 */
bool movegen::MoveGenerator::nothernly(DIRECTION dir) {
  if(dir == DIRECTION::NORTH || dir == DIRECTION::NORTH_WEST || dir == DIRECTION::NORTH_EAST
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
bitboard::bitboard_t movegen::MoveGenerator::reduceVector(bitboard::bitboard_t vector, bitboard::COLOR color,
                                                          DIRECTION dir, bool pawn) {
  bitboard::bitboard_t ownBlock, oponentBlock, oponent, own, ownInter, oppInter;
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
bitboard::bitboard_t* movegen::MoveGenerator::rookMovement(bitboard::bitboard_t board) {
  int boardValue = 0;
  bitboard_t *boards;
  boards = new bitboard_t[utils::numberOfPieces(board) + 1];
  for (bitboard_t index = utils::LSB(board); index; index = utils::NSB(board), ++boardValue) {
    bitboard_t temp = 0;
    bitboard_t row = index / 8;

    // Two steps up, one left

    if (row - 2 == (index - 15) / 8 && index - 15 < 64) {
      utils::flipBitOn(temp, index - 15);
    }

    // Two steps up, one right
    if (row - 2 == (index - 17) / 8 && index - 17 < 64) {
      utils::flipBitOn(temp, index - 17);
    }

    // One step up, two left
    if (row - 1 == (index - 10) / 8 && index - 10 < 64) {
      utils::flipBitOn(temp, index - 10);
    }

    // One step up, two right
    if (row - 1 == (index - 6) / 8 && index - 6 < 64) {
      utils::flipBitOn(temp, index - 6);
    }

    // Two steps down, one left
    if (row + 2 == (index + 15) / 8 && index + 15 < 64) {
      utils::flipBitOn(temp, index + 15);
    }

    // Two steps down, one right
    if (row + 2 == (index + 17) / 8 && index + 17 < 64) {
      utils::flipBitOn(temp, index + 17);
    }

    // One step down, two left
    if (row + 1 == (index + 6) / 8 && index + 6 < 64) {
      utils::flipBitOn(temp, index + 6);
    }

    // One step down, two right
    if (row + 1 == (index + 10) / 8 && index + 10 < 64) {
      utils::flipBitOn(temp, index + 10);
    }
    boards[boardValue] = temp;
  }
  return boards;
}

/**
 * sets the current gameState
 * @param st - the gamestate to be set
 */
void movegen::MoveGenerator::setGameState(type::gameState_ptr st) {
  state = (*st); // dereferrence
}


void movegen::MoveGenerator::addAttack(bitboard::bitboard_t *vectors, int number) {
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
  bitboard::bitboard_t * vectorY, * vectorMd,  * vectorAd;
  bitboard::bitboard_t opp, numpiece, origin, pawns, interTo;



  bitboard::bitboard_t vecP, vec2, vec3, dpp, passArea;

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

  numpiece = utils::numberOfPieces(pawns);

  if (vector) {
    addAttack(vectorY, (int) numpiece);
    addAttack(vectorAd, (int) numpiece);
    addAttack(vectorMd, (int) numpiece);
  } else {

    for (bitboard::bitboard_t i = 0; i < numpiece; i++) {  // Loops the pieces / vectors
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
      if (color == bitboard::COLOR::WHITE && inter.getType() == movegen::MOVETYPE::DOUBLE_PAWN_PUSH) {

        if (interTo & passArea) {
          temp.set(utils::LSB(interTo << 8), origin, MOVETYPE::ENPASSANT);
          moveList.push_back(temp.getMove());
        }
      } else if (color == bitboard::COLOR::BLACK && inter.getType() == movegen::MOVETYPE::DOUBLE_PAWN_PUSH) {
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
 */
bitboard::bitboard_t movegen::MoveGenerator::moveToMap() {
  std::vector<bitboard::move_t>::iterator it;
  movegen::Move m;
  bitboard::bitboard_t res = 0;
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
  bitboard::bitboard_t pieces = (color == bitboard::COLOR::WHITE) ? state.WhiteBishop : state.BlackBishop;
  bitboard::bitboard_t * vects = getReducedDiagonals(pieces, color);
  bitboard::bitboard_t pcs, origin, to, opp, numVec, numpiece, p;
  opp = (color == bitboard::COLOR::WHITE) ? black() : white();
  numpiece = utils::numberOfPieces(pieces);

  Move inter;

  if (vector) {
    addAttack(vects, (int) numpiece);
  } else {

    for (bitboard::bitboard_t i = 0; i < numpiece; i++) {  // Loops the pieces
      pcs = vects[i];
      origin = utils::LSB(pieces);
      numVec = utils::numberOfPieces(pcs);

      for (bitboard::bitboard_t j = 0; j < numVec; ++j) {
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
  bitboard::bitboard_t * vectors, numVec, origin, numpiece, opp, pieces, vecpiece, own, p;
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

  numpiece = utils::numberOfPieces(pieces);

  vectors = rookMovement(pieces);

  if (vector) {
    addAttack(vectors, (int) numpiece);
  } else {
    for (bitboard::bitboard_t i = 0; i < numpiece; ++i) {
      origin = utils::LSB(pieces);
      numVec = utils::numberOfPieces(vectors[i]);

      for (bitboard::bitboard_t j = 0; j < numVec; j++) {
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
  bitboard::bitboard_t pieces, * vectors, opponent, numpiece, numvec, p, origin, piece;
  Move inter;

  if(color == bitboard::COLOR::WHITE) {
    opponent = black();
    pieces = state.WhiteRook;
  } else {
    opponent = white();
    pieces = state.BlackRook;
  }

  vectors = getReducedXY(pieces, color, 0);
  numpiece = utils::numberOfPieces(pieces);

  if (vector) {
    addAttack(vectors, (int) numpiece);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::numberOfPieces(vectors[i]);
      origin = utils::LSB(pieces);
      for (int j = 0; j < (int) numvec; ++j) {
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
  bitboard::bitboard_t pieces, * vectorsDiag, *vectorsXY, opponent, numpiece, numvec, p, origin, piece;
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
  numpiece = utils::numberOfPieces(pieces);

  if (vector) {
    addAttack(vectorsXY, (int) numpiece);
    addAttack(vectorsDiag, (int) numpiece);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::numberOfPieces(vectorsDiag[i]);
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

      numvec = utils::numberOfPieces(vectorsXY[i]);

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
  bitboard::bitboard_t pieces, * vectorsDiag, *vectorsXY, opponent, numpiece, numvec, p, origin, piece;
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
  numpiece = utils::numberOfPieces(pieces);

  if (vector) {
    addAttack(vectorsDiag, (int) numpiece);
    addAttack(vectorsXY, (int) numpiece);
  } else {

    for (int i = 0; i < (int) numpiece; ++i) {
      numvec = utils::numberOfPieces(vectorsDiag[i]);
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

      numvec = utils::numberOfPieces(vectorsXY[i]);

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
 */
bitboard::bitboard_t* movegen::MoveGenerator::getReducedDiagonals(bitboard::bitboard_t pieces, bitboard::COLOR color, int step) {
  bitboard::bitboard_t numPiece, vec;
  bitboard::bitboard_t * vectorNW, * vectorNE, * vectorSW, * vectorSE, * ret;
  vectorNW = createVectors(pieces, DIRECTION::NORTH_WEST, step);
  vectorNE = createVectors(pieces, DIRECTION::NORTH_EAST, step);
  vectorSE = createVectors(pieces, DIRECTION::SOUTH_EAST, step);
  vectorSW = createVectors(pieces, DIRECTION::SOUTH_WEST, step);
  numPiece = utils::numberOfPieces(pieces);

  ret = new bitboard::bitboard_t[numPiece];

  for (bitboard::bitboard_t i = 0ULL; i < numPiece; ++i) {
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
bitboard::bitboard_t* movegen::MoveGenerator::getReducedXY(bitboard::bitboard_t pieces, bitboard::COLOR color, int step) {
  bitboard::bitboard_t numPiece, vec;
  bitboard::bitboard_t * vectorN, * vectorE, * vectorS, * vectorW, * ret;
  vectorN = createVectors(pieces, DIRECTION::NORTH, step);
  vectorS = createVectors(pieces, DIRECTION::SOUTH, step);
  vectorW = createVectors(pieces, DIRECTION::WEST, step);
  vectorE = createVectors(pieces, DIRECTION::EAST, step);
  numPiece = utils::numberOfPieces(pieces);

  ret = new bitboard::bitboard_t[numPiece];

  for (bitboard::bitboard_t i = 0ULL; i < numPiece; ++i) {
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
  bitboard::bitboard_t origin, vector, pieces, intersects;
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
    if ((utils::numberOfPieces(vector & intersects)) == 1ULL) {
      inter.set(utils::LSB(pieces << 2ULL), origin, MOVETYPE::CASTLE_Q);
      moveList.push_back(inter.getMove());
    }
  }

  if(kingC) {
    vector = *createVectors(pieces, DIRECTION::EAST, 0);
    if ((utils::numberOfPieces(vector & intersects)) == 1ULL) {
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


void movegen::MoveGenerator::generateMoves(bitboard::COLOR color, bool legacy) {
  clearLists();
  pawnMoves(color);
  bishopMoves(color);
  knightMoves(color);
  rookMoves(color);
  queenMoves(color);
  kingMoves(color);



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

void movegen::MoveGenerator::generateGameStates(std::vector<bitboard::gameState> &states) {
  bitboard::gameState temp;
  temp = state;
  temp.playerColor = (state.playerColor == bitboard::COLOR::WHITE) ? bitboard::COLOR::BLACK : bitboard::COLOR::WHITE;

  generateMoves(state.playerColor);

  for (int i = (int) moveList.size(); i >= 0; i--) {
    if (moveIsLegal(moveList[i], state.playerColor)) {
      applyMove(moveList[i], temp);
      states.push_back(temp);
    }
  }
}

bool movegen::MoveGenerator::moveIsLegal(bitboard::move_t m, bitboard::COLOR c) {
  bitboard::gameState s = state;
  bitboard::bitboard_t res = 0ULL;

  applyMove(m, state);

  if (c == bitboard::COLOR::WHITE) {
    generateAttacks(bitboard::COLOR::BLACK);
    res  = (attacks & state.WhiteKing);
  } else {
    generateAttacks(bitboard::COLOR::WHITE);
    res  = (attacks & state.BlackKing);
  }

  state = s;

  return (res == 0ULL);

}


/**
 * Applies a move to a given gamestate
 * @param m - move_t
 * @param s - referenced state
 */

void movegen::MoveGenerator::applyMove(bitboard::move_t m, bitboard::gameState & s) {
  Move inter(m);
  bitboard::bitboard_t origin = 0, board;
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

    if (origin == 19223372036854775808ULL)
      s.blackKingCastling = false;
    if (origin == 72057594037927936ULL)
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

}

/**
 * removes a piece from the board
 * @param color - color of the piece  being removed
 * @param index - index of the field being taken
 * @param st - the gamestate
 */

void movegen::MoveGenerator::capturePiece(bitboard::COLOR color, bitboard::bitboard_t index, bitboard::gameState &st) {
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
void movegen::Move::set(bitboard::bitboard_t to, bitboard::bitboard_t from, MOVETYPE type) {
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
  movegen::MOVETYPE t = MOVETYPE::QUIET;
  return ((movegen::MOVETYPE) (mv&15));
}


/**
 * gets the to from value
 * @return returns the from value in bitboard-form
 */
bitboard::bitboard_t movegen::Move::getTo() {
  return (bitboard::bitboard_t) ((mv >> 10U) & 63);
}

/**
 * gets the to value from the move
 * @return the value as a bitboard
 */
bitboard::bitboard_t movegen::Move::getFrom() {
  return (bitboard::bitboard_t) ((mv >> 4U) & 63);
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