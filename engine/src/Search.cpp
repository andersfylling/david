//
// Created by anders on 5/3/17.
//

#include "Search.h"
using namespace search;

//Root search
void Search::search(/*Pseudo Node*/) {
    resetSearchValues();

    searchScore = iterativeDeepening(/*Pseudo node*/);
}

int Search::iterativeDeepening() {
    int bestScore = std::numeric_limits<int>::min();
    int depth = 1;

    /**/

    return 0;
}

int Search::negamax() {
    return 0;
}

void Search::resetSearchValues() {

}


