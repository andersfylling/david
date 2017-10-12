#pragma once

namespace david {

/**
 * This is what the game tree is made up of.
 */
class ScoreNode {
 private:
  int score;
  //BitboardsNode* bitboardsNode;

 public:
  ScoreNode();
  ScoreNode(int score);
  int setScore(int score);
  int getScore();

};
}