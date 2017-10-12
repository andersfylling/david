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
class TreeGen;
}

} // ::david


// all dem typedefs
namespace david {
namespace type {

//
// Typedefs
//

// Used by classes to communicate with other instances not directly related except on a parent level
typedef ::david::EngineContext            EngineContext_t;

// a pointer to an chess engine instance
typedef ::david::ChessEngine              Engine_t;

// a pointer to the search class
typedef ::david::Search                   Search_t;

// a pointer to a ANN instance
typedef ::david::ANN                      NeuralNetwork_t;

// a gameTree is the node tree which the Search class search through
typedef ::david::gameTree::TreeGen        TreeGen_t;

// holds the uci protocol for listening to incoming commands
typedef ::uci::Listener                   UciProtocol_t;

// gameState is a node in the gameTree. Each one contains a board score and some other details.
typedef ::david::bitboard::gameState      gameState_t;

typedef uint64_t bitboard_t;  // Represents a bitboard_t
typedef uint16_t move_t;      // Representing moves
}
}