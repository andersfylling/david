
#include "david/EngineMaster.h"
#include "david/ChessEngine.h"

#include <memory>
#include <sstream>
#include <david/david.h>

david::EngineMaster::EngineMaster(const std::string filename)
    :ANNFilename(filename),
     lastEngineInstanceID(1),
     lastEngineBattleID(1),
     engineInstances()
{}

int david::EngineMaster::spawnEngine() {
  auto engine = std::make_shared<ChessEngine>(this->ANNFilename);

  if (engine->hasANNInstance()) {
    // insert engine and update last id
    this->lastEngineInstanceID += 1;
    this->engineInstances[this->lastEngineInstanceID] = engine; //.insert( std::make_pair(this->lastEngineInstanceID, engine) );
    return this->lastEngineInstanceID;
  }
  else {
    return -1;
  }
}

/**
 * Initiates a battle with default chess layout
 *
 * @param engineID1 an engine instance ID
 * @param engineID2 an engine instance ID, != engineID1
 * @return
 */
int david::EngineMaster::battle(int engineID1, int engineID2) {
  return this->battle(engineID1, engineID2, david::FENStartPosition);
}

/**
 * Initiates a battle with desired chess layout
 *
 * @param engineID1 an engine instance ID
 * @param engineID2 an engine instance ID, != engineID1
 * @return
 */
int david::EngineMaster::battle(const int engineID1, const int engineID2, const std::string fen) {
  // check if one if the engines wasn't initiated
  if (engineID1 + engineID2 <= 0) {
    return -1;
  }

  // check if its the same instance
  if (engineID1 == engineID2) {
    return -1;
  }

  // make sure they don't use UCI
  auto eng1 = this->engineInstances[engineID1];
  auto eng2 = this->engineInstances[engineID2];
  if (eng1->hasInitiatedUCIProtocol()) {
    return -1;
  }
  if (eng2->hasInitiatedUCIProtocol()) {
    return -1;
  }

  // construct the board for each engine
  eng1->setNewGameBoard(fen);
  eng2->setNewGameBoard(fen);

  // set eng1 to white.
  eng1->setPlayerColor(bitboard::COLOR::WHITE);
  eng2->setPlayerColor(bitboard::COLOR::BLACK);

  // Find out who is the current player
  std::stringstream sstr(fen);
  std::string color = "w";
  sstr >> color; // now it contains the board layout
  sstr >> color; // now it gets the color

  auto currentPlayer = color == "w" ? eng1 : eng2;
  bool whitesTurn = true;

  // lets start the game!
  definitions::gameState_ptr currentGame = currentPlayer->getGameState();
  do {
    whitesTurn = color == "w";

    utils::printGameState(currentGame);

    // update game state of new current player
    currentPlayer->setGameState(currentGame);

    // ask for player / engine move decision
    currentPlayer->findBestMove();

    // update current game state
    currentGame = currentPlayer->getGameState();

    std::cout << "current game" << std::endl;
    utils::printGameState(currentGame);

    // update active player / engine
    currentPlayer = whitesTurn ? eng2 : eng1;
    color = whitesTurn ? "b" : "w";
  } while (currentGame->halfMoves < 50 && currentGame->possibleSubMoves != 0);

  std::cout << currentGame->fullMoves << std::endl;

  auto winnerID = -1;
  if (eng1->lost()) {
    winnerID = engineID2;
  }
  else if (eng2->lost()) {
    winnerID = engineID1;
  }

  this->lastEngineBattleID += 1;
  this->engineBattleWinnerLog.insert( std::pair<int, int>(this->lastEngineBattleID, winnerID) );

  return this->lastEngineBattleID;

}
bool david::EngineMaster::battleWinner(int battleID, int mainEngineID) {
  if (this->engineBattleWinnerLog.count(battleID) == 0) {
    return false;
  }

  return this->engineBattleWinnerLog[battleID] == mainEngineID;
}
int david::EngineMaster::battleWinner(int battleID) {}
void david::EngineMaster::trainUntilWinner(int mainEngineID, int opponentEngineID) {}