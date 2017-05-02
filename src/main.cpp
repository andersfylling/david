#include "../lib/Catch/single_include/catch.hpp"


#include <limits>
#include "./enviornment.h"
#include "./bitboard.h"
#include "../include/search.h"
#include "variables.h"
#include <iostream>

using namespace std;
//
//Main
//

int main () {
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    /* Enviornment test(COLOR::BLACK);
     bitboard white = test.whitePieces();
     test.printBoard(test.blackPieces());*/

    Node * root;
    root = new Node(4, -1);
    root->print();

   cout << endl << endl;
    int  bestvalue;
    bestvalue = negamax::negamax(root, alpha, beta, MAX_DEPTH);
    cout << bestvalue << endl << endl;
    return 0;
}

