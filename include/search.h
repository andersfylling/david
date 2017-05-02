//
// Created by markus on 4/24/17.
//

#ifndef CHESS_ANN_SEARCH_H
#define CHESS_ANN_SEARCH_H


#include <list>

class Node {
public:
    int value;
    std::list<Node *> children;
    Node();
    Node(int depth, int n);
    ~Node();
    void print();
};

namespace negamax {
    int negamax(Node* node, int alpha, int beta, int depth);
    int max(int value1, int value2);
}






#endif //CHESS_ANN_SEARCH_H
