#include "uci/Listener.h"
#include "uci/UCIEvent.h"
#include "genericUCIResponses.h"
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

  // setup thread
  uciListener.setupListener();

  // Close program with exit code 0
  return 0;
}