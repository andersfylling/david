#include "david/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "david/bitboard.h"

#ifdef __linux__
//linux code goes here
#include <unistd.h>
#include <bitset>
#include <david/GameTree.h>
#include <david/environment.h>
#include <david/ChessEngine.h>
#include <assert.h>
#include <david/MoveGeneration.h>
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

bool utils::isHalfMove(type::gameState_t& parent, type::gameState_t& child) {
  using bitboard::COLOR::WHITE;
  using bitboard::COLOR::BLACK;
  using type::bitboard_t;

  movegen::MoveGenerator genP;
  genP.setGameState(parent);

  movegen::MoveGenerator genC;
  genC.setGameState(child);

  // ** cheeze

  /// This counter is reset after captures or pawn moves, and incremented otherwise.
  /// Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  /// including castling itself, increment the Halfmove Clock.
  /// However, those moves are irreversible in the sense to reverse the same rights -
  /// since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

  // check if parent or child is overlapping, AKA. a piece has been captured.
  if (genP.white() & genC.black() != 0ULL) {
    return false;
  }

  // check if the black pawns has moved
  if (parent.BlackPawn | child.BlackPawn != 0ULL) {
    return false;
  }

  // check if white pawns has moved
  if (parent.WhitePawn | child.WhitePawn != 0ULL) {
    return false;
  }

  // no rules of half moving were broken
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
  using type::bitboard_t;

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

  yellDeprecated("utils::generateFen(type::gameState_ptr node)");

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
std::vector<float> utils::convertGameStateToInputs(const type::gameState_t& node) {
//  movegen::MoveGenerator gen;
//  gen.setGameState(node);
//  gen.generateAttacks(node.playerColor);
//
//  // These are used to define whats benefitial and negative inputs
//  auto attacks = gen.getAttackState();

  environment::Environment env(node.playerColor);
  env.setGameState(node);
  env.generateAttacks();

  // These are used to define whats benefitial and negative inputs
  auto attacks = env.getAttackState();

  auto nrOfBlackBishop = static_cast<double>(utils::nrOfActiveBits(node.BlackBishop));
  auto nrOfBlackKing = static_cast<double>(utils::nrOfActiveBits(node.BlackKing));
  auto nrOfBlackKnight = static_cast<double>(utils::nrOfActiveBits(node.BlackKnight));
  auto nrOfBlackPawn = static_cast<double>(utils::nrOfActiveBits(node.BlackPawn));
  auto nrOfBlackQueen = static_cast<double>(utils::nrOfActiveBits(node.BlackQueen));
  auto nrOfBlackRook = static_cast<double>(utils::nrOfActiveBits(node.BlackRook));

  auto nrOfWhiteBishop = static_cast<double>(utils::nrOfActiveBits(node.WhiteBishop));
  auto nrOfWhiteKing = static_cast<double>(utils::nrOfActiveBits(node.WhiteKing));
  auto nrOfWhiteKnight = static_cast<double>(utils::nrOfActiveBits(node.WhiteKnight));
  auto nrOfWhitePawn = static_cast<double>(utils::nrOfActiveBits(node.WhitePawn));
  auto nrOfWhiteQueen = static_cast<double>(utils::nrOfActiveBits(node.WhiteQueen));
  auto nrOfWhiteRook = static_cast<double>(utils::nrOfActiveBits(node.WhiteRook));

  auto combinedWhiteAttacks = env.combinedWhiteAttacks();
  auto combinedBlackAttacks = env.combinedBlackAttacks();
  auto whitePieces = env.whitePieces();
  auto blackPieces = env.blackPieces();

  std::array<double, 61> boardInfo = {
      node.playerColor == bitboard::COLOR::WHITE ? 1.0 : -1.0,
      
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
      
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackBishop) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackKing)   & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackKnight) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackPawn)   & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackQueen)  & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackRook)   & whitePieces)) / 100.0,
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
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteBishop) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteKing)   & blackPieces))   / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteKnight) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhitePawn)   & blackPieces))   / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteQueen)  & blackPieces))  / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteRook)   & blackPieces))   / 100.0,
      // Are any of the piece types safe?
      combinedBlackAttacks & (*attacks.WhiteBishop) == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteKing)   == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteKnight) == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhitePawn)   == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteQueen)  == 0 ? 1.0 : -1.0,
      combinedBlackAttacks & (*attacks.WhiteRook)   == 0 ? 1.0 : -1.0,


      static_cast<double>(utils::nrOfActiveBits(whitePieces)) / 100.0, // is never 0
      static_cast<double>(utils::nrOfActiveBits(blackPieces)) / 100.0, // is never 0
      static_cast<double>(utils::nrOfActiveBits(whitePieces | blackPieces)) / 100.0,

      utils::nrOfActiveBits(combinedBlackAttacks & whitePieces) == 0 ?  1.0 : -1.0 * (static_cast<double>(utils::nrOfActiveBits(env.combinedBlackAttacks() & env.whitePieces())) / 100.0),
      utils::nrOfActiveBits(combinedWhiteAttacks & blackPieces) == 0 ? -1.0 : static_cast<double>(utils::nrOfActiveBits(env.combinedWhiteAttacks() & env.blackPieces())) / 100.0,

      node.blackQueenCastling ? -1.0 :  1.0,
      node.blackKingCastling  ? -1.0 :  1.0,
      node.whiteQueenCastling ?  1.0 : -1.0,
      node.whiteKingCastling  ?  1.0 : -1.0,

      static_cast<double>(100 - node.halfMoves) / 100.0,
      static_cast<double>(50 - node.fullMoves) / 100.0,

      // if the color playing is not yours, and the number here is high, it should not be a good thing.
      static_cast<double>(10/*node.children.size()*/) / 100.0 // will always be 0 unless children are generated before comparing score.

  };

  // input
  std::vector<float> inputs;

  // generate inputs
  for (auto b : boardInfo) {
    inputs.push_back(static_cast<float>(b));
  }

  std::array<type::bitboard_t, 2> boards1 = {
      node.BlackKing,
      node.WhiteKing
  };
  std::array<type::bitboard_t, 8> boards2 = {
      node.BlackBishop,
      node.BlackKnight,
      node.BlackQueen,
      node.BlackRook,
      node.WhiteBishop,
      node.WhiteQueen,
      node.WhiteKnight,
      node.WhiteRook
  };
  std::array<type::bitboard_t, 2> boards8 = {
      node.BlackPawn,
      node.WhitePawn
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
fann_type* utils::convertInputsToFannType (const std::vector<float>& inputs, unsigned long size) {
  fann_type* fi = new fann_type[size];

  int prog = 0;
  for (auto i : inputs) {
    fi[prog++] = i;
  }

  return fi;
}

fann_type* utils::boardToFannInputs (const type::gameState_t& node) {
  const auto inputs = convertGameStateToInputs(node);
  return convertInputsToFannType(inputs, inputs.size());
}

void utils::yellDeprecated(const std::string info) {
  std::cerr << "@Deprecated function used " << (info.length() == 0 ? "!" : info) << std::endl;
}

/**
 * @deprecated
 * @param node
 */
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

  yellDeprecated("utils::setDefaultChessLayout(type::gameState_ptr node)");

}

void utils::setDefaultChessLayout(type::gameState_t& node) {
  node.BlackRook    = constant::defaultPiecePosition::black::ROOK;
  node.BlackQueen   = constant::defaultPiecePosition::black::QUEEN;
  node.BlackPawn    = constant::defaultPiecePosition::black::PAWN;
  node.BlackKnight  = constant::defaultPiecePosition::black::KNIGHT;
  node.BlackKing    = constant::defaultPiecePosition::black::KING;
  node.BlackBishop  = constant::defaultPiecePosition::black::BISHOP;
  
  node.WhiteRook    = constant::defaultPiecePosition::white::ROOK;
  node.WhiteQueen   = constant::defaultPiecePosition::white::QUEEN;
  node.WhitePawn    = constant::defaultPiecePosition::white::PAWN;
  node.WhiteKnight  = constant::defaultPiecePosition::white::KNIGHT;
  node.WhiteKing    = constant::defaultPiecePosition::white::KING;
  node.WhiteBishop  = constant::defaultPiecePosition::white::BISHOP;

  node.blackKingCastling  = true;
  node.blackQueenCastling = true;
  node.whiteKingCastling  = true;
  node.whiteQueenCastling = true;
}

/**
 * Print out the chess board based on a gameState node
 * @param gs
 * @deprecated
 */
void utils::printGameState(type::gameState_ptr gs) {

  std::map<const char, type::bitboard_t &> links = {
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
  for (int i = 7; i >= 0; i--) {
    std::cout << i + 1 << " | ";
    for (int j = 0; j < 8; j++) {
      std::cout << board.at((i * 8) + j) << " | ";
    }
    std::cout << '\n';
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
  }
  std::cout << '\n';

  yellDeprecated("utils::printGameState(type::gameState_ptr gs)");
}


/**
 * Print out the chess board based on a gameState node
 * @param gs
 */
void utils::printGameState(type::gameState_t& gs) {

  std::map<const char, type::bitboard_t &> links = {
      {'b', gs.BlackBishop},
      {'k', gs.BlackKing},
      {'n', gs.BlackKnight},
      {'p', gs.BlackPawn},
      {'q', gs.BlackQueen},
      {'r', gs.BlackRook},

      {'B', gs.WhiteBishop},
      {'K', gs.WhiteKing},
      {'N', gs.WhiteKnight},
      {'P', gs.WhitePawn},
      {'Q', gs.WhiteQueen},
      {'R', gs.WhiteRook}
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
  for (int i = 7; i >= 0; i--) {
    std::cout << i + 1 << " | ";
    for (int j = 0; j < 8; j++) {
      std::cout << board.at((i * 8) + j) << " | ";
    }
    std::cout << '\n';
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
  }
  std::cout << '\n';
}

/**
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param fen std::string, must be correctly formatted (!)
 * @return new shared_ptr of gameState
 * @deprecated
 */
type::gameState_ptr utils::generateBoardFromFen(const std::string fen) {
  type::gameState_ptr node = new type::gameState_t();

  std::map<const char, type::bitboard_t &> links = {
      {'b', node->BlackBishop},
      {'k', node->BlackKing},
      {'n', node->BlackKnight},
      {'p', node->BlackPawn},
      {'q', node->BlackQueen},
      {'r', node->BlackRook},

      {'B', node->WhiteBishop},
      {'K', node->WhiteKing},
      {'N', node->WhiteKnight},
      {'P', node->WhitePawn},
      {'Q', node->WhiteQueen},
      {'R', node->WhiteRook}
  };

  type::bitboard_t index = 0;
  bool gotColor = false;
  for (const auto c : fen) {
    if (index == 64) {

      if (index == 65) {
        node->playerColor = c == 'w' ? bitboard::COLOR::WHITE : bitboard::COLOR::BLACK;
        gotColor = true;
      }

      if (gotColor) {
        break;
      }
      else {
        index++;
        continue;
      }
    }
    if (c == '/') {
      continue;
    }

    // check if the char is a piece type
    if (links.count(c) > 0) {
      // it's a piece type
      flipBit(links.at(c), index); // set bit at correct index on correct board
      index += 1;
    } else {
      // assumption: it's a number
      // update index with this number
      index += utils::stoi(c);
    }
  }

  yellDeprecated("utils::generateBoardFromFen(const std::string fen)");

  return node;
}


void utils::generateBoardFromFen(type::gameState_t& gs, const std::string& fen) {
  std::map<const char, type::bitboard_t&> links = {
      {'b', gs.BlackBishop},
      {'k', gs.BlackKing},
      {'n', gs.BlackKnight},
      {'p', gs.BlackPawn},
      {'q', gs.BlackQueen},
      {'r', gs.BlackRook},

      {'B', gs.WhiteBishop},
      {'K', gs.WhiteKing},
      {'N', gs.WhiteKnight},
      {'P', gs.WhitePawn},
      {'Q', gs.WhiteQueen},
      {'R', gs.WhiteRook}
  };

  uint8_t index = 0; //64 = space, 66 = space,
  auto len = static_cast<uint8_t>(fen.length());
  for (uint8_t i = 0; i < len; i++) {
    const auto& c = fen[i];

    // piece positions
    if (index < 64) {
      if (c == '/') {
        continue;
      }

      // check if the char is a piece type
      if (links.count(c) > 0) {
        // it's a piece type
        flipBit(links.at(c), index); // set bit at correct index on correct board
        index += 1;
      } else {
        // assumption: it's a number
        // update index with this number
        index += utils::stoi(c); // should be uint8, but isn't needed..
      }
    }

    // color
    else if (index == 64) {
      gs.playerColor = fen[i + 1] == 'w' ? bitboard::COLOR::WHITE : bitboard::COLOR::BLACK;

      i += 2; // skip char and space afterwards
      index += 1;
    }

    // castling
    else if (index == 65) {
      for (; i < len && fen[i] != ' '; i += 1) {

      }

      i += 1; // skip space
      index += 1;
    }

      // en passant
    else if (index == 66) {
      std::string pos = "";
      for (; i < len && fen[i] != ' '; i += 1) {
        pos += fen[i];
      }

      if (pos.length() == 2) {
        // this ignores '-' and only takes board positions

        auto passantBoard = ::david::utils::chessIndexToBitboard(pos);
      }

      i += 1; // skip space
      index += 1;
    }

      // half move
    else if (index == 67) {
      std::string hm = "";
      for (; i < len && fen[i] != ' '; i += 1) {
        hm += fen[i];
      }

      gs.halfMoves = ::david::utils::stoi(hm);

      i += 1; // skip space
      index += 1;
    }

      // full move
    else if (index == 68) {
      std::string fm = "";
      for (; i < len && fen[i] != ' '; i += 1) {
        fm += fen[i];
      }

      gs.fullMoves = ::david::utils::stoi(fm);
      break; // finished reading FEN string
    }
  }

}

/**
 * Converts indexes such as "E6" into an integer index: H1 == 0.
 *
 * @param chessIndex String such as "E6"
 * @return integer bitboard_t index, 64 on error.
 */
uint8_t utils::chessIndexToArrayIndex(const std::string& chessIndex) {
  uint8_t index = 64;

  // Is always 2 in size "alpha" + "numeric" => "H1"
  if (chessIndex.length() != 2) {
    return index;
  }

  // store first and second char
  const char &a = chessIndex.front();
  const char &b = chessIndex.back();

  char cPos[]   = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'}; // matches index value
  char cPos2[]  = {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'}; // matches index value
  uint8_t first = 0;
  uint8_t second = b - 48 - 1;
  for (int i = 0; i < 8; i++) {
    if (a == cPos[i] || a == cPos2[i]) {
      first = i;
      break;
    }
  }

  index = second * 8 + first;

  if (index > 64 || index < 0) {
    return 64;
  }

  return index;
}

/**
 * Takes a chess position such as "E6" and creates a bitboard with a on bit at that given index.
 *
 * @param chessIndex String such as "E6"
 * @return bitboard with a active bit in given position
 */
type::bitboard_t utils::chessIndexToBitboard(const std::string& chessIndex) {
  type::bitboard_t board = 0ULL;
  flipBit(board, static_cast<type::bitboard_t>(chessIndexToArrayIndex(chessIndex)));

  return board;
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
type::bitboard_t utils::LSB(type::bitboard_t board) {
#ifdef __linux__
  return (board != 0LL) ? __builtin_ffsll(board) - 1 : 0LL;
#elif _WIN32
  // windows code goes here
  return 0LL;
#elif _WIN64
  // windows code goes here
  return 0LL;
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return 0LL;
#endif
}

// Needs compiler support for Microsoft c++ compiler
// Works with gcc based compilers
type::bitboard_t utils::NSB(type::bitboard_t &board) {
  board &= ~(1LL << LSB(board));
  return LSB(board);
}

type::bitboard_t utils::MSB(type::bitboard_t board) {
#ifdef __linux__
  return 63 - __builtin_clzll(board);
#elif _WIN32
  // windows code goes here
  return 0LL;
#elif _WIN64
  // windows code goes here
  return 0LL;
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return 0LL;
#endif
}

type::bitboard_t utils::NSB_r(type::bitboard_t &board) {
  board &= ~(1LL << MSB(board));
  return MSB(board);
}

// Turns on bit
void utils::flipBit(type::bitboard_t &board, type::bitboard_t index) {
  board |= (1LL << index);
}

// YEAH tell that bit to flipp off!!!
// Nobody wants you bit... NOBODY WANTS YOU
void utils::flipBitOff(type::bitboard_t &board, type::bitboard_t index) {
  board &= ~(1ULL << index);
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
#elif _WIN64
  // windows code goes here
  return "";
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return "";
#endif
}

/**
 * Prints a bitboard as a chessboard with 0's and 1's
 * @param board - at bitboard_t
 */
void utils::printBoard(type::bitboard_t board) {
    std::string bits = std::bitset<64>(board).to_string();
    std::cout << "\n  ";
    for (int i = 'A'; i < 'A' + 8; i++)
      std::cout << "  " << (char) i << " ";
    std::cout << std::endl;
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    for (int i = 0; i < 8; i++) {
      std::cout << i + 1 << " | ";
      for (int j = 0; j < 8; j++) {
        std::cout << bits.at((i * 8) + j) << " | ";
      }
      std::cout << '\n';
      std::cout << "  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << '\n';
}

// Turns on bit
void utils::flipBitOn(type::bitboard_t &board, type::bitboard_t index) {
  board |= (1LL << index);
}

int utils::nrOfActiveBits(type::bitboard_t b) {
#ifdef __linux__
  return __builtin_popcountll(b);
#elif _WIN32
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#elif _WIN64
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  int counter = 0;
  for (int i = 0; i < 64; i++) {
    if (bitAt(b, i)) counter += 1;
  }
  return counter;
#endif
}

} // End of david

