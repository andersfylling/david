//
// Created by markus on 4/24/17.
//

#include <iostream>
#include <limits>
#include <list>
#include <random>
#include <search.h>
#include "./variables.h"

using namespace std;

//
//Class functions & Global functions
//
Node::Node() {
    value = 0;
}

Node::Node(int depth, int n) {
    if (-1 == n) {
        n = rand() % 2 + 1;

        if (n < 0) {
            n = 7;
        }
    }

    value = 0;

    //Hvis maks dybde er n�dd, exit
    if (depth <= -1) {
        return;
    }
    else if (depth == 0) {
        value = rand() % 25 + 1;
    }

    depth -= 1;

    while (children.size() <= n && depth != -1) {
        children.push_back(new Node(depth, rand() % 2 + 1));
    }
}

Node::~Node() {
    children.clear();
}

void Node::print() {
    cout << "Children [" << children.size() << "], value: " << value << endl;
    //Auto, burde caste til Node*
    for (auto i : children) {
        cout << "Children [" << i->children.size() << "], value: " << i->value << endl;
    }

    for (auto i : children) {
        i->print();
    }
}

//
// Basic negamax, may need to expand to take in to considerations some more advanced techniques
//

namespace negamax{

    int negamax(Node* node, int alpha, int beta, int depth) {
        int v;


        if (depth == 0 || node->children.empty()) {
            return node->value; //evaluate(/*some node*/); //Heuristic value of that node
        }
        int bestValue = numeric_limits<int>::min();
        for (auto i : node->children/*Needs to be corrected*/) {
            v = -negamax(i, depth - 1, -beta, -alpha);
            bestValue = max(bestValue, v);
            alpha = max(alpha, v);
            if (alpha>=beta)
            {
                continue;
            }
        }

        return bestValue;
    }

    int max(int value1, int value2) {
        return ((value1 > value2) ? value1 : value2);
    }
}
