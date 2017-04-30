//
// Created by anders on 4/15/17.
//

#ifndef CHESS_ANN_COMMANDS_IN_H
#define CHESS_ANN_COMMANDS_IN_H
#include <iostream>
namespace uci {
void command_uci(std::string, std::string);
void command_debug(std::string, std::string);
void command_isready(std::string, std::string);
void command_setoption(std::string, std::string);
void command_register(std::string, std::string);
void command_ucinewgame(std::string, std::string);
void command_position(std::string, std::string);
void command_go(std::string, std::string);
void command_stop(std::string, std::string);
void command_ponderhit(std::string, std::string);
void command_quit(std::string, std::string);
}

#endif //CHESS_ANN_COMMANDS_IN_H
