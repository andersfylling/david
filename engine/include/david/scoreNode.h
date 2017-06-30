#pragma once

#include <array>

#include "./types.h"

namespace david {
struct scoreNode {
  int16_t score = constant::boardScore::LOWEST;
  type::gameState_ptr gs = nullptr;
  std::array<type::scoreNode_ptr, 40> children = {nullptr};
};

inline void scoreNodeDestructor(type::scoreNode_ptr sn) {
  for (auto i = static_cast<uint8_t>(sn->children.size() - 1); i >= 0; i -= 1) {
    if (sn->children[i] != nullptr) { // TODO: can break be used on nullptr hit?
      scoreNodeDestructor(sn->children[i]);
    }
  }

  if (sn->gs != nullptr) {
    delete sn->gs;
  }

  delete sn;
}
}