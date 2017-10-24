#include "david/MoveGen.h"


namespace david {

namespace movegen {
std::array<std::array<type::bitboard_t, 225>, 6> moves = {{0}};
std::array<::david::type::gameState_t, ::david::constant::MAXMOVES * ::david::constant::MAXDEPTH> stack{};
//std::array<uint_fast16_t, ::david::constant::MAXMOVES * ::david::constant::MAXDEPTH> stack_encoded{};
}



/**
 * Constructor
 * @param gs non-mutable gameState instance
 */
MoveGen::MoveGen(type::gameState_t& gs)
    : state(gs)
//, xRayRookPaths(0ULL)
//, xRayDiagonalPaths(0ULL)
{
  // use this to figure out which pieces can move without putting the king in check
  //xRayRookPaths = this->generateRookXRayPaths(this->state.piecesArr[this->state.iKings][0]);
  //xRayDiagonalPaths = this->generateDiagonalXRayPaths(this->state.piecesArr[this->state.iKings][0]);


  // create a reversed version of state.
  // this should speed up game state generation
  this->reversedState = this->state;

  // swap the active piece side
  this->reversedState.queenCastlings[0] = this->state.queenCastlings[1];
  this->reversedState.queenCastlings[1] = this->state.queenCastlings[0];
  this->reversedState.kingCastlings[0] = this->state.kingCastlings[1];
  this->reversedState.kingCastlings[1] = this->state.kingCastlings[0];
  this->reversedState.passant = false;
  this->reversedState.enPassant = 0;
  this->reversedState.enPassantPawn = 0;
  this->reversedState.depth += 1;

#ifdef DAVID_TEST
  this->reversedState.promoted = false;
  this->reversedState.castled = false;
  this->reversedState.isInCheck = false;
#endif

  // reverse the pieces to respect the active player change
  const auto nrOfPieces = this->state.piecesArr.size();
  for (uint8_t i = 0; i < nrOfPieces; i++) {
    this->reversedState.piecesArr[i][0] = this->state.piecesArr[i][1];
    this->reversedState.piecesArr[i][1] = this->state.piecesArr[i][0];
  }

  this->reversedState.piecess[0] = this->state.piecess[1];
  this->reversedState.piecess[1] = this->state.piecess[0];

  // xray v0.1
  //this->hostileAttackPaths_queen = this->generateXRay_queen(this->state.piecesArr[5][0]);
}

void MoveGen::setGameState(type::gameState_t& gs)
{
  this->state = gs;
  this->index_moves = {{0}};


  // create a reversed version of state.
  // this should speed up game state generation
  this->reversedState = this->state;

  // swap the active piece side
  this->reversedState.queenCastlings[0] = this->state.queenCastlings[1];
  this->reversedState.queenCastlings[1] = this->state.queenCastlings[0];
  this->reversedState.kingCastlings[0] = this->state.kingCastlings[1];
  this->reversedState.kingCastlings[1] = this->state.kingCastlings[0];
  this->reversedState.passant = false;
  this->reversedState.enPassant = 0;
  this->reversedState.enPassantPawn = 0;
  this->reversedState.depth += 1;

#ifdef DAVID_TEST
  this->reversedState.promoted = false;
  this->reversedState.castled = false;
  this->reversedState.isInCheck = false;
#endif

  // reverse the pieces to respect the active player change
  const auto nrOfPieces = this->state.piecesArr.size();
  for (uint8_t i = 0; i < nrOfPieces; i++) {
    this->reversedState.piecesArr[i][0] = this->state.piecesArr[i][1];
    this->reversedState.piecesArr[i][1] = this->state.piecesArr[i][0];
  }

  this->reversedState.piecess[0] = this->state.piecess[1];
  this->reversedState.piecess[1] = this->state.piecess[0];
}

void MoveGen::runAllMoveGenerators() {



}

/**
 * Generate all legal moves for pawns
 *
 */
void MoveGen::generatePawnMoves() {
  //const auto friendly = this->state.piecess[0];
  //const auto hostiles = this->state.piecess[1];
  //const auto pieces = this->state.combinedPieces;
  //const auto direction = this->state.directions[0];
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[0][0];

  // first lets just move every pawn up/down once
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    const auto resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[0][0], i);

    // psuedo paths for pawns
    auto paths = this->generatePawnPaths(i, this->state);// & this->hostileAttackPaths_queen;

    // if this piece blocks a enemy attack, don't bother moving it
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    //  paths &= this->xRayDiagonalPaths;
    //}
    //else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  paths &= xRayRookPaths;
    //}

    for (uint8_t ii = ::utils::LSB(paths); paths != 0; ii = ::utils::NSB(paths, ii)) {
      movegen::moves[0][index++] = ::utils::flipBitOnCopy(resultCache, ii);
    }
  }

  this->index_moves[0] = index;
}

/**
 * Generate legal moves for rook
 */
void MoveGen::generateRookMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[1][0];

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    auto resultCache = this->state.piecesArr[1][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateRookAttack(i, this->state);// & this->hostileAttackPaths_queen;


    // filter out moves that puts us in check
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayDiagonalPaths;
    //}
    //else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayRookPaths;
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);
      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

      // store the new board layout
      movegen::moves[1][index++] = ::utils::flipBitOnCopy(resultCache, position);
    }
  }

  this->index_moves[1] = index;
}

/**
 * Generate legal moves for knights
 */
void MoveGen::generateKnightMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[::david::constant::index::knight][0];


  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[2][0], i);

    type::bitboard_t attackPaths = this->generateKnightAttack(i);// & this->hostileAttackPaths_queen;

    // if this piece keeps the king from danger, don't move it out of the way.
    // but only in the directions where the king would still be safe.
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    // attackPaths &= this->xRayDiagonalPaths;
    //}
    //else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    // attackPaths &= this->xRayRookPaths;
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);
      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

      // store the new board layout
      movegen::moves[2][index++] = ::utils::flipBitOnCopy(resultCache, position);
    }
  }

  this->index_moves[2] = index;
}

/**
 * Generate legal moves for bishop
 *
 */
void MoveGen::generateBishopMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[3][0];

  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = this->state.piecesArr[3][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i, this->state);// & this->hostileAttackPaths_queen;

    // if this piece blocks a enemy attack, don't bother moving it outside the path
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayDiagonalPaths;
    //}
    //else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayRookPaths;
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      movegen::moves[3][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }
  }

  this->index_moves[3] = index;
}


/**
 * Generate legal moves for queen
 */
void MoveGen::generateQueenMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[4][0];

  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = this->state.piecesArr[4][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i, this->state);// & this->hostileAttackPaths_queen;
    attackPaths |= this->generateRookAttack(i, this->state);

    // if this piece blocks a enemy attack, don't bother moving it
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayDiagonalPaths;
    //}
    //else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= this->xRayRookPaths;
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      movegen::moves[4][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }
  }

  this->index_moves[4] = index;
}


/**
 * Generate legal moves for king
 */
void MoveGen::generateKingMoves() {
  unsigned long index = 0;
  const auto kingBoard = this->state.piecesArr[5][0];

  // assumptions, there's only one king
  type::bitboard_t attackPaths = this->generateKingAttack(::utils::LSB(kingBoard));

  while (attackPaths != 0) {
    uint8_t position = ::utils::LSB(attackPaths);
    attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

    // store the new board layout
    movegen::moves[5][index++] = ::utils::indexToBitboard(position);
  }

  // castling if not in check
  if (
      (this->state.queenCastlings[0] || this->state.kingCastlings[0])
      && !this->dangerousPosition(kingBoard, this->state)
      && (this->state.piecesArr[5][0] & 576460752303423496ull) > 0
      ) {
    // queen side castling
    if (this->state.queenCastlings[0] && ((this->state.isWhite ? 112 : 8070450532247928832ULL) & this->state.combinedPieces) == 0
        && (9223372036854775936ULL & this->state.piecesArr[::david::constant::index::rook][0]) > 0) {
      type::bitboard_t board = kingBoard << 2; // move two left

      // make sure the squares in between aren't in check
      // make sure the king isn't in check now
      if (!this->dangerousPosition(board, this->state) && !this->dangerousPosition(kingBoard << 1, this->state)) {
        movegen::moves[5][index++] = board;
      }
    }

    // king side castling
    if (this->state.kingCastlings[0] && ((this->state.isWhite ? 6 : 432345564227567616ULL) & this->state.combinedPieces) == 0
        && (72057594037927937ULL & this->state.piecesArr[::david::constant::index::rook][0]) > 0) {
      type::bitboard_t board = kingBoard >> 2; // move two right
      if (!this->dangerousPosition(board, this->state) && !this->dangerousPosition(kingBoard >> 1, this->state)) {
        movegen::moves[5][index++] = board;
      }
    }
  }

  this->index_moves[5] = index;
}
}