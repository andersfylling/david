//
// Created by anders on 4/17/17.
//

#include <iostream>
#include "UCIHandler.h"


UCIHandler::UCIHandler() {
  this->runListener = false;
  //this->listener = std::thread();

}

UCIHandler::~UCIHandler() {
}

bool UCIHandler::addFunction(const uint8_t event, const std::function<void()> func) {

  this->events[event].push_back(func);

  return false;
}

bool UCIHandler::initiateListener() {

  this->runListener = true;


  this->listener = std::thread([&] {
    std::string line;
    std::cout << "test" << std::endl;
    while (this->runListener && std::getline(std::cin, line)) {
      if (line == "") {
        continue;
      }

      std::cout << ">" << line << std::endl;

      if (line == "stop") {
        this->runListener = false;
      }
    }
  });

  return false;
}

bool UCIHandler::joinListener() {
  if (this->listener.joinable()) {
    this->listener.join();
    return true;
  }
  return false;
}

bool UCIHandler::setupListener() {
  this->initiateListener();
  return this->joinListener();
}


void UCIHandler::test() {

  for (auto& entry : this->events) {
    std::cout << "Running event: " << entry.first << std::endl;
    for (auto& func : entry.second) {
      func();
    }
  }
}
void UCIHandler::fireEvent(const uint8_t event) {
  auto entry = this->events.find(event);

  if (entry == this->events.end()) {
    return;
  }

  for (auto& observer : entry->second) {
    observer();
  }
}




