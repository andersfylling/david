
#include "chess_ann/uci/Response.h"

#include <iostream>
#include "chess_ann/uci/definitions.h"

namespace uci {
void send(std::string res) {
  std::cout << res << std::endl;
}
void send(std::string command, uci::arguments_t args) {
  std::string res = command;

  for (auto& entry : args) {
    res += entry.first + ' ' + entry.second;
  }

  send(res);
}
}