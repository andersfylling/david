// negamaxWithAlphaBetaPruning.cpp : Defines the entry point for the console application.

//
//Include & namespace
//
#include <iostream>
#include <random>
#include <list>
#include <limits>

using namespace std;

//
//Const & Enum
//
const int MAX_DEPTH = 2;

//
//Class & struct
//
class Node {
public:
	int value;
	list<Node*> children;
	Node();
	Node(int depth, int n);
	~Node();
	void print();
};

//
//Global functions
//
int negamax(Node* node, int alpha, int beta, int depth, bool maximizingPlayer);
int max(int value1, int value2);

//
//globale variable
//
int alpha = numeric_limits<int>::min();
int beta = numeric_limits<int>::max();

//
//Main
//
int main(int argc, char *args[]) {

	//Default constructor for root, setter bare value = 0
	Node* root = new Node(MAX_DEPTH, -1);
	root->print();

	cout << endl << endl; 
	int  bestvalue = negamax(root, alpha, beta, MAX_DEPTH, 1);
	cout << bestvalue << endl << endl;
	return 0;
}

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

	//Hvis maks dybde er nådd, exit
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



int negamax(Node* node, int alpha, int beta, int depth, bool maximizingPlayer) {
	int v;

	
	if (depth == 0 || node->children.empty()) {
		return node->value; //Heuristic value of that node
	}
	int bestValue = numeric_limits<int>::min();
	for (auto i : node->children) {
		v = -negamax(i, depth - 1, -beta, -alpha, 0);
		bestValue = max(bestValue, v);
		alpha = max(alpha, v);
		switch (alpha>=beta)
		{
		case true:
			  break;
		}	
	}
	
	return bestValue;
}

int max(int value1, int value2) {
	return ((value1 > value2) ? value1 : value2);
}




