#pragma once

// local dependencies
#include "david/types.h"
#include <david/bitboard.h>

// git submodule libraries
#include "fann/floatfann.h"

// system dependencies
#include <iostream>

// forward decleration

namespace david {
class ANN {
 public:
  ANN(std::string filename);
  ANN(type::engineContext_ptr context, std::string filename);
  ~ANN();


  /**
   * Retrieve the ANN file this engine instance uses for evaluating game boards.
   * @return std::string absolute path of ann file.
   */
  std::string getANNFile();

  /**
   * Check if there exists a ANNFile
   */
  bool hasANNFile();

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
  bool hasANNInstance();

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
  int ANNEvaluate(const type::gameState_t& board);


  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param fen std::string FEN(Forsyth–Edwards Notation)
   * @return int board evaluation
   */
  int ANNEvaluate(const std::string& fen);



 private:
  type::engineContext_ptr engineContextPtr;
  std::string ANNFile;
  fann* ANNInstance;
};
} // namespace david end