#include "david/bitboard.h"
#include "david/GameTree.h"
#include <memory>
#include <iostream>
#include <algorithm>
#include <david/environment.h>
#include <david/utils.h>
#include "david/ANN/ANN.h"
#include "david/MoveGeneration.h"

namespace david {

/**
 * Constructor
 */
gameTree::GameTree::GameTree(type::engineContext_ptr ctx)
    : engineContextPtr(ctx),
      maxNumberOfNodes(100),
      maxDepth(5)
{
  this->preallocateSearchTree();
}

/**
 * Allocate needed memory space for the game tree only once
 */
void gameTree::GameTree::preallocateSearchTree ()
{
  //this->tree;
}


type::gameState_t& gameTree::GameTree::getGameState(unsigned int index) {
  return this->tree[index];
}

int gameTree::GameTree::getGameStateScore(unsigned int index) {
  return this->tree[index].score;
}

unsigned int gameTree::GameTree::treeIndex(uint8_t depth, uint8_t index) {
  return constant::MAXMOVES * depth + index + 1; // will never be below 0
}

void gameTree::GameTree::setRootNodeFromFEN(const std::string &FEN) {
  utils::generateBoardFromFen(this->tree.front(), FEN);
}

/**
 * Destructor
 */
gameTree::GameTree::~GameTree() {
}

/**
 * Remove every node in this class instance memory.
 */
void gameTree::GameTree::reset() {
  // since the game tree is depends on be overwritten. theres no need to delete it.

  this->maxNumberOfNodes = 100;
}

/**
 * Resets a single node. This also removes it's children if no other pointer
 * is storing their address.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
//void gameTree::GameTree::reset(type::gameState_ptr node) {
//  if (node == nullptr) {
//    return;
//  }
//
//  scoreNode
//}

/**
 * This deletes a nodes children, and potentially the childrens children.
 * @param node std::share_ptr<::bitboard::gameState>
 */
//void gameTree::GameTree::resetChildren(type::gameState_ptr node) {
//  if (node == nullptr) {
//    return;
//  }
//
//  for (auto &child : node->children) {
//    this->reset(child);
//  }
//}

/**
 * Swap out the current node with a new one. The old node is stored in case
 * this change is regretted.
 * This does not reset the max allowed nodes.
 *
 * @see ::gameTree::GameTree::regretNewRootNode()
 * @param node std::share_ptr<::bitboard::gameState>
 */
//void gameTree::GameTree::newRootNode(type::gameState_ptr node) {
//  if (node == nullptr) {
//    return;
//  }
//
//  //this->previous.reset();
//  //this->previous = this->current;
//  //this->previous = nullptr; // make it dissapear, is this safe? TODO: validate.
//  this->current = node;
//}

unsigned int gameTree::GameTree::getChildIndex(unsigned int parent, unsigned int child) {
  unsigned int index = parent > constant::MAXMOVES ? parent / constant::MAXMOVES : 1;
}

/**
 * Generates children for a given node, and sorts the children.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
void gameTree::GameTree::generateChildren(unsigned int index) {
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
  // uint8 has a max num of 255. Max nr of children is 256. perfect af.
  unsigned int firstChildPos = index == 0 ? constant::MAXMOVES - 1 : (index - 1) % constant::MAXMOVES;
  firstChildPos = index + (constant::MAXMOVES - firstChildPos);
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
 * This is a restriction to keep the generator from generating infinite nodes.
 * The number given here will equal the max allowed number of nodes in memory.
 *
 * @param n std::share_ptr<::bitboard::gameState>
 */
void gameTree::GameTree::setMaxNumberOfNodes(int n) {
  if (n >= 0) {
    this->maxNumberOfNodes = n;
  } else {
    std::cerr << "::gameTree::GameTree::setMaxNumberOfNodes(int n): Integer 'n' can not be smaller than 0."
              << std::endl;
  }
}

/**
 * Retrieve the max allowed number of nodes in memory.
 * This amount is only limited to this instance.
 *
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int gameTree::GameTree::getMaxNumberOfNodes() {
  return this->maxNumberOfNodes;
}

/**
 * Just starts generating nodes until the max allowed number of nodes has been reached.
 * Also sorts children.
 *
 * I hate this function @deprecated, see generateChildren in stead.
 */
//void gameTree::GameTree::generateNodes() {
//  using bitboard::gameState;
//
//  if (this->current == nullptr) {
//    std::cerr << "Need a node pointer first" << std::endl;
//    return;
//  }
//
//  type::gameState_ptr node = this->current;
//  int nrOfNodes = this->getNumberOfNodes();
//
//  while (nrOfNodes < this->maxNumberOfNodes) {
//
//    movegen::MoveGenerator gen;
//    gen.setGameState(node);
//    std::vector<gameState> states;
//    gen.generateGameStates(states);
//
//    int children = 0;
//    for (int i = 0; i < states.size() && nrOfNodes < this->maxNumberOfNodes; nrOfNodes++, i++) {
//      this->generateNode(node, states.at(i));
//      children += 1;
//    }
//    this->sortChildren(node);
//
//    if (children > 0) {
//      //node = node->children.at(0);
//    }
//  }
//
//}

/**
 * Gives the number of nodes in memory, within this class instance.
 * This is done by looping through every node, except nullptr.
 *
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int gameTree::GameTree::getNumberOfNodes() {
  return 0;
//  using bitboard::gameState;
//
//  if (this->current == nullptr) {
//    return 0;
//  }
//
//  int count = 1;
//
//  for (std::shared_ptr<gameState> node : this->current->children) {
//    this->getNumberOfNodes(node, count);
//  }
//
//  return count;
}

/**
 * Private method used for recursive action to count nodes.
 *
 * @param node
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
void gameTree::GameTree::getNumberOfNodes(type::gameState_ptr node, int &count) {
//
//  using bitboard::gameState;
//
//  if (node->children.size() == 0) {
//    count += 1;
//  }
//  for (std::shared_ptr<gameState> child : node->children) {
//    this->getNumberOfNodes(child, count);
//  }
}

int gameTree::GameTree::getDepth() {
//  using bitboard::gameState;
//
//  if (this->current == nullptr) {
//    return 0;
//  }
//
//  int depth = this->current->gameTreeLevel;
//
//  for (std::shared_ptr<gameState> node : this->current->children) {
//    this->getDepth(node, depth);
//  }
//
//  return depth;
  return 0;
}
void gameTree::GameTree::getDepth(type::gameState_ptr node, int &depth) {
//  using bitboard::gameState;
//
//  if (node == nullptr) {
//    return; // this should never fire.
//  }
//
//  if (depth < node->gameTreeLevel) {
//    depth += 1;
//  }
//
//  for (std::shared_ptr<gameState> child : node->children) {
//    this->getDepth(child, depth);
//  }

}

/**
 * Print score and depth of all nodes
 * @param root
 */
//void gameTree::GameTree::printAllScores(type::gameState_ptr root) {
//  std::cout << "Node scores: (Game tree level)";
//  for (auto child : root->children) {
//    if (child->children.empty()) {
//      std::cout << ' ' << child->score << '(' << child->gameTreeLevel << ") ";
//    } else
//      printAllScores(child);
//  }
//  std::cout << std::endl;
//}

namespace gameTree { // why..

/**
 * Generates a child node for a parent and correctly link them, and insert info.
 *
 * @param parent nodePtr
 * @return nodePtr of the new child, however parent will link to this anyways.
 */
void GameTree::generateNode(type::gameState_t& parent, type::gameState_t& n, type::gameState_t child) {
  using bitboard::gameState;
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::bitboard_t;

  // There should be some record that stores the difference between the node left to this one to improve
  // updating the node. cause shit this is way slower than it needs to be.
  n.WhitePawn    = child.WhitePawn;
  n.WhiteRook    = child.WhiteRook;
  n.WhiteKnight  = child.WhiteKnight;
  n.WhiteBishop  = child.WhiteBishop;
  n.WhiteQueen   = child.WhiteQueen;
  n.WhiteKing    = child.WhiteKing;

  n.BlackPawn    = child.BlackPawn;
  n.BlackRook    = child.BlackRook;
  n.BlackKnight  = child.BlackKnight;
  n.BlackBishop  = child.BlackBishop;
  n.BlackQueen   = child.BlackQueen;
  n.BlackKing    = child.BlackKing;

  n.blackPieces = n.BlackPawn | n.BlackQueen | n.BlackKnight | n.BlackKing | n.BlackBishop | n.BlackRook;
  n.whitePieces = n.WhitePawn | n.WhiteQueen | n.WhiteKnight | n.WhiteKing | n.WhiteBishop | n.WhiteRook;

  n.pieces = n.blackPieces | n.whitePieces;

  n.lastBlackMove = child.lastBlackMove;
  n.lastWhiteMove = child.lastWhiteMove;

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

//  // set sub possibilities
//  movegen::MoveGenerator gen;
//  gen.setGameState(parent->children[index]->gs);
//  std::vector<gameState> states;
//  gen.generateGameStates(states);
//  parent->children[index]->gs->possibleSubMoves = static_cast<int>(states.size()); //won't go above 40 or something.
}

type::gameState_ptr GameTree::getCurrent() {
  return this->current;
}

/**
 * Get the active node pointer in this class instance.
 *
 * @return nodePtr
 */
//type::gameState_ptr GameTree::getCurrentNode() {
//  return this->current;
//}

/**
 * Converts the previous node to the current node.
 * Does not reset maxAllowedNumberOfNodes.
 *
 * @return The new node, which was the old one.
 */
//type::gameState_ptr GameTree::regretNewRootNode() {
//  if (this->previous == nullptr) {
//    return nullptr;
//  }
//
//  this->current = this->previous;
//  this->previous = nullptr;
//
//  return this->current;
//}

void GameTree::setMaxDepth(int d)
{
  bool timeToGrow = d > this->maxDepth;
  this->maxDepth = d;

  // if this is a larger number than we started with we need to add depths!
  if (timeToGrow) {
    std::cerr << "NOT IMPLEMENTED YET!!!!!" << std::endl;
  }
}

}
}