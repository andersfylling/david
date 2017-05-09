#include "fann/floatfann.h"
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
#include <fann/parallel_fann.h>
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
    const unsigned int nrOfLayers,
    const float learning_rate,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports)
{
  cout << endl << "training started." << endl;

  cout << endl << "Creating network." << endl;

  FANN::neural_net net(FANN::network_type_enum::LAYER, nrOfLayers, layers);
  //net.create_standard_array(nrOfLayers, layers);

  net.set_learning_rate(learning_rate);

  net.set_activation_steepness_hidden(1.0);
  net.set_activation_steepness_output(1.0);

  net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC);
  net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC);

  // Set additional properties such as the training algorithm
  net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

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

void binaryNetwork::train_network_parallel(
    const std::string folder,
    const unsigned int* layers,
    const unsigned int nrOfLayers,
    const float learning_rate,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports)
{
  cout << endl << "training started." << endl;

  cout << endl << "Creating network." << endl;

  const unsigned int max_epochs = 1000;
  unsigned int num_threads = 4;
  struct fann_train_data *data;
  struct fann *ann;
  float error;
  unsigned int i;

  auto fiStr = folder + '/' + trainingdatafile;
  data = fann_read_train_from_file(fiStr.c_str());
  ann = fann_create_standard(nrOfLayers, layers);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_SIGMOID);

  for(i = 1; i <= max_epochs; i++)
  {
    error = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann, data, num_threads) : fann_train_epoch(ann, data);
    printf("Epochs     %8d. Current error: %.10f\n", i, error);
  }

  fann_destroy(ann);
  fann_destroy_train(data);
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
    const unsigned int* layers,
    const unsigned int nrOfLayers)
{
  std::ifstream infile(folder + "/trainingdata/fenAndStockfishScores.data");
  std::fstream output(folder + "/binaryNetwork/BUFFER_" + trainingdatafile, std::ios::out | std::ios::trunc);

  int trainingPairs = 0;
  int lines = 0;
  ::environment::Environment env(::bitboard::COLOR::WHITE);

  std::string line;
  int lineNr = 1;
  int skippedtrainingSets = 0;
  std::stringstream fileStringInput;
  while (std::getline(infile, line) && max_trainingSets > (trainingPairs - skippedtrainingSets) && !infile.eof()) {
    lines += 1;

    if (lines % 1000 == 0) {
      std::cout << 100 * (trainingPairs - skippedtrainingSets) / (max_trainingSets) << '%' << std::endl;
    }


    // write this to a file
    if (output.is_open()) {

      ::gameTree::nodePtr node = env.generateBoardFromFen(line);
      std::stringstream strm(line);
      std::string blackTurn = "";
      strm >> blackTurn;
      blackTurn = "";
      strm >> blackTurn;
      env.setGameState(node);
      env.generateAttacks();
      bool isB = blackTurn == "b";
      std::array<double, 30> inputs = {
          blackTurn == "b" ? -1.0 : 1.0,
          (isB ? -1 : 1 ) * (env.numberOfPieces(node->BlackBishop) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackKing) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackKnight) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackPawn) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackQueen) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackRook) > 0.1 ? 1 : -1),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackBishop) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackBishop)) / 10.0),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackKing) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackKing)) / 10.0),
          (isB ? -1 : 1 )  * (env.numberOfPieces(node->BlackKnight) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackKnight)) / 10.0),
          (isB ? -1 : 1 ) * (env.numberOfPieces(node->BlackPawn) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackPawn)) / 10.0),
          (isB ? -1 : 1 ) * (env.numberOfPieces(node->BlackQueen) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackQueen)) / 10.0),
          (isB ? -1 : 1 ) * (env.numberOfPieces(node->BlackRook) < 1.0 ? -1 : static_cast<double>(env.numberOfPieces(node->BlackRook)) / 10.0),


          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteBishop) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteQueen) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteKnight) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhitePawn) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteRook) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteKing) > 0.1 ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteBishop) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhiteBishop)) / 10.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteQueen) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhiteQueen)) / 10.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteKnight) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhiteKnight)) / 10.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhitePawn) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhitePawn)) / 10.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteRook) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhiteRook)) / 10.0),
          (isB ? 1 : -1 ) * (env.numberOfPieces(node->WhiteKing) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(node->WhiteKing)) / 10.0),


          (isB ? 1 : -1 ) * static_cast<double>(env.numberOfPieces(env.whitePieces())) / 10.0, // is never 0
          (isB ? -1 : 1 ) * static_cast<double>(env.numberOfPieces(env.blackPieces())) / 10.0, // is never 0
          static_cast<double>(env.numberOfPieces(env.whitePieces() | env.blackPieces())) / 10.0,

          (isB ? -1 : 1 ) * static_cast<double>(env.numberOfPieces(env.combinedBlackAttacks() & env.whitePieces())) < 1.0 ? 1.0 : -1.0 * (static_cast<double>(env.numberOfPieces(env.combinedBlackAttacks() & env.whitePieces())) / 10.0),
          (isB ? 1 : -1 ) * static_cast<double>(env.numberOfPieces(env.combinedWhiteAttacks() & env.blackPieces())) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(env.combinedWhiteAttacks() & env.blackPieces())) / 10.0,
      };

      // generate inputs
      int nInputs = 0;
      for (auto b : inputs) {
        fileStringInput << std::setprecision(2) << b << ' '; // limit to one decimal... I think
        nInputs += 1;
      }
      std::array<::bitboard::bitboard_t, 2> boards1 = {
          node->BlackKing,
          node->WhiteKing
      };
      std::array<::bitboard::bitboard_t, 8> boards2 = {
          node->BlackBishop,
          node->BlackKnight,
          node->BlackQueen,
          node->BlackRook,
          node->WhiteBishop,
          node->WhiteQueen,
          node->WhiteKnight,
          node->WhiteRook
      };
      std::array<::bitboard::bitboard_t, 2> boards8 = {
          node->BlackPawn,
          node->WhitePawn
      };

      // generate inputs
      for (auto b : boards1) {
        auto ba = std::bitset<64>(b);
        double arr[1] = {-1.0};
        auto prog = 0;
        for (int i = 0; i < ba.size(); i++) {
          if (::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }

        for (auto e : arr) {
          fileStringInput << std::setprecision(2) << e << ' ';
          nInputs += 1;
        }
      }

      // Issue, what if the first on is gone? same on boards8
      for (auto b : boards2) {
        auto ba = std::bitset<64>(b);
        double arr[2] = {-1.0, -1.0};
        auto prog = 0;
        for (int i = 0; i < ba.size(); i++) {
          if (::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }

        for (auto e : arr) {
          fileStringInput << std::setprecision(2) << e << ' ';
          nInputs += 1;
        }
      }
      for (auto b : boards8) {
        auto ba = std::bitset<64>(b);
        double arr[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
        auto prog = 0;
        for (int i = 0; i < ba.size(); i++) {
          if (::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }

        for (auto e : arr) {
          fileStringInput << std::setprecision(2) << e << ' ';
          nInputs += 1;
        }
      }
      fileStringInput << std::endl;


      // expected output
      std::string score;
      std::getline(infile, score);
      fileStringInput << score << std::endl;

      // inc record
      trainingPairs += 1;
      lineNr += 1;


      // make sure u have the right amount of inputs
      if (nInputs != layers[0]) {
        //std::cerr << "nInputs: " << nInputs << ", expected: " << layers[0] << ". Line#" << lineNr << std::endl;
        skippedtrainingSets += 1;
        fileStringInput.clear();
        continue;
      }
      else {
        output << fileStringInput.str();
      }
      //assert(nInputs == layers[0]);

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
    outputUpdate << std::to_string(trainingPairs - skippedtrainingSets) << " "
                 << std::to_string(layers[0]) << " "
                 << std::to_string(layers[nrOfLayers - 1])
                 << std::endl;
    outputUpdate << fromBufferFile.rdbuf();
    outputUpdate.close();
  }
  fromBufferFile.close();

  // info
  std::cout << "TRAINING FILE GENERATED!" << std::endl;
  std::cout << "Created # of training sets: " << (trainingPairs - skippedtrainingSets) << " / " << trainingPairs << std::endl;
}

/**
 * Contains configuration settings for this specific neural network.
 */
void binaryNetwork::run()
{
  const float learning_rate = 0.7f;
  const float desired_error = 0.001f;
  const unsigned int max_iterations = 10000;
  const unsigned int max_trainingSets = 30000;
  const unsigned int iterations_between_reports = 1;
  const unsigned int nrOfLayers = 3;
  const unsigned int layers[nrOfLayers] = {64, 84, 1}; // input, hidden1, ..., hiddenN, output

  const auto folder = ::utils::getAbsoluteProjectPath() + "/engine/ANNTraining";

  // Generates the training data and returns the filename.
  generateTrainingFile(folder, max_trainingSets, layers, nrOfLayers);

  try {
    std::ios::sync_with_stdio(); // Syncronize cout and printf output
    train_network(
        folder + "/binaryNetwork",
        layers,
        nrOfLayers,
        learning_rate,
        desired_error,
        max_iterations,
        iterations_between_reports);
  }
  catch (...) {
    cerr << endl << "Abnormal exception." << endl;
  }
}