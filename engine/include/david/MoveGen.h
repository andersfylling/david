#pragma once

#include "david/david.h"
#include "david/types.h"
#include "david/bitboard.h"

namespace david {



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

  void test_knightAttackPaths () {

    // print a few knight boards for debugging
    for (int i = 0; i < 64; i++) {
      ::utils::printBoard(this->knightAttacks[i], i);
    }
  }

  void test_rookAttackPaths () {

    // print a few knight boards for debugging
    for (int i = 0; i < 64; i++) {
      ::utils::printBoard(this->generateRookAttack(i), i);
    }
  }

  void test_queenAttackPaths () {


    // print a few knight boards for debugging
    for (int i = 0; i < 64; i++) {

      auto a = this->generateDiagonals(i);
      a |= this->generateRookAttack(i);

      ::utils::printBoard(a, i);
    }
  }

  void test_bishopAttackPaths () {


    // print a few knight boards for debugging
    for (int i = 0; i < 64; i++) {
      auto a = this->generateDiagonals(i);
      ::utils::printBoard(a, i);
    }
  }

 private:
  // the current board status of the game
  const type::gameState_t& state;

  // TODO: reduce memory size.
  // knight attacks
  const ::std::array<type::bitboard_t, 64> knightAttacks = ::utils::compileKnightAttacks();

  const ::std::array<type::bitboard_t, 64> rookAttacks = ::utils::compileRookAttacks();

  // horixontal attack paths
  const ::std::array<type::bitboard_t, 64> horizontalAttacks = ::utils::compileHorizontalAttacks();

  // Vertical attack paths
  const ::std::array<type::bitboard_t, 64> verticalAttacks = ::utils::compileVerticalAttacks();

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
    // find all path blockers
    type::bitboard_t northBlockers = (friendlyBlockers & northArea) | ((hostiles << offset) & northArea);

    // find the area south of the closest blocker
    type::bitboard_t northBlockersArea = (::utils::indexToBitboard(::utils::LSB(northBlockers))) - 1;

    // get only legal moves (north of piece) that's before the first blocker.
    return psuedoPath & northBlockersArea;
  }

  //TODO sometimes, when close to a enemy. neighbour swuare (north). the path ud doesnt get added..
  inline type::bitboard_t extractLegalSouthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t southArea,
      const uint8_t offset = 7
  ) const {
    // find all southern blockers
    type::bitboard_t southBlockers = (friendlyBlockers & southArea) | ((hostiles >> offset) & southArea);

    // get the area of illegal moves, starting from the first blocker downwards to index 0.
    type::bitboard_t southBlockersArea = (::utils::indexToBitboard(::utils::MSB(southBlockers)) - 1) * 2 + 1;

    // get the legal path that's outside the illegal area.
    return psuedoPath & (~southBlockersArea);
  }

  /**
   * Use for only one piece at the time!
   * @param piece index
   * @param piece index
   * @return
   */
  inline type::bitboard_t generateDiagonals (const uint8_t index, const bool hostilePath = false) const {
    // ->, down to up
    const std::array<type::bitboard_t, 15> duPaths = {
        0,
        4647714815446351872,
        2323998145211531264,
        1161999622361579520,
        580999813328273408,
        290499906672525312,
        145249953336295424,
        72624976668147840, // middle
        283691315109952,
        1108169199648,
        4328785936,
        16909320,
        66052,
        258,
        0
    };

    // ->, up to down
    const std::array<type::bitboard_t, 15> udPaths = {
        9241421688590303745, // middle diagonal
        36099303471055874,
        141012904183812,
        550831656968,
        2151686160,
        8405024,
        32832,
        0,
        4620710844295151872,
        2310355422147575808,
        1155177711073755136,
        577588855528488960,
        288794425616760832,
        144396663052566528,
        0
    };

    const std::array<uint8_t, 64> du = {
        14, 13, 12, 11, 10,  9,  8,  7,
        13, 12, 11, 10,  9,  8,  7,  6,
        12, 11, 10,  9,  8,  7,  6,  5,
        11, 10,  9,  8,  7,  6,  5,  4,
        10,  9,  8,  7,  6,  5,  4,  3,
         9,  8,  7,  6,  5,  4,  3,  2,
         8,  7,  6,  5,  4,  3,  2,  1,
         7,  6,  5,  4,  3,  2,  1,  0
    };

    const std::array<uint8_t, 64> ud = {
         0,  1,  2,  3,  4,  5,  6,  7,
         8,  0,  1,  2,  3,  4,  5,  6,
         9,  8,  0,  1,  2,  3,  4,  5,
         10, 9,  8,  0,  1,  2,  3,  4,
         11, 10, 9,  8,  0,  1,  2,  3,
         12, 11, 10, 9,  8,  0,  1,  2,
         13, 12, 11, 10, 9,  8,  0,  1,
         14, 13, 12, 11, 10, 9,  8,  0,
    };

    const auto friendly   = this->state.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = this->state.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = ~southArea;

    type::bitboard_t blockers = 0ULL;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = iBoard ^ duPaths[du[index]];
    blockers = r1 & friendly;

    r1 = this->extractLegalSouthPath(r1, blockers, hostiles, southArea >> 6);
    r1 = this->extractLegalNorthPath(r1, blockers, hostiles, northArea);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = iBoard ^ udPaths[ud[index]];
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

    return ~friendly & this->knightAttacks[index];
  }

  inline uint64_t generateRookAttack (const uint8_t index, const bool hostilePath = false) const {
    const auto friendly   = this->state.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = this->state.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = ~southArea;

    type::bitboard_t blockers = 0ULL;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = iBoard ^ this->verticalAttacks[index];
    blockers = r1 & friendly;

    r1 = this->extractLegalSouthPath(r1, blockers, hostiles, southArea & r1, 8);
    r1 = this->extractLegalNorthPath(r1, blockers, hostiles, northArea & r1, 8);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = iBoard ^ this->horizontalAttacks[index];
    blockers = r2 & friendly;

    r2 = this->extractLegalSouthPath(r2, blockers, hostiles, southArea, 1);
    r2 = this->extractLegalNorthPath(r2, blockers, hostiles, northArea, 1);

    // combine
    return r1 | r2; // remove start position of piece
  }


};

}
