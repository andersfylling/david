#ifndef PROJECT_SEARCH_H
#define PROJECT_SEARCH_H

#include <iostream>
#include <string>
#include <map>
#include "chess_ann/bitboard.h"
#include "chess_ann/variables.h"
#include "chess_ann/uci/Parser.h"
#include "chess_ann/uci/UCIEvent.h"
#include <chess_ann/uci/Listener.h>

using ::bitboard::COLOR;

namespace search {

class Search {
 public:
  Search(::uci::Listener &uci);
  void searchInit(/*Pseudo node*/);
  int iterativeDeepening(/*Pseudo node*/);
  int negamax(/*Pseudo node*/);
 private:
  int searchScore;
  int /*time[COLOR], inc[COLOR],*/ npmsec, movestogo, depth, movetime, mate, infinite, ponder;
  //void uciOutput();
  void resetSearchValues();

  // uci protocol functions, used for uci protocol events
  ::uci::callback_t uci_go = [&](::uci::arguments_t args){
    if (args.count("depth") > 0) {
      this->setDepth(args);
    }
  };
  ::uci::callback_t uci_stop = [&](::uci::arguments_t args){
    this->stopSearch(args);
  };
  ::uci::callback_t uci_quit = [&](::uci::arguments_t args){
    this->quitSearch(args);
  };

  // uci protocol methods, this can be used in unit testing
  void setDepth(::uci::arguments_t args);
  void stopSearch(::uci::arguments_t args);
  void quitSearch(::uci::arguments_t args);
};

}

#endif //PROJECT_SEARCH_H
