#include "fann/floatfann.h"
#include "fann/fann_cpp.h"
#include "david/utils.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fstream>
#include <array>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <david/environment.h>
#include <david/GameTree.h>
#include <fann/parallel_fann.h>
#include "david/ANN/binaryInputs.h"
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;

namespace binaryNetwork {
std::ofstream trainingAccuracy;
std::ofstream trainingMSE;
}

// Callback function that simply prints the information to cout
int binaryNetwork::print_callback(FANN::neural_net &net, FANN::training_data &train,
                                  unsigned int max_epochs, unsigned int epochs_between_reports,
                                  float desired_error, unsigned int epochs, void *user_data) {
  float engine = net.run(train.get_input()[epochs])[0];
  float expected = train.get_output()[epochs][0];
  cout << "Epochs     " << setw(8) << epochs << ". "
       << "Current Error: " << left << net.get_MSE() << " <> " << desired_error << right << endl; // << " <> " << engine << " == " << expected

  trainingAccuracy << epochs << ", " << engine << ", " << expected << std::endl;
  trainingMSE << epochs << ", " << net.get_MSE() << std::endl;

  return 0;
}

// Test function that demonstrates usage of the fann C++ wrapper
void binaryNetwork::train_network(
    const std::string folder,
    const std::string filename,
    const unsigned int* layers,
    const unsigned int nrOfLayers,
    const float learning_rate,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports)
{
  trainingMSE.open ("training_mse.csv");
  trainingAccuracy.open ("training_accuracy.csv");
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
  if (data.read_train_from_file(folder + "/ANNTraining/binaryNetwork/tmp/" + filename + ".data"))
  {
    // Initialize and train the network with the data
    //net.init_weights(data);
    net.randomize_weights(-0.301, 0.301);

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

    std::time_t id = std::time(nullptr);

    std::string idStr = std::to_string(id);
    net.save(folder + "/src/ANN/networks/float_" + filename + "_" + idStr + ".net");
    unsigned int decimal_point = net.save_to_fixed(folder + "/src/ANN/networks/fixed_" + filename + "_" + idStr + ".net");
    data.save_train_to_fixed(folder + "src/ANN/networks/fixed_" + filename + "_" + idStr + ".data", decimal_point);



    //
    // Write statistic to csv file
    //
//    std::ofstream searchStats;
//    searchStats.open ("search_statistics.csv");
//    for(int i = 0; i < iterations; i++){
//      searchStats << iterationsArray[i][0] << ',' << iterationsArray[i][1] << ',' << i << std::endl;
//    }
//    searchStats.close();

    cout << endl << "Test completed." << endl;
    trainingAccuracy.close();
    trainingMSE.close();
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
    const std::string filename,
    const unsigned int max_trainingSets,
    const unsigned int* layers,
    const unsigned int nrOfLayers)
{
  std::ifstream infile(folder + "/trainingdata/fenAndStockfishScores.data");
  std::fstream output(folder + "/binaryNetwork/tmp/BUFFER_" + filename + ".data", std::ios::out | std::ios::trunc);

  int trainingPairs = 0;
  int lines = 0;
  ::david::environment::Environment env(::david::bitboard::COLOR::WHITE);

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

      ::david::definitions::gameState_ptr node = env.generateBoardFromFen(line);
      std::stringstream strm(line);
      std::string blackTurn = "";
      strm >> blackTurn;
      blackTurn = "";
      strm >> blackTurn;
      env.setGameState(node);
      env.generateAttacks();
      bool isB = blackTurn == "b";
      std::array<double, 37> inputs = {
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

          // should be a const that shows who this board is being evaluated for
          // then seperate it into, friendly & enemies.
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


          (isB ? 1 : -1 ) * (static_cast<double>(env.numberOfPieces(env.whitePieces())) / 100.0), // is never 0
          (isB ? -1 : 1 ) * (static_cast<double>(env.numberOfPieces(env.blackPieces())) / 100.0), // is never 0
          static_cast<double>(env.numberOfPieces(env.whitePieces() | env.blackPieces())) / 100.0,

          (isB ? -1 : 1 ) * (static_cast<double>(env.numberOfPieces(env.combinedBlackAttacks() & env.whitePieces())) < 1.0 ? 1.0 : -1.0 * (static_cast<double>(env.numberOfPieces(env.combinedBlackAttacks() & env.whitePieces())) / 100.0)),
          (isB ? 1 : -1 ) * (static_cast<double>(env.numberOfPieces(env.combinedWhiteAttacks() & env.blackPieces())) < 1.0 ? -1.0 : static_cast<double>(env.numberOfPieces(env.combinedWhiteAttacks() & env.blackPieces())) / 100.0),

          (isB ? 1 : -1 ) * (node->blackQueenCastling ? 1.0 : -1.0),
          (isB ? 1 : -1 ) * (node->blackKingCastling  ? 1.0 : -1.0),
          (isB ? -1 : 1 ) * (node->whiteQueenCastling ? 1.0 : -1.0),
          (isB ? -1 : 1 ) * (node->whiteKingCastling  ? 1.0 : -1.0),

          static_cast<double>(node->halfMoves) / 100.0,
          static_cast<double>(node->fullMoves) / 100.0,

          // if the color playing is not yours, and the number here is high, it should not be a good thing.
          (isB ? -1 : 1 ) * static_cast<double>(node->children.size()) / 100.0 // will always be 0 unless children are generated before comparing score.

      };

      // generate inputs
      int nInputs = 0;
      for (auto b : inputs) {
        fileStringInput << std::setprecision(2) << b << ' '; // limit to one decimal... I think
        nInputs += 1;
      }
      std::array<::david::bitboard::bitboard_t, 2> boards1 = {
          node->BlackKing,
          node->WhiteKing
      };
      std::array<::david::bitboard::bitboard_t, 8> boards2 = {
          node->BlackBishop,
          node->BlackKnight,
          node->BlackQueen,
          node->BlackRook,
          node->WhiteBishop,
          node->WhiteQueen,
          node->WhiteKnight,
          node->WhiteRook
      };
      std::array<::david::bitboard::bitboard_t, 2> boards8 = {
          node->BlackPawn,
          node->WhitePawn
      };

      // generate inputs
      for (auto b : boards1) {
        auto ba = std::bitset<64>(b);
        double arr[1] = {-1.0};
        auto prog = 0;
        for (uint8_t i = 0; i < ba.size(); i++) {
          if (::david::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }

        for (; prog < 1; prog++) {
          arr[prog] = -1.0;
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
        for (uint8_t i = 0; i < ba.size(); i++) {
          if (::david::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }
        for (; prog < 2; prog++) {
          arr[prog] = -1.0;
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
        for (uint8_t i = 0; i < ba.size(); i++) {
          if (::david::utils::bitAt(b, i)) {
            arr[prog++] = i == 0 ? 0 : i / 10.0;
          }
        }
        for (; prog < 8; prog++) {
          arr[prog] = -1.0;
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
      double fScore = std::stoi(score) * 0.0001;
      fileStringInput << fScore << std::endl;

      // inc record
      trainingPairs += 1;
      lineNr += 1;


      // make sure u have the right amount of inputs
      if (nInputs != layers[0]) {
        //std::cerr << "nInputs: " << nInputs << ", expected: " << layers[0] << ". Line#" << lineNr << std::endl;
        skippedtrainingSets += 1;
        fileStringInput.str("");
        fileStringInput.clear();
        continue;
      }
      else {
        output << fileStringInput.str();
        fileStringInput.str("");
        fileStringInput.clear();
      }
      //assert(nInputs == layers[0]);

    }
  }
  infile.close();
  output.close();

  // update file info
  // set training information in the top of the file
  // and the rest of the content below
  std::ifstream fromBufferFile(folder + "/binaryNetwork/tmp/BUFFER_" + filename + ".data");
  std::ofstream outputUpdate(folder + "/binaryNetwork/tmp/" + filename + ".data", std::ios::out | std::ios::trunc);
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
  const float desired_error = 0.00001f;
  const unsigned int max_iterations = 120000;
  const unsigned int max_trainingSets = 150000;
  const unsigned int iterations_between_reports = 1;
  const unsigned int nrOfLayers = 5;
  const unsigned int layers[nrOfLayers] = {71, 200, 40, 12, 1}; // input, hidden1, ..., hiddenN, output
  const auto folder = ::david::utils::getAbsoluteProjectPath() + "/engine";

  // Generates the training data and returns the filename.
  std::string fileNameSuffix = trainingdatafile + "_" + std::to_string(nrOfLayers) + "_" + std::to_string(layers[0]) + "_" + std::to_string(layers[nrOfLayers - 1]);
  generateTrainingFile(folder + "/ANNTraining", fileNameSuffix, max_trainingSets, layers, nrOfLayers);

  try {
    std::ios::sync_with_stdio(); // Syncronize cout and printf output
    train_network(
        folder,
        fileNameSuffix,
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