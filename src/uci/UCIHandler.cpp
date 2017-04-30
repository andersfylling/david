//
// Created by anders on 4/17/17.
//

#include <iostream>
#include "UCIHandler.h"


UCIHandler::UCIHandler() {
  this->runListener = false;
  this->lastID = 2;
  //this->listener = std::thread();

}

UCIHandler::~UCIHandler() {
}

int UCIHandler::addListener(const uint8_t event, const std::function<void()> func) {
  this->lastID += 1;

  this->events[event].insert( std::pair<int, std::function<void()>>(this->lastID, func) );
  this->eventIDs.insert( std::pair<int, uint8_t>(this->lastID, event) );

  return this->lastID;
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
    for (auto& funcEntry : entry.second) {
      auto& func = funcEntry.second;
      func();
    }
  }
}
void UCIHandler::fireEvent(const uint8_t event) {
  auto entry = this->events.find(event);

  if (entry == this->events.end()) {
    return;
  }

  for (auto& observerEntry : entry->second) {
    auto& observer = observerEntry.second;
    observer();
  }
}

/**
 * Checks whether or not a listener exists base on ID.
 *
 * @multithreadingSupport false
 * @param int listenerID
 * @return True if listener with listenerID exists
 */
bool UCIHandler::hasListener(int listenerID) {
  return this->eventIDs.count(listenerID) > 0;
}

/**
 * Checks whether or not a listener exists base on ID.
 * Then calls the callback during mutex lock of both events and eventIDs.
 *
 * @multithreadingSupport true
 * @param int listenerID
 * @return True if listener with listenerID exists
 */
void UCIHandler::hasListener(int listenerID, std::function<void(bool exists)> lockedCallback) {
  static std::mutex m;
  {
      std::lock_guard<std::mutex> e(m);
  };

  {
    std::lock(this->eventIDsMutex, this->eventsMutex);
    std::lock_guard<std::mutex> es(this->eventsMutex, std::adopt_lock);
    std::lock_guard<std::mutex> eis(this->eventIDsMutex, std::adopt_lock);

    {
      std::lock_guard<std::mutex> e(m);
    }

    // ok
    bool exist = this->hasListener(listenerID);
    lockedCallback(exist);
  }

}

/**
 * Makes sure a listenerID (+ it's listener) does not exist in the system afterwards.
 *
 * @multithreadingSupport false
 * @param listenerID
 * @return true if the listener has been removed or does not exist.
 */
bool UCIHandler::removeListener(int listenerID) {
  if (!this->hasListener(listenerID)) {
    return true;
  }

  // find and remove function
  auto eventID = this->eventIDs.find(listenerID)->second;
  auto& eventEntry = this->events.find(eventID)->second;
  eventEntry.erase(eventEntry.find(listenerID));

  // then remove the listenerID
  this->eventIDs.erase(this->eventIDs.find(listenerID));

  return this->hasListener(listenerID);
}

/**
 * Makes sure a listenerID (+ it's listener) does not exist in the system afterwards.
 *
 * @multithreadingSupport true
 * @param listenerID
 * @return true if the listener has been removed or does not exist.
 */
void UCIHandler::removeListenerThread(int listenerID) {
  this->hasListener(listenerID, [&](bool exists){
    this->removeListener(listenerID);
  });
}




