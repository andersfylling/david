#pragma once

#include "david/david.h"
#include "david/types.h"

namespace utils {

//! Utilities specific to the neural net.
namespace neuralNet {


std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(::david::type::gameState_t &node);


} // neuralNet
} // utils