#pragma once

#include "david/david.h"
#include "david/types.h"
#include "david/bitboard.h"
#include <assert.h>
#include "david/utils/gameState.h"

#ifdef DAVID_TEST
#include "david/MoveGenTest.h"
#endif

namespace david {

//! contains data used by the MoveGen instance, search or perft runs. TreeGen alternative.
namespace movegen {
extern std::array<std::array<type::bitboard_t, 30>, 6> moves;
extern std::array<::david::type::gameState_t, ::david::constant::MAXMOVES * ::david::constant::MAXDEPTH> stack;
//extern std::array<uint_fast16_t, ::david::constant::MAXMOVES * ::david::constant::MAXDEPTH> stack_encoded;
}

class MoveGen {
#ifdef DAVID_TEST
  friend class MoveGenTest;
#endif
 public:
  MoveGen(type::gameState_t& gs);
  void setGameState(type::gameState_t& gs);

  // run all the psuedo move generators
  void runAllMoveGenerators();

  // generate moves for every piece type
  void generatePawnMoves();   // pawns
  void generateRookMoves();   // rooks
  void generateKnightMoves(); // knights
  void generateBishopMoves(); // bishops
  void generateQueenMoves();  // queens
  void generateKingMoves();   // kings


  //
  // Template functions
  //

  // extract all possible sub gameStates
  template <size_t N>
  uint16_t generateGameStates(std::array<type::gameState_t, N>& arr, const unsigned long start = 0, const unsigned long stop = N - 1)
  {
    if (stop >= N) {
      throw 0;
    }

    // #########################################################################
    uint16_t index_gameStates = 0;

    // generate all the bitboards
    this->generateRookMoves();
    this->generateKnightMoves();
    this->generateBishopMoves();
    this->generateQueenMoves();
    this->generateKingMoves();
    this->generatePawnMoves(); // Must be last due to promotions

    //generate gameStates based on moves
    const auto nrOfPieceTypes = movegen::moves.size();
    for (unsigned long pieceType = 0; pieceType < nrOfPieceTypes; pieceType++) {

      // for every pieceType
      for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {

        //make sure the king hasn't been captured.
        if ((this->state.piecesArr[::david::constant::index::king][1] & movegen::moves[pieceType][board]) > 0) {
          continue;
        }

        // create a child game state
        type::gameState_t gs = this->reversedState;
        gs.piecesArr[pieceType][1] = movegen::moves[pieceType][board];       // the colour that just moved. now opponent.

        // update moved piece
        gs.piecess[1] ^= this->state.piecesArr[pieceType][0]; // turn off all pieces
        gs.piecess[1] |= movegen::moves[pieceType][board]; // since the move contains the not moved pieces, + the newly moved one. just add it.


        // Check for capture, and destroy captured piece!
        if ((movegen::moves[pieceType][board] & this->state.piecess[1]) > 0) {
          const uint8_t attackedPiecePosition = ::utils::LSB(movegen::moves[pieceType][board] & this->state.piecess[1]);

          for (auto& bbArr : gs.piecesArr) {
            // since gs has the opposite indexes, use 0 in stead of 1.
            if (::utils::bitAt(bbArr[0], attackedPiecePosition)) {
              ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
              break;
            }
          }

          // update pieces
          ::utils::flipBitOff(gs.piecess[0], attackedPiecePosition);
        }

          // en passant capture.
        else if (pieceType == 0 && this->state.enPassant > 15 && ::utils::bitAt(movegen::moves[0][board], this->state.enPassant)) {
          ::utils::flipBitOff(gs.piecesArr[0][0], this->state.enPassantPawn);
          ::utils::flipBitOff(gs.piecess[0], this->state.enPassantPawn);
          gs.passant = true;
        }

          // identify a castling situation
          // TODO: verify that the new positions cannot be attacked!
        else if (pieceType == ::david::constant::index::king) {
          // TODO-castling1a: assumption, castling happens early, so i assume that a friendly rook,
          // TODO-castling1b: is not on the opposite side, vertically. From the castling rook
          // TODO-castling1c: OTHERWISE. this is a flaw

          // king side castling
          if (this->state.kingCastlings[0] && (gs.piecesArr[5][1] & 144115188075855874ULL) > 0) {
            uint8_t castlePos = ::utils::LSB(gs.piecesArr[::david::constant::index::rook][1] & 72057594037927937ULL);
            ::utils::flipBitOff(gs.piecesArr[::david::constant::index::rook][1], castlePos);

            ::utils::flipBitOn(gs.piecesArr[::david::constant::index::rook][1], castlePos + 2);
            gs.kingCastlings[1] = false;
            gs.queenCastlings[1] = false;

            ::utils::flipBitOff(gs.piecess[1], castlePos);
            ::utils::flipBitOn(gs.piecess[1], castlePos + 2);

#ifdef DAVID_TEST
            gs.castled = true;
#endif
          }

          else if (this->state.queenCastlings[0] && (gs.piecesArr[5][1] & 2305843009213693984ULL) > 0) {
            uint8_t castlePos = ::utils::LSB(gs.piecesArr[::david::constant::index::rook][1] & 9223372036854775936ULL);
            ::utils::flipBitOff(gs.piecesArr[::david::constant::index::rook][1], castlePos);

            ::utils::flipBitOn(gs.piecesArr[::david::constant::index::rook][1], castlePos - 3);
            gs.kingCastlings[1] = false;
            gs.queenCastlings[1] = false;

            ::utils::flipBitOff(gs.piecess[1], castlePos);
            ::utils::flipBitOn(gs.piecess[1], castlePos - 3);

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
        else if (pieceType == ::david::constant::index::rook) {
          // king side
          if (this->state.kingCastlings[0] && (gs.piecesArr[::david::constant::index::rook][1] & 72057594037927937ULL) == 0) {
            // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
            gs.kingCastlings[1] = false;
          }
            // queen side
          else if (this->state.queenCastlings[0] && (gs.piecesArr[::david::constant::index::rook][1] & 9223372036854775936ULL) == 0) {
            // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
            gs.queenCastlings[1] = false;
          }
        }

        // a piece was promoted, so remove the pawn that was sacrificed for this promotion
        if (pieceType > 0 && pieceType < 5 && (movegen::moves[pieceType][board] & this->state.piecesArr[0][0]) > 0) {
          const type::bitboard_t pawnBoard = movegen::moves[pieceType][board] & this->state.piecesArr[0][0];

          gs.piecesArr[0][1] ^= pawnBoard;
          gs.piecesArr[pieceType][1] ^= pawnBoard;

          // Deactivate the pawn position that gets promoted.
          gs.piecess[1] ^= pawnBoard;

#ifdef DAVID_TEST
          gs.promoted = true;
#endif
        }


        // complete board merge
        gs.combinedPieces = gs.piecess[0] | gs.piecess[1];


        // en passant record
        if (pieceType == ::david::constant::index::pawn && !gs.passant) {
          auto before = this->state.piecesArr[0][0];
          auto now = gs.piecesArr[0][1];
          const auto diff = (before ^ now);
          before &= diff;
          now &= diff;

          if ((before & 71776119061282560) > 0 && (now & 1099494850560) > 0) {
            // its en passant
            gs.enPassantPawn = ::utils::LSB(now);
            gs.enPassant = this->state.isWhite ? gs.enPassantPawn - 8 : gs.enPassantPawn + 8;
          }
        }



        // check?
        if (this->dangerousPosition(gs.piecesArr[::david::constant::index::king][1], gs, 0)) {
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
        if (this->dangerousPosition(gs.piecesArr[::david::constant::index::king][0], gs)) {
          gs.isInCheck = true;
        }
#endif
        // increment depth
        gs.depth += 1;

        // valid move, add it to the record.
        arr[start + index_gameStates++] = gs;
      }
    }

    // #########################################################################

    return index_gameStates;
  }

  // how many possible moves can be generated from this state?
  uint16_t nrOfPossibleMoves()
  {
    return 0;
  }

  type::gameState_t generateAttacks() {
    type::gameState_t gs = this->state;

    type::bitboard_t que = 0ULL;

    // pawns
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[0][j];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[0][j] |= this->generatePawnPaths(i, gs, j == 1);
      }
    }

    // knights
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::knight][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::knight][0] |= this->generateKnightAttack(i, j == 1);
      }
    }

    // rooks
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::rook][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::rook][0] |= this->generateRookAttack(i, gs, j == 1);
      }
    }

    // bishop
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::bishop][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::bishop][0] |= this->generateDiagonals(i, gs, j == 1);
      }
    }

    // queen
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::queen][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::queen][0] |= this->generateDiagonals(i, gs, j == 1) | this->generateRookAttack(i, gs, j == 1);
      }
    }

    // king
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::king][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::king][0] |= this->generateKingAttack(i, j == 1);
      }
    }

    // TODO: don't let pieces move into places where their king goes into check.

    return gs;
  }


 private:
#ifdef DAVID_TEST
  bool printedState{false};
  bool printMoves{false};
#endif

  // the current board status of the game
  type::gameState_t state;
  type::gameState_t reversedState;

  // xray attack path for king
  //type::bitboard_t xRayRookPaths;
  //type::bitboard_t xRayDiagonalPaths;

  // moves being generated from the state, for the active colour only.
  // 0 pawns
  // 1 rooks
  // 2 knight
  // 3 bishop
  // 4 queen
  // 5 king
  //std::array<std::array<type::bitboard_t, 30>, 6> moves = {{0}}; // initiator list?
  std::array<unsigned long, 6> index_moves = {{0}}; // index for every type

  // xray v0.1
  uint64_t hostileAttackPaths_queen   = 0ull - 1;
  uint64_t hostileAttackPaths_knight  = 0ull - 1;
  uint64_t hostileAttackPaths_pawn    = 0ull - 1;

  /**
   * Remove the parts of the Psuedo attack path that is illegal / invalid.
   * This only affects the path north of the piece.
   *
   * @param friendlyBlockers
   * @param northArea
   * @param psuedoPath
   * @return
   */
  inline type::bitboard_t extractLegalNorthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t northArea,
      const uint8_t offset = 7
  ) const {
    // find all path blockers
    type::bitboard_t northBlockers = (friendlyBlockers | (((psuedoPath & hostiles) & northArea) << offset)) & northArea; // & northArea);

    if (northBlockers > 0) {
      // find the area south of the closest blocker
      type::bitboard_t northBlockersArea = ::utils::indexToBitboard(::utils::LSB(northBlockers)) - 1;

      // get only legal moves (north of piece) that's before the first blocker.
      return psuedoPath & northBlockersArea;
    }
    else {
      return psuedoPath;
    }
  }

  inline uint64_t generateXRay_queen(const uint64_t board) const
  {
    return 0ull - 1;
  }

  inline type::bitboard_t extractLegalSouthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t southArea,
      const uint8_t offset = 7
  ) const {
    // find all southern blockers
    type::bitboard_t southBlockers = (friendlyBlockers | (((psuedoPath & hostiles) & southArea) >> offset)) & southArea;

    if (southBlockers > 0) {
      // get the area of illegal moves, starting from the first blocker downwards to index 0.
      type::bitboard_t southBlockersArea = ::utils::indexToBitboard(::utils::MSB(southBlockers) + 1) - 1;

      // get the legal path that's outside the illegal area.
      return psuedoPath & (~southBlockersArea);
    }
    else {
      return psuedoPath;
    }
  }

  template<uint8_t DIRECTION>
  inline type::bitboard_t extractLegalRange(
      const type::bitboard_t south,
      const type::bitboard_t north,
      const type::bitboard_t psuedo,
      const type::bitboard_t friendly,
      const type::bitboard_t hostiles
  ) const {
    using bb = type::bitboard_t;

    // south blockers
    bb southBlockers = south & (((psuedo & hostiles) >> DIRECTION) | friendly);

    // north blockers
    bb northBlockers = north & (((psuedo & hostiles) << DIRECTION) | friendly);

    // legal north area
    bb legalNorth = northBlockers > 0 ? (::utils::indexToBitboard(::utils::LSB(northBlockers & psuedo)) - 1) : 0ull - 1;

    // illegal south area
    bb illegalSouth = southBlockers > 0 ? (::utils::indexToBitboard(::utils::MSB(southBlockers & psuedo) + 1) - 1) : 0ull;

    // legal range
    return psuedo & (legalNorth ^ illegalSouth);
  }

  /**
   * Create promotions
   */
   inline void generatePromotions (const type::bitboard_t board, const type::bitboard_t pawn) {
    // add every option
    for (uint8_t pieceType = 1/*skip Pawn*/; pieceType < 5/*skip king*/; pieceType++) {
      // for every promotion, the pawn position is added to its board.
      // so u can just quickly check if any of the set bits overlap with any of the pawn board.
      movegen::moves[pieceType][this->index_moves[pieceType]++] = pawn | board | this->state.piecesArr[pieceType][0];
    }
  }

  /**
   * generate pawn moves for a given index
   *
   * On promotions, the promoted pieces are added directly to the move array
   */
  inline type::bitboard_t generatePawnPaths (const uint8_t index, const type::gameState_t& gs, const bool hostile = false) {
    const auto pieceBoard = ::utils::indexToBitboard(index);
    const auto hostiles = gs.piecess[hostile ? 0 : 1];
    //const auto friendly = gs.piecess[hostile ? 1 : 0];
    const auto allowedSquares = (~this->state.combinedPieces) ^ ::utils::indexToBitboard(index);
    const bool whiteMoving = gs.isWhite && !hostile;

    type::bitboard_t board = 0ULL;
    type::bitboard_t attacks = ::utils::constant::pawnAttackPaths[index];

    if (whiteMoving) {
      // move one and two squares up if possible
      board = ((pieceBoard << 8) | ((65280ULL & pieceBoard) << 16)) & (allowedSquares & (allowedSquares << 8));

      // attacks
      attacks &= (hostiles | ::utils::indexToBitboard(this->state.enPassant));
      board |= attacks & ~(pieceBoard - 1);
    }

    // black
    else {
      // move one and two squares up if possible
      board = ((pieceBoard >> 8) | ((71776119061217280ULL & pieceBoard) >> 16)) & (allowedSquares & (allowedSquares >> 8));

      // attacks
      attacks &= (hostiles | (this->state.enPassant > 0 ? ::utils::indexToBitboard(this->state.enPassant) : 0)); // hostile pieces that can be attacked.
      board |= attacks & (pieceBoard - 1);
    }

    // extract the promotions from the moves
    type::bitboard_t promotions = 18374686479671623935ULL & board;
    board ^= promotions;

    while (promotions != 0) {
      unsigned int i = ::utils::LSB(promotions);
      promotions = ::utils::flipBitOffCopy(promotions, i);

      this->generatePromotions(::utils::indexToBitboard(i), pieceBoard);
    }

    return board;
  }

  /**
   * Given a board with one set bit. Calculate if that position is in danger from an enemy attacks.
   *
   * @param board type::bitboard_t
   * @return bool
   * @slow
   */
   // TODO.. but this doesnt use the new move
  inline bool dangerousPosition(const type::bitboard_t board, const type::gameState_t& gs, const uint8_t hostile = 1) const {
    const uint8_t pos = ::utils::LSB(board);

    // knights
    if ((::utils::constant::knightAttackPaths[pos] & gs.piecesArr[::david::constant::index::knight][hostile]) > 0) {
      return true;
    }

    // pawns.
    type::bitboard_t pawnAttacks = ::utils::constant::pawnAttackPaths[pos];
    pawnAttacks &= gs.isWhite ? ~(board - 1) : board - 1;
    if ((pawnAttacks & gs.piecesArr[::david::constant::index::pawn][hostile]) > 0) {
      return true;
    }

    // bishop & diagonal queens
    const type::bitboard_t diagonals = this->generateDiagonals(pos, gs, hostile == 0);
    if (((diagonals & gs.piecesArr[::david::constant::index::bishop][hostile]) | (diagonals & gs.piecesArr[::david::constant::index::queen][hostile])) > 0) {
      // bug: generates a attack path that goes through a hostile piece.
      // in this case, king is at E8, a friendly pawn at G6 and a hostile queen at H5.
      // yet the path stretches beyond the pawn and into the queen, making this seem like
      // the queen has a open route to attack the king. This is obviusly false.
      // attack path generated: 1128103225065472

      return true;
    }

    // rook & vertical+horizontal queens
    const type::bitboard_t rookPaths = this->generateRookAttack(pos, gs, hostile == 0);
    if (((rookPaths & gs.piecesArr[::david::constant::index::rook][hostile]) | (rookPaths & gs.piecesArr[::david::constant::index::queen][hostile])) > 0) {
      return true;
    }

    // king
    if ((this->generateKingAttack(pos) & gs.piecesArr[::david::constant::index::king][hostile]) > 0) {
      return true;
    }

    // position seems to be safe
    return false;
  }

  /**
   * Use for only one piece at the time!
   * @param piece index
   * @param piece index
   * @return
   */
  inline type::bitboard_t generateDiagonals (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const {
    const auto friendly   = gs.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = gs.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = ::utils::constant::diagonalDUAttackPaths[index];

    r1 = this->extractLegalSouthPath(r1, friendly, hostiles, southArea & r1, 7);
    r1 = this->extractLegalNorthPath(r1, friendly, hostiles, northArea & r1, 7);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = ::utils::constant::diagonalUDAttackPaths[index];

    r2 = this->extractLegalSouthPath(r2, friendly, hostiles, southArea & r2, 9);
    r2 = this->extractLegalNorthPath(r2, friendly, hostiles, northArea & r2, 9);

    // combine
    return r1 | r2; // remove start position of piece
  }

  /**
   * Attack position for knights
   */
  inline uint64_t generateKnightAttack (const uint8_t index, const uint8_t friendly = 0) const
  {
    return (~this->state.piecess[friendly]) & ::utils::constant::knightAttackPaths[index];
  }

  /**
   * Generate King attack
   * @param index
   * @param hostilePath
   * @return
   */
  inline uint64_t generateKingAttack (const uint8_t index, const bool hostilePath = false) const
  {
    // <---
    uint8_t maskRowOffset = 3; // offset from row 0
    const std::array<type::bitboard_t, 8> masks = {
        12918652928,
        30149115904,
        60298231808,
        120596463616,
        241192927232,
        482385854464,
        964771708928,
        825720045568
    };

    uint8_t row = index / 8;
    type::bitboard_t mask = masks[index % 8];
    type::bitboard_t rMask = 0ULL;

    if (row > maskRowOffset) {
      rMask = mask << (8 * (row - maskRowOffset));
    }
    else {
      rMask = mask >> (8 * (maskRowOffset - row));
    }

    // legal moves, not checked if in check mate or smth
    return (rMask & this->state.piecess[hostilePath ? 1 : 0]) ^ rMask;
  }

  inline uint64_t generateRookAttack (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const
  {
//    using bb = uint64_t;
//
//    const auto friendly = gs.piecess[hostilePath ? 1 : 0];
//    const auto hostiles = gs.piecess[hostilePath ? 0 : 1];
//    const auto iBoard   = ::utils::indexToBitboard(index);
//
//    // attack path starting from bottom going upwards, reading left to right
//    bb r1 = this->template extractLegalRange<8>(iBoard - 1, (~iBoard) ^ (iBoard - 1), ::utils::constant::verticalAttackPaths[index], friendly, hostiles);
//
//    // attack path starting from bottom going upwards, reading left to right
//    bb r2 = this->template extractLegalRange<1>(iBoard - 1, (~iBoard) ^ (iBoard - 1), ::utils::constant::horizontalAttackPaths[index], friendly, hostiles);
//
    const auto friendly   = gs.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = gs.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = ::utils::constant::verticalAttackPaths[index];
    r1 = this->extractLegalSouthPath(r1, friendly, hostiles, southArea & r1, 8);
    r1 = this->extractLegalNorthPath(r1, friendly, hostiles, northArea & r1, 8);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = ::utils::constant::horizontalAttackPaths[index];
    r2 = this->extractLegalSouthPath(r2, friendly, hostiles, southArea & r2, 1);
    r2 = this->extractLegalNorthPath(r2, friendly, hostiles, northArea & r2, 1);

    // combine
    return r1 | r2;
  }


};


}
