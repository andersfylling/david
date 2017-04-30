
#include <iostream>
#include <listener.h>
#include <vector>
#include "uci/UCIHandler.h"
#include "uci/UCIEvent.h"

int main (int argc, char * argv[])
{
  // Store threads
  //std::vector<std::thread> threads;

  // Add threads
  //threads.push_back(UCI::init());

  // join threads
  //for (auto& t : threads) {
  //  t.join();
  //}

  UCIHandler uciHandler;

  uciHandler.addListener(UCI::event::TEST, [] {
    std::cout << "called function 1" << std::endl;
  });

  uciHandler.test();


  // Join the UCI listener thread, so it exits correctly.
  uciHandler.setupListener();

  // Close program with exit code 0
  return 0;
}
