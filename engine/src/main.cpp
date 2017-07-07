#include <iostream>
#include <cassert>
#include <chrono>

#include "david/bitboard.h"
#include "david/types.h"
#include "david/utils.h"
#include "david/EngineMaster.h"



int main (int argc, char * argv[])
{
  // Make sure its not some weird "cpu architecture".
  assert(sizeof(uint8_t)  == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);

  ::david::EngineMaster em(::david::constant::ANNFile);

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

