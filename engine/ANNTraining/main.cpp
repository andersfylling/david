#include "fann/fann.h"
#include "fann/fann_cpp.h"
#include "david/utils/utils.h"

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
#include <david/TreeGen.h>
#include "david/ANN/binaryInputs.h"
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::showpos;
using std::noshowpos;



int main(int argc, char *argv[]) {

  ::binaryNetwork::run();

  // Close program with exit code 0.
  return 0;
}