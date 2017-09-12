#include "david/utils/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "david/types.h"
#include "david/MoveGen.h"
#include <david/ChessEngine.h>
#include <david/utils/gameState.h>

#ifdef __linux__
//linux code goes here
#include <assert.h>
#include <sstream>
#elif _WIN32
// windows code goes here
#endif

namespace utils {

/**
 * Converts a numeric integer string into an int, empty strings included.
 *
 * @param v The std::string value, eg. "123", or "" or " "
 * @return the int value: 123 or 0
 */
inline namespace convert {
int stoi(const std::string v) {
  return v == "" || v == " " ? 0 : ::std::stoi(v);
}
//constexpr int ctoi(const char c) {
//  return c == ' ' ? 0 : c - '0';
//}

void legacyGameStateUpdate(::david::type::gameState_t& n) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;

}

}


bool fileExists(const std::string &file) {
  struct stat buffer;
  return (stat(file.c_str(), &buffer) == 0);
}

void yellDeprecated(const std::string info) {
  std::cerr << "@Deprecated function used " << (info.length() == 0 ? "!" : info) << std::endl;
}

/**
 * Prints a bitboard as a chessboard with 0's and 1's
 * @param board - at bitboard_t
 *
63	62	61	60	59	58	57	56
55	54	53	52	51	50	49	48
47	46	45	44	43	42	41	40
39	38	37	36	35	34	33	32
31	30	29	28	27	26	25	24
23	22	21	20	19	18	17	16
15	14	13	12	11	10	09	08
07	06	05	04	03	02	01	00
 *
 *
 */
void printBoard(uint64_t bb, const int index) {
  std::string board(64, '-');

  while (bb != 0ULL) {
    const auto i = LSB(bb);
    flipBitOff(bb, i);
    board.at(63 - i) = 'X';
  }

  // add the index as a O
  if (index > -1) {
    board.at(63 - index) = 'O';
  }

  std::cout << "\n ";
  std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  for (uint8_t i = 0; i < 8; i++) {
    std::cout << 8 - i << " | ";
    for (uint8_t j = 0; j < 8; j++) {
      std::cout << board.at(63 - ((i * 8) + j)) << " | ";
    }
    std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  }
  std::cout << "  ";
  for (int i = 'A'; i < 'A' + 8; i++) {
    std::cout << "  " << (char) i << " ";
  }
  std::cout << std::endl;
}


/**
 * Converts indexes such as "E6" into an integer index: H1 == 0.
 *
 * @param chessIndex String such as "E6"
 * @return integer bitboard_t index, 64 on error.
 */
uint8_t chessIndexToArrayIndex(const std::string &chessIndex) {
  uint8_t index = 64;

  // Is always 2 in size "alpha" + "numeric" => "H1"
  if (chessIndex.length() != 2) {
    return index;
  }

  // store first and second char
  const char &a = chessIndex.front();
  const char &b = chessIndex.back();

  char cPos[] = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'}; // matches index value
  char cPos2[] = {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}; // matches index value
  uint8_t first = 0;
  uint8_t second = b - 48 - 1;
  for (int i = 0; i < 8; i++) {
    if (a == cPos[i] || a == cPos2[i]) {
      first = i;
      break;
    }
  }

  index = second * 8 + first;

  if (index > 64) {
    return 64;
  }

  return index;
}


std::string getAbsoluteProjectPath() {
#ifdef __linux__
  //linux code goes here
  //char buffer[PATH_MAX];
  //std::string path = getcwd(buffer, PATH_MAX) ? std::string(buffer) : std::string("");

  if (const char* usr = std::getenv("USER")) {
    std::string username(usr);
    return "/home/" + username + "/.chess_david/"; //folder
  }
  else {
    std::__throw_logic_error("Unable to get username in linux!");
  }

  return "";
#elif _WIN32
  // windows code goes here
  return "";
#elif _WIN64
  // windows code goes here
  return "";
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return "";
#endif
}


void perft() {
  perft(6); // from depth 0 to depth 6, is default.
}

// oh this is bad, how hte first arg is now the max depth
void perft(const uint8_t depth) {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);
  perft(gs, 0, depth);
}

// and here its the start.
// by giving an gs here, we can change the board layouts before perfts.
void perft(const ::david::type::gameState_t& gs, const uint8_t start, const uint8_t end) {
  std::printf("+%7s+%32s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------");
  std::printf("| %5s | %30s | %8s |\n",
              "Depth",
              "Nodes",
              "Time");
  std::printf("+%7s+%32s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------");
  for (uint8_t depth = start; depth <= end; depth++) {
    // start time
    struct timeval tp, tp2;
    gettimeofday(&tp, NULL);
    long int ms1 = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    // get nodes for given depth
    auto nodes = depth == 0 ? 1 : ::utils::perft(depth, gs);

    // time finished
    gettimeofday(&tp2, NULL);
    long int ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000;

    // print results
    std::printf("| %5i | %30lu | %8.2f |\n",
                  depth, nodes,  (ms2 - ms1) / 1000.0
    );
  }

  // print ending
  std::printf("+%7s+%32s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------");
}

/**
 * Assumes that the depth is never 0!
 * @param depth
 * @param gs
 * @return
 */
uint64_t perft(const uint8_t depth, const ::david::type::gameState_t &gs) {
  // create a holder for possible game outputs
  ::david::MoveGen moveGen{gs};

  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
  const uint16_t len = moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);

  // if depth is 1 just return the length
  if (depth == 1) {
    return len;
  }

  // otherwise continue down the spiral
  uint64_t nodes = 0;
  const uint8_t nextDepth = depth - 1;
  for (unsigned long i = 0; i < len; i++) {
    nodes += perft(nextDepth, states[i]);
  }

  // return amount
  return nodes;
}



// for debugging perft!!!
bool perft_debug() {
  return perft_debug(-1);
}

// @deprecated
bool perft_debug(const int limit, const uint8_t startDepth) {
  ::david::type::gameState_t gs;
  ::utils::gameState::setDefaultChessLayout(gs);

  yellDeprecated("use perft_debug_advanced");

  // expected outputs
  std::array<uint64_t, 14> perftScores = {
      /*  0 */1,                  // 1
      /*  1 */20,                 // 20
      /*  2 */400,                // 400
      /*  3 */8902,               // 8,902
      /*  4 */197281,             // 197,281
      /*  5 */4865609,            // 4,865,609
      /*  6 */119060324,          // 119,060,324
      /*  7 */3195901860,         // 3,195,901,860
      /*  8 */84998978956,        // 84,998,978,956
      /*  9 */2439530234167,      // 2,439,530,234,167
      /* 10 */69352859712417,     // 69,352,859,712,417
      /* 11 */2097651003696806,   // 2,097,651,003,696,806
      /* 12 */62854969236701747,  // 62,854,969,236,701,747
      /* 13 */1981066775000396239 // 1,981,066,775,000,396,239
  };

  std::array<std::array<int, 7>, 6> expected = {{
      // captures
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */34,
          /* 4 */1576,
          /* 5 */82719,
          /* 6 */2812008
      },

      // en passant
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */0,
          /* 4 */0,
          /* 5 */258,
          /* 6 */5248
      },

      // castling
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */0,
          /* 4 */0,
          /* 5 */0,
          /* 6 */0
      },

      // promos
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */0,
          /* 4 */0,
          /* 5 */0,
          /* 6 */0
      },

      // checks
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */12,
          /* 4 */469,
          /* 5 */27351,
          /* 6 */809099
      },

      //checkmates
      {
          /* 0 */0,
          /* 1 */0,
          /* 2 */0,
          /* 3 */0,
          /* 4 */8,
          /* 5 */347,
          /* 6 */10828
      }
  }};

  auto len = limit < 0 ? static_cast<int>(perftScores.size()) : limit;
  bool success = true;

  if (limit != -1) {
    std::cerr << " * perft is depth limited to " << (int)limit << ", max depth is " << (perftScores.size() - 1) << std::endl;
  }

  // info
  std::cout << "This is not a normal perft! This only shows the difference between your results and expected results!" << std::endl;


  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::printf("+%7s+%10s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");
  std::printf("| %5s | %8s | %8s | %8s | %8s | %8s | %8s | %8s |\n",
              "Depth",
              "Nodes",
              "Captures",
              "E.P.",
              "Castling",
              "Promos",
              "Checks",
              "Checkm's");
  std::printf("+%7s+%10s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");
  for (uint8_t i = startDepth; i <= len; i++) {
    // run perft
    std::array<int, 6> perftResults = {}; // zero initialization
    auto moveGenPerft = ::utils::perft_debug(i, gs, perftResults);


    // expect node count
    auto expectedPerft = perftScores[i];

    // generated node error estimation
    const long long int diff = (long long int)moveGenPerft - (long long int)expectedPerft;

    // print results for perft[i]

    auto extras = [&]() -> bool {
      if (i > 6) {
        return false;
      }

      // captures, en passant, castling, promotions, check, checkmate
      for (uint8_t j = 0; j < 6; j++) {
        if (perftResults[j] - expected[j][i] != 0) {
          return true;
        }
      }

      return false;
    };

    // show that this has an error
    std::string msg = "";
    if (diff != 0 || extras()) {
      msg = " <-- ERROR";
    }
    if (i < 7) {
      std::printf("| %5i | %8lli | %8i | %8i | %8i | %8i | %8i | %8i | %8s\n",
                  i,
                  diff, //difference * 100.0, // difference
                  perftResults[0] - expected[0][i],
                  perftResults[1] - expected[1][i],
                  perftResults[2] - expected[2][i],
                  perftResults[3] - expected[3][i],
                  perftResults[4] - expected[4][i],
                  perftResults[5] - expected[5][i],
                  msg.c_str());
    } else {
      std::printf("| %5i | %8lli | %8c | %8c | %8c | %8c | %8c | %8c | %8s\n",
                  i,
                  diff, //difference * 100.0, // difference
                  '?',
                  '?',
                  '?',
                  '?',
                  '?',
                  '?',
                  msg.c_str());
    }

    // if the results are bad, don't continue
    if (msg != "") {
      success = false;
      //break;
    }
  }

  std::printf("+%7s+%10s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");

  return success;
}

uint64_t perft_debug(const uint8_t depth, const ::david::type::gameState_t &gs, std::array<int, 6>& results) {
  if (depth == 0) {
    return 1;
  }

  // create a holder for possible game outputs
  ::david::MoveGen moveGen{gs};

  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
  const uint16_t len = moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
  uint64_t nodes = 0;

  // calculate move for every move
  const auto nextDepth = depth - 1;
  for (unsigned long i = 0; i < len; i++) {

    //if (depth == 1 && states[i].piecesArr[gs.iBishops][1] != gs.piecesArr[gs.iBishops][0]) {
    //  printGameState(states[i]);
    //}
    auto& state = states[i];
    // NOTE: state is not const, but gs is!

    if (depth == 1) {
      // add any captures
      if ((gs.piecess[1] & state.piecess[1]) > 0 || state.passant) {
        results[0] += 1;

        // en passant
        if (state.passant) {
          results[1] += 1;
        }
      }

        // castling
      else if ((gs.kingCastlings[0] != state.kingCastlings[1]) || (gs.queenCastlings[0] != state.queenCastlings[1])) {
        results[2] += 1;
      }

        // promotion
      else if (::utils::nrOfActiveBits(gs.piecesArr[0][0]) - ::utils::nrOfActiveBits(state.piecesArr[0][1]) == 1) {
        results[3] += 1;
      }

      // check, but also check mate
      if (state.isInCheck) {
        results[4] += 1;

        // to see if the state is in check mate, lets just see how many moves it can generate
        ::david::MoveGen mg{state};
        if (mg.nrOfPossibleMoves() == 0) {
          results[5] += 1; // unable to generate any valid moves, aka check mate.
        }
      }

      nodes += 1;
    }
    else {
      nodes += perft_debug(nextDepth, state, results);
    }
  }

  return nodes;
}


// advanced movegen debugging
bool perft_debug_advanced(const ::david::type::gameState_t& gs, const uint8_t start, const uint8_t end, const bool showEGN) {

  // Display perft for given FEN
  std::cout << "FEN string: " << ::utils::gameState::generateFen(gs) << '\n';

  std::printf("+%7s+%32s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");
  std::printf("| %5s | %30s | %8s | %8s | %8s | %8s | %8s | %8s |\n",
              "Depth",
              "Nodes",
              "Captures",
              "E.P.",
              "Castles",
              "Promos",
              "Checks",
              "Checkm's");
  std::printf("+%7s+%32s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");
  for (uint8_t depth = start; depth <= end; depth++) {
    // run perft
    std::array<int, 6> perftResults = {}; // zero initialization

    auto moveGenPerft = ::utils::perft_debug_advanced(depth, gs, perftResults);

    std::printf("| %5i | %30lu | %8i | %8i | %8i | %8i | %8i | %8i |\n",
                depth,
                moveGenPerft, // nodes
                perftResults[0],
                perftResults[1],
                perftResults[2],
                perftResults[3],
                perftResults[4],
                perftResults[5]);
  }

  std::printf("+%7s+%32s+%10s+%10s+%10s+%10s+%10s+%10s+\n",
              "-------",
              "--------------------------------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------",
              "----------");
}

uint64_t perft_debug_advanced(const uint8_t depth, const ::david::type::gameState_t &gs, std::array<int, 6>& results) {
  if (depth == 0) {
    return 1;
  }

  // create a holder for possible game outputs
  ::david::MoveGen moveGen{gs};

  std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
  const uint16_t len = moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
  uint64_t nodes = 0;

  // calculate move for every move
  const uint8_t nextDepth = depth - 1;
  for (unsigned long i = 0; i < len; i++) {
    auto& state = states[i];

    if (depth == 1) {
      // add any captures
      if (((gs.piecess[1] & state.piecess[1]) > 0 || state.passant)
          && ::utils::nrOfActiveBits(gs.piecess[0] ^ state.piecess[1]) == 2
          && ::utils::nrOfActiveBits(gs.piecess[1] ^ state.piecess[0]) == 1
          ) {
        results[0] += 1;

        // en passant
        if (state.passant) {
          results[1] += 1;
        }
      }

      // castling
#ifdef DAVID_TEST
      if (state.castled) {
        results[2] += 1;
      }
#else
      else if ((gs.kingCastlings[0] != state.kingCastlings[1]) || (gs.queenCastlings[0] != state.queenCastlings[1])) {
        // castling state has changed, but did the king move?
        // check if the king has moved more than one position.
        if ((gs.piecesArr[5][0] & 576460752303423496) > 0 && (state.piecesArr[5][1] & 2449958197289549858) > 0) {
          // check that rook also moved
          if ((gs.piecesArr[gs.iRooks][0] & 9295429630892703873) > 0 && (state.piecesArr[gs.iRooks][1] & 1441151880758558740) > 0) {
            results[2] += 1;
          }
        }
      }
#endif

      // promotion
#ifdef DAVID_TEST
      if (state.promoted) {
        results[3] += 1;
      }
#else
      // check if a pawn has been reduced by one, but that nr of total pieces hasn't been reduced
      if (
          ::utils::nrOfActiveBits(gs.piecesArr[0][0]) - ::utils::nrOfActiveBits(state.piecesArr[0][1]) == 1 &&
              ::utils::nrOfActiveBits(gs.piecess[0]) == ::utils::nrOfActiveBits(state.piecess[1])
          ) {
        results[3] += 1;
      }
#endif
      // check, but also check mate
      if (state.isInCheck) {
        results[4] += 1;

        // to see if the state is in check mate, lets just see how many moves it can generate
        ::david::MoveGen mg{state};
        if (mg.nrOfPossibleMoves() == 0) {
          results[5] += 1; // unable to generate any valid moves, aka check mate.
        }
      }

      nodes += 1;
    }
    else {
      nodes += ::utils::perft_debug_advanced(nextDepth, state, results);
    }
  }

  return nodes;
}


} // End of utils

