//
// Created by anders on 5/3/17.
//

#ifndef PROJECT_SEARCH_H
#define PROJECT_SEARCH_H

#include <limits>

namespace search {

class Search {
public:
    void search(/*Pseudo node*/);
    int iterativeDeepening(/*Pseudo node*/);
    int negamax(/*Pseudo node*/);
private:
    int searchDepth;
    int searchScore;
    void uciOutput();
    void resetSearchValues();

};

    inline void Search::uciOutput() {

    }
}

#endif //PROJECT_SEARCH_H
