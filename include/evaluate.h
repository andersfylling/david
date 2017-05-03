//
// Created by markus on 4/24/17.
//

#ifndef CHESS_ANN_EVALUATE_H
#define CHESS_ANN_EVALUATE_H

#include "../include/variables.h"
#include "../src/environment.h"


    //
    // Main evaluate function, declared there
    //
    enum Values evaluate(Enviornment * node);
    enum Values materialEvaluation(Enviornment * node);


#endif //CHESS_ANN_EVALUATE_H
