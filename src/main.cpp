#include "enviornment.h"
#include "bitboard.h"

#include <iostream>

int main (int argc, char * argv[])
{

  bitboard testt = 0LL;
  Enviornment test(COLOR::WHITE);

  std::cout << test.MSB(BP) << std::endl;

  bitboard * bits = test.getDiagYAxis(WP, UP);
  test.printBoard(test.generateBlocK(bits[0], UP, BP));

  return 0;
}