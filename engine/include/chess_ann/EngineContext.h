#pragma once

#include "chess_ann/definitions.h"

namespace chess_ann {
class EngineContext {
 private:

 public:
  EngineContext();


  definitions::search_ptr        searchPtr;
  definitions::neuralNetwork_ptr neuralNetworkPtr;
  definitions::gameTree_ptr      gameTreePtr;

};
}
