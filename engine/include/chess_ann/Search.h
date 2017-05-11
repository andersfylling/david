#pragma once


// local dependencies
#include "chess_ann/definitions.h"
#include "chess_ann/bitboard.h"
#include "chess_ann/variables.h"
#include "chess_ann/GameTree.h"

// system dependencies
#include <string>


// forward declarations
#include "chess_ann/forwards/EngineContext.h"
#include "chess_ann/forwards/uci/Listener.h"


namespace search {
using ::bitboard::COLOR;


/**
 * Search class is responsible for searching through a gamestate tree
 * bestScore is stored in object, this is the heuristic og a move
 * bestMove stores the gamestate which is the best throughout.
 * Should return a fen string or something to UCI
 */
class Search {
 public:
  Search(definitions::engineContext_ptr ctx); // This can be used for unit testing and benchmarking.
  Search(definitions::engineContext_ptr ctx, ::uci::Listener& uci);
  definitions::gameState_ptr searchInit(definitions::gameState_ptr node);
  int iterativeDeepening(definitions::gameState_ptr node);
  int negamax(definitions::gameState_ptr board, int alpha, int beta, int depth);
  void setAbort(bool isAborted);
  void setComplete(bool isComplete);

  //Test/debug
  int returnDepth();
  int returnTimeToSearch();
  int returnScore();
  bool returnComplete();
  void setDebug(bool debug);
  void performanceTest(definitions::gameState_ptr node, int iterations);

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
  definitions::gameState_ptr bestMove;
  int /*time[COLOR], inc[COLOR],*/ npmsec;
  //void uciOutput();
  void resetSearchValues();
  bool isAborted;
  bool isComplete;
  bool debug;
  int nodesSearched;
  definitions::engineContext_ptr engineContextPtr;
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