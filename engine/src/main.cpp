
// handle macros
#include "david/MACROS.h"


// includes
#include <iostream>
#include <cassert>

#include "david/types.h"
#include <david/ChessEngine.h>
#include <david/utils/gameState.h>
#include "david/utils/utils.h"
#include "david/EngineMaster.h"
#include "david/MoveGen.h"

#include "david/utils/logger.h"


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

int juddperft (int argc, char * argv[])
{
  // get command
  if (argc < 2) {
    std::cerr << "no juddperft argument" << std::endl;
    return EXIT_FAILURE;
  }
  std::string FEN{argv[1]};
  int         depth{::utils::stoi(argv[2])};
  int         score{::utils::stoi(argv[3])}; //TODO make uin64_t

  ::david::type::gameState_t gs;
  ::utils::gameState::generateFromFEN(gs, FEN);

  if (::utils::perft(static_cast<uint8_t>(depth), gs) != static_cast<uint64_t>(score)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; //test-external /home/anders/github.com/andersfylling/david/cmake-build-release/bin/chess_ann_src

  std::string cmd{argv[1]};
  if (cmd == "juddperft") {
    std::string FEN{argv[2]};
    int         depth{::utils::stoi(argv[3])};
    uint64_t    score{::std::stoull(argv[4])}; //TODO make uin64_t

    ::david::type::gameState_t gs;
    ::utils::gameState::generateFromFEN(gs, FEN);

    if (::utils::perft(static_cast<uint8_t>(depth), gs) != static_cast<uint64_t>(score)) {
      return EXIT_FAILURE;
    }
  }
  else if (cmd == "juddperft-egn") {
    std::string FEN{argv[2]};

    ::david::type::gameState_t gs;
    ::utils::gameState::generateFromFEN(gs, FEN);

    ::david::MoveGen moveGen{gs};

    std::array<::david::type::gameState_t, 256> gameStates{};
    auto end = moveGen.template generateGameStates(gameStates);

    for (unsigned int i = 0; i < end; i++) {
      std::cout << ::utils::gameState::getEGN(gs, gameStates[i]) << "\n";
    }
  }

  return EXIT_SUCCESS;
}
//
//int main () {
//
//  ::david::type::gameState_t gs;
//  ::utils::gameState::setDefaultChessLayout(gs);
//
//  ::david::MoveGen moveGen{gs};
//
//  for (int i = 0; i < 100; i++) {
//    std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
//    moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
//  }
//}

// TODO: Should support changing mode without recompiling.
int main (int argc, char * argv[])
{
  // Make sure its not some weird "cpu architecture".
  assert(sizeof(uint8_t)  == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);


  const std::string mode = "perft"; // uci, fight, train, perft, judd-perft. Default: "uci"


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
    //::utils::perft(7);
    //::utils::perft_time(6, 10);
    //::utils::perft_egn(1, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    //::utils::perft(6, "rnbq1k1r/pp2bppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R", 1);
    //::utils::perft_egn(2, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    ::utils::perft_advanced(1, "r3k3/3N4/8/8/8/8/8/4K3 b q - 0 1");
    ::utils::perft_advanced(2, "r3k3/3N4/8/8/8/8/8/4K3 b q - 0 1");
    ::utils::perft_advanced(3, "r3k3/3N4/8/8/8/8/8/4K3 b q - 0 1");
    //::utils::perft(1, "r4k2/8/8/4N3/8/4K3/8/8 b - - 0 1");
  }
  else if (mode == "juddperft") {
    juddperft(argc, argv);
  }
  else if (mode == "profiling") {
    ::david::type::gameState_t gs;
    ::utils::gameState::setDefaultChessLayout(gs);

    ::david::MoveGen moveGen{gs};

    for (int i = 0; i < 100; i++) {
      std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
      moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
    }
  }


  return EXIT_SUCCESS;
}

