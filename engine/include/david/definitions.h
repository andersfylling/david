#pragma once

// system dependencies
#include <iostream>
#include <memory>

// Forward declaration.
// This file should not need to contain any header files to declare the type.
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
namespace definitions {
typedef std::shared_ptr<::david::EngineContext>       engineContext_ptr;
typedef std::shared_ptr<::david::ChessEngine>         engine_ptr;
typedef std::shared_ptr<::david::Search>              search_ptr;
typedef std::shared_ptr<::david::ANN>                 neuralNetwork_ptr;
typedef std::shared_ptr<::david::gameTree::GameTree>  gameTree_ptr;
typedef std::shared_ptr<::david::bitboard::gameState> gameState_ptr;

}
}