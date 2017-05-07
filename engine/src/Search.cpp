//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"
#include "chess_ann/utils.h"


namespace search {
    //Signals Signal; //Scrapped for now
    clock_t startTime;
}

/**
 * Constructor used in debug/test
 */
search::Search::Search(){

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
    }
    else if (args.count("searchmoves") > 0) {
      this->setSearchMoves(args["searchmoves"]);
    }
    else if (args.count("wtime") > 0) {
      this->setWTime(::utils::stoi(args["wtime"]));
    }
    else if (args.count("btime") > 0) {
      this->setBTime(::utils::stoi(args["btime"]));
    }
    else if (args.count("winc") > 0) {
      this->setWinc(::utils::stoi(args["winc"]));
    }
    else if (args.count("binc") > 0) {
      this->setBinc(::utils::stoi(args["binc"]));
    }
    else if (args.count("movestogo") > 0) {
      this->setMovesToGo(::utils::stoi(args["movestogo"]));
    }
    else if (args.count("nodes") > 0) {
      this->setNodes(::utils::stoi(args["nodes"]));
    }
    else if (args.count("movetime") > 0) {
      this->setMoveTime(::utils::stoi(args["movetime"]));
    }
    else if (args.count("mate") > 0) {
      this->setMate(::utils::stoi(args["mate"]));
    }
    else if (args.count("infinite") > 0) {
      this->setInfinite(::utils::stoi(args["infinite"]));
    }
    else if (args.count("ponder") > 0) {
      this->setPonder(::utils::stoi(args["ponder"]));
    }
  };
  auto uci_stop = [&](arguments_t args){
    this->stopSearch();
  };
  auto uci_quit = [&](arguments_t args){
    this->quitSearch();
  };
  auto uci_ponderhit = [&](arguments_t args){
//     * ponderhit
//	the user has played the expected move. This will be sent if the engine was told to ponder on the same move
//	the user has played. The engine should continue searching but switch from pondering to normal search.
  };
  auto uci_ucinewgame = [&](arguments_t args){
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
  uci.addListener(GO,         uci_go);
  uci.addListener(STOP,       uci_stop);
  uci.addListener(QUIT,       uci_quit);
  uci.addListener(PONDERHIT,  uci_ponderhit);
  uci.addListener(UCINEWGAME, uci_ucinewgame);
}


/**
 * Set Search variables. searchScore is a class member and
 * should be returned to UCI. Must rewrite to send best move and not "score"
 * @param node
 */
void search::Search::searchInit(std::shared_ptr<::bitboard::gameState> node) {
  resetSearchValues();

  //searchScore = iterativeDeepening(node);
}


/**
 * Main iterative loop, calls negamax until desired depth is reached
 * or the time limit is run out.
 * @param node
 * @return
 */
int search::Search::iterativeDeepening(std::shared_ptr<::bitboard::gameState> node) {
      int bestScore = -VALUE_INFINITE;  //Kinda deprecated, should be switched to a gamestate
      int alpha = -VALUE_INFINITE;
      int beta = VALUE_INFINITE;
      int lastDepth = 0;
      startTime = clock();              //Starting clock


      std::shared_ptr<::bitboard::gameState> bestMove;    

      std::shared_ptr<::bitboard::gameState> rootMoves;
      //rootMoves->generateMoves(node);
      //rootMoves->moveTree->sort();


      //
      // Iterate down in the search tree for each search tree
      //
      for (int currentDepth = 1; currentDepth <= depth || (static_cast<double> (clock() - startTime)/CLOCKS_PER_SEC) < movetime; currentDepth++) {
          std::shared_ptr<::bitboard::gameState> currentScore;
          currentScore->score  = -VALUE_INFINITE;

          //Old score, replaced my a gameState currentScore
          int score = -VALUE_INFINITE;
          lastDepth = currentDepth;     //Sent to UCI or some debug when search is done

          //Needs to be replaced
          score = negamax(node, alpha, beta, currentDepth);


          // Before we can do this, the return type of negamax needs to be changed
          //currentScore = negamax(node, alpha, beta, currentDepth);


          // Needs to be changed to compare moves instead
          //If score from a greater depth is better than current bestScore, update
          if(bestScore < score)
            bestScore = score;

      }
      //Does not return a move yet
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
int search::Search::negamax(std::shared_ptr<::bitboard::gameState> node, int alpha, int beta, int depth) {
    int score = -VALUE_INFINITE;
    int value = 0;
    int moveCounter=0;

    if (depth == 0 || node->children.empty()) {
        return node->score;
    }

    //Node->children does not return correct type atm
    for (auto i : node->children) {
        value = -negamax(i, -beta, -alpha, depth - 1);
        score = (score > value) ? score : value;
        alpha = (alpha > value) ? alpha : value;
        if (alpha>=beta)
        {
            continue;
        }
    }

    return score;
}

/**
 * Called by searchInit, reset/get settings from UCI
 * Mainly used for debugging and progress atm
 */
void search::Search::resetSearchValues() {
  this->movetime = 3000; //Hardcoded variables as of now, need to switch to uci later
  this->depth = 10;
}

/**
 * If the UCI at any time tells search to abort
 */
void search::Search::stopSearch() {
  // return bestMove maybe..
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

