#pragma once

#include <iostream>
#include "david/uci/definitions.h"

namespace david {
namespace uci {
void send(std::string res);
void send(std::string command, uci::arguments_t args);
}
}