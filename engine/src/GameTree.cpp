#include "chess_ann/bitboard.h"
#include "chess_ann/GameTree.h"
#include <memory>
#include <iostream>

::gameTree::GameTree::GameTree()
    : maxNumberOfNodes(100)
{}

::gameTree::GameTree::GameTree(std::shared_ptr<gameState> node)
    : current(node),
      maxNumberOfNodes(100)
{
}

::gameTree::GameTree::~GameTree()
{}

void ::gameTree::GameTree::reset() {
  this->current.reset();
  this->previous.reset();
  this->maxNumberOfNodes = 100;
}

void ::gameTree::GameTree::reset(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  node.reset();
}

void ::gameTree::GameTree::resetChildren(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  for (nodePtr child : node->children) {
    this->reset(child);
  }
}

void ::gameTree::GameTree::newRootNode(nodePtr node) {
  if (node == nullptr) {
    return;
  }

  this->previous.reset();
  this->previous = current;
  this->current = node;
  this->maxNumberOfNodes = 100;
}




void ::gameTree::GameTree::setMaxNumberOfNodes(int n) {
  if (n >= 0) {
    this->maxNumberOfNodes = n;
  }
  else {
    std::cerr << "::gameTree::GameTree::setMaxNumberOfNodes(int n): Integer 'n' can not be smaller than 0." << std::endl;
  }
}
bool ::gameTree::GameTree::hasMaxNumberOfNodes() {
  return this->maxNumberOfNodes == this->getNumberOfNodes();
}
int ::gameTree::GameTree::getMaxNumberOfNodes() {
  return this->maxNumberOfNodes;
}


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

}

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

int ::gameTree::GameTree::getNumberOfNodes(nodePtr node) {
  if (node->children.size() == 0) {
    return 1;
  }
  for (std::shared_ptr<gameState> node : this->current->children) {
    return this->getNumberOfNodes(node);
  }
}

namespace gameTree { // why..

nodePtr GameTree::generateNode(nodePtr parent) {
  nodePtr node = std::make_shared<gameState>();

  node->gameTreeLevel = parent->gameTreeLevel + 1;
  node->score = std::rand() % 2429 + 21;
  node->fullMoves = (node->gameTreeLevel + 1) / 2;
  node->halfMoves = 0; // eh..
  node->weakParent = parent;

  // do bitboard stuff when applicable

  // add child to parent
  parent->children.push_back(node);

  return node;
}

nodePtr GameTree::getCurrentNode() {
  return this->current;
}

nodePtr GameTree::regretNewRootNode() {
  if (this->previous == nullptr) {
    return nullptr;
  }

  this->current = this->previous;
  this->previous = nullptr;
  this->maxNumberOfNodes = 100;

  return this->current;
}

}