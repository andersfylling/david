//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"
using namespace search;

//
// Constructor
//
Search::Search(::uci::Listener &uci) {
    uci.addListener(::uci::event::QUIT, this->uci_quit);
    uci.addListener(::uci::event::STOP, this->uci_stop); // etc
}

//Root search
void Search::searchInit(/*Pseudo Node*/) {
    resetSearchValues();

    searchScore = iterativeDeepening(/*Pseudo node*/);
}

int Search::iterativeDeepening() {
    int bestScore = -VALUE_INFINITE;
    int aplha = -VALUE_INFINITE;
    int beta = VALUE_INFINITE;
    int lastDepth = 0;

    /*Must create a move tree based on the root node sent to iterative Deepening*/

    //
    // Iterate down in the search tree for each search tree
    //
    for(int currentDepth = 1; currentDepth <= depth; currentDepth++){
        int score = -VALUE_INFINITE;
        lastDepth = currentDepth;


        bool finished = false;
        while(!finished){
            //
            // Do negamax
            //
        }
    }
    return 0;
}

int Search::negamax() {
    return 0;
}

void Search::resetSearchValues() {

}

void Search::uci_go_depth(::uci::arguments_t args) {
    std::string d = args.at("depth").second;


    // convert to int or whatever:
    int depth = std::stoi(d);
}

void Search::uci_go(uci::arguments_t args) {
    if (args.count("depth") > 0) {
        this->uci_go_depth(args);
    }
}


