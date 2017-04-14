#include <thread>
#include <iostream>
#include <UCIListener.h>


void UCI::listen() {
  std::string line;
  bool run = true;

  while (run && std::getline(std::cin, line)) {
    std::cout << "> " << line << std::endl;

    if (line == "stop" || line == "shutdown") {
      run = false;
    }
  }
}



std::thread UCI::init() {
  auto listenerThread = std::thread(UCI::listen);
  //listenerThread.detach();

  return listenerThread;
}
