#pragma once

#include <limits>

// only holds information about the engine or const that can be used.
namespace david {
inline namespace constant {
// Folder to ANN networks files from project root dir.
static const std::string neuralNetworksFolder = "/engine/src/ANN/networks/";

// a standard chess board layout given as a FEN string.
static const std::string FENStartPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// ANN file
static const std::string ANNFile = "float_ANNFile_6_83_1_1497360313.net";

// information about the engine.
namespace engineInformation {
// MUST BE THE SAME AS FOLDER NAME, WILL BE REMOVED BEFORE FIRST RELEASE!!
static const std::string PROJECT_NAME     = "david";
static const std::string ENGINE_NAME      = "david";
static const std::string ENGINE_AUTHOR    = "Anders Øen Fylling, Martin Klingenberg, Markus Jacobsen, Martin Trehjøring";
static const std::string ENGINE_VERSION   = "0.2.3"; // THIS IS JUST SOME BS NUMBER.
static const std::string ENGINE_COPYRIGHT = ENGINE_NAME + ". Copyright (C) 2017-2017 " + ENGINE_AUTHOR + ". ";
} // ::david::constants::engineInformation

namespace defaultPiecePosition {
namespace black {
static const type::bitboard_t BISHOP  = 2594073385365405696ULL;
static const type::bitboard_t KING    = 576460752303423488ULL;
static const type::bitboard_t KNIGHT  = 4755801206503243776ULL;
static const type::bitboard_t PAWN    = 71776119061217280ULL;
static const type::bitboard_t QUEEN   = 1152921504606846976ULL;
static const type::bitboard_t ROOK    = 9295429630892703744ULL;
}
namespace white {
static const type::bitboard_t BISHOP  = 36ULL;
static const type::bitboard_t KNIGHT  = 66ULL;
static const type::bitboard_t PAWN    = 65280ULL;
static const type::bitboard_t QUEEN   = 16ULL;
static const type::bitboard_t KING    = 8ULL;
static const type::bitboard_t ROOK    = 129ULL;
}

static const type::bitboard_t EMPTY   = 0ULL;
} // ::david::constants::defaultPiecePosition

static const type::bitboard_t EMPTYBOARD = 0ULL;

namespace boardScore {
static const int HIGHEST  = +(std::numeric_limits<int>::max());
static const int LOWEST   = -(std::numeric_limits<int>::max());
}

static const int MAXMOVES = 256;

} // ::david::constants


struct NodeCache {
  type::gameState_ptr node; // should not be a leaf but a leaf parent.
  // in the future create a setup where the best node doesnt get overwritten
  // this will let u search trough the already generated, sorted and pruned children.
  // should speed up search time.

  // When this system is complete
  // Record performance and switch to PVS.

  // also check out MTD-f

  int alpha;
  int beta;
  int score; // search score, negamax result

  // to exclude the leafs search score, you must subtract or add it to the final negamax score.
};


} // ::david
