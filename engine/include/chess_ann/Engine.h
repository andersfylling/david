//
// Created by anders on 5/10/17.
//

#ifndef CHESS_ANN_ENGINE_H
#define CHESS_ANN_ENGINE_H

#include "chess_ann/bitboard.h"
#include "chess_ann/uci/Listener.h"
#include "chess_ann/uci/events.h"
#include <chess_ann/Search.h>
#include <chess_ann/GameTree.h>
#include <chess_ann/utils.h>
#include <chess_ann/environment.h>
#include "chess_ann/genericUCIResponses.h"

namespace chess_ann {

/**
 * A player object that will represent who this engine is on the board.
 */
struct Player {
  ::bitboard::COLOR color = ::bitboard::COLOR::WHITE;
};


class Engine {
  ::uci::Listener uciProtocol;
  ::search::Search searchAgent;

  std::string ANNFile;
  fann* ANNInstance;
  Player player;

  /**
   * Creates the neural network based on the ANNFile
   */
  void createANNInstance();

 public:

  Engine();
  Engine(Player self);
  Engine(std::string ANNFile);
  Engine(Player self, std::string ANNFile);
  ~Engine();

  /**
   * Adds typical UCI responses to the engine
   */
  void configureUCIProtocol();

  /**
   * Actives the UCI protocol, and keeps it running in another thread.
   */
  void activateUCIProtocol();

  /**
   * Gives this engine a UCI Protocol command, but does not return responses.
   * @deprecated, hasn't been implemented yet.
   */
  void sayUCICommand(std::string command);

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
   * Check if this engine plays as white
   */
  bool isWhite();

  /**
   * Get ::bitboard::COLOR color
   */
  ::bitboard::COLOR getColor();

  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param board ::gameTree::gameState, of shared_ptr type
   * @return int board evaluation
   */
  int ANNEvaluate(::gameTree::nodePtr board);


  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param fen std::string FEN(Forsyth–Edwards Notation)
   * @return int board evaluation
   */
  int ANNEvaluate(std::string fen);


};

}

#endif //CHESS_ANN_ENGINE_H
