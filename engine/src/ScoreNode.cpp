//
// Created by anders on 5/2/17.
//

#include "chess_ann/ScoreNode.h"
ScoreNode::ScoreNode() {

}

ScoreNode::ScoreNode(int score)
  : score(score)
{

}

int ScoreNode::setScore(int score) {
  return this->score = score;
}

int ScoreNode::getScore() {
  return this->score;
}
