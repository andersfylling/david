#pragma once

#include "david/definitions.h"
#include "david/bitboard.h"

namespace david {
class EngineContext {
 private:

 public:
  EngineContext();


  definitions::search_ptr         searchPtr = nullptr;
  definitions::neuralNetwork_ptr  neuralNetworkPtr = nullptr;
  definitions::gameTree_ptr       gameTreePtr = nullptr;
  ::bitboard::COLOR               playerColor = ::bitboard::COLOR::WHITE;

};
}
