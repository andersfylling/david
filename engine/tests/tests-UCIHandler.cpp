#include "catch.hpp"
#include "david/uci/Parser.h"
#include "david/uci/events.h"

// gui to engine
TEST_CASE( "Making sure commands from GUI to Engine are correctly parsed", "[Parser.parseInput]" ) {

  david::uci::Parser parser;

  std::map<uint8_t, std::string> commands = {
      {david::uci::event::NO_MATCHING_COMMAND, "akdjas"},
      {david::uci::event::NO_MATCHING_COMMAND, "ucii"},
      {david::uci::event::NO_MATCHING_COMMAND, "sready"},
      {david::uci::event::NO_MATCHING_COMMAND, "ståp"},
      {david::uci::event::NO_MATCHING_COMMAND, "sada4g432 fa4f23"},
      {david::uci::event::UCI,                 "uci"},
      {david::uci::event::DEBUG,               "debug"},
      {david::uci::event::ISREADY,             "isready"},
      {david::uci::event::SETOPTION,           "setoption"},
      {david::uci::event::REGISTER,            "register"},
      {david::uci::event::UCINEWGAME,          "ucinewgame"},
      {david::uci::event::POSITION,            "position"},
      {david::uci::event::GO,                  "go"},
      {david::uci::event::STOP,                "stop"},
      {david::uci::event::PONDERHIT,           "ponderhit"},
      {david::uci::event::QUIT,                "quit"}
  };

  for (const auto entry : commands) {
    REQUIRE( parser.parseInputForCommand(entry.second) == entry.first );
  }
}

// gui to engine
TEST_CASE( "Making sure arguments from GUI to Engine are correctly parsed", "[Parser.parseInputForArguments]" ) {
  david::uci::Parser parser;

  std::map<std::string, david::uci::arguments_t> inputs = {
      {"uci", {}},
      {"uci this text should be completely ignored by the parser.", {}},

      {"debug", {}},

      {"debug on", {{"on", ""}} },

      {"debug off", {{"off", ""}} },

      {"isready", {} },

      {"setoption", {} },
      {"setoption name", {{"name", ""}} },
      {"setoption name test", {{"name", "test"}} },
      {"setoption name test value", {{"name", "test"}, {"value", ""}} },
      {"setoption name test value lalala", {{"name", "test"}, {"value", "lalala"}} },

      {"register later", {{"later", ""}} },
      {"register name Stefan MK code 349284", {{"name", "Stefan MK"}, {"code", "349284"}} },

      {"position", {} },
      {"position fen moves sadha", {{"fen", ""}, {"moves", "sadha"}} },
      {"position startpos moves sadha", {{"startpos", ""}, {"moves", "sadha"}} }
  };

  for (const auto entry : inputs) { // this runs a few rounds before error.
    auto a = parser.parseInputForArguments(entry.first);
    auto b = entry.second;
    REQUIRE(a == b);
  }
}