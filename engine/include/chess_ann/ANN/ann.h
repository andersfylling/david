#ifndef CHESS_ANN_ANN_H
#define CHESS_ANN_ANN_H

#include "fann/floatfann.h"
#include "fann/fann_cpp.h"

namespace ANN {
int evaluate(gameTree::nodePtr node);
fann_type* extractInputs(gameTree::nodePtr node);
}

#endif //CHESS_ANN_ANN_H
