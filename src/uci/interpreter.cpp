


#include <iostream>
#include <interpreter.h>
#include <sstream>
#include <commands_in.h>

void UCI::delegateCommand(std::string input) {

  std::stringstream request(input);

  std::string command;
  request >> command; // get the first word, the command


  if (command == "uci") {
    UCI::command_uci(command, request.str());
  }
  else if (command == "debug") {
    UCI::command_debug(command, request.str());
  }
  else if (command == "isready") {
    UCI::command_isready(command, request.str());
  }
  else if (command == "setoption") {
    UCI::command_setoption(command, request.str());
  }
  else if (command == "register") {
    UCI::command_register(command, request.str());
  }
  else if (command == "ucinewgame") {
    UCI::command_ucinewgame(command, request.str());
  }
  else if (command == "position") {
    UCI::command_position(command, request.str());
  }
  else if (command == "go") {
    UCI::command_go(command, request.str());
  }
  else if (command == "stop") {
    UCI::command_stop(command, request.str());
  }
  else if (command == "ponderhit") {
    UCI::command_ponderhit(command, request.str());
  }
  else if (command == "quit") {
    UCI::command_quit(command, request.str());
  }
}