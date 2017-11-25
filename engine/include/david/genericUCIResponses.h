#pragma once

#include "david/david.h"

#include "uci/definitions.h"
#include "uci/Response.h"

namespace david {
namespace uciResponses {
// http://stackoverflow.com/questions/17003561/using-the-universal-chess-interface


/**
 * Send id information to gui.
 */
auto id = [&]() {
  uci::send("id name " + constant::engineInformation::ENGINE_NAME);
  uci::send("id author " + constant::engineInformation::ENGINE_AUTHOR);
};

/**
 * Send uciok when all the options params have been sent.
 */
auto uciok = [&]() {
  uci::send("uciok");
};

/**
 * Send different engine options to the GUI.
 * Not required.
 */
auto option = [&]() {
  //::forwards::send("option name Hash type spin default 1 min 1 max 128");
};

/**
 * Typical initial response to the UCI.
 */
auto responseToUCI = [&](const uci::arguments_t args) {
  id();
  option();
  uciok();
};

// TODO: This needs to wait until everything has been setup correctly.
auto responseToISREADY = [&](const uci::arguments_t args) {
  uci::send("readyok");
};

}
}