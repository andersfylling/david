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
  std::printf("+-%75s-+\n", "---------------------------------------------------------------------------");
  std::printf("| %75s |\n", FEN.c_str());
  std::printf("+-%75s-+\n", "---------------------------------------------------------------------------");

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
      std::cout << " > perft(" << std::to_string(fail) << "): got " << perft << ", wants " << results.at(i);
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


  usleep(1000);

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
  const bool useMoveGenTestSuite = true;
  if (useMoveGenTestSuite) {
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
    expected.push_back(3195901860); // 7
    //expected.push_back(84998978956); // 8. might take over 40 minutes.
#endif
    moveGenPerfter("", expected);

    // Position 2
    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    expected.clear();
    expected.push_back(1);
    expected.push_back(48);
    expected.push_back(2039);
    expected.push_back(97862);
    expected.push_back(4085603);
    expected.push_back(193690690);
    moveGenPerfter("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", expected);

    // Position 3
    // 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -
    expected.clear();
    expected.push_back(1);
    expected.push_back(14);
    expected.push_back(191);
    expected.push_back(2812);
    expected.push_back(43238);
    expected.push_back(674624);
    expected.push_back(11030083); // 6
#ifdef DAVID_PRODUCTION
    expected.push_back(178633661);
#endif
    moveGenPerfter("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", expected);

    // Position 4
    // r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
    // Or mirrored (with the same perft results):
    // r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1
    expected.clear();
    expected.push_back(1);
    expected.push_back(6);
    expected.push_back(264);
    expected.push_back(9467);
    expected.push_back(422333);
    expected.push_back(15833292);
    expected.push_back(706045033);
    moveGenPerfter("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", expected);
    moveGenPerfter("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", expected);

    // Position 5
    // rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
    expected.clear();
    expected.push_back(1);
    expected.push_back(44);
    expected.push_back(1486);
    expected.push_back(62379);
    expected.push_back(2103487);
    expected.push_back(89941194);
    moveGenPerfter("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", expected);

    // Position 6
    // r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10
    expected.clear();
    expected.push_back(1);
    expected.push_back(46);
    expected.push_back(2079);
    expected.push_back(89890);
    expected.push_back(3894594);
    expected.push_back(164075551); // 5
#ifdef DAVID_PRODUCTION
    expected.push_back(6923051137); // ish 3.5min
    //expected.push_back(287188994746);
    //expected.push_back(11923589843526);
    //expected.push_back(490154852788714);
#endif
    moveGenPerfter("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", expected);


    // done
    std::cout << "\n\nDONE!\n";
  }


  // Note that on unices only the lower 8 bits are usually used, clamping
  // the return value to 255 prevents false negative when some multiple
  // of 256 tests has failed
  return ( numFailed < 0xff ? numFailed : 0xff );
}