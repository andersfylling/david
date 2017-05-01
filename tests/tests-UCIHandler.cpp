#include "catch.hpp"
#include "../src/uci/UCIHandler.h"
#include "../src/uci/UCIEvent.h"


TEST_CASE( "Making sure commands from GUI to Engine are correctly parsed", "[UCIHandler.parseInput]" ) {
  UCIHandler uciHandler;

  std::map<uint8_t, std::string> commands = {
      {uci::event::NO_MATCHING_COMMAND, "akdjas"},
      {uci::event::UCI,                 "uci"},
      {uci::event::DEBUG,               "debug"},
      {uci::event::ISREADY,             "isready"},
      {uci::event::SETOPTION,           "setoption"},
      {uci::event::REGISTER,            "register"},
      {uci::event::UCINEWGAME,          "ucinewgame"},
      {uci::event::POSITION,            "position"},
      {uci::event::GO,                  "go"},
      {uci::event::STOP,                "stop"},
      {uci::event::PONDERHIT,           "ponderhit"},
      {uci::event::QUIT,                "quit"}
  };

  for (const auto entry : commands) {
    REQUIRE( uciHandler.parseInputForCommand(entry.second) == entry.first );
  }
}
