//
// Created by anders on 5/2/17.
//

#include "Response.h"

namespace uci {


void Response::send(std::string res) {
  std::cout << res << std::endl;
}
void Response::send(std::string command, uci::arguments_t args) {
  std::string res = command;

  for (auto& entry : args) {
    res += entry.first + ' ' + entry.second;
  }

  this->send(res);
}

}