#pragma once

#include "david/definitions.h"
#include "david/bitboard.h"

namespace david {

/**
 * Holds different classes and variables residing within the Engine.
 * These are used by "sub classes" to communicate with each other..
 */
class EngineContext {
 private:

 public:
  EngineContext();


  // Different classes that are allowed to communicate with each other.
  definitions::search_ptr         searchPtr         = nullptr;
  definitions::neuralNetwork_ptr  neuralNetworkPtr  = nullptr;
  definitions::gameTree_ptr       gameTreePtr       = nullptr;
  bitboard::COLOR                 playerColor       = bitboard::COLOR::WHITE;

};
}
