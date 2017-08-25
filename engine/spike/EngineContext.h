#pragma once

#include "david/types.h"
#include "david/bitboard.h"
#include "uci/Listener.h"
#include "david/ANN/ANN.h"
#include "david/Search.h"
#include "david/TreeGen.h"

namespace david {

/**
 * Holds different classes and variables residing within the Engine.
 * These are used by "sub classes" to communicate with each other..
 */
class EngineContext {
 private:

 public:
  EngineContext();
  EngineContext(const std::string ANNFileName);
  ~EngineContext();


  // Different classes that are allowed to communicate with each other.
  //type::search_ptr          searchPtr         = nullptr;
  //type::neuralNetwork_ptr   neuralNetworkPtr  = nullptr;
  //type::gameTree_ptr        gameTreePtr       = nullptr;
  //type::uciProtocol_ptr     uciProtocolPtr    = nullptr;
  bitboard::COLOR           playerColor       = bitboard::COLOR::WHITE;

  type::NeuralNetwork_t NeuralNet;
  type::TreeGen_t TreeGen;
  type::Search_t Search;
  type::UciProtocol_t UCI;

  // used for testing
  bool testing = false;

};
}
