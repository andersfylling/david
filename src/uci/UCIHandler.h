//
// Created by anders on 4/17/17.
//

#ifndef CHESS_ANN_UCIHANDLER_H
#define CHESS_ANN_UCIHANDLER_H

#include <map>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
class UCIHandler {
 private:
  std::map<const uint8_t, std::map<const int, const std::function<void()>>> events; // (eventID, (listenerID, eventHandler))
  std::map<const int, const uint8_t> eventIDs; //(listenerID, eventID)
  int lastID;
  std::thread listener;
  bool runListener;

  std::mutex eventsMutex;
  std::mutex eventIDsMutex;

 public:
  UCIHandler();
  ~UCIHandler();

  bool initiateListener();
  bool joinListener();
  bool setupListener();

  int addListener(const uint8_t event, const std::function<void()> func);
  bool hasListener(int listenerID);
  void hasListener(int listenerID, std::function<void(bool exists)> lockedCallback);
  bool removeListener(int listenerID);
  void fireEvent(const uint8_t event);

  void test();

  void removeListenerThread(int listenerID);
};

#endif //CHESS_ANN_UCIHANDLER_H
