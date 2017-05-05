//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"

//
// Constructor
//
search::Search::Search(::uci::Listener &uci) {
  uci.addClassListener(::uci::event::QUIT, std::ref(uci_quit));
  uci.addClassListener(::uci::event::STOP, std::ref(uci_stop));
  uci.addClassListener(::uci::event::GO, uci_go);

  uci.addListener(::uci::event::GO, [&](::uci::arguments_t args){ std:: cout << 4;});
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

static void search::Search::uci_go_depth(::uci::arguments_t args) {
  std::string d = args["depth"];


  // convert to int or whatever:
  int depth = std::stoi(d);
  std::cout << depth << std::endl;
}

static void search::Search::uci_go(uci::arguments_t args) {
  if (args.count("depth") > 0) {
    uci_go_depth(args);
  }
}

void search::Search::uci_stop(::uci::arguments_t args) {}
void search::Search::uci_quit(::uci::arguments_t args) {}

