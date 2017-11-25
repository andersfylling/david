// local dependencies
#include "david/ANN/ANN.h"
#include "david/david.h"
#include "david/utils/utils.h"
#include "david/utils/gameState.h"

// system dependencies
#include <sstream>
#include <david/utils/neuralNet.h>

namespace david {
/**
 * Constructor
 * @param filename The ANN weight file.
 */
ANN::ANN()
    : ANNFile(""),
      ANNInstance(nullptr) {}

/**
 * Constructor
 * @param filename The ANN weight file.
 */
ANN::ANN(const std::string filename)
    : ANNFile(utils::getAbsoluteProjectPath() + ::david::constant::neuralNetworksFolder + filename),
      ANNInstance(nullptr) {}


void ANN::guarenteeANNFile() const {
#if defined(__linux__)
  // make sure the chess david folder exists
  // make sure the ANN folder exists
  // make sure the ann file exists.
  //::utils::fileExists(::utils::getAbsoluteProjectPath() + ::david::neuralNetworksFolder + filename)
#elif defined(_WIN32) || defined(_WIN64)
  std::__throw_runtime_error("ERROR: Windows support is lacking!")
#endif

}

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
std::string ANN::getANNFile() const {
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
  std::string file = utils::getAbsoluteProjectPath() + ::david::constant::neuralNetworksFolder + filename;
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
bool ANN::hasANNFile() const {
  return this->ANNFile != "";
}

/**
 * Check if there exists a ANN instance
 */
bool ANN::hasANNInstance() const {
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
    std::string error = "ANN file does not exist: " + this->ANNFile;
    std::__throw_runtime_error(error.c_str());
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
int ANN::ANNEvaluate(type::gameState_t& board) const {
  const auto arr = ::utils::neuralNet::convertGameStateToInputs(board); // float array of the inputs

  // populate array
  fann_type inputs[::david::constant::nn::INPUTSIZE];
  const auto len = arr.size();
  for (unsigned int i = 0; i < len; i++) {
    inputs[i] = arr[i];
  }

  fann_type* outputs = fann_run(this->ANNInstance, inputs); // float array

  return static_cast<int>(outputs[0] * 1000); // The expected output during training was multiplied by 0.001
}

/**
 * Run the boards through the trained neural network to get a generated output.
 *
 * @param fen std::string FEN(Forsyth–Edwards Notation)
 * @return int board evaluation
 */
int ANN::ANNEvaluate(const std::string& fen) const {
  type::gameState_t gs;
  utils::gameState::generateFromFEN(gs, fen);

  return this->ANNEvaluate(gs);
}

}