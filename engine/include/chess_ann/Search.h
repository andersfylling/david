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
#include "chess_ann/GameTree.h"
#include <fstream>
#include <chrono>
#include <time.h>
#include <memory>
#include <algorithm>
#include <mutex>
#include "chess_ann/Context.h"

using ::bitboard::COLOR;

namespace search {


/**
 * Search class is responsible for searching through a gamestate tree
 * bestScore is stored in object, this is the heuristic og a move
 * bestMove stores the gamestate which is the best throughout.
 * Should return a fen string or something to UCI
 */
class Search {
 public:
  Search(std::shared_ptr<chess_ann::Context> context); // This can be used for unit testing and benchmarking.
  Search(std::shared_ptr<chess_ann::Context> context, ::uci::Listener &uci);
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
  void setDebug(bool debug);
  void performanceTest(std::shared_ptr<::bitboard::gameState> node, int iterations);

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
  bool debug;
  int nodesSearched;
  std::shared_ptr<chess_ann::Context> context;
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


  /**
   * Set aborted search
   * @param isAborted
   */
  inline void Search::setAbort(bool isAborted) {
    this->isAborted = isAborted;
  }

  /**
   * Set complete search
   * @param isComplete
   */
  inline void Search::setComplete(bool isComplete) {
    this->isComplete = isComplete;
  }

  /**
   * Disable/enable some window output
   * @param debug
   */
  inline void Search::setDebug(bool debug) {
    this->debug == debug;
  }



}

#endif //PROJECT_SEARCH_H
