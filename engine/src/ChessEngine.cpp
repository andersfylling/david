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
      uciProtocol(),
      searchPtr(std::make_shared<Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(nullptr),
      player(),
      UCIProtocolActivated(false),
      currentGameState(std::make_shared<bitboard::gameState>())

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

/**
 * Constructor
 *
 * @param self
 */
david::ChessEngine::ChessEngine(Player self)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(nullptr),
      player(self),
      UCIProtocolActivated(false),
      currentGameState(std::make_shared<bitboard::gameState>())

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

/**
 * Constructor
 *
 * @param ANNFile std::string The ANN network file (The product of ANN training)
 */
david::ChessEngine::ChessEngine(std::string ANNFile)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(),
      UCIProtocolActivated(false),
      currentGameState(std::make_shared<bitboard::gameState>())

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

/**
 * Constructor
 *
 * @param self Player struct Which player instance you are (black / white)
 * @param ANNFile std::string The ANN network file (The product of ANN training)
 */
david::ChessEngine::ChessEngine(Player self, std::string ANNFile)
    : engineContextPtr(std::make_shared<david::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(self),
      UCIProtocolActivated(false),
      currentGameState(std::make_shared<bitboard::gameState>())

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

/**
 * Destructor
 */
david::ChessEngine::~ChessEngine() {
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
  this->uciProtocol.addListener(UCI, uciResponses::responseToUCI); // forwards
  this->uciProtocol.addListener(ISREADY, uciResponses::responseToISREADY); // isready

  // Got a quit forwards command so stop listener [and exit program]
  this->uciProtocol.addListener(QUIT, [&](::uci::arguments_t args){
    this->uciProtocol.stopListening();
  });
}

/**
 * Actives the UCI protocol, and keeps it running in another thread.
 */
void david::ChessEngine::activateUCIProtocol() {

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  this->UCIProtocolActivated = this->uciProtocol.setupListener();
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
int david::ChessEngine::ANNEvaluate(definitions::gameState_ptr board) {
  return this->neuralNetworkPtr->ANNEvaluate(board, this->player.color);
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
david::definitions::gameState_ptr david::ChessEngine::getGameState() {
  return this->currentGameState;
}


/**
 * Sets the game state based on a node.
 * Used when updating board states during battles.
 *
 * @param state shared_ptr of a gameState
 * @return true if the state was updated
 */
bool david::ChessEngine::setGameState(definitions::gameState_ptr state) {
  this->currentGameState = state;
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
  // TODO: Markus
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
  this->gameTreePtr->resetChildren(this->currentGameState);

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