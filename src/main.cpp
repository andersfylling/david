#include "../lib/Catch/single_include/catch.hpp"


#include "./enviornment.h"
#include "./bitboard.h"
#include <iostream>

//
//Main
//

int main () {
  Enviornment test(COLOR::BLACK);
  bitboard white = test.whitePieces();
  test.printBoard(test.blackPieces());

}