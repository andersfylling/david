#pragma once

#include "david/david.h"
#include "david/types.h"
#include "david/bitboard.h"
#include "david/MoveGenTest.h"
#include <assert.h>

namespace david {

class MoveGen {
  friend class MoveGenTest;
 public:
  MoveGen(const type::gameState_t& gs);

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
  uint16_t generateGameStates(std::array<type::gameState_t, N>& arr, unsigned long start = 0, unsigned long stop = N - 1)
  {
    if (stop >= N) {
      std::__throw_runtime_error("stop param is larger than array size");
    }

    this->runAllMoveGenerators();

    uint16_t nrOfMoves = 0; // use nrOfMoves++ for everyone added. at end, it represents length.

    for (; nrOfMoves < this->index_gameStates && nrOfMoves < (stop - start); nrOfMoves++) {
      arr[start + nrOfMoves] = this->gameStates[nrOfMoves];
    }


    return nrOfMoves;
  }

  // how many possible moves can be generated from this state?
  uint16_t nrOfPossibleMoves()
  {
    this->runAllMoveGenerators();

    return static_cast<uint16_t>(this->index_gameStates);
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
      que = gs.piecesArr[gs.iKnights][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[gs.iKnights][0] |= this->generateKnightAttack(i, j == 1);
      }
    }

    // rooks
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[gs.iRooks][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[gs.iRooks][0] |= this->generateRookAttack(i, gs, j == 1);
      }
    }

    // bishop
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[gs.iBishops][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[gs.iBishops][0] |= this->generateDiagonals(i, gs, j == 1);
      }
    }

    // queen
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[gs.iQueens][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[gs.iQueens][0] |= this->generateDiagonals(i, gs, j == 1) | this->generateRookAttack(i, gs, j == 1);
      }
    }

    // king
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[gs.iKings][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[gs.iKings][0] |= this->generateKingAttack(i, j == 1);
      }
    }

    // TODO: don't let pieces move into places where their king goes into check.

    return gs;
  }


 private:
  // the current board status of the game
  const type::gameState_t& state;
  type::gameState_t reversedState;

  // xray attack path for king
  //type::bitboard_t xRayRookPaths;
  //type::bitboard_t xRayDiagonalPaths;

  // TODO: reduce memory usage
  std::array<std::array<type::bitboard_t, 256>, 6> promotedPawns = {{0}};

  // TODO: reduce memory size.
  // knight attacks
  //const ::std::array<type::bitboard_t, 64> knightAttacks = ::utils::compileKnightAttacks();

  //const ::std::array<type::bitboard_t, 64> rookAttacks = ::utils::compileRookAttacks();

  // horixontal attack paths
  //const ::std::array<type::bitboard_t, 64> horizontalAttacks = ::utils::compileHorizontalAttacks();

  // Vertical attack paths
  //const ::std::array<type::bitboard_t, 64> verticalAttacks = ::utils::compileVerticalAttacks();

  // moves being generated from the state, for the active colour only.
  // 0 pawns
  // 1 rooks
  // 2 knight
  // 3 bishop
  // 4 queen
  // 5 king
  std::array<std::array<type::bitboard_t, 256>, 6> moves = {{0}}; // initiator list?
  std::array<unsigned long, 6> index_moves = {{0}}; // index for every type

  std::array<type::gameState_t, ::david::constant::MAXMOVES> gameStates;
  unsigned long index_gameStates;

  type::bitboard_t hostileAttackPaths;

  // attack paths for pawns
//  inline type::bitboard_t generatePawnAttack(const type::gameState_t& gs) const {
//    using type::bitboard_t;
//
//    bitboard_t attacks = 0ULL;
//    bitboard_t pawns = gs.piecesArr[gs.iPawns][1];
//
//    if (!this->state.isWhite) {
//      // pawn attack for white, up right
//      const bitboard_t whitePawnsRightAttackFilter = 282578800148737;
//      bitboard_t r1 = (pawns ^ (pawns & whitePawnsRightAttackFilter)) << 7; // attack up and right
//      r1 ^= pawns & r1;
//      // filter on &= hostiles
//      // loop
//
//      // pawn attack for white, up left
//      const bitboard_t whitePawnsLeftAttackFilter = 36170086419038336;
//      bitboard_t r2 = (pawns ^ (pawns & whitePawnsLeftAttackFilter)) << 9;
//      r2 ^= pawns & r2;
//
//      attacks = r1 | r2;
//    }
//    else {
//      const bitboard_t blackPawnsRightAttackFilter = 72340172838076672;
//      bitboard_t r1 = (pawns ^ (pawns & blackPawnsRightAttackFilter)) >> 9;
//      r1 ^= pawns & r1;
//
//      attacks = r1;
//    }
//
//    return attacks;
//  }

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
    type::bitboard_t northBlockers = (friendlyBlockers | ((hostiles & northArea) << offset)) & northArea; // & northArea);

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

  //TODO sometimes, when close to a enemy. neighbour square (north). the path ud doesnt get added..
  inline type::bitboard_t extractLegalSouthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t southArea,
      const uint8_t offset = 7
  ) const {
    // find all southern blockers
    type::bitboard_t southBlockers = (friendlyBlockers | ((hostiles & southArea) >> offset)) & southArea;

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

  /**
   * Create promotions
   */
   inline void generatePromotions (const type::bitboard_t board, const type::bitboard_t pawn) {
    // add every option
    for (uint8_t pieceType = 1/*skip Pawn*/; pieceType < 5/*skip king*/; pieceType++) {
      // since promotions removes a pawn with a promoted piece
      // it's needed to store which pawn should be removed for a given
      // move/promotion!
      this->promotedPawns[pieceType][this->index_moves[pieceType]] = pawn;
      this->moves[pieceType][this->index_moves[pieceType]++] = board | this->state.piecesArr[pieceType][0];
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
    const auto friendly = gs.piecess[hostile ? 1 : 0];
    const bool whiteMoving = gs.isWhite && !hostile;

    type::bitboard_t board = 0ULL;
    type::bitboard_t promotions = 0ULL;



    if (whiteMoving) {
      // shift upwards
      board |= pieceBoard << 8;

      // double shift
      if ((65280 & pieceBoard) > 0 && (this->state.combinedPieces & (pieceBoard << 8)) == 0) {
        board |= pieceBoard << 16;
      }

      // remove any blocking pieces
      board &= ~this->state.combinedPieces;

      // attacks
      auto attacks = ::utils::constant::pawnAttackPaths[index];
      attacks &= (hostiles | ::utils::indexToBitboard(this->state.enPassant));
      board |= attacks & ~(pieceBoard - 1);

      // promotions
      promotions = 18374686479671623680 & board;
      // remove pawns that will turn into other pieces
      board ^= promotions;
    }

    // black
    else {
      // shift downwards
      board |= pieceBoard >> 8;

      // double shift
      if ((71776119061217280 & pieceBoard) > 0 && (this->state.combinedPieces & (pieceBoard >> 8)) == 0) {
        board |= pieceBoard >> 16;
      }

      // remove any blocking pieces
      board &= ~this->state.combinedPieces;

      // attacks
      auto attacks = ::utils::constant::pawnAttackPaths[index];
      attacks &= (hostiles | ::utils::indexToBitboard(this->state.enPassant)); // hostile pieces that can be attacked.
      board |= attacks & (pieceBoard - 1);

      // promotions
      promotions = 255 & board;
      // remove pawns that will turn into other pieces
      board ^= promotions;
    }

    while (promotions != 0) {
      auto i = ::utils::LSB(promotions);
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
    if ((::utils::constant::knightAttackPaths[pos] & gs.piecesArr[gs.iKnights][hostile]) > 0) {
      return true;
    }

    // pawns. TODO: wtf? need to be related to direction of active pieces.
     type::bitboard_t pawnAttacks = ::utils::constant::pawnAttackPaths[pos];
     pawnAttacks &= gs.isWhite ? ~(board - 1) : board - 1;
    if ((pawnAttacks & gs.piecesArr[gs.iPawns][hostile]) > 0) {
      return true;
    }

    // bishop & diagonal queens
     //TODO: must use data from the new game state...
     // right now this only works if its a king that has moved..
    const type::bitboard_t diagonals = this->generateDiagonals(pos, gs, hostile == 0);
    if (((diagonals & gs.piecesArr[gs.iBishops][hostile]) | (diagonals & gs.piecesArr[gs.iQueens][hostile])) > 0) {
      // bug: generates a attack path that goes through a hostile piece.
      // in this case, king is at E8, a friendly pawn at G6 and a hostile queen at H5.
      // yet the path stretches beyond the pawn and into the queen, making this seem like
      // the queen has a open route to attack the king. This is obviusly false.
      // attack path generated: 1128103225065472

      //std::cout << diagonals << std::endl;
      return true;
    }

    // rook & vertical+horizontal queens
    const type::bitboard_t rookPaths = this->generateRookAttack(pos, gs, hostile == 0);
    if (((rookPaths & gs.piecesArr[gs.iRooks][hostile]) | (rookPaths & gs.piecesArr[gs.iQueens][hostile])) > 0) {
      return true;
    }

    // king
    if ((this->generateKingAttack(pos) & gs.piecesArr[gs.iKings][hostile]) > 0) {
      return true;
    }

    // position seems to be safe
    return false;
  }

  // TODO: if this->state is in check, then only some pieces can move.
  // TODO, does this generate paths that the king can actually be attacked from? NOPE
  inline type::bitboard_t generateRookXRayPaths (const type::bitboard_t pBoard, const uint8_t friendly = 0, const uint8_t hostile = 1) const {
    // pass through friendlies, but only the first friendly.
    const auto friendlies = this->state.piecess[friendly];
    const uint8_t index = ::utils::LSB(pBoard);
    const uint8_t iRow = index / 8;
    const uint8_t iCol = index % 8;
    const type::bitboard_t hostiles = (this->state.piecesArr[this->state.iQueens][hostile] | this->state.piecesArr[this->state.iRooks][hostile]);

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;

    uint8_t mult = 1;


    // #####
    // Vertical attacks
    //

    // upwards, TODO: slow..
    for (uint8_t i = iRow + 1; i < 8; i++, mult++) {
      type::bitboard_t board = pBoard << (8 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;

    // downwards, TODO: slow..
    for (uint8_t i = iRow - 1; i > 0; i--, mult++) {
      type::bitboard_t board = pBoard >> (8 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // #####
    // Horizontal attacks
    //

    // left, TODO: slow..
    for (uint8_t i = iCol + 1; i < 8; i++, mult++) {
      type::bitboard_t board = pBoard << (1 * mult); // left

      // is it on the same row?
      if (::utils::LSB(board) != iRow) {
        break;
      }

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // right, TODO: slow..
    for (uint8_t i = iCol - 1; i > 0; i--, mult++) {
      type::bitboard_t board = pBoard >> (1 * mult);

      // is it on the same row?
      if (::utils::LSB(board) != iRow) {
        break;
      }

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    return result;
  } // generateRookXRayPaths

  inline type::bitboard_t generateDiagonalXRayPaths (const type::bitboard_t pBoard, const uint8_t friendly = 0, const uint8_t hostile = 1) const {
    // pass through friendlies, but only the first friendly.
    const auto friendlies = this->state.piecess[friendly];
    const uint8_t index = ::utils::LSB(pBoard);
    const uint8_t iRow = index / 8;
    const uint8_t iCol = index % 8;
    const type::bitboard_t hostiles = (this->state.piecesArr[this->state.iQueens][hostile] | this->state.piecesArr[this->state.iBishops][hostile]);

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;

    uint8_t mult = 1;

    // ######
    // Diagonal attacks, up right
    //

    // up and right
    for (uint8_t i = 0; i < 8 && (tmp & 9259542123273814271) == 0; i++, mult++) {
      type::bitboard_t board = pBoard << (7 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // up and left
    for (uint8_t i = 0; i < 8 && (tmp & 72340172838076927) == 0; i++, mult++) {
      type::bitboard_t board = pBoard << (9 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }


    // down and right
    for (uint8_t i = 0; i < 8 && (tmp & 18410856566090662016) == 0; i++, mult++) {
      type::bitboard_t board = pBoard >> (9 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }


    // down and left
    for (uint8_t i = 0; i < 8 && (tmp & 18374969058471772417) == 0; i++, mult++) {
      type::bitboard_t board = pBoard >> (7 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    return result;
  } // generateDiagonalXRayPaths

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
  inline uint64_t generateKnightAttack (const uint8_t index, const uint8_t friendly = 0) const {
    const type::bitboard_t friendlies = this->state.piecess[friendly];

    return (~friendlies) & ::utils::constant::knightAttackPaths[index];
  }

  /**
   * Generate King attack
   * @param index
   * @param hostilePath
   * @return
   */
  inline uint64_t generateKingAttack (const uint8_t index, const bool hostilePath = false) const {
    const type::bitboard_t friendly = this->state.piecess[hostilePath ? 1 : 0];

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
    return (rMask & friendly) ^ rMask;
  }

  inline uint64_t generateRookAttack (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const {
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
    return r1 | r2; // remove start position of piece
  }


};


}
