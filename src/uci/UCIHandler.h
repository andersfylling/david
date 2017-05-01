//
// Created by anders on 5/1/17.
//

#ifndef CHESS_ANN_UCIHANDLER_H
#define CHESS_ANN_UCIHANDLER_H

#include <vector>
#include <map>
class UCIHandler {
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
          {"name"}
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
  UCIHandler();
  ~UCIHandler();


  uint8_t parseInputForCommand(std::string input);
  std::map<std::string, std::string> parseInputForArguments(std::string input);
  std::pair<uint8_t, std::map<std::string, std::string>> parseInput(std::string input);
};

#endif //CHESS_ANN_UCIHANDLER_H
