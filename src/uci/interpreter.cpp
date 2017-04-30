


#include <iostream>
#include <interpreter.h>
#include <sstream>
#include <commands_in.h>

void uci::delegateCommand(std::string input) {

  std::stringstream request(input);

  std::string command;
  request >> command; // get the first word, the command


  if (command == "uci") {
    uci::command_uci(command, request.str());
  }
  else if (command == "debug") {
    uci::command_debug(command, request.str());
  }
  else if (command == "isready") {
    uci::command_isready(command, request.str());
  }
  else if (command == "setoption") {
    uci::command_setoption(command, request.str());
  }
  else if (command == "register") {
    uci::command_register(command, request.str());
  }
  else if (command == "ucinewgame") {
    uci::command_ucinewgame(command, request.str());
  }
  else if (command == "position") {
    uci::command_position(command, request.str());
  }
  else if (command == "go") {
    uci::command_go(command, request.str());
  }
  else if (command == "stop") {
    uci::command_stop(command, request.str());
  }
  else if (command == "ponderhit") {
    uci::command_ponderhit(command, request.str());
  }
  else if (command == "quit") {
    uci::command_quit(command, request.str());
  }
}