//
// Created by anders on 4/17/17.
//

#ifndef CHESS_ANN_UCIHANDLER_H
#define CHESS_ANN_UCIHANDLER_H

#include <map>
#include <functional>
#include <vector>
#include <thread>
class UCIHandler {
 private:
  std::map<const uint8_t, std::vector<std::function<void()>>> events;
  std::thread listener;
  bool runListener;

 public:
  UCIHandler();
  ~UCIHandler();

  bool initiateListener();
  bool joinListener();
  bool setupListener();

  bool addFunction(const uint8_t event, const std::function<void()> func);
  void fireEvent(const uint8_t event);

  void test();


};

#endif //CHESS_ANN_UCIHANDLER_H
