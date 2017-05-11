#pragma once

// system dependencies
#include <iostream>
#include <memory>

// Forward declaration.
// This file should not need to contain any header files to declare the type.
#include "chess_ann/forwards/Search.h"
namespace chess_ann {
class EngineContext;
class Engine;
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
typedef std::shared_ptr<chess_ann::EngineContext> engineContext_ptr;
typedef std::shared_ptr<chess_ann::Engine>        engine_ptr;
typedef std::shared_ptr<search::Search>           search_ptr;
typedef std::shared_ptr<ANN>                      neuralNetwork_ptr;
typedef std::shared_ptr<gameTree::GameTree>       gameTree_ptr;
typedef std::shared_ptr<bitboard::gameState>      gameState_ptr;

}