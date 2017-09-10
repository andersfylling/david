#pragma once

#include "david/david.h"
#include "david/types.h"

namespace utils {
namespace neuralNet {


std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(const ::david::type::gameState_t &node);


} // neuralNet
} // utils