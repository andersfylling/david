//
// Created by anders on 5/1/17.
//

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include "UCIHandler.h"

UCIHandler::UCIHandler() {
}

UCIHandler::~UCIHandler() {
}
std::pair<uint8_t, std::map<std::string, std::string>> UCIHandler::parseUCIInput(std::string input) {
  std::stringstream request(input);

  std::string command;
  request >> command; // get the first word, the command

  // check if the command exists
  auto entry = this->commands.find(command);
  if (entry != this->commands.end()) {

  }

  return std::pair<uint8_t, std::map<std::string, std::string>>();
}