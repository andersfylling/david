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

 private:
  // the current board status of the game
  const type::gameState_t& state;

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




};

}
