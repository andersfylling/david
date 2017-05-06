//
// Created by anders on 5/3/17.
//

#include "chess_ann/Search.h"
#include "chess_ann/utils.h"


namespace Search {
    Signals Signal;
}
//
// Constructor
//
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


//Root search
void search::Search::searchInit(/*Pseudo Node*/) {
  resetSearchValues();

  searchScore = iterativeDeepening(/*Pseudo node*/);
}

int search::Search::iterativeDeepening(/*Pseudo node*/) {
    using namespace search;
      int bestScore = -VALUE_INFINITE;
      int alpha = -VALUE_INFINITE;
      int beta = VALUE_INFINITE;
      int lastDepth = 0;

      /*Must create a move tree based on the root node sent to iterative Deepening*/

      //
      // Iterate down in the search tree for each search tree
      //
      for (int currentDepth = 1; currentDepth <= depth && !Signal.stop; currentDepth++) {
        int score = -VALUE_INFINITE;
        lastDepth = currentDepth;

        bool finished = false;
        while (!finished) {
          //
          // Do negamax
          //
          score = negamax(/*Pseudo node*/ alpha, beta, currentDepth);
        }
      }
      return 0;
}

int search::Search::negamax(/*Pseudo node*/ int alpha, int beta, int depth) {
    int score = -VALUE_INFINITE;
    int value = 0;
    int moveCounter=0;

    if (depth == 0 /*|| node->children.empty()*/) {
        /*return evaluate(node);*/
    }

    /**
     *  childNodes := GenerateMoves(node)
     *  childNodes := OrderMoves(childNodes)
     * */

    for (auto i : node->children /*Needs to be corrected*/) {
        value = -negamax(/*i,*/ -beta, -alpha, depth - 1);
        score = (score > value) ? score : value;
        alpha = (alpha > value) ? alpha : value;
        if (alpha>=beta)
        {
            continue;
        }
    }

    return score;
}

void search::Search::resetSearchValues() {

}

void search::Search::stopSearch() {

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

