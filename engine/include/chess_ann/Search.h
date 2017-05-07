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
#include <atomic>
#include <time.h>
#include <memory>

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
  void setIterationComplete(bool isCompleted);

  //Test/debug
  bool returnMembers();

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

  inline bool Search::returnMembers() {
    if(this->depth)
      return true;
    else
      return false;
  }

}

#endif //PROJECT_SEARCH_H
