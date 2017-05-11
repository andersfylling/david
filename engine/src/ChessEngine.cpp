#include "chess_ann/ChessEngine.h"

// local dependencies
#include "chess_ann/EngineContext.h"
#include "chess_ann/uci/events.h"
#include "chess_ann/ANN/ANN.h"
#include "chess_ann/genericUCIResponses.h"
#include "chess_ann/uci/Listener.h"
#include "chess_ann/Search.h"

// git submodule dependencies
#include "fann/floatfann.h"

// system dependencies


chess_ann::ChessEngine::ChessEngine()
    : engineContextPtr(std::make_shared<chess_ann::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<::search::Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<::gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(nullptr),
      player(),
      UCIProtocolActivated(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::ChessEngine::ChessEngine(Player self)
    : engineContextPtr(std::make_shared<chess_ann::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<::search::Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(nullptr),
      player(self),
      UCIProtocolActivated(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::ChessEngine::ChessEngine(std::string ANNFile)
    : engineContextPtr(std::make_shared<chess_ann::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<::search::Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(),
      UCIProtocolActivated(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::ChessEngine::ChessEngine(Player self, std::string ANNFile)
    : engineContextPtr(std::make_shared<chess_ann::EngineContext>()),
      uciProtocol(),
      searchPtr(std::make_shared<::search::Search>(engineContextPtr, uciProtocol)),
      gameTreePtr(std::make_shared<gameTree::GameTree>(engineContextPtr)),
      neuralNetworkPtr(std::make_shared<ANN>(engineContextPtr, ANNFile)),
      player(self),
      UCIProtocolActivated(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

chess_ann::ChessEngine::~ChessEngine() {
}


void chess_ann::ChessEngine::setupEngine(std::string ANNFile) {
  this->neuralNetworkPtr = std::make_shared<ANN>(this->engineContextPtr, ANNFile);
  this->searchPtr = std::make_shared<::search::Search>(this->engineContextPtr, this->uciProtocol);
  this->gameTreePtr = std::make_shared<::gameTree::GameTree>(this->engineContextPtr);
}

/**
 * Retrieve the ANN file this engine instance uses for evaluating game boards.
 * @return std::string absolute path of ann file.
 */
std::string chess_ann::ChessEngine::getANNFile() {
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
bool chess_ann::ChessEngine::hasANNFile() {
  return this->neuralNetworkPtr->getANNFile() != "";
}

/**
 * Check if there exists a ANN instance
 */
bool chess_ann::ChessEngine::hasANNInstance() {
  return this->neuralNetworkPtr != nullptr && this->neuralNetworkPtr->hasANNInstance();
}

/**
 * Check if this engine plays as white
 */
bool chess_ann::ChessEngine::isWhite() {
  return this->player.color == ::bitboard::COLOR::WHITE;
}

/**
 * Get ::bitboard::COLOR color
 */
bitboard::COLOR chess_ann::ChessEngine::getColor() {
  return this->player.color;
}


/**
 * Adds typical UCI responses to the engine
 */
void chess_ann::ChessEngine::configureUCIProtocol() {
  using ::uci::event::UCI;
  using ::uci::event::ISREADY;
  using ::uci::event::QUIT;


  // ###
  // initiate all other classes here.
  //this->searchA(uciListener);

  // ###
  // Here the UCI protocol is dealt with.
  // add basic responses
  this->uciProtocol.addListener(UCI, uciResponses::responseToUCI); // uci
  this->uciProtocol.addListener(ISREADY, uciResponses::responseToISREADY); // isready

  // Got a quit uci command so stop listener [and exit program]
  this->uciProtocol.addListener(QUIT, [&](uci::arguments_t args){
    this->uciProtocol.stopListening();
  });
}

/**
 * Actives the UCI protocol, and keeps it running in another thread.
 */
void chess_ann::ChessEngine::activateUCIProtocol() {

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  this->UCIProtocolActivated = this->uciProtocol.setupListener();
}

/**
 * Check if UCI is activated on this engine or not.
 *
 * @return bool if UCI protocol has been activated.
 */
bool chess_ann::ChessEngine::hasInitiatedUCIProtocol() {
  return this->UCIProtocolActivated;
}

/**
 * Gives this engine a UCI Protocol command, but does not return responses.
 *
 * @deprecated, hasn't been implemented yet.
 */
void chess_ann::ChessEngine::sayUCICommand(std::string command) {
  // eh.. not yet
}

/**
 * Creates the neural network based on the ANNFile
 */
void chess_ann::ChessEngine::createANNInstance(std::string ANNFile) {
  this->neuralNetworkPtr->createANNInstance();
}

/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param board ::gameTree::gameState, of shared_ptr type
 * @return int board evaluation
 */
int chess_ann::ChessEngine::ANNEvaluate(definitions::gameState_ptr board) {
  return this->neuralNetworkPtr->ANNEvaluate(board, this->player.color);
}


/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param fen std::string FEN(Forsyth–Edwards Notation)
 * @return int board evaluation
 */
int chess_ann::ChessEngine::ANNEvaluate(std::string fen) {
  return this->neuralNetworkPtr->ANNEvaluate(fen);
}

/**
 * Used to reset old data, and construct a new fresh game for this engine.
 *
 * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
 */
void chess_ann::ChessEngine::setNewGameBoard(const std::string fen) {
  this->currentGameState = nullptr;
}


/**
 * Update this players color, color means what piece this player can move..
 *
 * @param color
 */
void chess_ann::ChessEngine::setPlayerColor(bitboard::COLOR color) {
  this->player.color = color;
}


/**
 * Get the game node from a engine. Will be known as the root node for the search class.
 *
 * @return shared_ptr of gameState
 */
definitions::gameState_ptr chess_ann::ChessEngine::getGameState() {
  return this->currentGameState;
}


/**
 * Sets the game state based on a node.
 * Used when updating board states during battles.
 *
 * @param state shared_ptr of a gameState
 * @return true if the state was updated
 */
bool chess_ann::ChessEngine::setGameState(definitions::gameState_ptr state) {
  this->currentGameState = state;
}


/**
 * Check if the engine has lost.
 *
 * @return true on losing
 */
bool chess_ann::ChessEngine::lost() {
  return this->currentGameState->possibleSubMoves == 0;
}


/**
 * Find the best move, and update the current game state.
 */
void chess_ann::ChessEngine::findBestMove() {
  // TODO: Markus
  // update currentGameState
  this->currentGameState = this->searchPtr->searchInit(this->currentGameState);
}