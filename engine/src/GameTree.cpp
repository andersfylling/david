#include "chess_ann/bitboard.h"
#include "chess_ann/GameTree.h"
#include <memory>
#include <iostream>
#include <algorithm>

/**
 * Constructor
 */
::gameTree::GameTree::GameTree()
    : maxNumberOfNodes(100)
{}

/**
 * Constructor
 * @param node std::share_ptr<::bitboard::gameState>, pass by copy(!!)
 */
::gameTree::GameTree::GameTree(std::shared_ptr<gameState> node)
    : current(node),
      maxNumberOfNodes(100)
{
}

/**
 * Destructor
 */
::gameTree::GameTree::~GameTree()
{}

/**
 * Remove every node in this class instance memory.
 */
void ::gameTree::GameTree::reset() {
  this->current.reset();
  this->previous.reset();
  this->maxNumberOfNodes = 100;
}

/**
 * Resets a single node. This also removes it's children if no other pointer
 * is storing their address.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::reset(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  node.reset();
}

/**
 * This deletes a nodes children, and potentially the childrens children.
 * @param node std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::resetChildren(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  for (nodePtr child : node->children) {
    this->reset(child);
  }
}

/**
 * Swap out the current node with a new one. The old node is stored in case
 * this change is regretted.
 * This does not reset the max allowed nodes.
 *
 * @see ::gameTree::GameTree::regretNewRootNode()
 * @param node std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::newRootNode(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  this->previous = this->current;
  this->previous = nullptr; // make it dissapear, is this safe? TODO: validate.
  this->current = node;
}

/**
 * Generates children for a given node, and sorts the children.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::generateChildren(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  // find some way to get all the different boards
  // then call generate node
  // you prolly need to change it's parameters
  int livingNodes = this->getNumberOfNodes();

  // mock test
  for (int i = 0; i < 35 && i < livingNodes; livingNodes++, i++) {
    this->generateNode(node);
  }

  this->sortChildren(node);
}

/**
 * Sorts children of a given node based on score.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::sortChildren(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  std::sort(node->children.begin(), node->children.end(),
       [](const nodePtr& a, const nodePtr& b) -> bool
       {
         return a->score > b->score;
       });
}

/**
 * This is a restriction to keep the generator from generating infinite nodes.
 * The number given here will equal the max allowed number of nodes in memory.
 *
 * @param n std::share_ptr<::bitboard::gameState>
 */
void ::gameTree::GameTree::setMaxNumberOfNodes(int n) {
  if (n >= 0) {
    this->maxNumberOfNodes = n;
  }
  else {
    std::cerr << "::gameTree::GameTree::setMaxNumberOfNodes(int n): Integer 'n' can not be smaller than 0." << std::endl;
  }
}

/**
 * Retrieve the max allowed number of nodes in memory.
 * This amount is only limited to this instance.
 *
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int ::gameTree::GameTree::getMaxNumberOfNodes() {
  return this->maxNumberOfNodes;
}

/**
 * Just starts generating nodes until the max allowed number of nodes has been reached.
 * Also sorts children.
 */
void ::gameTree::GameTree::generateNodes() {
  if (this->current == nullptr) {
    std::cerr << "Need a node pointer first" << std::endl;
    return;
  }

  int livingNodes = this->getNumberOfNodes();

  nodePtr node = this->current;
  while (livingNodes < this->maxNumberOfNodes) {
    this->generateNode(node);
    livingNodes += 1;
  }

  this->sortChildren(node);

}

/**
 * Gives the number of nodes in memory, within this class instance.
 * This is done by looping through every node, except nullptr.
 *
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int ::gameTree::GameTree::getNumberOfNodes() {
  if (this->current == nullptr) {
    return 0;
  }

  int count = 1;

  for (std::shared_ptr<gameState> node : this->current->children) {
    count += this->getNumberOfNodes(node);
  }

  return count;
}

/**
 * Private method used for recursive action to count nodes.
 *
 * @param node
 * @return int from 0 to N, N <= maxNumberOfNodes
 */
int ::gameTree::GameTree::getNumberOfNodes(nodePtr node) {
  if (node->children.size() == 0) {
    return 1;
  }
  for (std::shared_ptr<gameState> node : this->current->children) {
    return this->getNumberOfNodes(node);
  }
}

namespace gameTree { // why..

/**
 * Generates a child node for a parent and correctly link them, and insert info.
 *
 * @param parent nodePtr
 * @return nodePtr of the new child, however parent will link to this anyways.
 */
nodePtr GameTree::generateNode(nodePtr parent) {
  nodePtr node = std::make_shared<gameState>();

  node->gameTreeLevel = parent->gameTreeLevel + 1;
  node->score = std::rand() % 2429 + 21;
  node->fullMoves = (node->gameTreeLevel + 1) / 2;
  node->halfMoves = 0; // eh..
  node->weakParent = parent;

  // check if there are any possible moves after this state


  // add child to parent
  parent->children.push_back(node);

  return node;
}

/**
 * Get the active node pointer in this class instance.
 *
 * @return nodePtr
 */
nodePtr GameTree::getCurrentNode() {
  return this->current;
}

/**
 * Converts the previous node to the current node.
 * Does not reset maxAllowedNumberOfNodes.
 *
 * @return The new node, which was the old one.
 */
nodePtr GameTree::regretNewRootNode() {
  if (this->previous == nullptr) {
    return nullptr;
  }

  this->current = this->previous;
  this->previous = nullptr;

  return this->current;
}

}