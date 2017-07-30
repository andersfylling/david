#include "david/bitboard.h"
#include "david/TreeGen.h"
#include <memory>
#include <iostream>
#include <algorithm>
#include <david/environment.h>
#include <david/utils.h>
#include <assert.h>
#include "david/ANN/ANN.h"
#include "david/MoveGeneration.h"

namespace david {
namespace gameTree {

/**
 * Constructor
 */
TreeGen::TreeGen(type::engineContext_ptr ctx)
    : engineContextPtr(ctx),
      maxNumberOfNodes(100),
      maxDepth(5),
      startposFEN(constant::FENStartPosition),
      nrOfEGNMoves(-1)
{
  int i = 0;
  int iAfterFirstMove = 0;
  while (this->depthIndexes.end() == 0) {
    this->depthIndexes[++i] = constant::MAXMOVES * iAfterFirstMove + 1;
  }
}

TreeGen::TreeGen()
    : engineContextPtr(),
      maxNumberOfNodes(100),
      maxDepth(5),
      startposFEN(constant::FENStartPosition),
      nrOfEGNMoves(-1)
{
#ifndef DAVID_TEST
  std::cerr << "TreeGen::TreeGen() empty contructor can only be used for unit testing!" << std::endl;
#endif

  int i = 0;
  int iAfterFirstMove = 0;
  while (this->depthIndexes.end() == 0) {
    this->depthIndexes[++i] = constant::MAXMOVES * iAfterFirstMove + 1;
  }
}


type::gameState_t& TreeGen::getGameState(unsigned int index) {
  return this->tree[index];
}

int TreeGen::getGameStateScore(unsigned int index) {
  return this->tree[index].score;
}

unsigned int TreeGen::treeIndex(uint8_t depth, uint8_t index) {
  return constant::MAXMOVES * depth + index + 1; // will never be below 0
}

void TreeGen::updateRootNodeTo(int index) {
  auto c = this->tree.front().playerColor;
  this->tree.front() = std::move(this->tree[index]);

  if (c == this->tree.front().playerColor) {
    std::cerr << "color was not changed!!!" << std::endl;
  }
}

void TreeGen::setRootNodeFromFEN(const std::string& FEN) {
  if (FEN == constant::FENStartPosition) {
    utils::setDefaultChessLayout(this->tree.front());
  }
  else {
    utils::generateBoardFromFen(this->tree.front(), FEN);
  }
}

void TreeGen::setRootNode(const type::gameState_t& gs) {
  this->tree.front() = std::move(gs);
}

/**
 * Destructor
 */
TreeGen::~TreeGen() {
}

/**
 * Remove every node in this class instance memory.
 */
void TreeGen::reset() {
  // no need to delete the game tree itself as it just gets overwritten on demand

  // clear history
  this->history.clear();

  // clear start position
  this->startposFEN = "";

  // remove EGN moves and nrOfEGNMoves
  this->EGNMoves.empty();
  this->nrOfEGNMoves = -1;

}


unsigned int TreeGen::getChildIndex(unsigned int parent, unsigned int child) {
  //return this->depthIndexes[parent + 1 + child];
  //utils::yellDeprecated("TreeGen::getChildIndex is not working correctly!!!!");
  if (parent == 0) {
    return 1 + child;
  }
  else if (parent <= constant::MAXMOVES) {
    return constant::MAXMOVES + 1 + child;
  }
  else if (parent > constant::MAXMOVES) {
    return ((parent / constant::MAXMOVES) + 1) * constant::MAXMOVES + 1 + child;
  }
}

/**
 * Generates children for a given node, and sorts the children.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
void TreeGen::generateChildren(unsigned int index) {
  using bitboard::gameState;
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;

  auto& node = this->tree[index];

  // find some way to get all the different boards
  // then call generate node
  // you prolly need to change it's parameters
  //int livingNodes = this->getNumberOfNodes();

  // mock test
  //node->gs->children.resize(0);

  // created a environ instance based on parent node
  movegen::MoveGenerator gen;
  gen.setGameState(node);

  //env.setGameStateColor(node->playerColor == WHITE ? BLACK : WHITE);
//  if (!(node->fullMoves == 1 && node->playerColor == WHITE)) {
  //   env.setGameStateColor(node->playerColor == WHITE ? BLACK : WHITE);
  //}

  // create a holder for possible game outputs
  std::vector<gameState> states;

  // generate possible game outputs
  gen.generateGameStates(states);

  // create node pointers, and set some internal data
  //std::cout << "OPTIONS ========== " << std::endl;
  const auto len = node.possibleSubMoves = static_cast<int>(states.size());
  //utils::printGameState(node);
  // uint8 has a max num of 255. Max nr of children is 256. perfect af.
  unsigned int firstChildPos = index == 0 ? constant::MAXMOVES - 1 : (index - 1) % constant::MAXMOVES;
  firstChildPos = index + (constant::MAXMOVES - firstChildPos);

#ifdef DAVID_DEVELOPMENT
  assert(firstChildPos != 0);
#endif

  for (uint8_t i = 0; i < len; i++) {
    this->generateNode(node, this->tree[firstChildPos + i], states.at(i));
  }
  //std::cout << "OPTIONS END ====== " << std::endl;

  // once all the nodes are set, we need to sort the children.
  std::sort(this->tree.begin() + firstChildPos, this->tree.begin() + firstChildPos + len,
            [](const type::gameState_t& a, const type::gameState_t& b) -> bool {
              return a.score > b.score;
            });
}


/**
 * Retrieve the max allowed number of nodes in memory.
 * This amount is only limited to this instance.
 *
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int TreeGen::getMaxNumberOfNodes() {
  return this->maxNumberOfNodes;
}

/**
 * Generates a child node for a parent and correctly link them, and insert info.
 *
 * @param parent nodePtr
 * @return nodePtr of the new child, however parent will link to this anyways.
 */
void TreeGen::generateNode(type::gameState_t& parent, type::gameState_t& n, type::gameState_t child) {
  using bitboard::gameState;
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::bitboard_t;

  // There should be some record that stores the difference between the node left to this one to improve
  // updating the node. cause shit this is way slower than it needs to be.
  n = child;

  n.blackPieces = n.BlackPawn | n.BlackQueen | n.BlackKnight | n.BlackKing | n.BlackBishop | n.BlackRook;
  n.whitePieces = n.WhitePawn | n.WhiteQueen | n.WhiteKnight | n.WhiteKing | n.WhiteBishop | n.WhiteRook;

  n.pieces = n.blackPieces | n.whitePieces;

  n.playerColor = parent.playerColor == BLACK ? WHITE : BLACK;

  n.halfMoves = parent.halfMoves + 1;
  n.fullMoves = (parent.gameTreeLevel + 1) / 2;

  // Validate half moves
  if (!utils::isHalfMove(parent, n)) {
    n.halfMoves = 0;
  }
  // check if there are any possible moves after this state

  // use ann to get score
  if (this->engineContextPtr->neuralNetworkPtr != nullptr) {
    n.score = this->engineContextPtr->neuralNetworkPtr->ANNEvaluate(n);
  }

//  n.WhitePawn    = child.WhitePawn;
//  n.WhiteRook    = child.WhiteRook;
//  n.WhiteKnight  = child.WhiteKnight;
//  n.WhiteBishop  = child.WhiteBishop;
//  n.WhiteQueen   = child.WhiteQueen;
//  n.WhiteKing    = child.WhiteKing;
//
//  n.BlackPawn    = child.BlackPawn;
//  n.BlackRook    = child.BlackRook;
//  n.BlackKnight  = child.BlackKnight;
//  n.BlackBishop  = child.BlackBishop;
//  n.BlackQueen   = child.BlackQueen;
//  n.BlackKing    = child.BlackKing;

//  n.lastBlackMove = child.lastBlackMove;
//  n.lastWhiteMove = child.lastWhiteMove;

//  // set sub possibilities
//  movegen::MoveGenerator gen;
//  gen.setGameState(parent->children[index]->gs);
//  std::vector<gameState> states;
//  gen.generateGameStates(states);
//  parent->children[index]->gs->possibleSubMoves = static_cast<int>(states.size()); //won't go above 40 or something.
}

void TreeGen::setMaxDepth(int d)
{
  bool timeToGrow = d > this->maxDepth;
  this->maxDepth = d;

  // if this is a larger number than we started with we need to add depths!
  if (timeToGrow) {
    std::cerr << "NOT IMPLEMENTED YET!!!!!" << std::endl;
  }
}

void TreeGen::generateEGNMoves() {

  // TODO: have a bool that tells whether or not moves have been generated
  // TODO: This will avoid regenerating nodes that already exist.
  this->generateChildren(0);

  const auto& root = this->tree.front();
  const int len = this->nrOfEGNMoves = root.possibleSubMoves;
  for (int i = 0; i < len; i++) {
    this->EGNMoves[i] = utils::getEGN(root, this->tree[i + 1]);
  }
}

void TreeGen::applyEGNMove(const std::string& EGN) {
  if (EGN.length() != 4) {
    std::cerr << "EGN length is not 4!!" << std::endl;
    return;
  }

  // Locate the index (+1) if EGNMoves have been generated.
  // Not sure how to check if EGN moves have been generated, so just generate them now.
  this->generateEGNMoves();

  // locate index
  int index = -1;
  for (int i = 0; i < this->nrOfEGNMoves; i++) {
    if (this->EGNMoves[i] == EGN) {
      index = i + 1; // +1 cause we need to offset for the root node in this->tree
      break;
    }
  }

  // if the EGNMove wasn't found, exit cause then it isn't valid for this board layout!
  if (index == -1) {
    std::cerr
        << "Invalid EGN based on current board layout inside the engine.. Did you forget to update the layout?"
        << std::endl;
    utils::printGameState(this->tree.front());
    return;
  }

  // index was found, let's update the root node !
  this->updateRootNodeTo(index);

  // Add this move to the history
  this->history.push_back(EGN);

  // now that the root node has changed, clear the EGN record
  this->EGNMoves.empty();
  this->nrOfEGNMoves = -1;
}

}
}