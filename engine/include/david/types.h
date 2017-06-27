#pragma once

// system dependencies
#include <iostream>
#include <memory>

// Forward declaration.
// This file should not need to contain any header files to declare the type.

#include <uci/forwards/Listener.h>

namespace david {
class EngineContext;
class ChessEngine;
class ANN;
class Search;

namespace bitboard {
struct gameState;
}

namespace gameTree {
class GameTree;
}
}


// all dem typedefs
namespace david {
namespace type {

//
// Typedefs
//

// Used by classes to communicate with other instances not directly related except on a parent level
typedef std::shared_ptr<::david::EngineContext>       engineContext_ptr;

// a pointer to an chess engine instance
typedef std::shared_ptr<::david::ChessEngine>         engine_ptr;

// a pointer to the search class
typedef std::shared_ptr<::david::Search>              search_ptr;

// a pointer to a ANN instance
typedef std::shared_ptr<::david::ANN>                 neuralNetwork_ptr;

// a gameTree is the node tree which the Search class search through
typedef std::shared_ptr<::david::gameTree::GameTree>  gameTree_ptr;

// holds the uci protocol for listening to incoming commands
typedef std::shared_ptr<::uci::Listener>              uciProtocol_ptr;

// gameState is a node in the gameTree. Each one contains a board score and some other details.
typedef std::shared_ptr<::david::bitboard::gameState> gameState_ptr;

typedef uint64_t bitboard_t;  // Represents a bitboard_t
typedef uint16_t move_t;      // Representing moves


//
// Types
//


// Each game state is represented by a struct of
// bitboards. A tree of moves will be made up by
struct gameState {
  bitboard_t WhitePawn;
  bitboard_t WhiteRook;
  bitboard_t WhiteKnight;
  bitboard_t WhiteBishop;
  bitboard_t WhiteQueen;
  bitboard_t WhiteKing;

  bitboard_t BlackPawn;
  bitboard_t BlackRook;
  bitboard_t BlackKnight;
  bitboard_t BlackBishop;
  bitboard_t BlackQueen;
  bitboard_t BlackKing;

  bitboard::COLOR playerColor = bitboard::COLOR::WHITE; // Whose turn it is now
  std::vector<std::shared_ptr<bitboard::gameState>> children;

  int halfMoves = 0; // number of moves since last capture or pawn moves, otherwise incremented.
  int fullMoves = 1; // starts at 1, increments after every time black moves.

  int score = 0; // board score

  int gameTreeLevel = 0;

  int possibleSubMoves = 0;

  bool blackQueenCastling = true;
  bool blackKingCastling = true;
  bool whiteQueenCastling = true;
  bool whiteKingCastling = true;

  std::weak_ptr<bitboard::gameState> weakParent; //...
};

}
}