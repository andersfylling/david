//
// Created by anders on 4/15/17.
//

#ifndef CHESS_ANN_UCILISTENER_H
#define CHESS_ANN_UCILISTENER_H

#include <thread>
namespace UCI {

void listen();
std::thread init();
}

#endif //CHESS_ANN_UCILISTENER_H
