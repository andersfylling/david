//
// Created by anders on 5/1/17.
//

#ifndef CHESS_ANN_UCIHANDLER_H
#define CHESS_ANN_UCIHANDLER_H

#include <vector>
#include <map>
#include "Definitions.h"

namespace uci {
class Parser {
 private:
  const std::map<std::string, std::vector<std::vector<std::string>>> commands = {
      {"uci", {
          {}
      }},
      {"debug", {
          {"on"},
          {"off"}
      }},
      {"isready", {
          {}
      }},
      {"setoption", {
          {"name", "value"}
      }},
      {"register", {
          {"later"},
          {"name", "code"}
      }},
      {"ucinewgame", {
          {}
      }},
      {"position", {
          {"fen", "moves"},
          {"startpos", "moves"}
      }},
      {"go", {
          {"searchmoves"},
          {"ponder"},
          {"wtime"},
          {"btime"},
          {"winc"},
          {"binc"},
          {"movestogo"},
          {"depth"},
          {"nodes"},
          {"mate"},
          {"movetime"},
          {"infinite"}
      }},
      {"stop", {
          {}
      }},
      {"ponderhit", {
          {}
      }},
      {"quit", {
          {}
      }},
  };

 public:
  Parser();
  ~Parser();

  uint8_t parseInputForCommand(std::string input);
  arguments_t parseInputForArguments(std::string input);
  std::pair<uint8_t, arguments_t> parseInput(std::string input);
};

}

#endif //CHESS_ANN_UCIHANDLER_H
