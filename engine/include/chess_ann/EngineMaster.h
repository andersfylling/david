#ifndef CHESS_ANN_ENGINEMASTER_H
#define CHESS_ANN_ENGINEMASTER_H

#include "map"
#include "array"
//#include "chess_ann/Engine.h"
#include "chess_ann/utils.h"

namespace utils {
std::string getAbsoluteProjectPath();
}

namespace chess_ann {
class Engine;
typedef std::shared_ptr<chess_ann::Engine> enginePtr;
const std::string annExecFile = utils::getAbsoluteProjectPath() + "/engine/src/ANN/networks/";

class EngineMaster {
 private:
  std::map<int, enginePtr> engineInstances; // id => instance
  std::map<int, std::array<int, 2>> engineBattle;
  std::map<int, int> engineBattleWinnerLog; // battle id => winner id (engineInstance)

  int lastEngineInstanceID;
  int lastEngineBattleID;

  const std::string filename;

 public:
  EngineMaster(const std::string filename);

  int spawnEngine();
  int battle(const int engineID1, const int engineID2);
  int battle(const int engineID1, const int engineID2, const std::string fen);
  bool battleWinner(int battleID, int mainEngineID);
  int battleWinner(int battleID);
  void trainUntilWinner(int mainEngineID, int opponentEngineID);


};
}

#endif //CHESS_ANN_ENGINEMASTER_H
