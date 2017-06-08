#pragma once

#include "david/types.h"
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
  type::search_ptr         searchPtr         = nullptr;
  type::neuralNetwork_ptr  neuralNetworkPtr  = nullptr;
  type::gameTree_ptr       gameTreePtr       = nullptr;
  bitboard::COLOR          playerColor       = bitboard::COLOR::WHITE;

  // used for testing
  bool testing = false;

};
}
