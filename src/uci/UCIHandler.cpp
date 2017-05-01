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
  std::stringstream request(input);

  // remove command, if it hasn't been removed already.
  // This assumes that if the first word is a match for a command, its a true command
  // and removes it.
  std::string command;
  if (this->parseInputForCommand(input) != uci::event::NO_MATCHING_COMMAND) {
    request >> command;
  }

  // get arguments from argumentsStr.
  std::map<std::string, std::string> arguments;
  std::string firstWord;
  request >> firstWord;

  const auto cmdEntry = this->commands.find(command);
  auto& vecHolder = cmdEntry->second;
  for (int i = 0; i < vecHolder.size(); i++) {
    auto& v = vecHolder.at(i);

    // if the first key in this vector doesn't equal the first word, ignore the rest and continue
    // to the next vector
    if (v.size() == 0 || v.front() != firstWord) {
      continue;
    }

    // add the key to the arguments map
    arguments.insert( std::pair<std::string, std::string>(firstWord, ""));

    // if the next word is not another key, it should be the value of this key.
    // this value is appended upon until the next key is met.
    std::string key = firstWord;
    std::string value = "";
    std::string nextWord;
    int j = 1;
    while (request >> nextWord) {
      std::cout << request.str() << std::endl;

      if (j < v.size() && v.at(j) == nextWord) {
        // the next word is a key, store the existing value to the old key and clear it.
        j += 1;
        arguments[key] = value;
        value = "";
        key = nextWord;
        arguments.insert( std::pair<std::string, std::string>(key, "") );
        continue; // update the next word!
      }

      value += " " + nextWord;
    }

    arguments[key] = value;

    // finished
    break;
  }


  return arguments;
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

