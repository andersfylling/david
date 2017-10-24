#pragma once


// local dependencies
#include "david/types.h"
#include "david/bitboard.h"
#include "david/TreeGen.h"

// system dependencies
#include <string>
#include <atomic>
#include <future>
#include <thread>


// forward declarations
#include "david/forwards/EngineContext.h"
#include "uci/forwards/Listener.h"


namespace david {

/**
 * Search class is responsible for searching through a gamestate tree
 * bestScore is stored in object, this is the heuristic og a move
 * bestMove stores the gamestate which is the best throughout.
 * Should return a fen string or something to UCI
 */
class Search {
 public:
  Search(type::TreeGen_t& tg);
  Search(const Search&) = delete;             // delete the copy constructor
  void operator=(const Search&) = delete;     // delete the copy-assignment operator
  int searchInit();
  int iterativeDeepening();
  int negamax(unsigned int index, int alpha, int beta, int depth, int iterativeDepthLimit);
  void setAbort(bool isAborted);
  void setComplete(bool isComplete);
  //std::future<int> searchInstance;
  int getSearchResult();

  void uciSearchWaiter();
  void uciSearchWaiterJoin();

  void enableUCIMode();


  //Test/debug
  int returnDepth();
  int returnTimeToSearch();
  int returnScore();
  bool returnComplete();
  bool aborted();
  void performanceTest(type::gameState_t& node, int iterations);

  // forwards protocol methods, this can be used in unit testing
  void stopSearch();
  void quitSearch();
  void setDepth(int depth);
  void setSearchMoves(std::string moves);
  void setWTime(int wtime);
  void setBTime(int btime);
  void setWinc(int winc);
  void setBinc(int binc);
  void setMovesToGo(int movestogo);
  void setNodes(uint64_t nodes);
  void setMoveTime(int movetime);
  void setMate(int mate);
  void setInfinite(bool inf);
  void setPonder(bool ponder);
  void setDifficulty(int difficulty);
  int getTimeUsed();

  clock_t startTime;

 private:
  type::TreeGen_t& treeGen;

  bool uciMode;
  std::thread searchThread;
  int depth;
  int movestogo;
  int movetime;
  int timeUsed;
  int mate;
  bool infinite;
  bool ponder;
  uint64_t nodes;
  std::string searchMoves;
  int searchScore;
  type::gameState_t bestMove;
  int bestMoveIndex;
  int /*time[COLOR], inc[COLOR],*/ npmsec;
  //void uciOutput();
  void resetSearchValues();
  std::atomic<bool> isAborted;
  int currentSearchID;
  bool isComplete;
  bool debug;
  uint64_t nodesSearched;
};





}