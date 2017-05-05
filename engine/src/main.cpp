#include <iostream>

#include "chess_ann/uci/Listener.h"
#include "chess_ann/uci/UCIEvent.h"
#include <chess_ann/Search.h>
#include "chess_ann/genericUCIResponses.h"
int main (int argc, char * argv[])
{
  using ::uci::event::UCI;
  using ::uci::event::ISREADY;
  using ::uci::event::QUIT;


  ::uci::Listener uciListener;

  // ###
  // initiate all other classes here.
  ::search::Search search(uciListener);

  // ###
  // Here the UCI protocol is dealt with.
  // add basic responses
  uciListener.addListener(UCI, uciResponses::responseToUCI); // uci
  uciListener.addListener(ISREADY, uciResponses::responseToISREADY); // isready

  // Got a quit uci command so stop listener [and exit program]
  uciListener.addListener(QUIT, [&](uci::arguments_t args){
    uciListener.stopListening();
  });

  // ###
  // Creates a forever listening UCI thread, this is to not block everything else.
  uciListener.setupListener();

  // Close program with exit code 0 after all threads have joined.
  return 0;
}

