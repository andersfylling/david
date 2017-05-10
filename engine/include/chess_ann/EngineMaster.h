#ifndef CHESS_ANN_ENGINEMASTER_H
#define CHESS_ANN_ENGINEMASTER_H

#include "map"
#include "array"
#include "chess_ann/Engine.h"

namespace chess_ann {
class EngineMaster {
 private:
  std::map<int, ::chess_ann::Engine> engineInstances; // id => instance
  std::map<int, std::array<int, 2>> engineDuals;

 public:
  EngineMaster();

  int spawnEngine();
  int battle(int engineID1, int engineID2);
  bool battleWinner(int battleID, int mainEngineID);
  int battleWinner(int battleID);
  void trainUntilWinner(int mainEngineID, int opponentEngineID);


};
}

#endif //CHESS_ANN_ENGINEMASTER_H
