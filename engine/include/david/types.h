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



}
}