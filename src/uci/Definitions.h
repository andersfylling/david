//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_DEFINITIONS_H
#define CHESS_ANN_DEFINITIONS_H

#include <iostream>
#include <map>
#include <functional>

namespace uci {
typedef std::function<void(std::map<std::string, std::string>)> callback_t;
typedef std::map<std::string, std::string> arguments_t;
}

#endif //CHESS_ANN_DEFINITIONS_H
