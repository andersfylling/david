
#include "david/uci/Response.h"

#include <iostream>
#include "david/uci/definitions.h"

namespace david {
namespace uci {
void send(std::string res) {
  std::cout << res << std::endl;
}
void send(std::string command, uci::arguments_t args) {
  std::string res = command;

  for (auto &entry : args) {
    res += entry.first + ' ' + entry.second;
  }

  send(res);
}
}
}