//
// Created by anders on 4/17/17.
//

#ifndef CHESS_ANN_UCILISTENER_H
#define CHESS_ANN_UCILISTENER_H


#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include "UCIHandler.h"

class UCIListener {
 private:
  std::map<const uint8_t, std::map<const int, const std::function<void(std::map<std::string, std::string>)>>> events; // (eventID, (listenerID, eventHandler))
  std::map<const int, const uint8_t> eventIDs; //(listenerID, eventID)
  int lastID;
  std::thread listener;
  bool runListener;
  UCIHandler uciHandler;

  std::mutex eventsMutex;
  std::mutex eventIDsMutex;

 public:
  UCIListener();
  ~UCIListener();

  bool initiateListener();
  bool joinListener();
  bool setupListener();

  int addListener(const uint8_t event, const std::function<void(std::map<std::string, std::string>)> func);
  bool hasListener(int listenerID);
  void hasListener(int listenerID, std::function<void(bool exists)> lockedCallback);
  bool removeListener(int listenerID);
  void fireEvent(const uint8_t event);
  void fireEvent(const uint8_t event, const std::map<std::string, std::string> arguments);

  void removeListenerThread(int listenerID);
  bool joinListenerAndStop();
};

#endif //CHESS_ANN_UCILISTENER_H
