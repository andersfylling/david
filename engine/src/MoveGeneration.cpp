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
  state.WhiteQueen = 8;
  state.WhiteKing = 16;
  state.WhiteRook = 129;

  state.blackKingCastling = true;
  state.blackQueenCastling = true;
  state.whiteKingCastling = true;
  state.whiteQueenCastling = true;
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
 *
 * @param dir - the direction of the movement
 * @param steps - the number of steps to be taken. 0 = infinity
 * @return pointer to an array of bitboards
 */

bitboard::bitboard_t *david::movegen::MoveGenerator::createVectors(bitboard::bitboard_t board, DIRECTION dir, int steps) {
  bitboard::bitboard_t numberofpieces = utils::numberOfPieces(board);
  bitboard::bitboard_t tempBoard, distance, currentBit, adjecent;
  int directionNumber = dir;
  bool nothernly = false; // Is the direction against the north or the south

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
        // Shift the number one directionnumber above
        adjecent = currentBit + (bitboard::bitboard_t) directionNumber;
        tempBoard = add_vector_to_board(adjecent, dir, distance);

      } else if ((dir == DIRECTION::SOUTH || dir == DIRECTION::SOUTH_EAST || dir == DIRECTION::SOUTH_WEST ||
          dir == DIRECTION::EAST) && distance){
        // Shift the number to currentbit - (distance * directionnumber);
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
  if (own) {
    return *createVectors(board, dir, 0);
  } else {
    return *createVectors(board + dir, dir, 0);
  }
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
 * Simplifies the vector by taking in account the blocks created by other pieces
 * @param vector - bitboard vector
 * @param color - color of the pieces you want to calculate
 * @param dir - direction of the movement
 * @param pawn - a pawn cant attack in some directions. Also treat own as an opponent
 * @return - a much simpler vector that is reasonable to loop through
 */
bitboard::bitboard_t movegen::MoveGenerator::reduceVector(bitboard::bitboard_t vector, bitboard::COLOR color,
                                                          DIRECTION dir, bool pawn) {
  bitboard::bitboard_t ownPieces, oponentPieces;
  bitboard::bitboard_t ownBlock, oponentBlock;

  ownBlock = generateBlock(vector, dir, !pawn);
  oponentBlock = generateBlock(vector, dir);

  // result = (vector & ~(own | owB | opB));
  return (vector & ~(ownBlock | oponentBlock));



}

/**
 * Generates moves for pawns, and puts them in a move-vector
 * @param color - color of the side being calculated
 */
void movegen::MoveGenerator::pawnMoves(bitboard::COLOR color) {
  movegen::Move inter, temp;
  bitboard::bitboard_t * vectorY, * vectorMd,  * vectorAd;
  bitboard::bitboard_t own, opp, numpiece, origin, pawns;



  bitboard::bitboard_t vecP, vec2, vec3, dpp, passArea;

  // Gets correct vectors for pawns
  if (color == bitboard::COLOR::WHITE) {    // Pieces are white
    pawns = state.WhitePawn;
    vectorY = createVectors(state.WhitePawn, DIRECTION::NORTH, 1);
    vectorMd = createVectors(state.WhitePawn, DIRECTION::NORTH_WEST, 1);
    vectorAd = createVectors(state.WhitePawn, DIRECTION::NORTH_EAST, 1);
    own = white();
    opp = black();
    inter.set(state.lastBlackMove);

  } else {    // Pieces are black
    pawns = state.BlackPawn;
    vectorY = createVectors(state.BlackPawn, DIRECTION::SOUTH, 1);
    vectorMd = createVectors(state.BlackPawn, DIRECTION::SOUTH_EAST, 1);
    vectorAd = createVectors(state.BlackPawn, DIRECTION::SOUTH_WEST, 1);
    own = black(); opp = white();
    inter.set(state.lastWhiteMove);
  }

  numpiece = utils::numberOfPieces(pawns);

  for(bitboard::bitboard_t i = 0; i < numpiece; i++) {  // Loops the pieces / vectors
    origin = utils::LSB(pawns);
    //std::cout << origin << std::endl;
    if (color == bitboard::COLOR::WHITE) {
      vecP = reduceVector(vectorY[0], color, DIRECTION::NORTH, true);
      vec2 = reduceVector(vectorY[0], color, DIRECTION::NORTH_WEST);
      vec3 = reduceVector(vectorY[0], color, DIRECTION::NORTH_EAST);

      if (vecP && (vecP < 16777216ULL)) { // Is double pawn push possible
        dpp = (vecP << 8ULL);
        temp.set(utils::LSB(vecP), origin, MOVETYPE::DOUBLE_PAWN_PUSH);
        if (!(dpp & opp))
          moveList.push_back(temp.getMove());
      }
    } else {
      vecP = reduceVector(vectorY[0], color, DIRECTION::SOUTH, true);
      vec2 = reduceVector(vectorY[0], color, DIRECTION::SOUTH_WEST);
      vec3 = reduceVector(vectorY[0], color, DIRECTION::SOUTH_EAST);

      if (vecP && (vecP > 549755813888ULL)) { // Is double pawn push possible
        dpp = (vecP >> 8ULL);
        temp.set(utils::LSB(vecP), origin, MOVETYPE::DOUBLE_PAWN_PUSH);
        if (!(dpp & opp))
          moveList.push_back(temp.getMove());
      }
    }

    // Adds normal movement
    if (vecP) {  // move that cannot hurt anyone
      if (vecP > 36028797018963968ULL || vecP < 32768ULL) { // Pawn to be upgraded
        temp.set(utils::LSB(vecP), origin, MOVETYPE::QUEEN_PROMO); moveList.push_back(temp.getMove());
        temp.set(utils::LSB(vecP), origin, MOVETYPE::BISHOP_PROMO); moveList.push_back(temp.getMove());
        temp.set(utils::LSB(vecP), origin, MOVETYPE::ROOK_PROMO); moveList.push_back(temp.getMove());
        temp.set(utils::LSB(vecP), origin, MOVETYPE::KNIGHT_PROMO); moveList.push_back(temp.getMove());
      } else {
        temp.set(utils::LSB(vecP), origin, MOVETYPE::NONTHREAT);
        moveList.push_back(temp.getMove());
      }
    }

    if (vec2 && (vec2 & opp)) {  // can only move if capture
      temp.set(utils::LSB(vec2), origin, MOVETYPE::CAPTURES);
    }

    if (vec3 && (vec3 & opp)) {  // Can only move if capture
      temp.set(utils::LSB(vec3), origin, MOVETYPE::CAPTURES);
    }

    // En passant bitches
    passArea = origin | (origin + 1) | (origin - 1);
    if (color == bitboard::COLOR::WHITE && inter.getType() == movegen::MOVETYPE::DOUBLE_PAWN_PUSH) {
      if (inter.getTo() & passArea) {
        temp.set(utils::LSB(inter.getTo() << 8), origin, MOVETYPE::ENPASSANT);
      }
    } else if (color == bitboard::COLOR::BLACK && inter.getType() == movegen::MOVETYPE::DOUBLE_PAWN_PUSH) {
      if (inter.getTo() & passArea) {
        temp.set(utils::LSB(inter.getTo() >> 8), origin, MOVETYPE::ENPASSANT);
      }
    }


    utils::NSB(pawns);
  }

  delete [] vectorY;
  delete [] vectorAd;
  delete [] vectorMd;
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
    std::cout << m.getTo() << std::endl;
    m.set(*it);
    utils::flipBitOn(res, m.getTo());
  }
  return res;
}

// ###########################################
// ################# MOVE ####################
// ###########################################

/**
 * Constructor sets value to 0
 */
movegen::Move::Move() { mv = 0; }

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

} // End of david