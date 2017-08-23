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
}

void MoveGen::runAllMoveGenerators() {
  // generate all the bitboards

  // Not valid, TODO: improve perft results
  // +--------------------------------------------------------+
  // | # combination:  depth:    david / Chess.js, difference |
  // +--------------------------------------------------------+
  // | * pawns:           d7:153456226 /153456226,      0     | <- haven't tested for a higher depth, d8 crashes
  // | * pawns + knights: d6: 51508542 / 51452646,  55896     |
  // | * pawns + rooks:   d7:199668752 /199668724,     28     |
  // | * pawns + bishops: d3:     3882 /     4290,    408     | <- outdated
  // | * pawns + queens:  d3:     3902 /     4118,    216     | <- outdated
  // | * pawns + kings:   d6: 13530927 / 13529441,   1486     | <- outdated
  // +--------------------------------------------------------+
  //

  this->generateRookMoves();
  this->generateKnightMoves();  // valid alone, completed depth: 8.
  this->generateBishopMoves();
  this->generateQueenMoves();
  this->generateKingMoves();
  this->generatePawnMoves(); // Must be last due to promotions

  //generate gameStates based on moves
  const auto nrOfPieceTypes = this->moves.size();
  for (unsigned long pieceType = 0; pieceType < nrOfPieceTypes; pieceType++) {

    // for every pieceType
    for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {
      type::gameState_t gs = this->state;

      // swap the active piece side
      gs.directions[0] *= -1;
      gs.directions[1] *= -1;
      gs.isWhite = !this->state.isWhite;
      gs.queenCastlings[0] = this->state.queenCastlings[1];
      gs.queenCastlings[1] = this->state.queenCastlings[0];
      gs.kingCastlings[0] = this->state.kingCastlings[1];
      gs.kingCastlings[1] = this->state.kingCastlings[0];

      // reverse the pieces to respect the active player change
      const auto nrOfPieces = this->state.piecesArr.size();
      for (uint8_t i = 0; i < nrOfPieces; i++) {
        gs.piecesArr[i][0] = this->state.piecesArr[i][1];
        gs.piecesArr[i][1] = this->state.piecesArr[i][0];
      }
      gs.piecesArr[pieceType][1] = this->moves[pieceType][board];       // the colour that just moved. now opponent.

      // Check for capture, and destroy captured piece!
      if ((this->moves[pieceType][board] & this->state.piecess[1]) > 0) {
        const uint8_t attackedPiecePosition = ::utils::LSB(this->moves[pieceType][board] & this->state.piecess[1]);

        for (auto& bbArr : gs.piecesArr) {
          // since gs has the opposite indexes, use 0 in stead of 1.
          if ((bbArr[0] & (this->moves[pieceType][board] & this->state.piecess[1])) > 0) {
            ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
            break;
          }
        }
      }

      // en passant capture. this made no difference... wtf TODO
      else if (pieceType == 0 && this->state.enPassant > 0 && (::utils::indexToBitboard(this->state.enPassant) & this->moves[0][board]) > 0) {
        ::utils::flipBitOff(gs.piecesArr[0][0], this->state.enPassantPawn);
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
        if (diff > 0) {
          before &= diff;
          now &= diff;

          if ((before & 71776119061282560) > 0 && (now & 1099494850560) > 0) {
            // its en passant
            gs.enPassantPawn = ::utils::LSB(now);
            gs.enPassant = this->state.isWhite ? gs.enPassantPawn - 8 : gs.enPassantPawn + 8;
          }
        }
      }

      // identify a castling situation
      if (pieceType == gs.iKings) {
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

      // check?
      if (this->dangerousPosition(gs.piecesArr[gs.iKings][1], gs, 0)) {
        continue;
      }

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
  uint8_t i = ::utils::LSB(que);
  while (que != 0  || i != 0) {
    const auto resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iPawns][0], i);

    auto paths = this->generatePawnPaths(i);


    // if this piece blocks a enemy attack, don't bother moving it
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0 || (this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  paths &= (this->xRayDiagonalPaths | xRayRookPaths);
    //}

    for (uint8_t ii = ::utils::LSB(paths); paths != 0; ii = ::utils::NSB(paths, ii)) {
      this->moves[this->state.iPawns][index++] = ::utils::flipBitOnCopy(resultCache, ii);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
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
  uint8_t i = ::utils::LSB(que);
  do {


    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0 || (this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  // since this piece doesnt move diagonal nor vertical/horizontal, just ignore the piece
    //  i = ::utils::NSB(que, i);
    //  continue;
    //}


    auto resultCache = this->state.piecesArr[this->state.iRooks][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateRookAttack(i);

    // if this rook blocks a enemy attack, don't bother moving it
    if ((this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
      attackPaths &= this->xRayRookPaths;
    }

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iRooks][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
  } while(i != 0);

  this->index_moves[this->state.iRooks] = index;
}

/**
 * Generate legal moves for knights
 */
void MoveGen::generateKnightMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iKnights][0];

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {

    type::bitboard_t resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iKnights][0], i);

    type::bitboard_t attackPaths = this->generateKnightAttack(i);

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iKnights][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
  } while(i != 0);

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

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {

    type::bitboard_t resultCache = this->state.piecesArr[this->state.iBishops][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i);

    // if this piece blocks a enemy attack, don't bother moving it
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= (this->xRayDiagonalPaths);
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iBishops][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
  } while(i != 0);

  this->index_moves[this->state.iBishops] = index;
}


/**
 * Generate legal moves for queen
 */
void MoveGen::generateQueenMoves() {
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iQueens][0];

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {
    type::bitboard_t resultCache = this->state.piecesArr[this->state.iQueens][0];
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateDiagonals(i);
    attackPaths |= this->generateRookAttack(i);

    // if this piece blocks a enemy attack, don't bother moving it
    //if ((this->xRayDiagonalPaths & ::utils::indexToBitboard(i)) > 0 || (this->xRayRookPaths & ::utils::indexToBitboard(i)) > 0) {
    //  attackPaths &= (this->xRayDiagonalPaths | xRayRookPaths);
    //}

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      // store the new board layout
      this->moves[this->state.iQueens][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
  } while(i != 0);

  this->index_moves[this->state.iQueens] = index;
}


/**
 * Generate legal moves for king
 */
void MoveGen::generateKingMoves() {
  unsigned long index = 0;
  // TODO: castling
  const auto friendly = this->state.piecess[0];
  const auto kingBoard = this->state.piecesArr[this->state.iKings][0];

  // stack over pieces to handle
  auto que = kingBoard;

  uint8_t i = ::utils::LSB(que);
  do {
    type::bitboard_t resultCache = kingBoard;
    ::utils::flipBitOff(resultCache, i);

    type::bitboard_t attackPaths = this->generateKingAttack(i);

    while (attackPaths != 0) {
      uint8_t position = ::utils::LSB(attackPaths);

      //if (this->dangerousPosition(::utils::indexToBitboard(position), this->state)) {
      //  attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
      //  continue;
      //}

      // store the new board layout
      this->moves[this->state.iKings][index++] = ::utils::flipBitOnCopy(resultCache, position);

      attackPaths = ::utils::flipBitOffCopy(attackPaths, position);
    }

    // go to the next piece
    i = ::utils::NSB(que, i);
  } while(i != 0);

  // king side castling
  if (this->state.kingCastlings[0] && (6917529027641081952 & friendly) == 0 && (10376293541461622928 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
    this->moves[this->state.iKings][index++] = kingBoard << 2; // move two left
  }

  // queen side castling
  if (this->state.queenCastlings[0] && (1008806316530991118 & friendly) == 0 && (1224979098644774929 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
    this->moves[this->state.iKings][index++] = kingBoard >> 3; // move three right
  }

  this->index_moves[this->state.iKings] = index;
}


#endif
}