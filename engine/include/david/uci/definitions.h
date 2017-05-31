#pragma once

#include <iostream>
#include <map>
#include <functional>

namespace david {
namespace uci {
typedef std::function<void(std::map<std::string, std::string>)> callback_t;
typedef std::map<std::string, std::string> arguments_t;
}
}