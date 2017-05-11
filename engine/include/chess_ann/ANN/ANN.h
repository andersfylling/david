#pragma once

// local dependencies
#import "chess_ann/definitions.h"
#include <chess_ann/bitboard.h>

// git submodule libraries
#import "fann/floatfann.h"

// system dependencies
#import <iostream>

// forward decleration

class ANN {
 public:
  ANN(std::string filename);
  ANN(definitions::engineContext_ptr context, std::string filename);
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
  int ANNEvaluate(definitions::gameState_ptr board, ::bitboard::COLOR color);


  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param fen std::string FEN(Forsyth–Edwards Notation)
   * @return int board evaluation
   */
  int ANNEvaluate(std::string fen);



 private:
  definitions::engineContext_ptr engineContextPtr;
  std::string ANNFile;
  fann* ANNInstance;
};