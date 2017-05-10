#include <iostream>

#include "chess_ann/EngineMaster.h"

int main (int argc, char * argv[])
{
  chess_ann::EngineMaster em;

  int mainEngineID      = em.spawnEngine();
  int opponentEngineID  = em.spawnEngine();
  int battleID          = em.battle(mainEngineID, opponentEngineID);

  if (em.battleWinner(battleID, mainEngineID)) {
    em.trainUntilWinner(mainEngineID, opponentEngineID);
  }




  // Close program with exit code 0 after all threads have joined.
  return 0;
}

