//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"

//
// Constructor
//
search::Search::Search(::uci::Listener &uci) {
  uci.addListener(::uci::event::GO, this->uci_go);
}

//Root search
void search::Search::searchInit(/*Pseudo Node*/) {
  resetSearchValues();

  searchScore = iterativeDeepening(/*Pseudo node*/);
}

int search::Search::iterativeDeepening() {
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

int search::Search::negamax() {
  return 0;
}

void search::Search::resetSearchValues() {

}

void search::Search::setDepth(::uci::arguments_t args) {
  std::string d = args["depth"];


  // convert to int or whatever:
  int depth = d == "" ? 0 : std::stoi(d);
  std::cout << depth << std::endl;
}

void search::Search::stopSearch(::uci::arguments_t args) {}
void search::Search::quitSearch(::uci::arguments_t args) {}

