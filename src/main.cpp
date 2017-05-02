#include "uci/Listener.h"
#include "uci/UCIEvent.h"
#include "genericUCIResponses.h"
int main (int argc, char * argv[])
{
  ::uci::Listener uciListener;


  // add basic responses
  uciListener.addListener(uci::event::UCI, uciResponses::responseToUCI);

  // setup thread
  uciListener.setupListener();

  // Close program with exit code 0
  return 0;
}