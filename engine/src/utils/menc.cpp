#include <unistd.h>
#include <string>
#include <map>
#include "david/types.h"
#include "david/utils/gameState.h"

namespace utils {
namespace move {
// Move Encoder

// create encoded move based on two uint64_t
constexpr uint16_t encode(const uint64_t before, const uint64_t after)
{


  return (::utils::LSB(after) << 6) | ::utils::LSB(before);
}

constexpr uint8_t decodeFrom (const uint16_t move)
{
  return move & 0b00111111;
}

constexpr uint8_t decodeTo (const uint16_t move)
{
  return (move >> 6) & 0b00111111;
}

constexpr uint16_t promotion (const uint16_t move)
{
  return move | 0b1000000000000000;
}

constexpr uint16_t capture (const uint16_t move)
{
  return move | 0b0100000000000000;
}

constexpr uint16_t special1 (const uint16_t move)
{
  return move | 0b0010000000000000;
}

constexpr uint16_t special0 (const uint16_t move)
{
  return move | 0b0001000000000000;
}

} // ::utils::menc
} // End of utils

