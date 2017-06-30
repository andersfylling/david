#include "david/ChessEngine.h"

// local dependencies
#include "david/EngineContext.h"
#include "david/ANN/ANN.h"
#include "david/genericUCIResponses.h"
#include "david/Search.h"
#include "david/david.h"
#include "david/utils.h"
#include "david/environment.h"

// git submodule dependencies
#include "fann/floatfann.h"
#include "uci/Listener.h"
#include "uci/events.h"

// system dependencies

/**
 * Constructor
 */
david::ChessEngine::ChessEngine()
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocolPtr(std::make_shared<::uci::Listener>()),
      searchPtr(std::make_shared<Search>(engineContextPtr)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>("")),
      player(),
      UCIProtocolActivated(false),
      currentGameState(new type::gameState_t())

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
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>("")),
      player(self),
      UCIProtocolActivated(false),
      currentGameState(new type::gameState_t())

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
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(),
      UCIProtocolActivated(false),
      currentGameState(new type::gameState_t())

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
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(self),
      UCIProtocolActivated(false),
      currentGameState(new type::gameState_t())

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


void ::david::ChessEngine::linkUCICommands()
{
  using ::uci::event::GO;
  using ::uci::event::STOP;
  using ::uci::event::QUIT;
  using ::uci::event::PONDERHIT;
  using ::uci::event::UCINEWGAME;
  using ::uci::arguments_t;

  //
  // forwards protocol functions, used for forwards protocol events
  //
  auto uci_go = [&](arguments_t args) {
    // All of the "go" parameters
    if (args.count("depth") > 0) {
      this->searchPtr->setDepth(utils::stoi(args["depth"]));
    } else if (args.count("searchmoves") > 0) {
      this->searchPtr->setSearchMoves(args["searchmoves"]);
    } else if (args.count("wtime") > 0) {
      this->searchPtr->setWTime(utils::stoi(args["wtime"]));
    } else if (args.count("btime") > 0) {
      this->searchPtr->setBTime(utils::stoi(args["btime"]));
    } else if (args.count("winc") > 0) {
      this->searchPtr->setWinc(utils::stoi(args["winc"]));
    } else if (args.count("binc") > 0) {
      this->searchPtr->setBinc(utils::stoi(args["binc"]));
    } else if (args.count("movestogo") > 0) {
      this->searchPtr->setMovesToGo(utils::stoi(args["movestogo"]));
    } else if (args.count("nodes") > 0) {
      this->searchPtr->setNodes(utils::stoi(args["nodes"]));
    } else if (args.count("movetime") > 0) {
      this->searchPtr->setMoveTime(utils::stoi(args["movetime"]));
    } else if (args.count("mate") > 0) {
      this->searchPtr->setMate(utils::stoi(args["mate"]));
    } else if (args.count("infinite") > 0) {
      this->searchPtr->setInfinite(utils::stoi(args["infinite"]));
    } else if (args.count("ponder") > 0) {
      this->searchPtr->setPonder(utils::stoi(args["ponder"]));
    } else if (args.count("difficulty") > 0) {
      this->searchPtr->setDifficulty(utils::stoi(args["difficulty"]));
    }
  };
  auto uci_stop = [&](arguments_t args) {
    this->searchPtr->stopSearch();
  };
  auto uci_quit = [&](arguments_t args) {
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
  };


  // bind the listeners
  this->uciProtocolPtr->addListener(GO, uci_go);
  this->uciProtocolPtr->addListener(STOP, uci_stop);
  this->uciProtocolPtr->addListener(QUIT, uci_quit);
  this->uciProtocolPtr->addListener(PONDERHIT, uci_ponderhit);
  this->uciProtocolPtr->addListener(UCINEWGAME, uci_ucinewgame);
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
int david::ChessEngine::ANNEvaluate(type::gameState_ptr board) {
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
david::type::gameState_ptr david::ChessEngine::getGameState() {
  return this->currentGameState;
}


/**
 * Sets the game state based on a node.
 * Used when updating board states during battles.
 *
 * @param state shared_ptr of a gameState
 * @return true if the state was updated
 */
bool david::ChessEngine::setGameState(type::gameState_ptr state) {
  // TODO: dangling pointer if this node / state doesnt exist somewhere else..
  this->currentGameState = state; // switch pointer with state, state then dies by scope afterwards.
}


/**
 * Check if the engine has lost.
 *
 * @return true on losing
 */
bool david::ChessEngine::lost() {
  return this->currentGameState->possibleSubMoves == 0;
}


/**
 * Find the best move, and update the current game state.
 */
void david::ChessEngine::findBestMove() {
  // update currentGameState
  this->currentGameState = this->searchPtr->searchInit(this->currentGameState);
}


/**
 * Used to reset old data, and construct a new fresh game for this engine.
 *
 * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
 */
void david::ChessEngine::setNewGameBoard(const std::string fen) {

  // first clear any children
  //this->gameTreePtr->resetChildren(this->currentGameState);

  // check if its a default setup
  if (fen == david::FENStartPosition) {
    utils::setDefaultChessLayout(this->currentGameState);
    return;
  }

  // otherwise we need to correctly parse it
  std::stringstream sstr(fen);
  std::string color = "w";
  sstr >> color >> color; // now it gets the color

  environment::Environment env(color == "w" ? bitboard::COLOR::WHITE : bitboard::COLOR::BLACK);
  this->currentGameState = env.generateBoardFromFen(fen);
}