#ifndef CHESS_ANN_GENERICUCIRESPONSES_H
#define CHESS_ANN_GENERICUCIRESPONSES_H


#include "uci/Definitions.h"
#include "uci/Response.h"
#include "engineInformation.h"
namespace uciResponses {

/**
 * Send id information to gui.
 *
 * @param args
 */
auto id = [&]() {
  ::uci::send("id name " + engineInformation::ENGINE_NAME);
  ::uci::send("id author " + engineInformation::ENGINE_AUTHOR);
};

auto uciok = [&]() {
  ::uci::send("uciok");
};

auto option = [&]() {
  ::uci::send("option name Hash type spin default 1 min 1 max 128");
};

auto responseToUCI = [&](const uci::arguments_t args) {
  id();
  option();
  uciok();
};

}


#endif // CHESS_ANN_GENERICUCIRESPONSES_H