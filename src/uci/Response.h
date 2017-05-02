//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_RESPONSE_H
#define CHESS_ANN_RESPONSE_H

#include <iostream>
#include "Definitions.h"

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

#endif //CHESS_ANN_RESPONSE_H
