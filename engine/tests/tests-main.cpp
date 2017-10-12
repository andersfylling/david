#include "david/MACROS.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "david/utils/utils.h"
#include "david/david.h"
#include <vector>

// move gen perft test
int moveGenPerfter (std::string FEN, std::vector<uint64_t> results)
{
  if (FEN.empty() || FEN == "") {
    FEN = ::david::constant::FENStartPosition;
  }

  setbuf(stdout, NULL);
  std::printf("\n");
  std::printf("+-%70s-+\n", "----------------------------------------------------------------------");
  std::printf("| %70s |\n", FEN.c_str());
  std::printf("+-%70s-+\n", "----------------------------------------------------------------------");

  // for every results entry
  std::printf("| %8s |", "perft(n)");
  for (unsigned int  i = 0; i < results.size(); i++) {
    std::printf(" %4d |", i);
  }
  std::printf("\n+----------+");
  for (unsigned int  i = 0; i < results.size(); i++) {
    std::printf("------+");
  }
  std::printf("\n");
  //std::printf("\n+-%70s-+\n", "----------------------------------------------------------------------");

  // run perft for every result
  std::printf("| %8s |", "result");
  int fail = -1;
  for (unsigned int  i = 0; i < results.size(); i++) {
    const uint64_t perft = ::utils::perft_silent(i, FEN);

    if (perft != results.at(i)) {
      fail = i;
    }

    std::printf(" %4s |", fail == -1 ? "ok" : "fail");

    if (fail != -1) {
      break;
    }
  }
  std::printf("\n+----------+");
  for (unsigned int  i = 0; i < results.size() && (fail == -1 || i <= fail); i++) {
    std::printf("------+");
  }

  if (fail != -1) {
    usleep(100);
    std::cerr << "\n\nDetected failure. Doing advanced perft check. Please verify results.\n" << std::flush;
    usleep(100);
    ::utils::perft_advanced(fail, FEN);
  }

  return fail;
}

int main( int argc, char* argv[] )
{
  Catch::Session session; // There must be exactly one instance
#ifdef DAVID_CPU_INTEL
  std::cout << "OMFG" << std::endl;
#endif

  // writing to session.configData() here sets defaults
  // this is the preferred way to set them

  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
    return returnCode;

  // writing to session.configData() or session.Config() here
  // overrides command line args
  // only do this if you know you need to

  int numFailed = session.run();


  // Validate MoveGen here.
  if (true) {
    std::vector<uint64_t> expected{};

    // normal perft at first
    expected.push_back(1);
    expected.push_back(20);
    expected.push_back(400);
    expected.push_back(8902);
    expected.push_back(197281);
    expected.push_back(4865609);
#ifdef DAVID_PRODUCTION
    expected.push_back(119060324); // 6
#endif
    moveGenPerfter("", expected);

    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    expected.clear();
    expected.push_back(1);
    expected.push_back(48);
    expected.push_back(2039);
    expected.push_back(97862);
    expected.push_back(4085603);
    expected.push_back(193690690);
    moveGenPerfter("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", expected);
  }


  // Note that on unices only the lower 8 bits are usually used, clamping
  // the return value to 255 prevents false negative when some multiple
  // of 256 tests has failed
  return ( numFailed < 0xff ? numFailed : 0xff );
}