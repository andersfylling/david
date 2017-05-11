#include <iostream>

#include "chess_ann/EngineMaster.h"

int main (int argc, char * argv[])
{
  chess_ann::EngineMaster em("float_binaryNetworkTrainingFile_5_71_1_1494410133.net");

  int mainEngineID      = em.spawnEngine();
  int opponentEngineID  = em.spawnEngine();
  int battleID          = em.battle(mainEngineID, opponentEngineID);

  if (em.battleWinner(battleID, mainEngineID)) {
    std::cout << mainEngineID << std::endl;
    //em.trainUntilWinner(mainEngineID, opponentEngineID);
  }
  std::cout << "lost" << std::endl;

  // Close program with exit code 0 after all threads have joined.
  return 0;
}

