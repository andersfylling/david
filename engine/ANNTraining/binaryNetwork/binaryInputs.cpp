#include "fann/fann.h"
#include "fann/fann_cpp.h"
#include "chess_ann/utils.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <chess_ann/environment.h>
#include <chess_ann/GameTree.h>
#include "chess_ann/ANN/binaryInputs.h"
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;

// Callback function that simply prints the information to cout
int binaryNetwork::print_callback(FANN::neural_net &net, FANN::training_data &train,
                   unsigned int max_epochs, unsigned int epochs_between_reports,
                   float desired_error, unsigned int epochs, void *user_data) {
  cout << "Epochs     " << setw(8) << epochs << ". "
       << "Current Error: " << left << net.get_MSE() << right << endl;
  return 0;
}

// Test function that demonstrates usage of the fann C++ wrapper
void binaryNetwork::train_network(
    const std::string folder,
    const unsigned int* layers,
    const float learning_rate,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports)
{
  cout << endl << "training started." << endl;

  cout << endl << "Creating network." << endl;

  FANN::neural_net net;
  net.create_standard_array(3, layers);

  net.set_learning_rate(learning_rate);

  net.set_activation_steepness_hidden(1.0);
  net.set_activation_steepness_output(1.0);

  net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
  net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

  // Set additional properties such as the training algorithm
  //net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

  // Output network type and parameters
  cout << endl << "Network Type                         :  ";
  switch (net.get_network_type()) {
    case FANN::LAYER:cout << "LAYER" << endl;
      break;
    case FANN::SHORTCUT:cout << "SHORTCUT" << endl;
      break;
    default:cout << "UNKNOWN" << endl;
      break;
  }
  net.print_parameters();

  cout << endl << "Training network." << endl;

  FANN::training_data data;
  if (data.read_train_from_file(folder + '/' + trainingdatafile))
  {
    // Initialize and train the network with the data
    net.init_weights(data);

    cout << "Max Epochs " << setw(8) << max_iterations << ". "
         << "Desired Error: " << left << desired_error << right << endl;
    net.set_callback(print_callback, NULL);
    net.train_on_data(data, max_iterations,
                      iterations_between_reports, desired_error);

    cout << endl << "Testing network." << endl;

    for (unsigned int i = 0; i < data.length_train_data(); ++i) {
      // Run the network on the test data
      fann_type *calc_out = net.run(data.get_input()[i]);

//      cout << "Stockfish test (" << showpos << data.get_input()[i][0] << ", "
//           << data.get_input()[i][1] << ") -> " << *calc_out
//           << ", should be " << data.get_output()[i][0] << ", "
//           << "difference = " << noshowpos
//           << fann_abs(*calc_out - data.get_output()[i][0]) << endl;
    }

    cout << endl << "Saving network." << endl;

    // Save the network in floating point and fixed point
    net.save(folder + "/save_float.net");
    unsigned int decimal_point = net.save_to_fixed(folder + "/save_fixed.net");
    data.save_train_to_fixed(folder + "/save_fixed.data", decimal_point);

    cout << endl << "Test completed." << endl;
  }
}


/**
 * Creates a unique training file for this specific neural network.
 *
 * @param folder
 * @param max_trainingSets
 * @param layers
 */
void binaryNetwork::generateTrainingFile(
    const std::string folder,
    const unsigned int max_trainingSets,
    const unsigned int* layers)
{
  std::ifstream infile(folder + "/trainingdata/fenAndStockfishScores.data");
  std::fstream output(folder + "/binaryNetwork/BUFFER_" + trainingdatafile, std::ios::out | std::ios::trunc);

  int trainingPairs = 0;
  int lines = 0;
  ::environment::Environment env(::bitboard::COLOR::WHITE);

  std::string line;
  while (std::getline(infile, line) && max_trainingSets > trainingPairs && !infile.eof()) {
    lines += 1;

    if (lines % 1000 == 0) {
      std::cout << "Line#: " << lines << std::endl;
    }


    // write this to a file
    if (output.is_open()) {

      ::gameTree::nodePtr node = env.generateBoardFromFen(line);
      std::array<::bitboard::bitboard_t, 12> boards = {
          node->BlackBishop,
          node->BlackKing,
          node->BlackKnight,
          node->BlackPawn,
          node->BlackQueen,
          node->BlackRook,
          node->WhiteBishop,
          node->WhiteQueen,
          node->WhiteKnight,
          node->WhitePawn,
          node->WhiteRook,
          node->WhiteKing
      };

      // generate inputs
      for (auto b : boards) {
        auto ba = std::bitset<64>(b);
        for (int i = 0; i < ba.size(); i++) {
          output << ba[i] << ' ';
        }
      }
      output << std::endl;

      // expected output
      std::string score;
      std::getline(infile, score);
      output << score << std::endl;

      // inc record
      trainingPairs += 1;
    }
  }
  infile.close();
  output.close();

  // update file info
  // set training information in the top of the file
  // and the rest of the content below
  std::ifstream fromBufferFile(folder + "/binaryNetwork/BUFFER_" + trainingdatafile);
  std::ofstream outputUpdate(folder + "/binaryNetwork/" + trainingdatafile, std::ios::out | std::ios::trunc);
  if (outputUpdate.is_open()) {
    outputUpdate << std::to_string(trainingPairs) << " "
                 << std::to_string(layers[0]) << " "
                 << std::to_string(layers[(sizeof(layers) / sizeof(layers[0]))])
                 << std::endl;
    outputUpdate << fromBufferFile.rdbuf();
    outputUpdate.close();
  }
  fromBufferFile.close();

  // info
  std::cout << "TRAINING FILE GENERATED!" << std::endl;
  std::cout << "Created # of training sets: " << trainingPairs << std::endl;
}

/**
 * Contains configuration settings for this specific neural network.
 */
void binaryNetwork::run()
{
  const float learning_rate = 0.7f;
  const float desired_error = 0.001f;
  const unsigned int max_iterations = 1000;
  const unsigned int max_trainingSets = 30000;
  const unsigned int iterations_between_reports = 1;
  const unsigned int layers[3] = {768, 112, 1}; // input, hidden1, ..., hiddenN, output

  const auto folder = ::utils::getAbsoluteProjectPath() + "/engine/ANNTraining";

  // Generates the training data and returns the filename.
  //generateTrainingFile(folder, max_trainingSets, layers);

  try {
    std::ios::sync_with_stdio(); // Syncronize cout and printf output
    train_network(
        folder + "/binaryNetwork",
        layers,
        learning_rate,
        desired_error,
        max_iterations,
        iterations_between_reports);
  }
  catch (...) {
    cerr << endl << "Abnormal exception." << endl;
  }
}