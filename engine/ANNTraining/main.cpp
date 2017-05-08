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
#include "stockfish/stockfishMock.h"
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;


// Callback function that simply prints the information to cout
int print_callback(FANN::neural_net &net, FANN::training_data &train,
                   unsigned int max_epochs, unsigned int epochs_between_reports,
                   float desired_error, unsigned int epochs, void *user_data)
{
  cout << "Epochs     " << setw(8) << epochs << ". "
       << "Current Error: " << left << net.get_MSE() << right << endl;
  return 0;
}

// Test function that demonstrates usage of the fann C++ wrapper
void xor_test(
    const float learning_rate,
    const unsigned int num_layers,
    const unsigned int num_input,
    const unsigned int num_hidden,
    const unsigned int num_output,
    const float desired_error,
    const unsigned int max_iterations,
    const unsigned int iterations_between_reports
)
{
  cout << endl << "training started." << endl;


  cout << endl << "Creating network." << endl;

  FANN::neural_net net;
  net.create_standard(5, num_input, 400, 200, 100, num_output);

  net.set_learning_rate(learning_rate);

  net.set_activation_steepness_hidden(1.0);
  net.set_activation_steepness_output(1.0);

  net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
  net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

  // Set additional properties such as the training algorithm
  //net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

  // Output network type and parameters
  cout << endl << "Network Type                         :  ";
  switch (net.get_network_type())
  {
    case FANN::LAYER:
      cout << "LAYER" << endl;
      break;
    case FANN::SHORTCUT:
      cout << "SHORTCUT" << endl;
      break;
    default:
      cout << "UNKNOWN" << endl;
      break;
  }
  net.print_parameters();

  cout << endl << "Training network." << endl;


  FANN::training_data data;
  if (data.read_train_from_file(::utils::getAbsoluteProjectPath() + "/engine/ANNTraining/trainingdata/fenAndStockfishScore.data")) //hacky fix
  {
    // Initialize and train the network with the data
    net.init_weights(data);

    cout << "Max Epochs " << setw(8) << max_iterations << ". "
         << "Desired Error: " << left << desired_error << right << endl;
    net.set_callback(print_callback, NULL);
    net.train_on_data(data, max_iterations,
                      iterations_between_reports, desired_error);

    cout << endl << "Testing network." << endl;

    for (unsigned int i = 0; i < data.length_train_data(); ++i)
    {
      // Run the network on the test data
      fann_type *calc_out = net.run(data.get_input()[i]);

      cout << "Stockfish test (" << showpos << data.get_input()[i][0] << ", "
           << data.get_input()[i][1] << ") -> " << *calc_out
           << ", should be " << data.get_output()[i][0] << ", "
           << "difference = " << noshowpos
           << fann_abs(*calc_out - data.get_output()[i][0]) << endl;
    }

    cout << endl << "Saving network." << endl;

    // Save the network in floating point and fixed point
    net.save(::utils::getAbsoluteProjectPath() + "/engine/ANNTraining/trainingdata/output/xor_float.net");
    unsigned int decimal_point = net.save_to_fixed(::utils::getAbsoluteProjectPath() + "/engine/ANNTraining/trainingdata/output/xor_fixed.net");
    data.save_train_to_fixed(::utils::getAbsoluteProjectPath() + "/engine/ANNTraining/trainingdata/output/xor_fixed.data", decimal_point);

    cout << endl << "XOR test completed." << endl;
  }
}


std::string getScoreFromStockfish(std::shared_ptr<FILE> pipe, std::string fen) {
  std::array<char, 128> buffer;
  std::string result;
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != NULL)
      result += buffer.data();
  }
  return result;
}

void generateTrainingFile(std::string folder, std::string in, std::string out,

                          const unsigned int max_iterations,
                          const unsigned int num_input,
                          const unsigned int num_output) {
  std::ifstream infile(folder + in);
  std::fstream output(folder + "BUFFER_" + out, std::ios::out | std::ios::trunc);

  int trainingPairs = 0;
  int skipped = 0;
  int lines = 0;
  ::environment::Environment env(::bitboard::COLOR::WHITE);
  //std::shared_ptr<FILE> pipe(popen("./Stockfish_src", "r"), pclose);

  std::string line;
  std::clock_t initTimer = std::clock();
  std::array<double, 6> timers;
  while (std::getline(infile, line) && max_iterations > trainingPairs && !infile.eof())
  {
    //timers[0] = ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;
    lines += 1;

    if (lines % 100 == 0) {
      std::cout << "Line#: " << lines << ", skipped: " << skipped << std::endl;
    }

    //timers[1] =  ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;

    if (line.length() > 1) {
      bool legit = true;
      int score = 0;
      try {
        score = ::stockfishMock::evaluate(line);
      }
      catch (...)
      {
        legit = false;
        skipped += 1;
      }
      //timers[2] =  ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;

      // write this to a file
      if (output.is_open() && legit) {
        //timers[3] = ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;


        ::gameTree::nodePtr node = env.generateBoardFromFen(line);
        //timers[4] = ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;
        
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

        // inputs
        for (auto b : boards) {
          auto ba = std::bitset<64>(b);
          for (int i = 0; i < ba.size(); i++) {
            output << ba[i] << ' ';
          }
        }
        output << std::endl;

        output << score << std::endl;
        trainingPairs += 1;

        //timers[5] = ( std::clock() - initTimer ) / (double) CLOCKS_PER_SEC;

        //for (auto t : timers) {
          //std::cout << t * 10000 << std::endl;
          //}

        //break;

      }
    }
  }
  infile.close();
  output.close();

  // update file info
  std::ifstream fromBufferFile(folder + "BUFFER_" + out);
  std::ofstream outputUpdate(folder + out, std::ios::out | std::ios::trunc);
  if (output.is_open()) {
    outputUpdate << std::to_string(trainingPairs) << " " << std::to_string(num_input) << " " << std::to_string(num_output) << std::endl;
    outputUpdate << fromBufferFile.rdbuf();
    outputUpdate.close();
  }
  fromBufferFile.close();

  // info
  std::cout << "TRAINING FILE GENERATED!" << std::endl;
  std::cout << "Created # of training sets: " << trainingPairs << std::endl;
  std::cout << "Skipped # of fen strings  : "; std::cerr << skipped << std::endl;

}


int main (int argc, char * argv[])
{

  const float learning_rate = 0.9f;
  const unsigned int num_layers = 3;
  const unsigned int num_input = 768;
  const unsigned int num_hidden = 768;
  const unsigned int num_output = 1;
  const float desired_error = 0.001f;
  const unsigned int max_iterations = 10000;
  const unsigned int iterations_between_reports = 10;

  auto folder = ::utils::getAbsoluteProjectPath() + "/engine/ANNTraining/trainingdata/";
  auto infile = "fenstring.txt";
  auto outfile = "fenAndStockfishScore.data";
  //auto infile = "trainingdata/fenstring.txt";
  //auto outfile = "trainingdata/fenAndStockfishScore.data";
  //generateTrainingFile(folder, infile, outfile, max_iterations, num_input, num_output);

  try
  {
    std::ios::sync_with_stdio(); // Syncronize cout and printf output
    xor_test(learning_rate, num_layers, num_input, num_hidden, num_output, desired_error, max_iterations, iterations_between_reports);
  }
  catch (...)
  {
    cerr << endl << "Abnormal exception." << endl;
  }

  // Close program with exit code 0.
  return 0;
}