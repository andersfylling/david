#include "david/Search.h"
#include "david/utils/utils.h"
#include <ctime>
#include <david/EngineMaster.h>
#include <fstream>
#include <future>
#include "david/EngineContext.h"
#include "uci/events.h"
#include "uci/definitions.h"
#include "uci/Listener.h"

namespace david {
//Signals Signal; //Scrapped for now
//clock_t startTime; // moved to a class member


/**
 * Constructor used in debug/test
 */
Search::Search()
    : engineContextPtr(nullptr),
      depth(3),
      uciMode(false),
      isAborted(false),
      isComplete(false),
      movetime(1000)
{}

Search::Search(type::engineContext_ptr ctx)
    : engineContextPtr(ctx),
      depth(3),
      uciMode(false),
      isAborted(false),
      isComplete(false),
      movetime(1000)
{}

void Search::uciSearchWaiter() {
  this->searchThread = std::thread([&](){
//    while (true) {
//
//      if () {}
//    }
  });
}

void Search::uciSearchWaiterJoin() {
  if (this->searchThread.joinable()) {
    this->searchThread.join();
  }
}

void Search::enableUCIMode() {
  this->uciMode = true;
}

/**
 * Set Search variables. searchScore is a class member and
 * should be returned to UCI. Must rewrite to send best move and not "score"
 * @param node
 * @return
 */
 // TODO: return GameTree index in stead of a node copy..
int Search::searchInit() {
  resetSearchValues();

  // Index of best move in TreeGen.tree
   return this->iterativeDeepening();
}


int Search::getSearchResult() {
  return this->bestMoveIndex;
}

/**
 * Main iterative loop, calls negamax until desired depth is reached
 * or the time limit is run out.
 * @param board
 * @return
 */
int Search::iterativeDeepening() {
  int alpha = constant::boardScore::LOWEST;
  int beta = constant::boardScore::HIGHEST;
  int iterationScore[1000];
  //int lastDepth = 0;
  int aspirationDepth = 4;
  int bScore = constant::boardScore::LOWEST;

  this->resetSearchValues();

  startTime = clock();              //Starting clock


  //
  // Create move tree
  //
  this->engineContextPtr->gameTreePtr->generateChildren(0);
  auto nrOfPossibleMoves = this->engineContextPtr->gameTreePtr->getGameState(0).possibleSubMoves;

  //
  // Iterate down in the search tree for each search tree
  //
  time_t initTimer = std::time(nullptr);
  auto timeout = (initTimer * 1000) + movetime;
  for (
      int currentDepth = 1;

      // Continue until max depth or timeout has been reached
      (currentDepth <= this->depth && timeout > (std::time(nullptr) * 1000)) ||
          // Continue forever, or until max depth has been reached.
      (this->infinite && currentDepth < ::david::constant::MAXDEPTH);

      currentDepth++) {

    int aspirationDelta = 0;

    //
    // If the UCI command "stop" is sent, the best move should be returned.
    //
    if (this->isAborted.load()) {
      break;
    }

    //
    // Aspiration window, used to limit alpha beta window, which will increase cut-offs
    //
    // Reset aspiration window starting size
    if (currentDepth >= 5) {
      const int delta = iterationScore[currentDepth - 1] - iterationScore[currentDepth - 2];
      aspirationDelta = std::max(delta, 10) + 5;
      alpha = std::max(iterationScore[currentDepth - 1] - aspirationDelta, constant::boardScore::LOWEST);
      beta = std::min(iterationScore[currentDepth - 1] + aspirationDelta, constant::boardScore::HIGHEST);
    }


    // find which possibility is the best option
    //int leafScore = constant::boardScore::LOWEST;
    //int childIndex = 0;
    for (int index = 1; index <= nrOfPossibleMoves; index += 1) {
      // Since every child is gone through, we need to verify that uci stop command
      // has not been issued (!)
      if (this->isAborted.load()) {
        break;
      }

      // Start with a small aspiration window and, in the case of a fail
      // high/low, re-search with a bigger window until we're not failing
      // high/low anymore.
      bool iDone = false;
      while (!iDone) {

        if (this->isAborted.load()) {
          break;
        }

        int cScore = negamax(index, alpha, beta, 1, currentDepth);
        iterationScore[currentDepth] = cScore;

        //
        // Update best score / move in case of a abort
        //
        if (cScore > bScore) {
          bScore = cScore;
          //leafScore = this->bestLeafScore;
          //std::cout << cScore << std::endl;
          this->bestMoveIndex = index;
        }

        if (this->isAborted.load()) {
          break;
        }

        const bool fail       = bScore <= alpha || bScore >= beta;
        const bool fullWidth  = alpha == constant::boardScore::LOWEST && beta == constant::boardScore::HIGHEST;
        iDone = !fail || fullWidth || currentDepth < aspirationDepth;

        if (!iDone) {
          alpha = std::max(alpha - aspirationDelta, constant::boardScore::LOWEST);
          beta = std::min(beta + aspirationDelta, constant::boardScore::HIGHEST);
          aspirationDelta = std::max(aspirationDelta * 130 / 100, 15);
        }
      }
    }

    // store time used
    this->timeUsed = static_cast<int>((std::time(nullptr) * 1000) - initTimer);

    //lastDepth = currentDepth; // not accurate enough

    // uci info updates
    std::cout
        << "info "
        << "depth " << currentDepth << " "
        << "nodes " << this->nodesSearched << " "
        << std::endl;
  }

  // uci response
//  std::string EGN = "";
//  utils::getEGN(this->engineContextPtr->gameTreePtr->getGameState(0),
//                this->engineContextPtr->gameTreePtr->getGameState(this->bestMoveIndex),
//                EGN);
//  std::cout
//      << "bestmove " << EGN;

  this->searchScore = bScore;

  setComplete(true);

  return this->bestMoveIndex;
}

bool Search::aborted() {
  return this->isAborted.load();
}

/**
 * Negamax routine. Recursive.
 * Searches after best move in a gametree, will call itself until
 * all nodes in the tree is searched trough.
 * Returns best move
 *
 *
 * Local int values need to be changed to gamestate pointers instead
 * @param node
 * @param alpha
 * @param beta
 * @param depth
 * @return
 */
int Search::negamax(unsigned int index, int alpha, int beta, int iDepth, int iterativeDepthLimit) {
  int score = constant::boardScore::LOWEST;
  int bestScore = constant::boardScore::LOWEST;


  //
  // If UCI aborts the search in the middle of a recursive negamax
  // return -infinity
  //
  if (this->isAborted.load()) {
    return constant::boardScore::LOWEST;
  }

  //
  // Should do a quiescence search after to ensure we are not encountering
  // a danger move in the next depth in this branch
  //
  if (iDepth == iterativeDepthLimit) {
    return this->engineContextPtr->gameTreePtr->getGameStateScore(index);
  }

  // generate children for this board
  this->engineContextPtr->gameTreePtr->generateChildren(index);
  const uint16_t len = static_cast<uint16_t>(this->engineContextPtr->gameTreePtr->getGameState(index).possibleSubMoves);

  for (uint16_t i = 0; i < len; i++) { // uint8_t can cause issues if len == 256
    if (this->isAborted.load()) {
      break;
    }

    score = -negamax(this->engineContextPtr->gameTreePtr->getChildIndex(/*parent*/index, /*child0..256*/i),
                     -beta,
                     -alpha,
                     iDepth + 1,
                     iterativeDepthLimit);

    this->nodesSearched += 1;
    bestScore = std::max(score, bestScore);
    alpha = std::max(score, alpha);

    if (alpha >= beta) {
      break;
    }
  }

  return bestScore;
}

/**
 * Called by searchInit, reset/get settings from UCI
 * Mainly used for debugging and progress atm
 */
void Search::resetSearchValues() {
  //this->movetime = 1000; //Hardcoded variables as of now, need to switch to forwards later
  this->searchScore = 0;
  this->nodesSearched = 0;
  this->bestMoveIndex = -1;
}

/**
 * Used to gather statistcs over how much time and nodes a search uses and can search
 * over some iterations
 * @param node
 * @param iterations
 */
//void Search::performanceTest(std::shared_ptr<bitboard::gameState> node, int iterations) {
//  //
//  // For each iteration, save time and nodes searched
//  //
//  double iterationsArray[iterations][2];
//  std::mutex performancetest;
//
//
//  //
//  // Output / statistics
//  //
//  std::string s = "μs";
//#ifdef DAVID_DEBUG
//    std::cout << "--+---------------------+-----------------+\n" <<
//              "   | Time used in iter   |  Nodes searched |\n" <<
//              "--+---------------------+-----------------+\n";
//#endif
//
//  //
//  // Iterations loop, run the test as many times as needed
//  // Mutex to avoid any one else taking cpu time away for
//  // a more accurate result
//  //
//  for (int i = 0; i <= iterations && !this->isAborted; i++) {
//    resetSearchValues();
//    //
//    // Clock time it takes to execute iterative deepening and negamax
//    //
//    performancetest.lock();
//    auto start = std::chrono::steady_clock::now();
//    searchInit(node);
//    auto end = std::chrono::steady_clock::now();
//    performancetest.unlock();
//    auto diff = end - start;
//    iterationsArray[i][0] = std::chrono::duration<double, std::milli>(diff).count();
//    iterationsArray[i][1] = this->nodesSearched;
//
//#ifdef DAVID_DEBUG
//      std::cout << i + 1 << " | ";
//      std::cout << std::setw(10) << iterationsArray[i][0] << s << std::setw(10) <<
//                " | " << std::setw(8) << iterationsArray[i][1] << std::setw(10) << " | ";
//      std::cout << '\n';
//      std::cout << "  +---------------------+-----------------+\n";
//#endif
//  }
//
//  //
//  // Write statistic to csv file
//  //
//  std::ofstream searchStats;
//  searchStats.open("search_statistics.csv");
//  for (int i = 0; i < iterations; i++) {
//    searchStats << iterationsArray[i][0] << ',' << iterationsArray[i][1] << ',' << i << std::endl;
//  }
//  searchStats.close();
//}

/**
 * If the UCI at any time tells search to abort
 */
void Search::stopSearch() {
  setAbort(true);
}

void Search::quitSearch() {
}

/**
 * Set search depth, recieved by uci
 * @param depth
 */
void Search::setDepth(int depth) {
  this->depth = depth;
}


void Search::setSearchMoves(std::string moves) {
  this->searchMoves = moves;
}

void Search::setMovesToGo(int movestogo) {
  this->movestogo = movestogo;
}
void Search::setNodes(int nodes) {
  this->nodes = nodes;
}

/**
 * Set how much time to search
 * @param movetime
 */
void Search::setMoveTime(int movetime) {
  this->movetime = movetime;
}
void Search::setMate(int mate) {
  this->mate = mate;
}
void Search::setInfinite(bool infinite) {
  this->infinite = infinite;
} // bool ?
void Search::setPonder(bool ponder) {
  this->ponder = ponder;
} // bool ?

/**
 * Used to set game difficulty
 * @param difficulty
 */
void Search::setDifficulty(int difficulty) {
  switch (difficulty){
    case 1: this->setDepth(4); this->setMoveTime(100000); break;
    case 2: this->setDepth(5); this->setMoveTime(150000); break;
    case 3: this->setDepth(7); this->setMoveTime(200000); break;
    default: this->setDepth(5); this->setMoveTime(150000); break;
  }
}

/**
 * Returns depth to be searched, only used in debug
 * @return
 */
int Search::returnDepth()  {
  return this->depth;
}
/**
 * Returns how much time allocated to the search
 * @return
 */
int Search::returnTimeToSearch(){
  return this->movetime;
}
/**
 * Returns bestScore sat by searching
 * @return
 */
int Search::returnScore() {
  return this->searchScore;
}
/**
 * Returns if the search is complete, search not aborted and completed without issues
 * @return
 */
bool Search::returnComplete() {
  return this->isComplete;
}

int Search::getTimeUsed() {
  return this->timeUsed;
}


/**
 * Set aborted search
 * @param isAborted
 */
void Search::setAbort(bool isAborted) {
  this->isAborted = isAborted;
}

/**
 * Set complete search
 * @param isComplete
 */
void Search::setComplete(bool isComplete) {
  this->isComplete = isComplete;
}
}