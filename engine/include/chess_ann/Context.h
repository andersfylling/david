//
// Created by anders on 5/10/17.
//

#ifndef CHESS_ANN_CONTEXT_H
#define CHESS_ANN_CONTEXT_H

#include "memory"

namespace chess_ann {
class Engine;
}

namespace chess_ann {
class Context {
 private:

 public:
  Context();

  std::shared_ptr<chess_ann::Engine> engine;

};
}

#endif //CHESS_ANN_CONTEXT_H
