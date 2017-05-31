#pragma once

#include <iostream>
#include "david/uci/definitions.h"

namespace uci {
void send(std::string res);
void send(std::string command, uci::arguments_t args);
}