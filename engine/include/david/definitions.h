#pragma once

// system dependencies
#include <iostream>
#include <memory>

// Forward declaration.
// This file should not need to contain any header files to declare the type.
#include "david/forwards/Search.h"
namespace david {
class EngineContext;
class ChessEngine;
}

namespace bitboard {
struct gameState;
}

class ANN;

namespace gameTree {
class GameTree;
}

// all dem typedefs
namespace definitions {
typedef std::shared_ptr<david::EngineContext>     engineContext_ptr;
typedef std::shared_ptr<david::ChessEngine>       engine_ptr;
typedef std::shared_ptr<search::Search>           search_ptr;
typedef std::shared_ptr<ANN>                      neuralNetwork_ptr;
typedef std::shared_ptr<gameTree::GameTree>       gameTree_ptr;
typedef std::shared_ptr<bitboard::gameState>      gameState_ptr;

}