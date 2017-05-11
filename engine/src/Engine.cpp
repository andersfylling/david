#include "chess_ann/Engine.h"

// local dependencies
#include "chess_ann/EngineContext.h"
#include "chess_ann/uci/events.h"

// git submodule dependencies
#include "fann/floatfann.h"

// system dependencies


chess_ann::Engine::Engine()
    : engineContextPtr(),
      uciProtocol(),
      searchPtr(engineContextPtr, uciProtocol),
      neuralNetworkPtr(nullptr),
      player(),
      UCIProtocolActivated(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::Engine::Engine(Player self)
    : engineContextPtr(),
      uciProtocol(),
      searchPtr(engineContextPtr, uciProtocol),
      neuralNetworkPtr(nullptr),
      player(self),
      UCIProtocolActivated(false)

{
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::Engine::Engine(std::string ANNFile)
    : engineContextPtr(),
      uciProtocol(),
      searchPtr(engineContextPtr, uciProtocol),
      neuralNetworkPtr(engineContextPtr, ANNFile),
      player(),
      UCIProtocolActivated(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}
chess_ann::Engine::Engine(Player self, std::string ANNFile)
    : engineContextPtr(),
      uciProtocol(),
      searchPtr(engineContextPtr, uciProtocol),
      neuralNetworkPtr(engineContextPtr, ANNFile),
      player(self),
      UCIProtocolActivated(false)

{
  this->createANNInstance(ANNFile);
  this->engineContextPtr->neuralNetworkPtr  = this->neuralNetworkPtr;
  this->engineContextPtr->searchPtr         = this->searchPtr;
  this->engineContextPtr->gameTreePtr       = this->gameTreePtr;
}

chess_ann::Engine::~Engine() {
}

/**
 * Retrieve the ANN file this engine instance uses for evaluating game boards.
 * @return std::string absolute path of ann file.
 */
std::string chess_ann::Engine::getANNFile() {
  if (this->hasANNInstance()) {
    return this->neuralNetwork.getANNFile();
  }
  else {
    return "";
  }
}

/**
 * Check if there exists a ANN instance
 */
bool chess_ann::Engine::hasANNFile() {
  return this->ANNFile != "";
}

/**
 * Check if there exists a ANN instance
 */
bool chess_ann::Engine::hasANNInstance() {
  return this->neuralNetworkPtr != nullptr && this->neuralNetworkPtr->hasANNInstance();
}

/**
 * Check if this engine plays as white
 */
bool chess_ann::Engine::isWhite() {
  return this->player.color == ::bitboard::COLOR::WHITE;
}

/**
 * Get ::bitboard::COLOR color
 */
bitboard::COLOR chess_ann::Engine::getColor() {
  return this->player.color;
}


/**
 * Adds typical UCI responses to the engine
 */
void chess_ann::Engine::configureUCIProtocol() {
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
void chess_ann::Engine::activateUCIProtocol() {

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  this->UCIProtocolActivated = this->uciProtocol.setupListener();
}

/**
 * Check if UCI is activated on this engine or not.
 *
 * @return bool if UCI protocol has been activated.
 */
bool chess_ann::Engine::hasInitiatedUCIProtocol() {
  return this->UCIProtocolActivated;
}

/**
 * Gives this engine a UCI Protocol command, but does not return responses.
 *
 * @deprecated, hasn't been implemented yet.
 */
void chess_ann::Engine::sayUCICommand(std::string command) {
  // eh.. not yet
}

/**
 * Creates the neural network based on the ANNFile
 */
void chess_ann::Engine::createANNInstance(std::string ANNFile) {
  // make sure there is no instance already running
  if (this->hasANNInstance()) {
    std::cerr << "ANN instance already exists" << std::endl;
    return;
  }

  // make sure a file has been given
  if (!this->hasANNFile()) {
    std::cerr << "ANN file was not set" << std::endl;
    return;
  }

  // Check that the file exists on the machine
  if (!::utils::fileExists(this->ANNFile)) {
    std::cerr << "ANN file does not exist: " << this->ANNFile << std::endl;
    return;
  }

  // create instance from file
  this->ANNInstance = fann_create_from_file(this->ANNFile.c_str());
}

/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param board ::gameTree::gameState, of shared_ptr type
 * @return int board evaluation
 */
int chess_ann::Engine::ANNEvaluate(::gameTree::nodePtr board) {
  fann_type* inputs = ::utils::convertGameStateToInputs(board, this->player); // float array
  fann_type* outputs = fann_run(this->ANNInstance, inputs); // float array

  int output = static_cast<int>(outputs[0]);
  delete inputs;
  delete outputs;

  return output;
}


/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param fen std::string FEN(Forsyth–Edwards Notation)
 * @return int board evaluation
 */
int chess_ann::Engine::ANNEvaluate(std::string fen) {
  ::environment::Environment env(::bitboard::COLOR::WHITE);
  ::gameTree::nodePtr board = env.generateBoardFromFen(fen);

  return this->ANNEvaluate(board);
}

/**
 * Used to reset old data, and construct a new fresh game for this engine.
 *
 * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
 */
void chess_ann::Engine::setNewGameBoard(const std::string fen) {
  this->currentGameState = nullptr;
}


/**
 * Update this players color, color means what piece this player can move..
 *
 * @param color
 */
void chess_ann::Engine::setPlayerColor(bitboard::COLOR color) {
  this->player.color = color;
}


/**
 * Get the game node from a engine. Will be known as the root node for the search class.
 *
 * @return shared_ptr of gameState
 */
gameTree::nodePtr chess_ann::Engine::getGameState() {
  return this->currentGameState;
}


/**
 * Sets the game state based on a node.
 * Used when updating board states during battles.
 *
 * @param state shared_ptr of a gameState
 * @return true if the state was updated
 */
bool chess_ann::Engine::setGameState(gameTree::nodePtr state) {
  this->currentGameState = state;
}


/**
 * Check if the engine has lost.
 *
 * @return true on losing
 */
bool chess_ann::Engine::lost() {
  return this->currentGameState->possibleSubMoves == 0;
}


/**
 * Find the best move, and update the current game state.
 */
void chess_ann::Engine::findBestMove() {
  // TODO: Markus
  // update currentGameState
  this->currentGameState = this->searchAgent.searchInit(this->currentGameState);
}