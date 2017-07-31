// check if any of the development flags are used
#if defined(DAVID_DEBUG) || defined(DAVID_TEST) || defined(DAVID_BENCHMARK) || defined(DAVID_DEVELOPMENT)
#define DAVID_FLAG_DEVELOPMENT
#endif



#include <iostream>
#include <cassert>
#include <chrono>

#include "david/types.h"
#include <david/ChessEngine.h>
#include "david/bitboard.h"
#include "david/utils.h"
#include "david/EngineMaster.h"

#include "david/MoveGeneration.h"
#include "david/log.h"



void memTestMoveGen() {
  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES * 30 + 1> arr;

  // TreeGen
  for (int i = 0; i < arr.size(); i++) {
    ::david::utils::setDefaultChessLayout(arr[i]);
  }


  ::david::type::gameState_t gs;
  ::david::utils::setDefaultChessLayout(gs);

  const auto len = 1000000000;
  const auto N = 2;

  // MoveGen
  // Find everything from depth 0 to depth N and count the nodes.
  // Any memory stored by the recursive should be deleted after the loop round is complete.
  // Therefore anything else, suggests a issue in MoveGen.
  for (int i = 0; i < len; i++) {
    ::david::utils::perft(N, gs);
  }
}

void train() {}

void fight() {
  ::david::EngineMaster em(::david::constant::ANNFile);

  int mainEngineID = em.spawnEngine();
  int opponentEngineID = em.spawnEngine();
  int battleID = em.battle(mainEngineID, opponentEngineID);

  if (em.battleWinner(battleID, mainEngineID)) {
    std::cout << mainEngineID << std::endl;
    //em.trainUntilWinner(mainEngineID, opponentEngineID);
  }
  std::cout << "lost" << std::endl;
}

void gui() {
  std::cout << "David Chess Engine v1.0.0" << std::endl;
  ::david::ChessEngine engine("float_ANNFile_6_83_1_1497360313.net");
  engine.setNewGameBoard(::david::constant::FENStartPosition);
  engine.enableUCIMode();
  engine.linkUCICommands();
  engine.configureUCIProtocol();
  engine.activateUCIProtocol();
}

int log1() {
  int a = 432;

  ::david::log::log("test");

  return a;
}


int main (/*int argc, char * argv[]*/)
{
  // Make sure its not some weird "cpu architecture".
  assert(sizeof(uint8_t)  == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);

  const std::string mode = "gfdg"; // uci, fight, train, perft


  if (mode == "fight") {
    fight();
  }
  else if (mode == "uci") {
    gui();
  }
  else if (mode == "train") {
    train();
  }
  else if (mode == "perft") {
    ::david::utils::perft();
  }
  else if (mode == "memTestMoveGen") {
    memTestMoveGen();
  }

  log1();

  // Close program with exit code 0 after all threads have joined.
  return 0;
}

