//
// Created by anders on 5/1/17.
//

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include "UCIHandler.h"
#include "UCIEvent.h"

UCIHandler::UCIHandler() {
}

UCIHandler::~UCIHandler() {
}
std::pair<uint8_t, std::map<std::string, std::string>> UCIHandler::parseInput(std::string input) {
  auto command = this->parseInputForCommand(input);
  if (command == uci::event::NO_MATCHING_COMMAND) {
    return std::pair<uint8_t, std::map<std::string, std::string>>();
  }

  // get arguments
  auto arguments = this->parseInputForArguments(input);

  return std::pair<uint8_t, std::map<std::string, std::string>>();
}
std::map<std::string, std::string> UCIHandler::parseInputForArguments(std::string input) {
  std::string argumentsStr = input;
  if (this->parseInputForCommand(input) != uci::event::NO_MATCHING_COMMAND) {
    std::stringstream request(input);

    std::string command;
    request >> command;

    argumentsStr = request.str();
  }

  // get arguments from argumentsStr.



  return std::map<std::string, std::string>();
}

uint8_t UCIHandler::parseInputForCommand(std::string input) {
  std::stringstream request(input);

  std::string command;
  request >> command; // get the first word, the command

  // check if the command exists.
  // this is bad. find a way to store the uint8_t value or change it to a char, whatever it takes to reduce this mess.
  auto entry = this->commands.find(command);
  uint8_t event = 0;
  if (entry == this->commands.end()) {
    event = uci::event::NO_MATCHING_COMMAND;
  }
  else if (command == "uci") {
    event = uci::event::UCI;
  }
  else if (command == "debug") {
    event = uci::event::DEBUG;
  }
  else if (command == "isready") {
    event = uci::event::ISREADY;
  }
  else if (command == "setoption") {
    event = uci::event::SETOPTION;
  }
  else if (command == "register") {
    event = uci::event::REGISTER;
  }
  else if (command == "ucinewgame") {
    event = uci::event::UCINEWGAME;
  }
  else if (command == "position") {
    event = uci::event::POSITION;
  }
  else if (command == "go") {
    event = uci::event::GO;
  }
  else if (command == "stop") {
    event = uci::event::STOP;
  }
  else if (command == "ponderhit") {
    event = uci::event::PONDERHIT;
  }
  else if (command == "quit") {
    event = uci::event::QUIT;
  }

  return event;
}
