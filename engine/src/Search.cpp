//
// Created by anders on 5/3/17.
//

#include "david/Search.h"
#include "david/utils.h"
#include <ctime>
#include <david/EngineMaster.h>
#include <fstream>
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
    : depth(5)
{}

Search::Search(type::engineContext_ptr ctx)
    : engineContextPtr(ctx),
      depth(5)
{}

/**
 * Set Search variables. searchScore is a class member and
 * should be returned to UCI. Must rewrite to send best move and not "score"
 * @param node
 */
type::gameState_ptr Search::searchInit(type::gameState_ptr node) {
  resetSearchValues();
  //std::cout << "Search depth sat to: " << this->depth << std::endl;  //Debug
  //std::cout << "Search time sat to: " << this->movetime << std::endl;  //Debug

  this->searchScore = iterativeDeepening(node);

  if (!this->bestMove) {
    std::cerr << "bestMove is empty!" << std::endl;
  }

#ifdef DAVID_DEBUG
    std::vector<int>::iterator it;
    std::cout << "Search objects score after complete search: " << this->searchScore << std::endl;  //Debug
    for(it=this->expanded.begin(); it<this->expanded.end(); it++)
      std::cout << ' ' << *it;
    std::cout << std::endl;
#endif
  //
  // Send some values/fenstring or whatever to UCI
  //
  return this->bestMove;
}

/**
 * Main iterative loop, calls negamax until desired depth is reached
 * or the time limit is run out.
 * @param board
 * @return
 */
int Search::iterativeDeepening(type::gameState_ptr board) {
  int alpha = constant::boardScore::LOWEST;
  int beta = constant::boardScore::HIGHEST;
  int iterationScore[1000];
  int lastDepth = 0;
  int aspirationDepth = 4;
  int bScore = constant::boardScore::LOWEST;

  std::vector<int> scoreCache; // store unsorted scores (!), sort every node after unchached results. Rewrite negamax.

  startTime = clock();              //Starting clock


  //
  // Create move tree
  //
  // board->generateAllMoves();
  //
    if(!engineContextPtr->testing) {
      gtPtr = std::make_shared<type::gameTree_t>(this->engineContextPtr, board);

      if (board->children.size() == 0) {
        gtPtr->generateChildren(board);
      }
    }




  //
  // Iterate down in the search tree for each search tree
  //
  time_t initTimer = std::time(nullptr);
  auto timeout = (initTimer * 10000) + movetime;
  for (int currentDepth = 1; currentDepth <= this->depth && timeout > (std::time(nullptr) * 1000); currentDepth++) {
    int cScore = bScore;
    int aspirationDelta = 0;

    //
    // If UCI aborts the search, no move should be returned and -infinity will be returned
    //
    if (isAborted) {
      //bScore = (int) (-INFINITY);
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
    int leafScore = constant::boardScore::LOWEST;
    int childIndex = 0;
    for (auto child : board->children) {
      // Start with a small aspiration window and, in the case of a fail
      // high/low, re-search with a bigger window until we're not failing
      // high/low anymore.
      bool iDone = false;
      while (!iDone) {
        cScore = negamax(child, alpha, beta, 0, currentDepth);
        iterationScore[currentDepth] = cScore;

        // destroy and remove children
        for (auto cc : child->children) {
          cc.reset();
        }
        child->children.resize(0);

        // Store the scores in the cache
        // so far this only stores the cache for the first depth
        if (scoreCache.size() == childIndex) {
        //  scoreCache.push_back(cScore);
        }
        else {
          // cached score already exists.
        }

        //
        // Update best score in case of a abort
        //
        if (cScore > bScore) {
          bScore = cScore;
          //leafScore = this->bestLeafScore;
          //std::cout << cScore << std::endl;
          this->bestMove.reset();
          this->bestMove = std::make_shared<type::gameState_t>(*child); // copy
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

    lastDepth = currentDepth; // not accurate enough

    std::cout << "info depth " << currentDepth << ", max: " << this->depth << std::endl;
  }

  setComplete(true);

  this->gtPtr.reset();

  return bScore;
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
int Search::negamax(type::gameState_ptr node, int alpha, int beta, int iDepth, int iterativeDepthLimit) {
  int score;
  int bestScore = constant::boardScore::LOWEST;


  //
  // If UCI aborts the search in the middle of a recursive negamax
  // return -infinity
  //
  if (isAborted) {
    return constant::boardScore::LOWEST;
  }

  //
  // Should do a quiescence search after to ensure we are not encountering
  // a danger move in the next depth in this branch
  //
  if (iDepth == iterativeDepthLimit) {
    return node->score;
  }

  // generate children for this board
  if (node->children.size() == 0) {
    gtPtr->generateChildren(node);
  }

  for (auto child : node->children) {
    score = -negamax(child, -beta, -alpha, iDepth + 1, iterativeDepthLimit);
    this->nodesSearched++;
    bestScore = std::max(score, bestScore);
    alpha = std::max(score, alpha);

#ifdef DAVID_DEBUG
    std::cout << "Alpha: " << alpha << " Beta: " << beta << std::endl;
    this->expanded.push_back(child->score);
#endif

    if (alpha >= beta) {
      child.reset();
      // TODO[CRITICAL]: need to reset remaining children... Use for loop with indexing
      break;
    }

    // after a node is checked, remove it
    child.reset();
  }

  return bestScore;
}

/**
 * Called by searchInit, reset/get settings from UCI
 * Mainly used for debugging and progress atm
 */
void Search::resetSearchValues() {
  this->movetime = 1000; //Hardcoded variables as of now, need to switch to forwards later
  this->searchScore = 0;
  this->nodesSearched = 0;
  this->expanded.clear();
}

/**
 * Used to gather statistcs over how much time and nodes a search uses and can search
 * over some iterations
 * @param node
 * @param iterations
 */
void Search::performanceTest(std::shared_ptr<bitboard::gameState> node, int iterations) {
  //
  // For each iteration, save time and nodes searched
  //
  double iterationsArray[iterations][2];
  std::mutex performancetest;


  //
  // Output / statistics
  //
  std::string s = "μs";
#ifdef DAVID_DEBUG
    std::cout << "--+---------------------+-----------------+\n" <<
              "   | Time used in iter   |  Nodes searched |\n" <<
              "--+---------------------+-----------------+\n";
#endif

  //
  // Iterations loop, run the test as many times as needed
  // Mutex to avoid any one else taking cpu time away for
  // a more accurate result
  //
  for (int i = 0; i <= iterations && !this->isAborted; i++) {
    resetSearchValues();
    //
    // Clock time it takes to execute iterative deepening and negamax
    //
    performancetest.lock();
    auto start = std::chrono::steady_clock::now();
    searchInit(node);
    auto end = std::chrono::steady_clock::now();
    performancetest.unlock();
    auto diff = end - start;
    iterationsArray[i][0] = std::chrono::duration<double, std::milli>(diff).count();
    iterationsArray[i][1] = this->nodesSearched;

#ifdef DAVID_DEBUG
      std::cout << i + 1 << " | ";
      std::cout << std::setw(10) << iterationsArray[i][0] << s << std::setw(10) <<
                " | " << std::setw(8) << iterationsArray[i][1] << std::setw(10) << " | ";
      std::cout << '\n';
      std::cout << "  +---------------------+-----------------+\n";
#endif
  }

  //
  // Write statistic to csv file
  //
  std::ofstream searchStats;
  searchStats.open("search_statistics.csv");
  for (int i = 0; i < iterations; i++) {
    searchStats << iterationsArray[i][0] << ',' << iterationsArray[i][1] << ',' << i << std::endl;
  }
  searchStats.close();
}

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

void Search::setWTime(int wtime) {
  this->wtime = wtime;
}
void Search::setBTime(int btime) {
  this->btime = btime;
}
void Search::setWinc(int winc) {
  this->winc = winc;
}
void Search::setBinc(int binc) {
  this->binc = binc;
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
void Search::setInfinite(int infinite) {
  this->infinite = infinite;
} // bool ?
void Search::setPonder(int ponder) {
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