#pragma once

#include "chess_ann/definitions.h"
#include "chess_ann/bitboard.h"

namespace chess_ann {
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
