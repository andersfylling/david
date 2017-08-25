
#include "david/TreeGen.h"
#include <algorithm>
#include <david/utils/utils.h>
#include <assert.h>
#include "david/ANN/ANN.h"
#include "david/MoveGeneration.h"
#include "david/MoveGen.h"

namespace david {
namespace gameTree {

/**
 * Constructor
 */
TreeGen::TreeGen(const type::NeuralNetwork_t& NeuralNetRef)
    : neuralnet(NeuralNetRef),
      maxDepth(5),
      startposFEN(constant::FENStartPosition),
      nrOfEGNMoves(-1),
      historyIndex(0)
{
  int i = 0;
  int iAfterFirstMove = 0;
  while (this->depthIndexes.end() == 0) {
    this->depthIndexes[++i] = constant::MAXMOVES * iAfterFirstMove + 1;
  }
}

/**
 * Destructor
 */
TreeGen::~TreeGen() {}

/**
 * Get a reference of the given node at index.
 *
 * @param index unsigned int Index in game tree
 * @return Mutable gameState reference
 */
type::gameState_t& TreeGen::getGameState(const unsigned int index) {
  return this->tree[index];
}

/**
 * Get a copy of the given node at index.
 *
 * @param index unsigned int Index in game tree
 * @return Mutable gameState copy
 */
type::gameState_t TreeGen::getGameStateCopy(const unsigned int index) const {
  return this->tree[index];
}

/**
 * Get score of node at index.
 * @param index
 * @return
 */
int TreeGen::getGameStateScore(const unsigned int index) const {
  return this->tree[index].score;
}

/**
 * Maximum depth allowed to search to.
 *
 * @return int Max Depth of game tree
 */
int TreeGen::getDepth() const
{
  return this->maxDepth;
}

/**
 * Get the tree index that represents the element at relative position from depth and index.
 *
 * @param depth -1 < depth < constant::MAXDEPTH
 * @param index -1 < index < constant::MAXMOVES
 * @return index that can be used in the game tree, range [0, constant::MAXMOVES * constant::MAXDEPTH]
 */
unsigned int TreeGen::treeIndex(const uint8_t depth, const uint8_t index) const {
  return constant::MAXMOVES * depth + index + 1; // will never be below 0
}

void TreeGen::updateRootNodeTo(const int index) {
  auto c = this->tree.front().isWhite;
  this->tree.front() = std::move(this->tree[index]);

  if (c == this->tree.front().isWhite) {
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

void TreeGen::setRootNode(const type::gameState_t& gs) { // TODO: bad?
  this->tree.front() = std::move(gs);
}

/**
 * Remove every node in this class instance memory.
 */
void TreeGen::reset() {
  // no need to delete the game tree itself as it just gets overwritten on demand

  // clear history
  this->history.empty();
  this->historyIndex = 0;

  // clear start position
  this->startposFEN = "";

  // remove EGN moves and nrOfEGNMoves
  this->EGNMoves.empty();
  this->nrOfEGNMoves = -1;

}

/**
 * Generates children for a given node, and sorts the children.
 *
 * @param node std::share_ptr<::bitboard::gameState>
 */
uint16_t TreeGen::generateChildren(const unsigned int index) {
  using type::gameState_t;
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;

  auto& node = this->tree[index];

  // Get ready to generate all potential moves based on given chess board
#ifdef MOVEGEN
  MoveGen gen{node}; // new version
#else
  movegen::MoveGenerator gen; // old version
  gen.setGameState(node);     // old version
#endif

  // uint8 has a max num of 255. Max nr of children is 256. perfect af.
  unsigned int firstChildPos = index == 0 ? constant::MAXMOVES - 1 : (index - 1) % constant::MAXMOVES;
  firstChildPos = index + (constant::MAXMOVES - firstChildPos);

  // create a holder for possible game outputs
#ifdef MOVEGEN
  const uint16_t len = gen.generateGameStates(this->tree, firstChildPos, firstChildPos + constant::MAXMOVES); // new version
#else
  std::vector<gameState_t> states; // old version

  // generate possible game outputs
  gen.generateGameStates(states); // old version

  // create node pointers, and set some internal data
  //std::cout << "OPTIONS ========== " << std::endl;
  const uint16_t len = node.possibleSubMoves = static_cast<uint16_t>(states.size()); // should not need more than uint8
  //utils::printGameState(node);
#endif

#ifdef DAVID_DEVELOPMENT
  assert(firstChildPos != 0);
#endif

  for (uint16_t i = 0; i < len; i++) {
#ifdef MOVEGEN

    auto& n = this->tree[firstChildPos + i];

    // use ann to get score
    n.score = this->neuralnet.ANNEvaluate(n);
    //if (this->NN != nullptr) {
    //  n.score = this->engineContextPtr->neuralNetworkPtr->ANNEvaluate(n);
    //}
#else
    this->generateNode(node, this->tree[firstChildPos + i], states.at(i));
#endif
  }

  // once all the nodes are set, we need to sort the children.
  std::sort(this->tree.begin() + firstChildPos, this->tree.begin() + firstChildPos + len,
            [](const type::gameState_t& a, const type::gameState_t& b) -> bool {
              return a.score > b.score;
            });

  return len;
}

/**
 * Generates a child node for a parent and correctly link them, and insert info.
 * TODO: remove and write changes directly in loop.
 *
 * @param parent nodePtr
 * @return nodePtr of the new child, however parent will link to this anyways.
 */
void TreeGen::generateNode(const type::gameState_t& parent, type::gameState_t& n, const type::gameState_t child) {
  using bitboard::gameState;
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::bitboard_t;

  // There should be some record that stores the difference between the node left to this one to improve
  // updating the node. cause shit this is way slower than it needs to be.
  n = child;

#ifndef MOVEGEN

  n.blackPieces = n.BlackPawn | n.BlackQueen | n.BlackKnight | n.BlackKing | n.BlackBishop | n.BlackRook;
  n.whitePieces = n.WhitePawn | n.WhiteQueen | n.WhiteKnight | n.WhiteKing | n.WhiteBishop | n.WhiteRook;

  n.pieces = n.blackPieces | n.whitePieces;

  // this is now done in movegen.. but should it be?
  //n.isWhite = parent.playerColor == BLACK; // TODO: make sure this is set on new root elements
  //n.playerColor = n.isWhite ? BLACK : WHITE;

  n.halfMoves = parent.halfMoves + 1;
  n.fullMoves = (parent.gameTreeLevel + 1) / 2;
  
  // set the new array data
  int i = 0;
  int j = 1;
  // j -> white, so if the active colour is white. index 0 should reference to the active coloured pieces.
  if (n.isWhite) {
    i = 1;
    j = 0;
  }
  n.pawns[i]   = n.BlackPawn;
  n.pawns[j]   = n.WhitePawn;
  n.rooks[i]   = n.BlackRook;
  n.rooks[j]   = n.WhiteRook;
  n.knights[i] = n.BlackKnight;
  n.knights[j] = n.WhiteKnight;
  n.bishops[i] = n.BlackBishop;
  n.bishops[j] = n.WhiteBishop;
  n.queens[i]  = n.BlackQueen;
  n.queens[j]  = n.WhiteQueen;
  n.kings[i]   = n.BlackKing;
  n.kings[j]   = n.WhiteKing;
  n.piecess[i] = n.blackPieces;
  n.piecess[j] = n.whitePieces;
  n.combinedPieces = n.pieces;

  // Validate half moves
  if (!utils::isHalfMove(parent, n)) {
    n.halfMoves = 0;
  }

  // use ann to get score
  n.score = this->neuralnet.ANNEvaluate(n);
#endif
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

void TreeGen::generateEGNMoves()
{
  // TODO: have a bool that tells whether or not moves have been generated
  // TODO: This will avoid regenerating nodes that already exist.
  this->generateChildren(0);

  const auto& root = this->tree.front();
  const int len = this->nrOfEGNMoves = root.possibleSubMoves;
  for (int i = 0; i < len; i++) {
    this->EGNMoves[i] = utils::getEGN(root, this->tree[i + 1]);
  }
}

void TreeGen::applyEGNMove(const std::string& EGN)
{
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
  this->history[this->historyIndex++] = EGN;

  if (this->historyIndex >= this->history.size()) {
    std::__throw_runtime_error("Need to grow the history array in TreeGen!!!");
  }

  // now that the root node has changed, clear the EGN record
  this->EGNMoves.empty();
  this->nrOfEGNMoves = -1;
}

}
}