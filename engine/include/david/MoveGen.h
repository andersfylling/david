#pragma once

#include "david/david.h"
#include "david/types.h"
#include "david/bitboard.h"
#include <assert.h>

namespace david {

#ifdef MOVEGEN

class MoveGen {
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

    for (; nrOfMoves < this->index_gameStates && nrOfMoves < stop - start; nrOfMoves++) {
      arr[start + nrOfMoves] = this->gameStates[nrOfMoves];
    }


    return nrOfMoves;
  }

  uint64_t test_diagonalAttackPaths (const uint64_t board) {
    return this->generateDiagonals(::utils::LSB(board));
  }

  uint64_t test_knightAttackPaths (const uint64_t board) {
    return this->generateKnightAttack(::utils::LSB(board));
  }


 private:
  // the current board status of the game
  const type::gameState_t& state;

  // xray attack path for king
  type::bitboard_t xRayRookPaths;
  type::bitboard_t xRayDiagonalPaths;

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
  inline type::bitboard_t generatePawnAttack(const type::gameState_t& gs) {
    using type::bitboard_t;

    bitboard_t attacks = 0ULL;
    bitboard_t pawns = gs.piecesArr[gs.iPawns][1];

    if (!this->state.isWhite) {
      // pawn attack for white, up right
      const bitboard_t whitePawnsRightAttackFilter = 282578800148737;
      bitboard_t r1 = (pawns ^ (pawns & whitePawnsRightAttackFilter)) << 7; // attack up and right
      r1 ^= pawns & r1;
      // filter on &= hostiles
      // loop

      // pawn attack for white, up left
      const bitboard_t whitePawnsLeftAttackFilter = 36170086419038336;
      bitboard_t r2 = (pawns ^ (pawns & whitePawnsLeftAttackFilter)) << 9;
      r2 ^= pawns & r2;

      attacks = r1 | r2;
    }
    else {
      const bitboard_t blackPawnsRightAttackFilter = 72340172838076672;
      bitboard_t r1 = (pawns ^ (pawns & blackPawnsRightAttackFilter)) >> 9;
      r1 ^= pawns & r1;



      attacks = r1;
    }

    return attacks;
  }

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
    type::bitboard_t interest = psuedoPath & northArea;
    // find all path blockers
    type::bitboard_t northBlockers = (friendlyBlockers & interest) | (((hostiles & interest) << offset)); // & northArea);

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
    type::bitboard_t interest = psuedoPath & southArea;
    // find all southern blockers
    type::bitboard_t southBlockers = (friendlyBlockers & interest) | ((hostiles & interest) >> offset);

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
    // promotion
    if ((18374686479671623935 & board) > 0)  {
      // add every option
      for (uint8_t pieceType = 1/*skip Pawn*/; pieceType < 6; pieceType++) {
        // since promotions removes a pawn with a promoted piece
        // it's needed to store which pawn should be removed for a given
        // move/promotion!
        this->promotedPawns[pieceType][this->index_moves[pieceType]] = pawn;
        this->moves[pieceType][this->index_moves[pieceType]++] = board | this->state.piecesArr[pieceType][0];
      }
    }
  }

  /**
   * generate pawn moves for a given index
   */
  inline type::bitboard_t generatePawnPaths (const uint8_t index) {
    const auto pieceBoard = ::utils::indexToBitboard(index);

    type::bitboard_t board = 0ULL;

    // shift upwards
    board |= pieceBoard << 8;

    // shift downwards
    board |= pieceBoard >> 8;

    // if it's at an double square start position, shift 2 times....... -_-
    if ((71776119061282560 & pieceBoard) > 0) {
      // shift upwards
      if ((this->state.combinedPieces & (pieceBoard << 8)) == 0) {
        board |= pieceBoard << 16;
      }

      // shift downwards
      if ((this->state.combinedPieces & (pieceBoard >> 8)) == 0) {
        board |= pieceBoard >> 16;
      }
    }

    // remove any blocking pieces
    board &= ~this->state.combinedPieces;

    // attacks
    auto attacks = ::utils::constant::pawnAttackPaths[index];
    attacks &= (this->state.piecess[1] | ::utils::indexToBitboard(this->state.enPassant));
    board |= attacks;


    // remove illegal direction / move path
    if (this->state.isWhite) {
      board &= ~(pieceBoard - 1);
    }
    else {
      board &= pieceBoard - 1;
    }


    auto promotions = 18374686479671623935 & board;
    board ^= promotions; // remove promotions as those are no longer pawns

    auto i = ::utils::LSB(promotions);
    while (promotions != 0 || i != 0) {
      this->generatePromotions(board, pieceBoard);

      promotions = ::utils::flipBitOffCopy(promotions, i);
      i = ::utils::LSB(promotions);
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

    // pawns
    type::bitboard_t pawns = 0ULL;
    if (!gs.isWhite) {
      // but it's not int128..
      pawns = ((board & 18374403900871474688) >> 9) | ((board & 9187201950435737344) >> 7);
    }
    else {
      pawns = ((board & 35887507618889599) << 9) | ((board & 71775015237779198) << 7);
    }
    if ((pawns & gs.piecesArr[gs.iPawns][hostile]) > 0) {
      return true;
    }

    // bishop & diagonal queens
    const type::bitboard_t diagonals = this->generateDiagonals(pos);
    if (((diagonals & gs.piecesArr[gs.iBishops][1]) | (diagonals & gs.piecesArr[gs.iQueens][hostile])) > 0) {
      return true;
    }

    // rook & vertical+horizontal queens
    const type::bitboard_t rookPaths = this->generateRookAttack(pos);
    if (((rookPaths & gs.piecesArr[gs.iRooks][hostile]) | (rookPaths & gs.piecesArr[gs.iQueens][hostile])) > 0) {
      return true;
    }

    // king
    if ((this->generateKingAttack(pos) & gs.piecesArr[gs.iKings][hostile]) > 0) {
      return true;
    }

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

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;


    // #####
    // Vertical attacks
    //

    // upwards, TODO: slow..
    for (uint8_t i = iRow + 1; i < 8; i++) {
      type::bitboard_t board = pBoard << 8;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iRooks][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // downwards, TODO: slow..
    for (uint8_t i = iRow - 1; i > 0; i--) {
      type::bitboard_t board = pBoard >> 8;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iRooks][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // #####
    // Horizontal attacks
    //

    // left, TODO: slow..
    for (uint8_t i = iCol + 1; i < 8; i++) {
      type::bitboard_t board = pBoard << 1; // left

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iRooks][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // right, TODO: slow..
    for (uint8_t i = iCol - 1; i > 0; i--) {
      type::bitboard_t board = pBoard >> 1;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iRooks][hostile] & tmp) > 0) {
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

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;

    // ######
    // Diagonal attacks, up right
    //

    // up and right
    for (uint8_t i = 0; i < 8 && (tmp & 18411139144890810879) == 0; i++) {
      type::bitboard_t board = pBoard << 7;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iBishops][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // up and left
    for (uint8_t i = 0; i < 8 && (tmp & 18411139144890810879) == 0; i++) {
      type::bitboard_t board = pBoard << 9;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iBishops][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // down and right
    for (uint8_t i = 0; i < 8 && (tmp & 18411139144890810879) == 0; i++) {
      type::bitboard_t board = pBoard >> 9;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iBishops][hostile] & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // down and left
    for (uint8_t i = 0; i < 8 && (tmp & 18411139144890810879) == 0; i++) {
      type::bitboard_t board = pBoard >> 7;

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

    // check that there are hostile queen and rooks in this path
    if ((this->state.piecesArr[this->state.iQueens][hostile] & tmp) > 0 || (this->state.piecesArr[this->state.iBishops][hostile] & tmp) > 0) {
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
  inline type::bitboard_t generateDiagonals (const uint8_t index, const bool hostilePath = false) const {
    const auto friendly   = this->state.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = this->state.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    type::bitboard_t blockers = 0ULL;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = iBoard ^ ::utils::constant::duAttackPaths[14 - (index % 8) - (index / 8)];
    blockers = r1 & friendly;

    r1 = this->extractLegalSouthPath(r1, blockers, hostiles, southArea >> 6);
    r1 = this->extractLegalNorthPath(r1, blockers, hostiles, northArea);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = iBoard ^ ::utils::constant::udAttackPaths[7 - (index % 8) + (index / 8)];
    blockers = r2 & friendly;

    r2 = this->extractLegalSouthPath(r2, blockers, hostiles, southArea >> 8, 9);
    r2 = this->extractLegalNorthPath(r2, blockers, hostiles, northArea, 9);

    // combine
    return r1 | r2; // remove start position of piece
  }

  /**
   * Attack position for knights
   */
  inline uint64_t generateKnightAttack (const uint8_t index, const bool hostilePath = false) const {
    const type::bitboard_t friendly = this->state.piecess[hostilePath ? 1 : 0];

    return ~friendly & ::utils::constant::knightAttackPaths[index];
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

  inline uint64_t generateRookAttack (const uint8_t index, const bool hostilePath = false) const {
    const auto friendly   = this->state.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = this->state.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    type::bitboard_t blockers = 0ULL;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = iBoard ^ ::utils::constant::verticalAttackPaths[index];
    blockers = r1 & friendly;

    r1 = this->extractLegalSouthPath(r1, blockers, hostiles, southArea & r1, 8);
    r1 = this->extractLegalNorthPath(r1, blockers, hostiles, northArea & r1, 8);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = iBoard ^ ::utils::constant::horizontalAttackPaths[index];
    blockers = r2 & friendly;

    r2 = this->extractLegalSouthPath(r2, blockers, hostiles, southArea, 1);
    r2 = this->extractLegalNorthPath(r2, blockers, hostiles, northArea, 1);

    // combine
    return r1 | r2; // remove start position of piece
  }


};

#endif

}
