//
// Created by anders on 5/2/17.
//

#ifndef CHESS_ANN_NODE_H
#define CHESS_ANN_NODE_H

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

#endif //CHESS_ANN_NODE_H
