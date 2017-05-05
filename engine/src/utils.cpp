//
// Created by anders on 5/5/17.
//

#include "chess_ann/utils.h"

int utils::stoi(std::string v) {
  return v == "" || v == " " ? 0 : std::stoi(v);
}
