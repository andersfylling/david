#include "environment.h"
#include "bitboard.h"

#include <iostream>

int main (int argc, char * argv[])
{

  Enviornment test(WHITE);

  bitboard * bits = test.getXAxisFromBoard(WQ);
  test.printBoard(*bits & ~test.generateBlocK(*bits, UP, BP));


  //std::cout << test.MSB(BP) << std::endl;

  //bitboard * bits = test.getDiagYAxis(WP, UP);
  //test.printBoard(test.generateBlock(bits[0], UP, BP));

  return 0;
}