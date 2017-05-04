#include "chess_ann/environment.h"
#include "chess_ann/bitboard.h"

#include <iostream>

#include "chess_ann/uci/Listener.h"
#include "chess_ann/uci/UCIEvent.h"
#include "chess_ann/genericUCIResponses.h"
int main (int argc, char * argv[])
{
  ::uci::Listener uciListener;

  // ###
  // initiate all other classes here.


  // ###
  // Here the UCI protocol is dealt with.
  // add basic responses
  uciListener.addListener(uci::event::UCI, uciResponses::responseToUCI); // uci
  uciListener.addListener(uci::event::ISREADY, uciResponses::responseToISREADY); // isready

  // Got a quit uci command so stop listener [and exit program]
  uciListener.addListener(uci::event::QUIT, [&](uci::arguments_t args){
    uciListener.stopListening();
  });

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  uciListener.setupListener();

  // Close program with exit code 0 after all threads have joined.
  return 0;
}