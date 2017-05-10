//
// Created by anders on 5/10/17.
//

#ifndef CHESS_ANN_ENGINE_H
#define CHESS_ANN_ENGINE_H

#include "chess_ann/bitboard.h"
#include "chess_ann/uci/Listener.h"
#include "chess_ann/uci/events.h"
#include <chess_ann/GameTree.h>
#include <chess_ann/utils.h>
#include <chess_ann/environment.h>
#include "chess_ann/genericUCIResponses.h"
#include "chess_ann/Context.h"
#include <chess_ann/Search.h>

namespace search {}

namespace chess_ann {

/**
 * A player object that will represent who this engine is on the board.
 */
struct Player {
  ::bitboard::COLOR color = ::bitboard::COLOR::WHITE;
};


class Engine {
  uci::Listener uciProtocol;
  search::Search searchAgent;

  bool UCIProtocolActivated;

  std::string ANNFile;
  fann* ANNInstance;
  Player player;

  gameTree::nodePtr currentGameState;

  std::shared_ptr<Context> context;


 public:

  Engine(std::shared_ptr<chess_ann::Context> context);
  Engine(std::shared_ptr<chess_ann::Context> context, Player self);
  Engine(std::shared_ptr<chess_ann::Context> context, std::string ANNFile);
  Engine(std::shared_ptr<chess_ann::Context> context, Player self, std::string ANNFile);
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
   * Check if UCI is activated on this engine or not.
   *
   * @return bool if UCI protocol has been activated.
   */
  bool hasInitiatedUCIProtocol();

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
   * Start the ANN from given files.
   */
  void createANNInstance();

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

  /**
   * Used to reset old data, and construct a new fresh game for this engine.
   *
   * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
   */
  void setNewGameBoard(const std::string fen);

  /**
   * Update this players color, color means what piece this player can move..
   *
   * @param color
   */
  void setPlayerColor(bitboard::COLOR color);

  /**
   * Get the game node from a engine. Will be known as the root node for the search class.
   *
   * @return shared_ptr of gameState
   */
  gameTree::nodePtr getGameState();

  /**
   * Sets the game state based on a node.
   * Used when updating board states during battles.
   *
   * @param state shared_ptr of a gameState
   * @return true if the state was updated
   */
  bool setGameState(gameTree::nodePtr state);

  /**
   * Check if the engine has lost.
   *
   * @return true on losing
   */
  bool lost();

  /**
   * Find the best move, and update the current game state.
   */
  void findBestMove();


};

}

#endif //CHESS_ANN_ENGINE_H
