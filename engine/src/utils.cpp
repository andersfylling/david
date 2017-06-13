#include "david/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#ifdef __linux__
//linux code goes here
#include <unistd.h>
#include <bitset>
#include <david/GameTree.h>
#include <david/environment.h>
#include <david/ChessEngine.h>
#include <assert.h>
#elif _WIN32
// windows code goes here
#endif

namespace david {

/**
 * Converts a numeric integer string into an int, empty strings included.
 *
 * @param v The std::string value, eg. "123", or "" or " "
 * @return the int value: 123 or 0
 */
int utils::stoi(std::string v) {
  return v == "" || v == " " ? 0 : std::stoi(v);
}
int utils::stoi(const char c) {
  return c == ' ' ? 0 : c - '0';
}

bool utils::isHalfMove(type::gameState_ptr parent, type::gameState_ptr child) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::bitboard_t;

  environment::Environment env1(parent->playerColor);
  environment::Environment env2(child->playerColor);

  env1.setGameState(parent);
  env2.setGameState(child);

  bitboard_t parentMove = parent->playerColor == WHITE ? env1.whitePieces() : env1.blackPieces();
  bitboard_t childMove = parent->playerColor == WHITE ? env2.whitePieces() : env2.blackPieces();

  // ** cheeze

  /// This counter is reset after captures or pawn moves, and incremented otherwise.
  /// Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  /// including castling itself, increment the Halfmove Clock.
  /// However, those moves are irreversible in the sense to reverse the same rights -
  /// since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

  // check if parent or child is overlapping, AKA. a piece has been captured.
  if (parentMove & childMove != 0ULL) {
    return false;
  }

  // check if the black pawns has moved
  if (parent->BlackPawn | child->BlackPawn != 0ULL) {
    return false;
  }

  // check if white pawns has moved
  if (parent->WhitePawn | child->WhitePawn != 0ULL) {
    return false;
  }

  // no rules of halfmoving were broken
  return true;
}

/**
 * A FEN record contains six fields. The separator between fields is a space. The fields are:

    1. Piece placement (from white's perspective). Each rank is described, starting with rank 8 and ending with rank 1; within each rank, the contents of each square are described from file "a" through file "h". Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K").[1] White pieces are designated using upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8 (the number of empty squares), and "/" separates ranks.

    2. Active color. "w" means White moves next, "b" means Black.

    3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters: "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside), and/or "q" (Black can castle queenside).

    4. En passant target square in algebraic notation. If there's no en passant target square, this is "-". If a pawn has just made a two-square move, this is the position "behind" the pawn. This is recorded regardless of whether there is a pawn in position to make an en passant capture.[2]

    5. Halfmove clock: This is the number of halfmoves since the last capture or pawn advance. This is used to determine if a draw can be claimed under the fifty-move rule.

    6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.

 * @param node
 * @param whiteMovesNext
 * @return
 */
std::string utils::generateFen(type::gameState_ptr node) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using bitboard::bitboard_t;

  std::array<bitboard_t, 12> boards = {
      node->BlackBishop,
      node->BlackKing,
      node->BlackKnight,
      node->BlackPawn,
      node->BlackQueen,
      node->BlackRook,

      node->WhiteBishop,
      node->WhiteKing,
      node->WhiteKnight,
      node->WhitePawn,
      node->WhiteQueen,
      node->WhiteRook
  };

  std::array<char, 12> symbols = {
      'b',
      'k',
      'n',
      'p',
      'q',
      'r',

      'B',
      'K',
      'N',
      'P',
      'Q',
      'R'
  };

  char spaces[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8'};

  int spacing = 0;
  int pos = 1;
  std::string fen = "";

  for (uint8_t i = 0; i < 64; i++) {
    if (pos == 9) {
      pos = 1;
      fen += '/';
    }

    char p = ' ';
    for (uint8_t j = 0; j < 12; j++) {
      if (utils::bitAt(boards[j], i)) {
        p = symbols[j];
        break;
      }
    }

    if (p == ' ') {
      spacing += 1;
    }

    if (pos == 8 || p != ' ') {
      fen += spaces[spacing];
      spacing = 0;
    }

    if (p != ' ') {
      fen += p;
    }

    pos += 1;
  }

  fen += ' '; // spacing

  // who is the active player
  fen += node->playerColor == WHITE ? 'w' : 'b';
  fen += ' '; // spacing

  // missing castling verification support
  fen += "-";
  fen += ' '; // spacing

  // missing passant target verification support
  fen += "-";
  fen += ' '; // spacing

  // missing halfmove verification support
  fen += std::to_string(node->halfMoves);
  fen += ' '; // spacing

  // missing fullmove verification support
  fen += std::to_string(node->fullMoves);

  return fen;
}

/**
 * Check if a bit is set at given index.
 *
 * @param b The bits in form of a uint64_t
 * @param i the index as uint8_t
 * @return true if bit is 1 at index i
 */
bool utils::bitAt(uint64_t b, uint8_t i) {
  return (b & (1ULL << i)) != 0;
}

bool utils::fileExists(const std::string &file) {
  struct stat buffer;
  return (stat(file.c_str(), &buffer) == 0);
}

/**
 * Issue, what if there is more pieces of one type than the normal in a chess game?
 * Issue, should be floats and not doubles.
 *
 * @param node
 * @param player
 * @return
 */
std::vector<float> utils::convertGameStateToInputs(type::gameState_ptr node) {
  environment::Environment env(node->playerColor);
  env.setGameState(node);
  env.generateAttacks();

  // These are used to define whats benefitial and negative inputs
  auto attacks = env.getAttackState();

  auto nrOfBlackBishop = static_cast<double>(env.numberOfPieces(node->BlackBishop));
  auto nrOfBlackKing = static_cast<double>(env.numberOfPieces(node->BlackKing));
  auto nrOfBlackKnight = static_cast<double>(env.numberOfPieces(node->BlackKnight));
  auto nrOfBlackPawn = static_cast<double>(env.numberOfPieces(node->BlackPawn));
  auto nrOfBlackQueen = static_cast<double>(env.numberOfPieces(node->BlackQueen));
  auto nrOfBlackRook = static_cast<double>(env.numberOfPieces(node->BlackRook));

  auto nrOfWhiteBishop = static_cast<double>(env.numberOfPieces(node->WhiteBishop));
  auto nrOfWhiteKing = static_cast<double>(env.numberOfPieces(node->WhiteKing));
  auto nrOfWhiteKnight = static_cast<double>(env.numberOfPieces(node->WhiteKnight));
  auto nrOfWhitePawn = static_cast<double>(env.numberOfPieces(node->WhitePawn));
  auto nrOfWhiteQueen = static_cast<double>(env.numberOfPieces(node->WhiteQueen));
  auto nrOfWhiteRook = static_cast<double>(env.numberOfPieces(node->WhiteRook));

  auto combinedWhiteAttacks = env.combinedWhiteAttacks();
  auto combinedBlackAttacks = env.combinedBlackAttacks();
  auto whitePieces = env.whitePieces();
  auto blackPieces = env.blackPieces();

  std::array<double, 61> boardInfo = {
      node->playerColor == bitboard::COLOR::WHITE ? 1.0 : -1.0,
      
      nrOfBlackBishop < 0.1 ? 1.0 : -1.0,
      nrOfBlackKing   < 0.1 ? 1.0 : -1.0,
      nrOfBlackKnight < 0.1 ? 1.0 : -1.0,
      nrOfBlackPawn   < 0.1 ? 1.0 : -1.0,
      nrOfBlackQueen  < 0.1 ? 1.0 : -1.0,
      nrOfBlackRook   < 0.1 ? 1.0 : -1.0,
      
      nrOfBlackBishop < 0.1 ? 0.0 : nrOfBlackBishop / 100.0,
      nrOfBlackKing   < 0.1 ? 0.0 : nrOfBlackKing   / 100.0,
      nrOfBlackKnight < 0.1 ? 0.0 : nrOfBlackKnight / 100.0,
      nrOfBlackPawn   < 0.1 ? 0.0 : nrOfBlackPawn   / 100.0,
      nrOfBlackQueen  < 0.1 ? 0.0 : nrOfBlackQueen  / 100.0,
      nrOfBlackRook   < 0.1 ? 0.0 : nrOfBlackRook   / 100.0,
      
      static_cast<double>(env.numberOfPieces((*attacks.BlackBishop) & whitePieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.BlackKing)   & whitePieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.BlackKnight) & whitePieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.BlackPawn)   & whitePieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.BlackQueen)  & whitePieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.BlackRook)   & whitePieces)) / 100.0,
      combinedWhiteAttacks & (*attacks.BlackBishop) == 0 ? -1.0 : 1.0,
      combinedWhiteAttacks & (*attacks.BlackKing)   == 0 ? -1.0 : 1.0,
      combinedWhiteAttacks & (*attacks.BlackKnight) == 0 ? -1.0 : 1.0,
      combinedWhiteAttacks & (*attacks.BlackPawn)   == 0 ? -1.0 : 1.0,
      combinedWhiteAttacks & (*attacks.BlackQueen)  == 0 ? -1.0 : 1.0,
      combinedWhiteAttacks & (*attacks.BlackRook)   == 0 ? -1.0 : 1.0,


      // does the piece exist?
      nrOfWhiteBishop < 0.1 ? -1.0 : 1.0,
      nrOfWhiteKing   < 0.1 ? -1.0 : 1.0,
      nrOfWhiteKnight < 0.1 ? -1.0 : 1.0,
      nrOfWhitePawn   < 0.1 ? -1.0 : 1.0,
      nrOfWhiteQueen  < 0.1 ? -1.0 : 1.0,
      nrOfWhiteRook   < 0.1 ? -1.0 : 1.0,
      // How many of that piece type exists?
      nrOfWhiteBishop < 0.1 ? 0.0 : nrOfWhiteBishop / 100.0,
      nrOfWhiteKing   < 0.1 ? 0.0 : nrOfWhiteKing   / 100.0,
      nrOfWhiteKnight < 0.1 ? 0.0 : nrOfWhiteKnight / 100.0,
      nrOfWhitePawn   < 0.1 ? 0.0 : nrOfWhitePawn   / 100.0,
      nrOfWhiteQueen  < 0.1 ? 0.0 : nrOfWhiteQueen  / 100.0,
      nrOfWhiteRook   < 0.1 ? 0.0 : nrOfWhiteRook   / 100.0,
      // How many black pieces can each type attack?
      static_cast<double>(env.numberOfPieces((*attacks.WhiteBishop) & blackPieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.WhiteKing)   & blackPieces))   / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.WhiteKnight) & blackPieces)) / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.WhitePawn)   & blackPieces))   / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.WhiteQueen)  & blackPieces))  / 100.0,
      static_cast<double>(env.numberOfPieces((*attacks.WhiteRook)   & blackPieces))   / 100.0,
      // Are any of the piece types safe?
      combinedBlackAttacks & (*attacks.WhiteBishop) == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteKing)   == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteKnight) == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhitePawn)   == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteQueen)  == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteRook)   == 0 ? 1.0 : -1.0,


      static_cast<double>(env.numberOfPieces(whitePieces)) / 100.0, // is never 0
      static_cast<double>(env.numberOfPieces(blackPieces)) / 100.0, // is never 0
      static_cast<double>(env.numberOfPieces(whitePieces | blackPieces)) / 100.0,

      env.numberOfPieces(combinedBlackAttacks & whitePieces) == 0 ?  1.0 : -1.0 * (static_cast<double>(env.numberOfPieces(env.combinedBlackAttacks() & env.whitePieces())) / 100.0),
      env.numberOfPieces(combinedWhiteAttacks & blackPieces) == 0 ? -1.0 : static_cast<double>(env.numberOfPieces(env.combinedWhiteAttacks() & env.blackPieces())) / 100.0,

      node->blackQueenCastling ? -1.0 :  1.0,
      node->blackKingCastling  ? -1.0 :  1.0,
      node->whiteQueenCastling ?  1.0 : -1.0,
      node->whiteKingCastling  ?  1.0 : -1.0,

      static_cast<double>(100 - node->halfMoves) / 100.0,
      static_cast<double>(50 - node->fullMoves) / 100.0,

      // if the color playing is not yours, and the number here is high, it should not be a good thing.
      static_cast<double>(node->children.size()) / 100.0 // will always be 0 unless children are generated before comparing score.

  };

  // input
  std::vector<float> inputs;

  // generate inputs
  for (auto b : boardInfo) {
    inputs.push_back(static_cast<float>(b));
  }

  std::array<bitboard::bitboard_t, 2> boards1 = {
      node->BlackKing,
      node->WhiteKing
  };
  std::array<bitboard::bitboard_t, 8> boards2 = {
      node->BlackBishop,
      node->BlackKnight,
      node->BlackQueen,
      node->BlackRook,
      node->WhiteBishop,
      node->WhiteQueen,
      node->WhiteKnight,
      node->WhiteRook
  };
  std::array<bitboard::bitboard_t, 2> boards8 = {
      node->BlackPawn,
      node->WhitePawn
  };

  // generate inputs
  for (auto b : boards1) {
    //auto ba = std::bitset<64>(b);
    //double arr[1] = {-1.0};
    auto prog = 0;
    for (uint8_t i = 0; i < 64 && prog < 1; i++) {
      if (utils::bitAt(b, i)) {
        inputs.push_back(i == 0 ? 0.0f : i / 10.0f);
        prog += 1;
      }
    }
    // fill in missing pieces
    for (; prog < 1; prog++) {
      inputs.push_back(-1.0f);
    }
  }

  // Issue, what if the first on is gone? same on boards8
  for (auto b : boards2) {
    //auto ba = std::bitset<64>(b);
    //double arr[2] = {-1.0, -1.0};
    auto prog = 0;
    for (uint8_t i = 0; i < 64 && prog < 2; i++) {
      if (utils::bitAt(b, i)) {
        inputs.push_back(i == 0 ? 0.0f : i / 10.0f);
        prog += 1;
      }
    }

    // fill in missing pieces
    for (; prog < 2; prog++) {
      inputs.push_back(-1.0f);
    }
  }
  for (auto b : boards8) {
    //auto ba = std::bitset<64>(b);
    //double arr[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    auto prog = 0;
    for (uint8_t i = 0; i < 64 && prog < 8; i++) {
      if (utils::bitAt(b, i)) {
        inputs.push_back(i == 0 ? 0.0f : i / 10.0f);
        prog += 1;
      }
    }

    // fill in missing pieces
    for (; prog < 8; prog++) {
      inputs.push_back(-1.0f);
    }
  }

  // verify that the number of inputs are correct.
  //if (n != nrOfInputs) { std::cerr << "n was: " << n << std::endl; }
  //assert(n == nrOfInputs);

  return inputs;
}

/**
 * Used to convert an vector of float inputs to a fann_type input.
 *
 * @param inputs
 * @param size
 * @return
 */
fann_type* utils::convertInputsToFannType (std::vector<float> inputs, unsigned long size) {
  fann_type* fi = new fann_type[size];

  int prog = 0;
  for (auto i : inputs) {
    fi[prog++] = i;
  }

  return fi;
}

fann_type* utils::boardToFannInputs (type::gameState_ptr node) {
  auto inputs = convertGameStateToInputs(node);
  return convertInputsToFannType(inputs, inputs.size());
}


void utils::setDefaultChessLayout(type::gameState_ptr node) {
  node->BlackBishop = 2594073385365405696ULL;
  node->BlackKing = 576460752303423488ULL;
  node->BlackKnight = 4755801206503243776ULL;
  node->BlackPawn = 71776119061217280ULL;
  node->BlackQueen = 1152921504606846976ULL;
  node->BlackRook = 9295429630892703744ULL;

  node->WhiteBishop = 36;
  node->WhiteKnight = 66;
  node->WhitePawn = 65280;
  node->WhiteQueen = 8;
  node->WhiteKing = 16;
  node->WhiteRook = 129;

}

void utils::printGameState(type::gameState_ptr gs) {

  std::map<const char, bitboard::bitboard_t &> links = {
      {'b', gs->BlackBishop},
      {'k', gs->BlackKing},
      {'n', gs->BlackKnight},
      {'p', gs->BlackPawn},
      {'q', gs->BlackQueen},
      {'r', gs->BlackRook},

      {'B', gs->WhiteBishop},
      {'K', gs->WhiteKing},
      {'N', gs->WhiteKnight},
      {'P', gs->WhitePawn},
      {'Q', gs->WhiteQueen},
      {'R', gs->WhiteRook}
  };

  std::string board(64, '-');

  for (auto entry : links) {
    const char piece = entry.first;
    const auto bitboard = entry.second;

    for (uint8_t i = 0; i < 64; i++) {
      if (bitAt(bitboard, i)) {
        board.at(i) = piece;
      }
    }
  }

  std::cout << "\n  ";
  for (int i = 'A'; i < 'A' + 8; i++)
    std::cout << "  " << (char) i << " ";
  std::cout << std::endl;
  std::cout << "  +---+---+---+---+---+---+---+---+\n";
  for (int i = 0; i < 8; i++) {
    std::cout << i + 1 << " | ";
    for (int j = 0; j < 8; j++) {
      std::cout << board.at((i * 8) + j) << " | ";
    }
    std::cout << '\n';
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
  }
  std::cout << '\n';
}

std::string utils::getAbsoluteProjectPath() {
#ifdef __linux__
  //linux code goes here
  char buffer[PATH_MAX];
  std::string path = getcwd(buffer, PATH_MAX) ? std::string(buffer) : std::string("");

  if (path == "") {
    return "";
  }

  // find string until folder project name
  std::string projectPath = path.substr(0, path.find(engineInformation::PROJECT_NAME)); //hackz

  return projectPath + engineInformation::PROJECT_NAME;
#elif _WIN32
  // windows code goes here
  return "";
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return "";
#endif
}

}