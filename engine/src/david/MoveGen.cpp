#include "david/MoveGen.h"

#include <david/bitboard.h>

namespace david {

#ifdef MOVEGEN


/**
 * Constructor
 * @param gs non-mutable gameState instance
 */
MoveGen::MoveGen(const type::gameState_t& gs)
    : state(gs)
    , index_gameStates(0)
    , xRayRookPaths(0ULL)
    , xRayDiagonalPaths(0ULL)
{
  // use this to figure out which pieces can move without putting the king in check
  xRayRookPaths = this->generateRookXRayPaths(this->state.piecesArr[this->state.iKings][0]);
  xRayDiagonalPaths = this->generateDiagonalXRayPaths(this->state.piecesArr[this->state.iKings][0]);


  // create a reversed version of state.
  // this should speed up game state generation
  this->reversedState = this->state;

  // swap the active piece side
  this->reversedState.directions[0] *= -1;
  this->reversedState.directions[1] *= -1;
  this->reversedState.queenCastlings[0] = this->state.queenCastlings[1];
  this->reversedState.queenCastlings[1] = this->state.queenCastlings[0];
  this->reversedState.kingCastlings[0] = this->state.kingCastlings[1];
  this->reversedState.kingCastlings[1] = this->state.kingCastlings[0];
  this->reversedState.isInCheck = false;
  this->reversedState.passant = false;

  // reverse the pieces to respect the active player change
  const auto nrOfPieces = this->state.piecesArr.size();
  for (uint8_t i = 0; i < nrOfPieces; i++) {
    this->reversedState.piecesArr[i][0] = this->state.piecesArr[i][1];
    this->reversedState.piecesArr[i][1] = this->state.piecesArr[i][0];
  }
}

void MoveGen::runAllMoveGenerators() {
  // generate all the bitboards

  this->generateRookMoves();
  this->generateKnightMoves();
  this->generateBishopMoves();
  this->generateQueenMoves();
  this->generateKingMoves();
  this->generatePawnMoves(); // Must be last due to promotions

  //generate gameStates based on moves
  const auto nrOfPieceTypes = this->moves.size();
  for (unsigned long pieceType = 0; pieceType < nrOfPieceTypes; pieceType++) {

    // for every pieceType
    for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {

      //make sure the king hasnt been captured. this should never trigger.
      if ((this->state.piecesArr[this->state.iKings][1] & this->moves[pieceType][board]) > 0) {
        continue;
      }

      // create a child game state
      type::gameState_t gs = this->reversedState;
      gs.piecesArr[pieceType][1] = this->moves[pieceType][board];       // the colour that just moved. now opponent.

      // Check for capture, and destroy captured piece!
      if ((this->moves[pieceType][board] & this->state.piecess[1]) > 0) {
        const uint8_t attackedPiecePosition = ::utils::LSB(this->moves[pieceType][board] & this->state.piecess[1]);

        for (auto& bbArr : gs.piecesArr) {
          // since gs has the opposite indexes, use 0 in stead of 1.
          if (::utils::bitAt(bbArr[0], attackedPiecePosition)) {
            ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
            break;
          }
        }
      }

      // en passant capture.
      else if (pieceType == 0 && this->state.enPassant > 0 && ::utils::bitAt(this->moves[0][board], this->state.enPassant)) {
        ::utils::flipBitOff(gs.piecesArr[0][0], this->state.enPassantPawn);
        //::utils::printGameState(gs);
        gs.passant = true;
      }

      // identify a castling situation
      else if (pieceType == gs.iKings) {
        // TODO: assumption, castling happens early, so i assume that a friendly rook, is not on the opposite side, vertically. From the castling rook
        // OTHERWISE. this is a flaw

        // king side castling
        if (this->state.kingCastlings[0] && (gs.piecesArr[pieceType][1] & 4611686018427387968) > 0) {
          uint8_t castlePos = ::utils::LSB(gs.piecesArr[gs.iRooks][1] & 9223372036854775936);
          ::utils::flipBitOff(gs.piecesArr[gs.iRooks][1], castlePos);

          ::utils::flipBitOn(gs.piecesArr[gs.iRooks][1], castlePos >> 2);
          gs.kingCastlings[1] = false;
          gs.queenCastlings[1] = false;
        }

        else if (this->state.queenCastlings[0] && (gs.piecesArr[pieceType][1] & 144115188075855874) > 0) {
          uint8_t castlePos = ::utils::LSB(gs.piecesArr[gs.iRooks][1] & 72057594037927937);
          ::utils::flipBitOff(gs.piecesArr[gs.iRooks][1], castlePos);

          ::utils::flipBitOn(gs.piecesArr[gs.iRooks][1], castlePos << 2);
          gs.queenCastlings[1] = false;
          gs.kingCastlings[1] = false;
        }
      }

      // a piece was promoted, so remove the pawn that was sacrificed for this promotion
      if (this->promotedPawns[pieceType][board] > 0) {
        gs.piecesArr[0][1] ^= this->promotedPawns[pieceType][board];
      }


      // compile the new pieces
      gs.piecess[0] = gs.piecess[1] = 0; // reset
      for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 6; j++) {
          gs.piecess[i] |= gs.piecesArr[j][i];
        }
      }

      // complete board merge
      gs.combinedPieces = gs.piecess[0] | gs.piecess[1];

      //
      // ***************
      // generate attack paths of enemy pieces and check for check / check mate
      //

      //is the king in check? this is done while generating pieces



      // store the completed gamestate
      // TODO: fullstep, etc.

      // half step
      // Validate half moves
      if (!::utils::isHalfMove(
          this->state.piecess[0],
          gs.piecess[0],
          this->state.piecesArr[0][0],
          this->state.piecesArr[0][1],
          gs.piecesArr[0][0],
          gs.piecesArr[0][1]
      )) {
        gs.halfMoves = 0;
      }

      // en passant record
      gs.enPassant = 0;
      gs.enPassantPawn = 0;
      // TODO: slow
      if (pieceType == this->state.iPawns) {
        auto before = this->state.piecesArr[0][0];
        auto now = gs.piecesArr[0][1];
        const auto diff = (before ^ now);
        before &= diff;
        now &= diff;

        if ((before & 71776119061282560) > 0 && (now & 1099494850560) > 0) {
          // its en passant
          gs.enPassantPawn = ::utils::LSB(now);
          gs.enPassant = this->state.isWhite ? ::utils::LSB(now >> 8) : ::utils::LSB(now << 8);
        }
      }

      // check?
      if (this->dangerousPosition(gs.piecesArr[gs.iKings][1], gs, 0)) {
        continue;
      }

      // is this new game state in check?
      if (this->dangerousPosition(gs.piecesArr[gs.iKings][0], gs)) {
        gs.isInCheck = true;
      }

      gs.isWhite = !this->state.isWhite;
      // valid move, add it to the record.
      this->gameStates[this->index_gameStates++] = gs;
    }
  }


}

/**
 * Generate all legal moves for pawns
 *
 */
void MoveGen::generatePawnMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  const auto direction = this->state.directions[0];
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iPawns][0];

  // first lets just move every pawn up/down once
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    const auto resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iPawns][0], i);

    // psuedo paths for pawns
    auto paths = this->generatePawnPaths(i);

    // if this piece blocks a enemy attack, don't bother moving it
    if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
      //paths &= this->xRayDiagonalPaths;
    }
    else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
      //paths &= xRayRookPaths;
    }

    for (uint8_t ii = ::utils::LSB(paths); paths != 0; ii = ::utils::NSB(paths, ii)) {
      this->moves[this->state.iPawns][index++] = ::utils::flipBitOnCopy(resultCache, ii);
    }
  }

  this->index_moves[this->state.iPawns] = index;
}

/**
 * Generate legal moves for rook
 */
void MoveGen::generateRookMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iRooks][0];

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    auto resultCache = this->state.piecesArr[this->state.iRooks][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateRookAttack(i, this->state);


    // filter out moves that puts us in check
    if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayDiagonalPaths;
    }
    else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayRookPaths;
    }

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);
      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

      // store the new board layout
      this->moves[this->state.iRooks][index++] = ::utils::flipBitOnCopy(resultCache, position);
    }
  }

  this->index_moves[this->state.iRooks] = index;
}

/**
 * Generate legal moves for knights
 */
void MoveGen::generateKnightMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iKnights][0];


  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iKnights][0], i);

    type::bitboard_t attackPaths = this->generateKnightAttack(i);

    // if this piece keeps the king from danger, don't move it out of the way.
    // but only in the directions where the king would still be safe.
    if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
     // attackPaths &= this->xRayDiagonalPaths;
    }
    else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
     // attackPaths &= this->xRayRookPaths;
    }

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);
      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

      // store the new board layout
      this->moves[this->state.iKnights][index++] = ::utils::flipBitOnCopy(resultCache, position);
    }
  }

  this->index_moves[this->state.iKnights] = index;
}

/**
 * Generate legal moves for bishop
 *
 */
void MoveGen::generateBishopMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iBishops][0];

  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = this->state.piecesArr[this->state.iBishops][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i, this->state);

    // if this piece blocks a enemy attack, don't bother moving it outside the path
    if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayDiagonalPaths;
    }
    else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayRookPaths;
    }

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iBishops][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }
  }

  this->index_moves[this->state.iBishops] = index;
}


/**
 * Generate legal moves for queen
 */
void MoveGen::generateQueenMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iQueens][0];

  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    type::bitboard_t resultCache = this->state.piecesArr[this->state.iQueens][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i, this->state);
    attackPaths |= this->generateRookAttack(i, this->state);

    // if this piece blocks a enemy attack, don't bother moving it
    if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayDiagonalPaths;
    }
    else if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
      //attackPaths &= this->xRayRookPaths;
    }

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iQueens][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }
  }

  this->index_moves[this->state.iQueens] = index;
}


/**
 * Generate legal moves for king
 */
void MoveGen::generateKingMoves() {
  unsigned long index = 0;
  const auto friendly = this->state.piecess[0];
  const auto kingBoard = this->state.piecesArr[this->state.iKings][0];

  // assumptions, there's only one king
  type::bitboard_t attackPaths = this->generateKingAttack(::utils::LSB(kingBoard));

  while (attackPaths != 0) {
    uint8_t position = ::utils::LSB(attackPaths);
    attackPaths = ::utils::flipBitOffCopy(attackPaths, position);

    if (this->dangerousPosition(::utils::indexToBitboard(position), this->state)) {
      continue;
    }

    // store the new board layout
    this->moves[this->state.iKings][index++] = ::utils::indexToBitboard(position);
  }

  // king side castling
  if (this->state.kingCastlings[0] && (6917529027641081952 & friendly) == 0 && (10376293541461622928 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
    type::bitboard_t board = kingBoard << 2; // move two left
    if (!this->dangerousPosition(board, this->state)) {
      this->moves[this->state.iKings][index++] = board;
    }
  }

  // queen side castling
  if (this->state.queenCastlings[0] && (1008806316530991118 & friendly) == 0 && (1224979098644774929 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
    type::bitboard_t board = kingBoard >> 3; // move three right
    if (!this->dangerousPosition(board, this->state)) {
      this->moves[this->state.iKings][index++] = board;
    }
  }

  this->index_moves[this->state.iKings] = index;
}


#endif
}