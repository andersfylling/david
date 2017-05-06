#include "chess_ann/bitboard.h"
#include "chess_ann/GameTree.h"
#include <memory>
#include <iostream>

::gameTree::GameTree::GameTree()
    : previous(nullptr),
      current(nullptr),
      maxLevel(1),
      maxNumberOfNodes(100),
      maxStoredLevels(2)
{}

::gameTree::GameTree::GameTree(std::shared_ptr<gameState> node)
    :previous(nullptr),
     current(node),
     maxLevel(1),
     maxNumberOfNodes(100),
     maxStoredLevels(2)
{}

void ::gameTree::GameTree::reset() {
  this->previous = nullptr;
  this->current = nullptr;
  this->maxLevel = 1;
  this->maxNumberOfNodes = 100;
  this->maxStoredLevels = 2;
}

/**
 * Generates child nodes for given node. Make sure you do not
 * @param node
 */
void ::gameTree::GameTree::generateChildrenForNode(nodePtr node) {

}

void ::gameTree::GameTree::setMaxLevel(int level) {
  this->maxLevel = level;
}

void ::gameTree::GameTree::setMaxNumberOfNodes(int level) {
  this->maxLevel = level;
}
bool ::gameTree::GameTree::hasMaxLevel() {
  return this->maxLevel != 0;
}
bool ::gameTree::GameTree::hasMaxNumberOfNodes() {
  return this->maxNumberOfNodes != 0;
}
int ::gameTree::GameTree::getMaxLevel() {
  return this->maxLevel;
}
int ::gameTree::GameTree::getMaxNumberOfNodes() {
  return this->maxNumberOfNodes;
}
bool ::gameTree::GameTree::hasMaxStoreLevel() {
  return this->maxStoredLevels != 0;
}
void ::gameTree::GameTree::setMaxStoreLevel(int level) {
  this->maxStoredLevels = level;
}
int ::gameTree::GameTree::getMaxStoreLevel() {
  return this->maxStoredLevels;
}

/**
 * Generates nodes until the maxLevel is reached or if maxStoredLevel, if set, is reached first.
 * If both have the value -1, this will never stop to generated nodes. mhuahaha.
 */
void ::gameTree::GameTree::generateLevels() {
  if (this->current == nullptr) {
    std::cerr << "Need a node pointer first" << std::endl;
    return;
  }

  if (this->maxLevel == 0 && this->maxStoredLevels == 0) {
    std::cerr << "both maxLevel and maxStoredLevels equals 0" << std::endl;
    return;
  }

  int floor = this->current->gameTreeLevel;

}



void ::gameTree::GameTree::generateNodes() {
  if (this->current == nullptr) {
    std::cerr << "Need a node pointer first" << std::endl;
    return;
  }

}

int ::gameTree::GameTree::getNumberOfNodes() {
  if (this->current == nullptr) {
    std::cerr << "Need a node pointer first" << std::endl;
    return -1;
  }

  int count = 1;

  std::vector<nodePtr> que;
  que.push_back(this->current);
  do {
    nodePtr node = que.at(0);
    count += node->children.size();

    for (nodePtr child : node->children) {
      que.push_back(child);
    }

  } while ();

  for (std::shared_ptr<gameState> node : this->current->children) {

  }
}

namespace gameTree { // why..

nodePtr GameTree::generateNode(nodePtr parent) {
  nodePtr node = std::make_shared<gameState>();

  node->gameTreeLevel = parent->gameTreeLevel + 1;
  node->score = std::rand() % 2429 + 21;
  node->fullMoves = (node->gameTreeLevel + 1) / 2;
  node->halfMoves = 0; // eh..

  // do bitboard stuff when applicable

  // add child to parent
  parent->children.push_back(node);

  return node;
}

nodePtr GameTree::getCurrentNode() {
  return this->current;
}

}