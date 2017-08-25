#pragma once

// local dependencies
#include "david/types.h"
#include <david/bitboard.h>

// git submodule libraries
#include "fann/floatfann.h"

// system dependencies
#include <iostream>

// forward declaration

namespace david {
class ANN {
 public:
  ANN();
  ANN(const std::string filename);
  ~ANN();


  /**
   * Retrieve the ANN file this engine instance uses for evaluating game boards.
   * @return std::string absolute path of ann file.
   */
  std::string getANNFile() const;

  /**
   * Check if there exists a ANNFile
   */
  bool hasANNFile() const;

  /**
   * Update the ANN file even if already set or not.
   * This requires the ANN to not be running.
   *
   * @param std::string filename, a file that resides within the engine/src/ANN/networks/ folder.
   */
  void setANNFile(std::string filename);

  /**
   * Check if there exists a ANN instance
   */
  bool hasANNInstance() const;

  /**
   * Start the ANN from given files.
   */
  void createANNInstance();

  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param board ::gameTree::gameState, of shared_ptr type
   * @return int board evaluation
   */
  int ANNEvaluate(const type::gameState_t& board) const;


  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param fen std::string FEN(Forsyth–Edwards Notation)
   * @return int board evaluation
   */
  int ANNEvaluate(const std::string& fen) const;



 private:
  std::string ANNFile;
  fann* ANNInstance;
};
} // namespace david end