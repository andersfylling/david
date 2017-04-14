
#include <iostream>
#include <UCIListener.h>
#include <vector>
int main (int argc, char * argv[])
{
  std::vector<std::thread> threads;

  // Add threads
  threads.push_back(UCI::init());

  // join threads
  for (auto& t : threads) {
    t.join();
  }

  return 0;
}
