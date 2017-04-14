
#include <iostream>
#include <listener.h>
#include <vector>



int main (int argc, char * argv[])
{
  // Store threads
  std::vector<std::thread> threads;

  // Add threads
  threads.push_back(UCI::init());

  // join threads
  for (auto& t : threads) {
    t.join();
  }

  // Close program with exit code 0
  return 0;
}
