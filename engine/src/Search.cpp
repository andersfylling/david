//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"
#include "chess_ann/utils.h"
#include <ctime>

namespace search {
//Signals Signal; //Scrapped for now
clock_t startTime;
}

/**
 * Constructor used in debug/test
 */
search::Search::Search() {

};

search::Search::Search(::uci::Listener &uci) {
  using ::uci::event::GO;
  using ::uci::event::STOP;
  using ::uci::event::QUIT;
  using ::uci::event::PONDERHIT;
  using ::uci::event::UCINEWGAME;
  using ::uci::arguments_t;


  // uci protocol functions, used for uci protocol events
  auto uci_go = [&](arguments_t args) {
    // All of the "go" parameters
    if (args.count("depth") > 0) {
      this->setDepth(::utils::stoi(args["depth"]));
    } else if (args.count("searchmoves") > 0) {
      this->setSearchMoves(args["searchmoves"]);
    } else if (args.count("wtime") > 0) {
      this->setWTime(::utils::stoi(args["wtime"]));
    } else if (args.count("btime") > 0) {
      this->setBTime(::utils::stoi(args["btime"]));
    } else if (args.count("winc") > 0) {
      this->setWinc(::utils::stoi(args["winc"]));
    } else if (args.count("binc") > 0) {
      this->setBinc(::utils::stoi(args["binc"]));
    } else if (args.count("movestogo") > 0) {
      this->setMovesToGo(::utils::stoi(args["movestogo"]));
    } else if (args.count("nodes") > 0) {
      this->setNodes(::utils::stoi(args["nodes"]));
    } else if (args.count("movetime") > 0) {
      this->setMoveTime(::utils::stoi(args["movetime"]));
    } else if (args.count("mate") > 0) {
      this->setMate(::utils::stoi(args["mate"]));
    } else if (args.count("infinite") > 0) {
      this->setInfinite(::utils::stoi(args["infinite"]));
    } else if (args.count("ponder") > 0) {
      this->setPonder(::utils::stoi(args["ponder"]));
    }
  };
  auto uci_stop = [&](arguments_t args) {
    this->stopSearch();
  };
  auto uci_quit = [&](arguments_t args) {
    this->quitSearch();
  };
  auto uci_ponderhit = [&](arguments_t args) {
//     * ponderhit
//	the user has played the expected move. This will be sent if the engine was told to ponder on the same move
//	the user has played. The engine should continue searching but switch from pondering to normal search.
  };
  auto uci_ucinewgame = [&](arguments_t args) {
//    * ucinewgame
//    this is sent to the engine when the next search (started with "position" and "go") will be from
//    a different game. This can be a new game the engine should play or a new game it should analyse but
//    also the next position from a testsuite with positions only.
//      If the GUI hasn't sent a "ucinewgame" before the first "position" command, the engine shouldn't
//    expect any further ucinewgame commands as the GUI is probably not supporting the ucinewgame command.
//      So the engine should not rely on this command even though all new GUIs should support it.
//      As the engine's reaction to "ucinewgame" can take some time the GUI should always send "isready"
//    after "ucinewgame" to wait for the engine to finish its operation.
  };


  // bind the listeners
  uci.addListener(GO, uci_go);
  uci.addListener(STOP, uci_stop);
  uci.addListener(QUIT, uci_quit);
  uci.addListener(PONDERHIT, uci_ponderhit);
  uci.addListener(UCINEWGAME, uci_ucinewgame);
}

/**
 * Set Search variables. searchScore is a class member and
 * should be returned to UCI. Must rewrite to send best move and not "score"
 * @param node
 */
void search::Search::searchInit(std::shared_ptr<::bitboard::gameState> node) {
  resetSearchValues();
  //std::cout << "Search depth sat to: " << this->depth << std::endl;  //Debug
  //std::cout << "Search time sat to: " << this->movetime << std::endl;  //Debug

  this->searchScore = iterativeDeepening(node);

  if(this->debug)
    std::cout << "Search objects score after complete search: " << this->searchScore<< std::endl;  //Debug
  //
  // Send some values/fenstring or whatever to UCI
  //
}

/**
 * Main iterative loop, calls negamax until desired depth is reached
 * or the time limit is run out.
 * @param board
 * @return
 */
int search::Search::iterativeDeepening(std::shared_ptr<::bitboard::gameState> board) {
  int alpha = (int)(-INFINITY);
  int beta = (int)(INFINITY);
  int iterationScore[1000];
  int lastDepth = 0;
  int aspirationDepth = 4;
  startTime = clock();              //Starting clock
  int bestScore = (int)(-INFINITY);


  //
  // Create move tree
  //
  // board->generateAllMoves();
  //
  ::gameTree::GameTree rMoves(board);
  rMoves.setMaxNumberOfNodes(100000);
  rMoves.generateChildren(board);




  //
  // Iterate down in the search tree for each search tree
  //
  time_t initTimer = std::time(nullptr);
  auto timeout = (initTimer * 10000) + movetime;
  for (int currentDepth = 1; currentDepth <= depth && timeout > (std::time(nullptr) * 1000); currentDepth++) {
    int currentBestScore;
    int aspirationDelta = 0;

    //
    // If UCI aborts the search, no move should be returned and -infinity will be returned
    //
    if (isAborted) {
      bestScore = (int) (-INFINITY);
      break;
    }

    //
    // Aspiration window, used to limit alpha beta window, which will increase cut-offs
    //
    // Reset aspiration window starting size
    if (currentDepth >= 5) {
      const int delta = iterationScore[currentDepth - 1] - iterationScore[currentDepth - 2];
      aspirationDelta = std::max(delta, 10) + 5;
      alpha = std::max(iterationScore[currentDepth - 1] - aspirationDelta, (int) (-INFINITY));
      beta = std::min(iterationScore[currentDepth - 1] + aspirationDelta, (int) (+INFINITY));
    }


    // Start with a small aspiration window and, in the case of a fail
    // high/low, re-search with a bigger window until we're not failing
    // high/low anymore.
    bool iDone = false;
    while (!iDone) {
      currentBestScore = negamax(board, alpha, beta, currentDepth);
      iterationScore[currentDepth] = currentBestScore;

      //Update best score incase of a abort
      bestScore = (bestScore > currentBestScore) ? bestScore : currentBestScore;

      const bool fail = bestScore <= alpha || bestScore >= beta;
      const bool fullWidth = alpha == (int)(-INFINITY) && beta == (int)(INFINITY);
      iDone = !fail || fullWidth || currentDepth < aspirationDepth;

      if (!iDone) {
        alpha = std::max(alpha - aspirationDelta, (int)(-INFINITY));
        beta = std::min(beta + aspirationDelta, (int)(INFINITY));
        aspirationDelta = std::max(aspirationDelta * 130 / 100, 15);
      }
    }
  }

  setComplete(true);
  //Does not return a move yet, however Search.bestMove is sat in negamax
  //std::cout << "Score after iterative deepening search complete: " << bestMove << std::endl;  //Debug
  this->bestMove = board;
  return bestScore;
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
int search::Search::negamax(std::shared_ptr<::bitboard::gameState> node, int alpha, int beta, int iDepth) {
  int score;
  int bestScore = (int)(-INFINITY);


  //
  // If UCI aborts the search in the middle of a recursive negamax
  // return -infinity
  //
  if(isAborted){
    return (int)(-INFINITY);
  }

  //
  // Need to regenerate the incomming nodes children
  //

  //
  // Should do a quiescence search after to ensure we are not encountering
  // a danger move in the next depth in this branch
  //
  if (iDepth == depth) {
    return node->score;
  }


  //Node->children does not return correct type atm
  for (auto child : node->children) {
    score = -negamax(child, -beta, -alpha, iDepth+1); // usually start at node 0, which means this will loop forever..
    this->nodesSearched++;
    bestScore = std::max(score, bestScore);
    alpha = std::max(score, alpha);
    if(this->debug)
      std::cout << "Alpha: " << alpha << " Beta: " << beta << std::endl;
    if(alpha >= beta) {
      if(this->debug)
        std::cout << "Nodes children pruned\n";
      break;
    }
  }
  return bestScore;
}


/**
 * Called by searchInit, reset/get settings from UCI
 * Mainly used for debugging and progress atm
 */
void search::Search::resetSearchValues() {
  this->movetime = 10000; //Hardcoded variables as of now, need to switch to uci later
  this->depth = 6;
  this->searchScore = 0;
  this->nodesSearched = 0;
}

void search::Search::performanceTest(std::shared_ptr<::bitboard::gameState> node, int iterations) {
  //
  // For each iteration, save time and nodes searched
  //
  double iterationsArray[iterations][2];
  std::mutex performancetest;

  //
  // Iterations loop, run the test as many times as needed
  // Mutex to avoid any one else taking cpu time away for
  // a more accurate result
  //
  for(int i = 0; i <= iterations && !this->isAborted; i++){
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
    iterationsArray[i][0] = std::chrono::duration <double, std::milli> (diff).count();
    iterationsArray[i][1] = this->nodesSearched;
  }


  //
  // Output / statistics
  //
  std::string s = "μs";
  std::cout << "--+---------------------+-----------------+\n" <<
               "   | Time used in iter   |  Nodes searched |\n" <<
               "--+---------------------+-----------------+\n";
  for (int i = 0; i < iterations; i++) {
    std::cout << i + 1 << " | ";
    std::cout << std::setw(10) << iterationsArray[i][0] << s << std::setw(10) <<
      " | " << std::setw(8) << iterationsArray[i][1] << std::setw(10) << " | ";
    std::cout << '\n';
    std::cout << "  +---------------------+-----------------+\n";
  }

  //
  // Write statistic to csv file
  //
  std::ofstream searchStats;
  searchStats.open ("search_statistics.csv");
  for(int i = 0; i < iterations; i++){
    searchStats << iterationsArray[i][0] << ',' << iterationsArray[i][1] << ',' << i << std::endl;
  }
  searchStats.close();
}

/**
 * If the UCI at any time tells search to abort
 */
void search::Search::stopSearch() {
  setAbort(true);
}

void search::Search::quitSearch() {

}

void search::Search::setDepth(int depth) {
  this->depth = depth;
}

void search::Search::setSearchMoves(std::string moves) {
  this->searchMoves = moves;
}

void search::Search::setWTime(int wtime) {
  //this->wtime = wtime;
}
void search::Search::setBTime(int btime) {
  //this->btime = btime;
}
void search::Search::setWinc(int winc) {
  //this->winc = winc;
}
void search::Search::setBinc(int binc) {
  //this->binc = binc;
}
void search::Search::setMovesToGo(int movestogo) {
  this->movestogo = movestogo;
}
void search::Search::setNodes(int nodes) {
  //this->nodes = nodes;
}
void search::Search::setMoveTime(int movetime) {
  this->movetime = movetime;
}
void search::Search::setMate(int mate) {
  this->mate = mate;
}
void search::Search::setInfinite(int infinite) {
  this->infinite = infinite;
} // bool ?
void search::Search::setPonder(int ponder) {
  this->ponder = ponder;
} // bool ?


