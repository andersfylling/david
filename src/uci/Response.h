//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_RESPONSE_H
#define CHESS_ANN_RESPONSE_H

#include <iostream>
#include "Definitions.h"

namespace uci {
class Response {

 public:
  void send(const std::string res);
  void send(const std::string command, const uci::arguments_t args);

};
}

#endif //CHESS_ANN_RESPONSE_H
