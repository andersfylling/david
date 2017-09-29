#include "david/MoveGen.h"

#include <david/bitboard.h>
#include "david/utils/gameState.h"

namespace david {



/**
 * Constructor
 * @param gs non-mutable gameState instance
 */
MoveGen::MoveGen(const type::gameState_t& gs, const bool t)
    : state(gs)
    , index_gameStates(0)
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
  this->reversedState.directions[0] *= -1;
  this->reversedState.directions[1] *= -1;
  this->reversedState.queenCastlings[0] = this->state.queenCastlings[1];
  this->reversedState.queenCastlings[1] = this->state.queenCastlings[0];
  this->reversedState.kingCastlings[0] = this->state.kingCastlings[1];
  this->reversedState.kingCastlings[1] = this->state.kingCastlings[0];
  this->reversedState.passant = false;
  this->reversedState.enPassant = 0;
  this->reversedState.enPassantPawn = 0;

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

#ifdef DAVID_TEST
  this->printMoves = t;
#endif
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

    type::bitboard_t initialBoard = this->state.piecesArr[pieceType][0];
    type::bitboard_t que = this->state.piecesArr[pieceType][0];

    // for every pieceType
    for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {
      type::bitboard_t startPosition = que;
      que ^= 1ULL << ::utils::LSB(que);
      startPosition ^= que;

      // holds the initial position for this piece, but with the piece that's currently moving turned off
      type::bitboard_t resultCache = initialBoard ^ startPosition;

      // legal positions to walk
      type::bitboard_t positions = this->moves[pieceType][board];

      // create a gamestate for each
      while (positions != 0) {
        // game state
        type::gameState_t gs = this->reversedState;
        gs.piecesArr[pieceType][1] =

      }

      // make sure the move isnt empty, it should never be however..
      if (this->moves[pieceType][board] == 0) {
        continue;
      }

      //make sure the king hasn't been captured.
      if ((this->state.piecesArr[this->state.iKings][1] & this->moves[pieceType][board]) > 0) {
        continue;
      }

      // create a child game state
      type::gameState_t gs = this->reversedState;
      gs.piecesArr[pieceType][1] = this->moves[pieceType][board];       // the colour that just moved. now opponent.

      // Check for capture, and destroy captured piece!
      if ((this->moves[pieceType][board] & this->state.piecess[1]) > 0) {
        const uint8_t attackedPiecePosition = ::utils::LSB(this->moves[pieceType][board] & this->state.piecess[1]);

        //
        // This did not fire at depth 6. where there are +34 captures.
        //
//        if (::utils::nrOfActiveBits(this->moves[pieceType][board] & this->state.piecess[1]) != 1) {
//          std::cerr << " OMG MORE THAN ONE PIECE ATTACKED AT ONCE! " << std::endl;
//        }

        for (auto& bbArr : gs.piecesArr) {
          // since gs has the opposite indexes, use 0 in stead of 1.
          if (::utils::bitAt(bbArr[0], attackedPiecePosition)) {
            ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
            break;
          }
        }
      }

        // en passant capture.
      else if (pieceType == 0 && this->state.enPassant > 15 && ::utils::bitAt(this->moves[0][board], this->state.enPassant)) {
        ::utils::flipBitOff(gs.piecesArr[0][0], this->state.enPassantPawn);
        //::utils::printGameState(gs);
        gs.passant = true;
      }

        // identify a castling situation
        // TODO: verify that the new positions cannot be attacked!
      else if (pieceType == gs.iKings) {
        // TODO-castling1a: assumption, castling happens early, so i assume that a friendly rook,
        // TODO-castling1b: is not on the opposite side, vertically. From the castling rook
        // TODO-castling1c: OTHERWISE. this is a flaw

        // king side castling
        if (this->state.kingCastlings[0] && (gs.piecesArr[5][1] & 144115188075855874ULL) > 0) {
          uint8_t castlePos = ::utils::LSB(gs.piecesArr[gs.iRooks][1] & 72057594037927937ULL);
          ::utils::flipBitOff(gs.piecesArr[gs.iRooks][1], castlePos);

          ::utils::flipBitOn(gs.piecesArr[gs.iRooks][1], castlePos << 2);
          gs.kingCastlings[1] = false;
          gs.queenCastlings[1] = false;

#ifdef DAVID_TEST
          gs.castled = true;
#endif
        }

        else if (this->state.queenCastlings[0] && (gs.piecesArr[5][1] & 2305843009213693984ULL) > 0) {
          uint8_t castlePos = ::utils::LSB(gs.piecesArr[gs.iRooks][1] & 9223372036854775936ULL);
          ::utils::flipBitOff(gs.piecesArr[gs.iRooks][1], castlePos);

          ::utils::flipBitOn(gs.piecesArr[gs.iRooks][1], castlePos >> 3);
          gs.queenCastlings[1] = false;
          gs.kingCastlings[1] = false;

#ifdef DAVID_TEST
          gs.castled = true;
#endif
        }

          // the king has moved so disable castling for that colour
        else {
          gs.queenCastlings[1] = false;
          gs.kingCastlings[1] = false;
        }
      }

        // If a rook move, disable that sides castling rights
      else if (pieceType == this->state.iRooks) {
        // king side
        if (this->state.kingCastlings[0] && (gs.piecesArr[gs.iRooks][1] & 72057594037927937ULL) == 0) {
          // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
          gs.kingCastlings[1] = false;
        }
          // queen side
        else if (this->state.queenCastlings[0] && (gs.piecesArr[gs.iRooks][1] & 9223372036854775936ULL) == 0) {
          // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
          gs.queenCastlings[1] = false;
        }
      }

      // a piece was promoted, so remove the pawn that was sacrificed for this promotion
      if (pieceType > 0 && pieceType < 5 && (this->moves[pieceType][board] & this->state.piecesArr[0][0]) > 0) {
        const type::bitboard_t pawnBoard = this->moves[pieceType][board] & this->state.piecesArr[0][0];

        gs.piecesArr[0][1] ^= pawnBoard;
        gs.piecesArr[pieceType][1] ^= pawnBoard;
#ifdef DAVID_TEST
        gs.promoted = true;
#endif
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


      // en passant record
      // TODO: slow
      if (pieceType == this->state.iPawns && !gs.passant) {
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

      // print before check, just to validate how moves are generated
      //gs.isWhite = !this->state.isWhite;
      //::utils::printGameState(gs);
      //gs.isWhite = this->state.isWhite;

      // check?
      if (this->dangerousPosition(gs.piecesArr[gs.iKings][1], gs, 0)) {
        continue;
      }



      // store the completed gamestate
      // TODO: fullstep, etc.

      // half step
      // Validate half moves
      if (!::utils::gameState::isHalfMove(
          this->state.piecess[0],
          gs.piecess[0],
          this->state.piecesArr[0][0],
          this->state.piecesArr[0][1],
          gs.piecesArr[0][0],
          gs.piecesArr[0][1]
      )) {
        gs.halfMoves = 0;
      }

      gs.isWhite = !this->state.isWhite;

      // is this new game state in check?
#ifdef DAVID_TEST
      if (this->dangerousPosition(gs.piecesArr[gs.iKings][0], gs)) {
        gs.isInCheck = true;
      }
#endif

#ifdef DAVID_TEST
      //      // check for duplicates
//      bool duplicate = false;
//      for (unsigned long jj = 0; jj < board; jj++ ) {
//        if (this->moves[pieceType][board] == this->moves[pieceType][jj]) {
//          duplicate = true;
//          break;
//        }
//      }
//
//      if (duplicate) {
//        std::cerr << "DUPLICATE!!" << std::endl;
//        continue;
//      }
#endif

//#ifdef DAVID_TEST
//      // pawn move
//      if (this->printMoves && this->dangerousPosition(this->state.piecesArr[5][1], this->state, 0)) {
//        if (!this->printedState) { // pawn at f2
//          std::cout << "state" << std::endl;
//          ::utils::gameState::print(this->state);
//          this->printedState = true;
//        }
//
//        std::cout << "MOVE" << std::endl;
//        ::utils::gameState::print(gs);
//      }
//#endif

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
  //const auto friendly = this->state.piecess[0];
  //const auto hostiles = this->state.piecess[1];
  //const auto pieces = this->state.combinedPieces;
  //const auto direction = this->state.directions[0];
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iPawns][0];

  // first lets just move every pawn up/down once
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    // paths for pawns
    this->moves[this->state.iPawns][index++] = this->generatePawnPaths(i, this->state);
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

    this->moves[this->state.iRooks][index++] = this->generateRookAttack(i, this->state);
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

  // store every legal position for later
  while (que != 0) {
    uint8_t i = ::utils::LSB(que);
    que = ::utils::flipBitOffCopy(que, i);

    this->moves[this->state.iKnights][index++] = this->generateKnightAttack(i);
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

    this->moves[this->state.iBishops][index++] = this->generateDiagonals(i, this->state);
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

    this->moves[this->state.iQueens][index]    = this->generateDiagonals(i, this->state);
    this->moves[this->state.iQueens][index++] |= this->generateRookAttack(i, this->state);
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

  // assumption: there's only one king
  this->moves[this->state.iKings][index++] = this->generateKingAttack(::utils::LSB(kingBoard));

  // castling if not in check
  if ((this->state.queenCastlings[0] || this->state.kingCastlings[0]) && !this->dangerousPosition(kingBoard, this->state)) {
    // queen side castling
    if (this->state.queenCastlings[0] && (8070450532247928944 & friendly) == 0
        && (9223372036854775936 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
      type::bitboard_t board = kingBoard << 2; // move two left

      // make sure the squares in between aren't in check
      // make sure the king isn't in check now
      if (!this->dangerousPosition(board, this->state) && !this->dangerousPosition(kingBoard << 1, this->state)) {
        this->moves[this->state.iKings][index++] = board;
      }
    }

    // king side castling
    if (this->state.kingCastlings[0] && (432345564227567622 & friendly) == 0
        && (72057594037927937 & this->state.piecesArr[this->state.iRooks][0]) > 0) {
      type::bitboard_t board = kingBoard >> 2; // move two right
      if (!this->dangerousPosition(board, this->state) && !this->dangerousPosition(kingBoard >> 1, this->state)) {
        this->moves[this->state.iKings][index++] = board;
      }
    }
  }

  this->index_moves[this->state.iKings] = index;
}
}