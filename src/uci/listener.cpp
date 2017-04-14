#include <thread>
#include <iostream>
#include <listener.h>
#include <interpreter.h>

/**
 * Runs in the background, loops forever to catch any UCI commands headed into this executable.
 *
 * @see UCI::init() which starts this function.
 */
void UCI::listen() {
  std::string line;
  bool run = true;


  while (run && std::getline(std::cin, line)) {
    if (line == "") {
      continue;
    }

    std::cout << "#" << line << std::endl;

    UCI::delegateCommand(line);
  }
}


/**
 * Create a new thread that listens for console input until the program is stopped or a stop command is executed.
 *
 * @return std::thread instance
 */
std::thread UCI::init() {
  return std::thread(UCI::listen);
}
