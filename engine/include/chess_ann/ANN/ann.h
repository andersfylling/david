#ifndef CHESS_ANN_ANN_H
#define CHESS_ANN_ANN_H

#include "../lib/fann/src/include/floatfann.h"
#include "../lib/fann/src/include/fann_cpp.h"

namespace ANN {
void annTraining();
void annExecution();
void runTesting();
int print_callback(FANN::neural_net &net, FANN::training_data &train,
                   unsigned int max_epochs, unsigned int epochs_between_reports,
                   float desired_error, unsigned int epochs, void *user_data);
}

#endif //CHESS_ANN_ANN_H
