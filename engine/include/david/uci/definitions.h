#pragma once

#include <iostream>
#include <map>
#include <functional>



namespace david {
namespace uci {

// Every uci specific definition
typedef std::function<void(std::map<std::string, std::string>)> callback_t;
typedef std::map<std::string, std::string> arguments_t;
}
}