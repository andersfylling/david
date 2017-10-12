#pragma once

#include "david/utils/utils.h"
#include "david/types.h"

#include <map>
#include <array>

// forward declarations

namespace david {
class EngineMaster {
 private:
  std::map<int, type::Engine_t> engineInstances; // id => instance
  std::map<int, std::array<int, 2>> engineBattle;
  std::map<int, int> engineBattleWinnerLog; // battle id => winner id (engineInstance)

  int lastEngineInstanceID;
  int lastEngineBattleID;

  std::string ANNFilename;

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