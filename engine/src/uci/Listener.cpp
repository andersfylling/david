//
// Created by anders on 4/17/17.
//

#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>

#include "david/uci/definitions.h"
#include "david/uci/events.h"
#include "david/uci/Parser.h"
#include "david/uci/Listener.h"

namespace david {
uci::Listener::Listener()
    : runListener(false),
      lastID(2) {
}

uci::Listener::~Listener() {
}

int uci::Listener::addListener(const uint8_t event,
                               const std::function<void(std::map<std::string, std::string>)> &func) {
  const int id = this->lastID += 1;

  // check if event key already exists, if not create it.
  if (this->events.count(event) == 0) {
    this->events.insert(
        std::pair<const uint8_t, std::map<const int, const std::function<void(std::map<std::string, std::string>)>>>(
            event, std::map<const int, const std::function<void(std::map<std::string, std::string>)>>()
        )
    );
  }

  this->events[event].insert(std::pair<const int, const std::function<void(std::map<std::string, std::string>)>>(id,
                                                                                                                 func));
  this->eventIDs.insert(std::pair<const int, const uint8_t>(id, event));

  return id;
}

bool uci::Listener::initiateListener() {

  this->runListener = true;

  this->listener = std::thread([&] {
    std::string line;
    while (this->runListener && std::getline(std::cin, line)) {
      if (line == "") {
        continue;
      }

      auto event = this->parser.parseInputForCommand(line);
      auto args = this->parser.parseInputForArguments(line);

      if (event != uci::event::NO_MATCHING_COMMAND) {
        this->fireEvent(event, args);
      }
    }
  });

  return false;
}

bool uci::Listener::joinListener() {
  if (this->listener.joinable()) {
    this->listener.join();
    return true;
  }
  return false;
}

bool uci::Listener::joinListenerAndStop() {
  this->runListener = false;
  bool joined = this->joinListener();

  return joined && !this->runListener;
}

bool uci::Listener::setupListener() {
  this->initiateListener();
  return this->joinListener();
}

void uci::Listener::fireEvent(const uint8_t event) {
  this->fireEvent(event, std::map<std::string, std::string>());
}

void uci::Listener::fireEvent(const uint8_t event, const arguments_t args) {
  auto entry = this->events.find(event);

  if (entry == this->events.end()) {
    return;
  }

  for (auto &observerEntry : entry->second) {
    auto &observer = observerEntry.second;
    observer(args);
  }
}

/**
 * Checks whether or not a listener exists base on ID.
 *
 * @multithreadingSupport false
 * @param int listenerID
 * @return True if listener with listenerID exists
 */
bool uci::Listener::hasListener(const int listenerID) {
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
void uci::Listener::hasListener(int listenerID, std::function<void(bool exists)> lockedCallback) {
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
bool uci::Listener::removeListener(const int listenerID) {
  if (!this->hasListener(listenerID)) {
    return true;
  }

  // find and remove function
  auto eventID = this->eventIDs.find(listenerID)->second;
  auto &eventEntry = this->events.find(eventID)->second;
  eventEntry.erase(eventEntry.find(listenerID));

  // then remove the listenerID
  this->eventIDs.erase(this->eventIDs.find(listenerID));

  return !this->hasListener(listenerID);
}

/**
 * Makes sure a listenerID (+ it's listener) does not exist in the system afterwards.
 *
 * @multithreadingSupport true
 * @param listenerID
 * @return true if the listener has been removed or does not exist.
 */
void uci::Listener::removeListenerThread(int listenerID) {
  this->hasListener(listenerID, [&](bool exists) {
    this->removeListener(listenerID);
  });
}
void uci::Listener::stopListening() {
  this->runListener = false;
}
}