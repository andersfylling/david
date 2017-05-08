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
#include "chess_ann/GameTree.h"
#include <time.h>
#include <memory>
#include <algorithm>

using ::bitboard::COLOR;

namespace search {

/*struct Signals{
  std::atomic_bool stop;
};


extern Signals Signal;*/

class Search {
 public:
  Search(); // This can be used for unit testing and benchmarking.
  Search(::uci::Listener &uci);
  void searchInit(std::shared_ptr<::bitboard::gameState> node);
  int iterativeDeepening(std::shared_ptr<::bitboard::gameState> node);
  int negamax(std::shared_ptr<::bitboard::gameState> board, int alpha, int beta, int depth);
  void setAbort(bool isAborted);
  void setComplete(bool isComplete);

  //Test/debug
  int returnDepth();
  int returnTimeToSearch();
  int returnScore();
  bool returnComplete();

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
  std::shared_ptr<::bitboard::gameState> bestMove;
  int /*time[COLOR], inc[COLOR],*/ npmsec;
  //void uciOutput();
  void resetSearchValues();
  bool isAborted;
  bool isComplete;
};

  //
  // Debug functions
  //
  inline int Search::returnDepth()  {
    return this->depth;
  }

  inline int Search::returnTimeToSearch(){
    return this->movetime;
  }

  inline int Search::returnScore() {
    return this->searchScore;
  }

  inline bool Search::returnComplete() {
    return this->isComplete;
  }

  //
  //
  //
  inline void Search::setAbort(bool isAborted) {
    this->isAborted = isAborted;
  }

  inline void Search::setComplete(bool isComplete) {
    this->isComplete = isComplete;
  }



}

#endif //PROJECT_SEARCH_H
