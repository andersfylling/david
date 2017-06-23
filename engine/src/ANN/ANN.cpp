// local dependencies
#include "david/ANN/ANN.h"
#include "david/david.h"
#include "david/utils.h"
#include "david/environment.h"

// system dependencies
#include <sstream>

namespace david {
/**
 * Constructor
 * @param filename The ANN weight file.
 */
ANN::ANN(std::string filename)
    : engineContextPtr(nullptr),
      ANNFile(utils::getAbsoluteProjectPath() + ::david::neuralNetworksFolder + filename),
      ANNInstance(nullptr) {}

/**
 * Constructor
 *
 * @param ctx engineContext_ptr which holds links to other initiated classes
 * @param filename The ANN weight file.
 * @see /engine/includes/david/definitions.h
 */
ANN::ANN(type::engineContext_ptr ctx, std::string filename)
    : engineContextPtr(ctx),
      ANNFile(utils::getAbsoluteProjectPath() + ::david::neuralNetworksFolder + filename),
      ANNInstance(nullptr) {}

/**
 * Destructor
 */
ANN::~ANN() {
  if (this->hasANNInstance()) {
    fann_destroy(this->ANNInstance);
  }
}

/**
 * Retrieve the ANN file this engine instance uses for evaluating game boards.
 * @return std::string absolute path of ann file.
 */
std::string ANN::getANNFile() {
  return this->ANNFile;
}

/**
 * Update the ANN file even if already set or not.
 * This requires the ANN to not be running.
 *
 * This function does not validate the file.
 *
 * The file must end with '.data'
 *
 * @param std::string filename, a file that resides within the engine/src/ANN/networks/ folder.
 */
void ANN::setANNFile(std::string filename) {
  // make sure that the filename isn't empty
  if (filename.length() < 6) { // these should end with '.data' so it has to be larger than 5 chars.
    std::cerr << "Filename is too short. Missing '.data' suffix." << std::endl;
    return;
  }

  // make sure there is no instance already running
  if (this->hasANNInstance()) {
    std::cerr << "ANN instance already exists" << std::endl;
    return;
  }

  // Check that the file exists on the machine
  std::string file = utils::getAbsoluteProjectPath() + ::david::neuralNetworksFolder + filename;
  if (!utils::fileExists(file)) {
    std::cerr << "ANN file does not exist: " << this->ANNFile << std::endl;
    return;
  }

  // everything seems ok
  this->ANNFile = file;
}

/**
 * Check if there exists a ANN instance
 */
bool ANN::hasANNFile() {
  return this->ANNFile != "";
}

/**
 * Check if there exists a ANN instance
 */
bool ANN::hasANNInstance() {
  return this->ANNInstance != nullptr;
}

/**
 * Creates the neural network based on the ANNFile
 */
void ANN::createANNInstance() {
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
  if (!utils::fileExists(this->ANNFile)) {
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
int ANN::ANNEvaluate(type::gameState_ptr board, bitboard::COLOR color) {
  fann_type* inputs = utils::boardToFannInputs(board); // float array
  fann_type* outputs = fann_run(this->ANNInstance, inputs); // float array

  int output = static_cast<int>(outputs[0] * 1000); // The expected output during training was multiplied by 0.001
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
int ANN::ANNEvaluate(std::string fen) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::COLOR;

  // get color from fen string
  std::stringstream sstr(fen);
  std::string color = "w";
  sstr >> color >> color; // skip the board layouts and get the color
  COLOR c = color == "w" ? WHITE : BLACK;

  environment::Environment env(c);
  type::gameState_ptr board = env.generateBoardFromFen(fen);

  return this->ANNEvaluate(board, c);
}

}