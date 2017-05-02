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
  ::uci::Response r;
  r.send("id name " + engineInformation::ENGINE_NAME);
  r.send("id author " + engineInformation::ENGINE_AUTHOR);
};

auto uciok = [&]() {
  ::uci::Response r;
  r.send("uciok");
};

auto option = [&]() {
  ::uci::Response r;
  r.send("option name Hash type spin default 1 min 1 max 128");
};

auto responseToUCI = [&](const uci::arguments_t args) {
  id();
  option();
  uciok();
};

}


#endif // CHESS_ANN_GENERICUCIRESPONSES_H