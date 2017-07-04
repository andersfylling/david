#include <iostream>
#include <cassert>
#include <chrono>

#include "david/bitboard.h"
#include "david/types.h"
#include "david/utils.h"
#include "david/EngineMaster.h"



int main (int argc, char * argv[])
{
  const int DEPTH = 1;

  // TODO: once depth N - 1 is complete, do not regenerate everything when checking for depth N ?
//  std::cout << "Perft results" << std::endl;
//  std::cout << "=============" << std::endl;
//  std::cout << "Depth" << '\t' << "Nodes" << std::endl;
//  ::david::type::gameState_t gs;
//  for (int i = 0; i <= DEPTH; i++) {
//    // reset layout
//    ::david::utils::setDefaultChessLayout(gs);
//
//    // get perft results
//    std::cout << i << "\t\t" << ::david::utils::perft(i, gs) << std::endl;
//  }


  auto begin = std::chrono::high_resolution_clock::now();
  uint32_t iterations = 10000;
  ::david::type::gameState_t gs;
  for(uint32_t i = 0; i < iterations; ++i) {
    // reset layout
    ::david::utils::setDefaultChessLayout(gs);

    // get perft results
    ::david::utils::perft(DEPTH, gs);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout << duration << "ns total, average : " << duration / iterations << "ns." << std::endl;


  return 0;

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

