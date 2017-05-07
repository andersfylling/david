#ifndef PROJECT_SEARCH_H
#define PROJECT_SEARCH_H

#include <iostream>
#include <string>
#include <map>
#include "chess_ann/bitboard.h"
#include "chess_ann/variables.h"
#include "chess_ann/uci/Parser.h"
#include "chess_ann/uci/events.h"
#include <chess_ann/uci/Listener.h>

using ::bitboard::COLOR;

namespace search {

class Search {
 public:
  Search(); // This can be used for unit testing and benchmarking.
  Search(::uci::Listener &uci);
  void searchInit(/*Pseudo node*/);
  int iterativeDeepening(/*Pseudo node*/);
  int negamax(/*Pseudo node*/);


  // uci protocol methods, this can be used in unit testing
  void stopSearch();
  void quitSearch();
  void setDepth(int depth);
  void setSearchMoves(std::string moves);
  void setWTime(int wtime);
  void setBTime(int btime);
  void setWinc(int winc);
  void setBinc(int binc);
  void setMovesToGo(int movestogo);
  void setNodes(int nodes);
  void setMoveTime(int movetime);
  void setMate(int mate);
  void setInfinite(int mate); // bool ?
  void setPonder(int ponder); // bool ?

 private:
  int depth;
  int movestogo;
  int movetime;
  int mate;
  int infinite;
  int ponder;
  std::string searchMoves;
  int searchScore;
  int /*time[COLOR], inc[COLOR],*/ npmsec;
  //void uciOutput();
  void resetSearchValues();
};

}

#endif //PROJECT_SEARCH_H
