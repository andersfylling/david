//
// Created by anders on 5/10/17.
//

#include <sstream>
#include <chess_ann/Context.h>
#include <chess_ann/EngineMaster.h>


chess_ann::EngineMaster::EngineMaster(const std::string filename)
    :filename(filename),
     lastEngineInstanceID(1),
     lastEngineBattleID(1)
{}

int chess_ann::EngineMaster::spawnEngine() {
  auto context = std::make_shared<chess_ann::Context>();
  auto engine = std::make_shared<chess_ann::Engine>(context, annExecFile + this->filename);

  // add engine to context class pointer
  context->engine = engine;

  engine->createANNInstance();
  if (engine->hasANNInstance()) {
    // insert engine and update last id
    this->lastEngineInstanceID += 1;
    this->engineInstances.insert( std::pair<int, enginePtr>(this->lastEngineInstanceID, engine) );
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
int chess_ann::EngineMaster::battle(int engineID1, int engineID2) {
  return this->battle(engineID1, engineID2, bitboard::startFENPosition);
}

/**
 * Initiates a battle with desired chess layout
 *
 * @param engineID1 an engine instance ID
 * @param engineID2 an engine instance ID, != engineID1
 * @return
 */
int chess_ann::EngineMaster::battle(const int engineID1, const int engineID2, const std::string fen) {
  // check if one if the engines wasn't initiated
  if (engineID1 + engineID2 <= 0) {
    return -1;
  }

  // check if its the same instance
  if (engineID1 == engineID2) {
    return -1;
  }

  // make sure they don't use UCI
  enginePtr eng1 = this->engineInstances[engineID1];
  enginePtr eng2 = this->engineInstances[engineID2];
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

  enginePtr currentPlayer = color == "w" ? eng1 : eng2;

  // lets start the game!
  gameTree::nodePtr currentGame = currentPlayer->getGameState();
  do {

    // update game state of new current player
    currentPlayer->setGameState(currentGame);

    // ask for player / engine move decision
    currentPlayer->findBestMove();

    // update current game state
    currentGame = currentPlayer->getGameState();

    // update active player / engine
    currentPlayer = color == "w" ? eng2 : eng1;
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
bool chess_ann::EngineMaster::battleWinner(int battleID, int mainEngineID) {
  if (this->engineBattleWinnerLog.count(battleID) == 0) {
    return false;
  }

  return this->engineBattleWinnerLog[battleID] == mainEngineID;
}
int chess_ann::EngineMaster::battleWinner(int battleID) {}
void chess_ann::EngineMaster::trainUntilWinner(int mainEngineID, int opponentEngineID) {}