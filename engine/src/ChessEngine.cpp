#include "david/ChessEngine.h"

// local dependencies
#include "david/EngineContext.h"
#include "david/ANN/ANN.h"
#include "david/genericUCIResponses.h"
#include "david/Search.h"
#include "david/david.h"
#include "david/utils.h"
#include "david/environment.h"
#include "david/TreeGen.h"

// git submodule dependencies
#include "fann/floatfann.h"
#include "uci/Listener.h"
#include "uci/events.h"

// system dependencies
#include <functional>

/**
 * Constructor
 */
david::ChessEngine::ChessEngine()
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocolPtr(std::make_shared<::uci::Listener>()),
      searchPtr(std::make_shared<Search>(engineContextPtr)),
      gameTreePtr(std::make_shared<gameTree::TreeGen>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>("")),
      player(),
      UCIProtocolActivated(false),
      uciMode(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
  this->engineContextPtr->uciProtocolPtr    = this->uciProtocolPtr;
}

/**
 * Constructor
 *
 * @param self
 */
david::ChessEngine::ChessEngine(Player self)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocolPtr(std::make_shared<::uci::Listener>()),
      searchPtr(std::make_shared<Search>(engineContextPtr)),
      gameTreePtr(std::make_shared<gameTree::TreeGen>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>("")),
      player(self),
      UCIProtocolActivated(false),
      uciMode(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
  this->engineContextPtr->uciProtocolPtr    = this->uciProtocolPtr;
}

/**
 * Constructor
 *
 * @param ANNFile std::string The ANN network file (The product of ANN training)
 */
david::ChessEngine::ChessEngine(std::string ANNFile)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocolPtr(std::make_shared<::uci::Listener>()),
      searchPtr(std::make_shared<Search>(engineContextPtr)),
      gameTreePtr(std::make_shared<gameTree::TreeGen>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(),
      UCIProtocolActivated(false),
      uciMode(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
  this->engineContextPtr->uciProtocolPtr    = this->uciProtocolPtr;
}

/**
 * Constructor
 *
 * @param self Player struct Which player instance you are (black / white)
 * @param ANNFile std::string The ANN network file (The product of ANN training)
 */
david::ChessEngine::ChessEngine(Player self, std::string ANNFile)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocolPtr(std::make_shared<::uci::Listener>()),
      searchPtr(std::make_shared<Search>(engineContextPtr)),
      gameTreePtr(std::make_shared<gameTree::TreeGen>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(self),
      UCIProtocolActivated(false),
      uciMode(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
  this->engineContextPtr->uciProtocolPtr    = this->uciProtocolPtr;
}

/**
 * Destructor
 */
david::ChessEngine::~ChessEngine() {
}

void david::ChessEngine::enableUCIMode() {
  this->uciMode = true;
  this->searchPtr->enableUCIMode();
}


void ::david::ChessEngine::linkUCICommands()
{
  using ::uci::event::GO;
  using ::uci::event::STOP;
  using ::uci::event::QUIT;
  using ::uci::event::PONDERHIT;
  using ::uci::event::UCINEWGAME;
  using ::uci::event::POSITION;
  using ::uci::event::BLACK;
  using ::uci::event::WHITE;
  using ::uci::arguments_t;

  // set chess engine colour
  auto uci_colour_black = [&](arguments_t args) {
    this->player.color = bitboard::COLOR::BLACK;
  };
  auto uci_colour_white = [&](arguments_t args) {
    this->player.color = bitboard::COLOR::WHITE;
  };

  //
  // forwards protocol functions, used for forwards protocol events
  //
  auto uci_go = [&](arguments_t args) {


    bool startSearch = false;
    bool infinite = false;
    bool movetime = false;
    // All of the "go" parameters
    if (args.count("depth") > 0) {
      this->searchPtr->setDepth(utils::stoi(args["depth"]));
    }
    if (args.count("searchmoves") > 0) {
      this->searchPtr->setSearchMoves(args["searchmoves"]);
    }
    if (args.count("wtime") > 0) {
      //this->searchPtr->setWTime(utils::stoi(args["wtime"]));
      this->wtime = utils::stoi(args["wtime"]);
      if (this->player.color == bitboard::COLOR::WHITE) {
        this->timeLeft = this->wtime;
      }
    }
    if (args.count("btime") > 0) {
      //this->searchPtr->setBTime(utils::stoi(args["btime"]));
      this->btime = utils::stoi(args["btime"]);
      if (this->player.color == bitboard::COLOR::BLACK) {
        this->timeLeft = this->btime;
      }
    }
    if (args.count("winc") > 0) {
      //this->searchPtr->setWinc(utils::stoi(args["winc"]));
    }
    if (args.count("binc") > 0) {
      //this->searchPtr->setBinc(utils::stoi(args["binc"]));
    }
    if (args.count("movestogo") > 0) {
      this->searchPtr->setMovesToGo(utils::stoi(args["movestogo"]));
    }
    if (args.count("nodes") > 0) {
      this->searchPtr->setNodes(utils::stoi(args["nodes"]));
    }
    if (args.count("movetime") > 0) {
      this->searchPtr->setMoveTime(utils::stoi(args["movetime"]));
      movetime = true;
    }
    if (args.count("mate") > 0) {
      this->searchPtr->setMate(utils::stoi(args["mate"]));
    }
    if (args.count("infinite") > 0) {
      this->searchPtr->setInfinite(true);
      startSearch = true;
      infinite = true;
    }
    if (args.count("ponder") > 0) {
      this->searchPtr->setPonder(true);
    }
    if (args.count("difficulty") > 0) {
      this->searchPtr->setDifficulty(utils::stoi(args["difficulty"]));
    }


    // update time remaining for search class
    if (!movetime) {
      this->searchPtr->setMoveTime(this->timeLeft);
    }

    if (infinite) {
      this->searchThread = std::thread([&]() {
        this->searchPtr->searchInit();
      });
    }
    else {
      this->searchPtr->searchInit();
    }

    if (!infinite) {
      // send best move
      int bestIndex = this->searchPtr->getSearchResult();
      auto EGN = utils::getEGN(this->gameTreePtr->getGameState(0), gameTreePtr->getGameState(bestIndex));
      std::cout << "bestmove " << EGN << std::endl;

      // update time used
      this->timeLeft -= this->searchPtr->getTimeUsed();

    }
  };
  auto uci_stop = [&](arguments_t args) {
    //std::cerr << "called stop" << std::endl;
    this->searchPtr->setAbort(true); // needs semaphores to avoid caching the isAbort variable
    if (this->searchThread.joinable()) {
      this->searchThread.join();
    }
    int bestIndex = this->searchPtr->getSearchResult();
    auto EGN = utils::getEGN(this->gameTreePtr->getGameState(0), gameTreePtr->getGameState(bestIndex));
    std::cout << "bestmove " << EGN << std::endl;
  };
  auto uci_quit = [&](arguments_t args) {
    this->searchPtr->setAbort(true); // needs semaphores to avoid caching the isAbort variable
    if (this->searchThread.joinable()) {
      this->searchThread.join();
    }
    this->searchPtr->quitSearch();
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
    // TODO: clear gameTree history
    this->gameTreePtr->reset();

  };

  auto uci_position = [&](arguments_t args) {
    if (args.count("fen") > 0) {
      this->gameTreePtr->setRootNodeFromFEN(args["fen"]);

#ifdef DAVID_DEVELOPMENT
      std::cout << "Root node of game tree:" << std::endl;
      utils::printGameState(this->gameTreePtr->getGameState(0));
#endif
    }
    else if (args.count("startpos") > 0) {
      this->gameTreePtr->setRootNodeFromFEN(constant::FENStartPosition);
    }
    else {
      std::cerr << "NOT SUPPORTED UCI PARAMETERS!" << std::endl;
    }

    // check for moves after given position
    if (args.count("moves") > 0) {
      // replace by applyEGNMove,
      // but since startpos / fen, resets the node to the default each time
      // this command should run for every move in moves given to sync the engine correctly.

      // get every egn string and apply it.
      std::vector<std::string> egns;
      const std::string moves = args["moves"];
      const auto len = moves.length();
      std::string move = "";
      for (int i = 0; i < len; i++) {
        const char m = moves.at(i);
        if (m == ' ' || i == (len - 1)) {

          // in case we are on the last index, we need to add the last char
          if (i == (len - 1)) {
            move += m;
          }

          // a EGN has been registered
          //egns.push_back(move);
          this->gameTreePtr->applyEGNMove(move);

          // clear the move var
          move.clear();
          move = "";
        }
        else {
          move += m;
        }
      }

      // the root node should always have the same colour as the engine!
      if (this->player.color != this->gameTreePtr->getGameState(0).playerColor) {
        std::cerr << "Root node is not the same as engine, do not search!!!" << std::endl;
        std::cerr << "\tplayer:    " << (this->player.color == bitboard::COLOR::BLACK ? "black" : "white") << std::endl;
        std::cerr << "\tgameState: " << (this->gameTreePtr->getGameState(0).playerColor == bitboard::COLOR::BLACK ? "black" : "white") << std::endl;
      }

      //utils::affectGameStateByEGNMove(this->gameTreePtr->getGameState(0), args["moves"]);
    }

#ifdef DAVID_DEVELOPMENT
    utils::printGameState(this->gameTreePtr->getGameState(0));
#endif
  };


  // bind the listeners
  this->uciProtocolPtr->addListener(BLACK, uci_colour_black);
  this->uciProtocolPtr->addListener(WHITE, uci_colour_white);
  this->uciProtocolPtr->addListener(GO, uci_go);
  this->uciProtocolPtr->addListener(STOP, uci_stop);
  this->uciProtocolPtr->addListener(QUIT, uci_quit);
  this->uciProtocolPtr->addListener(PONDERHIT, uci_ponderhit);
  this->uciProtocolPtr->addListener(UCINEWGAME, uci_ucinewgame);
  this->uciProtocolPtr->addListener(POSITION, uci_position);
}


/**
 * Retrieve the ANN file this engine instance uses for evaluating game boards.
 * @return std::string absolute path of ann file.
 */
std::string david::ChessEngine::getANNFile() {
  if (this->hasANNInstance()) {
    return this->neuralNetworkPtr->getANNFile();
  }
  else {
    return "";
  }
}

/**
 * Check if there exists a ANN instance
 */
bool david::ChessEngine::hasANNFile() {
  return this->neuralNetworkPtr->getANNFile() != "";
}

/**
 * Check if there exists a ANN instance
 */
bool david::ChessEngine::hasANNInstance() {
  return this->neuralNetworkPtr != nullptr && this->neuralNetworkPtr->hasANNInstance();
}

/**
 * Check if this engine plays as white
 */
bool david::ChessEngine::isWhite() {
  return this->player.color == bitboard::COLOR::WHITE;
}

/**
 * Get ::bitboard::COLOR color
 */
david::bitboard::COLOR david::ChessEngine::getColor() {
  return this->player.color;
}


/**
 * Adds typical UCI responses to the engine
 */
void david::ChessEngine::configureUCIProtocol() {
  using ::uci::event::UCI;
  using ::uci::event::ISREADY;
  using ::uci::event::QUIT;


  // ###
  // initiate all other classes here.
  //this->searchA(uciListener);

  // ###
  // Here the UCI protocol is dealt with.
  // add basic responses
  this->uciProtocolPtr->addListener(UCI, uciResponses::responseToUCI); // forwards
  this->uciProtocolPtr->addListener(ISREADY, uciResponses::responseToISREADY); // isready

  // Got a quit forwards command so stop listener [and exit program]
  this->uciProtocolPtr->addListener(QUIT, [&](::uci::arguments_t args){
    this->uciProtocolPtr->stopListening();
  });
}

/**
 * Actives the UCI protocol, and keeps it running in another thread.
 */
void david::ChessEngine::activateUCIProtocol() {

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  this->UCIProtocolActivated = this->uciProtocolPtr->setupListener();
}

/**
 * Check if UCI is activated on this engine or not.
 *
 * @return bool if UCI protocol has been activated.
 */
bool david::ChessEngine::hasInitiatedUCIProtocol() {
  return this->UCIProtocolActivated;
}

/**
 * Gives this engine a UCI Protocol command, but does not return responses.
 *
 * @deprecated, hasn't been implemented yet.
 */
void david::ChessEngine::sayUCICommand(std::string command) {
  // eh.. not yet
}

/**
 * Creates the neural network based on the ANNFile
 */
void david::ChessEngine::createANNInstance(std::string ANNFile) {
  this->neuralNetworkPtr->createANNInstance();
}

/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param board ::gameTree::gameState, of shared_ptr type
 * @return int board evaluation
 */
int david::ChessEngine::ANNEvaluate(const type::gameState_t& board) {
  return this->neuralNetworkPtr->ANNEvaluate(board);
}


/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param fen std::string FEN(Forsyth–Edwards Notation)
 * @return int board evaluation
 */
int david::ChessEngine::ANNEvaluate(std::string fen) {
  return this->neuralNetworkPtr->ANNEvaluate(fen);
}


/**
 * Update this players color, color means what piece this player can move..
 *
 * @param color
 */
void david::ChessEngine::setPlayerColor(bitboard::COLOR color) {
  this->player.color = color;
}


/**
 * Get the game node from a engine. Will be known as the root node for the search class.
 *
 * @return shared_ptr of gameState
 */
david::type::gameState_t david::ChessEngine::getGameState() {
  return this->engineContextPtr->gameTreePtr->getGameState(0);
}


/**
 * Sets the game state based on a node.
 * Used when updating board states during battles.
 *
 * @param state shared_ptr of a gameState
 * @return true if the state was updated
 */
bool david::ChessEngine::setGameState(const type::gameState_t& gs) {
  // std::cerr << "updated version" << std::endl;
  // utils::printGameState(gs);
  this->gameTreePtr->setRootNode(gs);
}


/**
 * Check if the engine has lost.
 *
 * @return true on losing
 */
bool david::ChessEngine::lost() {
  return this->currentGameState.possibleSubMoves == 0;
}


/**
 * Find the best move, and update the current game state.
 */
void david::ChessEngine::findBestMove() {
  // update currentGameState
  // TODO: something is very wrong here
  this->engineContextPtr->searchPtr->setInfinite(false); // must be false, or a incorrect index is returned.
  int index = this->searchPtr->searchInit(); // maybe store the index and then idk? why is this sigsegv..

  auto& gs = this->gameTreePtr->getGameState(index);
  this->setGameState(gs);

  //utils::printGameState(this->gameTreePtr->getGameState(0));

  //this->engineContextPtr->gameTreePtr->updateRootNodeTo(index);
}


/**
 * Used to reset old data, and construct a new fresh game for this engine.
 *
 * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
 */
void david::ChessEngine::setNewGameBoard(const std::string fen) {

  // first clear any children
  //this->gameTreePtr->resetChildren(this->currentGameState);
  auto& node = this->engineContextPtr->gameTreePtr->getGameState(0);

  // check if its a default setup
  if (fen == david::FENStartPosition) {
    utils::setDefaultChessLayout(node);
    return;
  }

  utils::generateBoardFromFen(node, fen);
}