#include "environment.h"
#include "bitboard.h"

#include <iostream>

int main (int argc, char * argv[])
{

  bitboard testt = 0LL;
  Environment test(COLOR::WHITE);

  std::cout << test.MSB(BP) << std::endl;

  bitboard * bits = test.getDiagYAxis(WP, UP);
  test.printBoard(test.generateBlock(bits[0], UP, BP));

  return 0;
}