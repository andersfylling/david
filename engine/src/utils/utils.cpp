#include "david/utils/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "david/bitboard.h"

#ifdef __linux__
//linux code goes here
#include <unistd.h>
#include <bitset>
#include <david/TreeGen.h>
#include <david/environment.h>
#include <david/ChessEngine.h>
#include <assert.h>
#include <david/MoveGeneration.h>
#include <chrono>
#include <thread>
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
}

bool isHalfMove(const ::david::type::gameState_t &parent, const ::david::type::gameState_t &child) {
  //using ::david::bitboard::COLOR::WHITE;
  //using ::david::bitboard::COLOR::BLACK;
  //using ::david::type::bitboard_t;
  //using ::david::movegen::MoveGenerator;

  //MoveGenerator genP;
  //genP.setGameState(parent);

  //MoveGenerator genC;
  //genC.setGameState(child);

  // ** cheeze

  /// This counter is reset after captures or pawn moves, and incremented otherwise.
  /// Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  /// including castling itself, increment the Halfmove Clock.
  /// However, those moves are irreversible in the sense to reverse the same rights -
  /// since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

  // check if parent or child is overlapping, AKA. a piece has been captured.
  if ((parent.piecess[0] & child.piecess[0]) != 0ULL) {
    return false;
  }

  // check if the ---- pawns has moved
  if ((parent.pawns[0] | child.pawns[1]) != 0ULL) {
    return false;
  }

  // check if !---- pawns has moved
  //if ((parent.WhitePawn | child.WhitePawn) != 0ULL) {
  //  return false;
  //}
  //
  // no rules of half moving were broken
  //return true;
  return (parent.pawns[1] | child.pawns[0]) != 0ULL;
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
 * @Deprecated needs to use gameState_t not _ptr
 */
std::string generateFen(::david::type::gameState_ptr node) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;
  using ::david::type::bitboard_t;

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

bool fileExists(const std::string &file) {
  struct stat buffer;
  return (stat(file.c_str(), &buffer) == 0);
}


template<std::size_t SIZE>
void addPieceBoardIndexToVector(std::vector<float> &store,
                                       std::array<::david::type::bitboard_t, SIZE> &pieces,
                                       uint8_t nr) {
  // ISSUE, this doesnt scale when more pieces than nr exist. what if there are 3 queens? or just 2?
}


inline namespace neuralnet {
/**
 * Issue, what if there is more pieces of one type than the normal in a chess game?
 * Issue, should be floats and not doubles.
 *
 * @param node
 * @param player
 * @return
 */
std::vector<float> convertGameStateToVectorInputs(const ::david::type::gameState_t &node) {
//  movegen::MoveGenerator gen;
//  gen.setGameState(node);
//  gen.generateAttacks(node.playerColor);
//
//  // These are used to define whats benefitial and negative inputs
//  auto attacks = gen.getAttackState();

  ::david::environment::Environment env{node.playerColor};
  env.setGameState(node);
  env.generateAttacks();

  // These are used to define whats benefitial and negative inputs
  auto attacks = env.getAttackState();

  auto nrOfBlackBishop = static_cast<float>(utils::nrOfActiveBits(node.BlackBishop));
  auto nrOfBlackKing = static_cast<float>(utils::nrOfActiveBits(node.BlackKing));
  auto nrOfBlackKnight = static_cast<float>(utils::nrOfActiveBits(node.BlackKnight));
  auto nrOfBlackPawn = static_cast<float>(utils::nrOfActiveBits(node.BlackPawn));
  auto nrOfBlackQueen = static_cast<float>(utils::nrOfActiveBits(node.BlackQueen));
  auto nrOfBlackRook = static_cast<float>(utils::nrOfActiveBits(node.BlackRook));

  auto nrOfWhiteBishop = static_cast<float>(utils::nrOfActiveBits(node.WhiteBishop));
  auto nrOfWhiteKing = static_cast<float>(utils::nrOfActiveBits(node.WhiteKing));
  auto nrOfWhiteKnight = static_cast<float>(utils::nrOfActiveBits(node.WhiteKnight));
  auto nrOfWhitePawn = static_cast<float>(utils::nrOfActiveBits(node.WhitePawn));
  auto nrOfWhiteQueen = static_cast<float>(utils::nrOfActiveBits(node.WhiteQueen));
  auto nrOfWhiteRook = static_cast<float>(utils::nrOfActiveBits(node.WhiteRook));

  auto combinedWhiteAttacks = env.combinedWhiteAttacks();
  auto combinedBlackAttacks = env.combinedBlackAttacks();
  auto whitePieces = node.whitePieces;
  auto blackPieces = node.blackPieces;

  std::array<double, 61> boardInfo = {
      node.playerColor == ::david::bitboard::COLOR::WHITE ? 1.0 : -1.0,

      nrOfBlackBishop < 0.1 ? 1.0 : -1.0,
      nrOfBlackKing < 0.1 ? 1.0 : -1.0,
      nrOfBlackKnight < 0.1 ? 1.0 : -1.0,
      nrOfBlackPawn < 0.1 ? 1.0 : -1.0,
      nrOfBlackQueen < 0.1 ? 1.0 : -1.0,
      nrOfBlackRook < 0.1 ? 1.0 : -1.0,

      nrOfBlackBishop < 0.1 ? 0.0 : nrOfBlackBishop / 100.0,
      nrOfBlackKing < 0.1 ? 0.0 : nrOfBlackKing / 100.0,
      nrOfBlackKnight < 0.1 ? 0.0 : nrOfBlackKnight / 100.0,
      nrOfBlackPawn < 0.1 ? 0.0 : nrOfBlackPawn / 100.0,
      nrOfBlackQueen < 0.1 ? 0.0 : nrOfBlackQueen / 100.0,
      nrOfBlackRook < 0.1 ? 0.0 : nrOfBlackRook / 100.0,

      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackBishop) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackKing) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackKnight) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackPawn) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackQueen) & whitePieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.BlackRook) & whitePieces)) / 100.0,
      (combinedWhiteAttacks & (*attacks.BlackBishop)) == 0 ? -1.0 : 1.0,
      (combinedWhiteAttacks & (*attacks.BlackKing)) == 0 ? -1.0 : 1.0,
      (combinedWhiteAttacks & (*attacks.BlackKnight)) == 0 ? -1.0 : 1.0,
      (combinedWhiteAttacks & (*attacks.BlackPawn)) == 0 ? -1.0 : 1.0,
      (combinedWhiteAttacks & (*attacks.BlackQueen)) == 0 ? -1.0 : 1.0,
      (combinedWhiteAttacks & (*attacks.BlackRook)) == 0 ? -1.0 : 1.0,


      // does the piece exist?
      nrOfWhiteBishop < 0.1 ? -1.0 : 1.0,
      nrOfWhiteKing < 0.1 ? -1.0 : 1.0,
      nrOfWhiteKnight < 0.1 ? -1.0 : 1.0,
      nrOfWhitePawn < 0.1 ? -1.0 : 1.0,
      nrOfWhiteQueen < 0.1 ? -1.0 : 1.0,
      nrOfWhiteRook < 0.1 ? -1.0 : 1.0,
      // How many of that piece type exists?
      nrOfWhiteBishop < 0.1 ? 0.0 : nrOfWhiteBishop / 100.0,
      nrOfWhiteKing < 0.1 ? 0.0 : nrOfWhiteKing / 100.0,
      nrOfWhiteKnight < 0.1 ? 0.0 : nrOfWhiteKnight / 100.0,
      nrOfWhitePawn < 0.1 ? 0.0 : nrOfWhitePawn / 100.0,
      nrOfWhiteQueen < 0.1 ? 0.0 : nrOfWhiteQueen / 100.0,
      nrOfWhiteRook < 0.1 ? 0.0 : nrOfWhiteRook / 100.0,
      // How many black pieces can each type attack?
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteBishop) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteKing) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteKnight) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhitePawn) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteQueen) & blackPieces)) / 100.0,
      static_cast<double>(utils::nrOfActiveBits((*attacks.WhiteRook) & blackPieces)) / 100.0,
      // Are any of the piece types safe?
      (combinedBlackAttacks & (*attacks.WhiteBishop)) == 0 ? 1.0 : -1.0,
      (combinedBlackAttacks & (*attacks.WhiteKing)) == 0 ? 1.0 : -1.0,
      (combinedBlackAttacks & (*attacks.WhiteKnight)) == 0 ? 1.0 : -1.0,
      (combinedBlackAttacks & (*attacks.WhitePawn)) == 0 ? 1.0 : -1.0,
      (combinedBlackAttacks & (*attacks.WhiteQueen)) == 0 ? 1.0 : -1.0,
      (combinedBlackAttacks & (*attacks.WhiteRook)) == 0 ? 1.0 : -1.0,

      static_cast<double>(utils::nrOfActiveBits(whitePieces)) / 100.0, // is never 0
      static_cast<double>(utils::nrOfActiveBits(blackPieces)) / 100.0, // is never 0
      static_cast<double>(utils::nrOfActiveBits(whitePieces | blackPieces)) / 100.0,

      utils::nrOfActiveBits(combinedBlackAttacks & whitePieces) == 0 ? 1.0 : -1.0
          * (static_cast<double>(utils::nrOfActiveBits(combinedBlackAttacks & whitePieces)) / 100.0),
      utils::nrOfActiveBits(combinedWhiteAttacks & blackPieces) == 0 ? -1.0 :
      static_cast<double>(utils::nrOfActiveBits(combinedWhiteAttacks & blackPieces)) / 100.0,

      node.blackQueenCastling ? -1.0 : 1.0,
      node.blackKingCastling ? -1.0 : 1.0,
      node.whiteQueenCastling ? 1.0 : -1.0,
      node.whiteKingCastling ? 1.0 : -1.0,

      static_cast<double>(100 - node.halfMoves) / 100.0,
      static_cast<double>(50 - node.fullMoves) / 100.0,

      // if the color playing is not yours, and the number here is high, it should not be a good thing.
      static_cast<double>(10/*node.children.size()*/)
          / 100.0 // will always be 0 unless children are generated before comparing score.

  };

  // input
  std::vector<float> inputs;

  // generate inputs
  for (auto b : boardInfo) {
    inputs.push_back(static_cast<float>(b));
  }

  std::array<::david::type::bitboard_t, 2> boards1 = {
      node.BlackKing,
      node.WhiteKing
  };
  std::array<::david::type::bitboard_t, 8> boards2 = {
      node.BlackBishop,
      node.BlackKnight,
      node.BlackQueen,
      node.BlackRook,
      node.WhiteBishop,
      node.WhiteQueen,
      node.WhiteKnight,
      node.WhiteRook
  };
  std::array<::david::type::bitboard_t, 2> boards8 = {
      node.BlackPawn,
      node.WhitePawn
  };

  // generate inputs
  for (auto b : boards1) {
    //auto ba = std::bitset<64>(b);
    //double arr[1] = {-1.0};
    auto prog = 0;
    do {
      auto i = LSB(b);
      flipBitOff(b, i);

      inputs.push_back(i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 1);

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
    do {
      auto i = LSB(b);
      flipBitOff(b, i);

      inputs.push_back(i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 2);

    // fill in missing pieces
    for (; prog < 2; prog++) {
      inputs.push_back(-1.0f);
    }
  }
  for (auto b : boards8) {
    //auto ba = std::bitset<64>(b);
    //double arr[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    auto prog = 0;
    do {
      auto i = LSB(b);
      flipBitOff(b, i);

      inputs.push_back(i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 8);

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
fann_type* convertVectorInputsToFannType(const std::vector<float> &inputs, unsigned long size) {
  fann_type *fi = new fann_type[size];

  int prog = 0;
  for (float i : inputs) {
    fi[prog++] = i;
  }

  return fi;
}

fann_type* boardToFannInputs(const ::david::type::gameState_t& node) {
  std::vector<float> inputs = convertGameStateToVectorInputs(node);
  return convertVectorInputsToFannType(inputs, inputs.size());
}
}

void yellDeprecated(const std::string info) {
  std::cerr << "@Deprecated function used " << (info.length() == 0 ? "!" : info) << std::endl;
}

/**
 * Set default board values
 * @param node gameState_t&
 */
void setDefaultChessLayout(::david::type::gameState_t &n) {
  n.BlackRook = ::david::constant::defaultPiecePosition::black::ROOK;
  n.BlackQueen = ::david::constant::defaultPiecePosition::black::QUEEN;
  n.BlackPawn = ::david::constant::defaultPiecePosition::black::PAWN;
  n.BlackKnight = ::david::constant::defaultPiecePosition::black::KNIGHT;
  n.BlackKing = ::david::constant::defaultPiecePosition::black::KING;
  n.BlackBishop = ::david::constant::defaultPiecePosition::black::BISHOP;

  n.WhiteRook = ::david::constant::defaultPiecePosition::white::ROOK;
  n.WhiteQueen = ::david::constant::defaultPiecePosition::white::QUEEN;
  n.WhitePawn = ::david::constant::defaultPiecePosition::white::PAWN;
  n.WhiteKnight = ::david::constant::defaultPiecePosition::white::KNIGHT;
  n.WhiteKing = ::david::constant::defaultPiecePosition::white::KING;
  n.WhiteBishop = ::david::constant::defaultPiecePosition::white::BISHOP;

  n.blackPieces = n.BlackPawn | n.BlackQueen | n.BlackKnight | n.BlackKing | n.BlackBishop | n.BlackRook;
  n.whitePieces = n.WhitePawn | n.WhiteQueen | n.WhiteKnight | n.WhiteKing | n.WhiteBishop | n.WhiteRook;

  n.pieces = n.blackPieces | n.whitePieces;

  n.blackKingCastling = true;
  n.blackQueenCastling = true;
  n.whiteKingCastling = true;
  n.whiteQueenCastling = true;
}

inline namespace print {
inline namespace cout {
/**
 * Print out the chess board based on a gameState node
 * @param gs type::gameState_t&
 */
void printGameState(const ::david::type::gameState_t &gs) {

  std::map<const char, ::david::type::bitboard_t> links = {
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
    auto bitboard = entry.second;

    do {
      auto i = LSB(bitboard);
      flipBitOff(bitboard, i);
      board.at(i) = piece;
    } while (bitboard != 0ULL);
  }

  std::cout << "\n  ";
  for (int i = 'A'; i < 'A' + 8; i++) {
    std::cout << "  " << (char) i << " ";
  }
  std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  for (uint8_t i = 7; i < 8; i--) {
    std::cout << i + 1 << " | ";
    for (uint8_t j = 0; j < 8; j++) {
      std::cout << board.at((i * 8) + j) << " | ";
    }
    std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  }
  std::cout << std::endl;
}

/**
 * Prints a bitboard as a chessboard with 0's and 1's
 * @param board - at bitboard_t
 */
void printBoard(::david::type::bitboard_t board) {
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
}// inline namespace cout
}// inline namespace print

/**
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param gs gameState_t&
 * @param fen const std::string&, must be correctly formatted (!)
 */
void generateBoardFromFen(::david::type::gameState_t& gs, const std::string &fen) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;
  
  std::map<const char, ::david::type::bitboard_t&> links = {
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
    const auto &c = fen[i];

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
        index += ctoi(c);
      }
    }

      // color
    else if (index == 64) {
      gs.playerColor = fen[i + 1] == 'w' ? WHITE : BLACK;

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

        //auto passantBoard = ::david::utils::chessIndexToBitboard(pos);
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

      gs.halfMoves = ::utils::stoi(hm);

      i += 1; // skip space
      index += 1;
    }

      // full move
    else if (index == 68) {
      std::string fm = "";
      for (; i < len && fen[i] != ' '; i += 1) {
        fm += fen[i];
      }

      gs.fullMoves = ::utils::stoi(fm);  //TODO potential error here
      break; // finished reading FEN string
    }
  }

  gs.blackPieces = gs.BlackPawn | gs.BlackQueen | gs.BlackKnight | gs.BlackKing | gs.BlackBishop | gs.BlackRook;
  gs.whitePieces = gs.WhitePawn | gs.WhiteQueen | gs.WhiteKnight | gs.WhiteKing | gs.WhiteBishop | gs.WhiteRook;

  gs.pieces = gs.blackPieces | gs.whitePieces;

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

/**
 * Takes a chess position such as "E6" and creates a bitboard with a on bit at that given index.
 *
 * @param chessIndex String such as "E6"
 * @return bitboard with a active bit in given position
 */
::david::type::bitboard_t chessIndexToBitboard(const std::string &chessIndex) {
  using ::david::type::bitboard_t;
  
  bitboard_t board = 0ULL;
  flipBit(board, static_cast<bitboard_t>(chessIndexToArrayIndex(chessIndex)));

  return board;
}

void affectGameStateByEGNMove(::david::type::gameState_t& gs, const std::string& EGN) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;

  // get origin
  const std::string originEGN = EGN.substr(0, 2);
  const auto origin = chessIndexToArrayIndex(originEGN);

  // get destination
  const std::string destinationEGN = EGN.substr(2, 2);
  const auto destination = chessIndexToArrayIndex(destinationEGN);

  // check for issues
  if (origin == 64 || destination == 64) {
    std::cerr << "Unable to get origin || destination for EGN: " << EGN << std::endl;
    return;
  }

  // TODO: make the gameState use array for each type.. this is troublesome....
  if (!bitAt(gs.pieces, origin)) {
    std::cerr << "ORIGIN " << origin << " AT THE FOLLOWING BITBOARD DOES NOT EXIST!!" << std::endl;
    ::utils::printGameState(gs);
    return;
  }

  // is it on the black or white?
  if (bitAt(gs.blackPieces, origin)) {

    if (bitAt(gs.BlackRook, origin)) {
      movePiece(gs.BlackRook, origin, destination);
    }
    else if (bitAt(gs.BlackKnight, origin)) {
      movePiece(gs.BlackKnight, origin, destination);
    }
    else if (bitAt(gs.BlackBishop, origin)) {
      movePiece(gs.BlackBishop, origin, destination);
    }
    else if (bitAt(gs.BlackKing, origin)) {
      movePiece(gs.BlackKing, origin, destination);
    }
    else if (bitAt(gs.BlackQueen, origin)) {
      movePiece(gs.BlackQueen, origin, destination);
    }
    else if (bitAt(gs.BlackPawn, origin)) {
      movePiece(gs.BlackPawn, origin, destination);
    }
  }
  else if (bitAt(gs.whitePieces, origin)) {

    if (bitAt(gs.WhiteRook, origin)) {
      movePiece(gs.WhiteRook, origin, destination);
    }
    else if (bitAt(gs.WhiteKnight, origin)) {
      movePiece(gs.WhiteKnight, origin, destination);
    }
    else if (bitAt(gs.WhiteBishop, origin)) {
      movePiece(gs.WhiteBishop, origin, destination);
    }
    else if (bitAt(gs.WhiteKing, origin)) {
      movePiece(gs.WhiteKing, origin, destination);
    }
    else if (bitAt(gs.WhiteQueen, origin)) {
      movePiece(gs.WhiteQueen, origin, destination);
    }
    else if (bitAt(gs.WhitePawn, origin)) {
      movePiece(gs.WhitePawn, origin, destination);
    }
  }

  // Swap colours after move
  gs.playerColor = gs.playerColor == WHITE ? BLACK : WHITE;
  
}

void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest) {
  flipBitOff(board, orig);
  flipBitOn(board, dest);
}

/**
 * Check if a bit is set at given index.
 *
 * @param b The bits in form of a uint64_t
 * @param i the index as uint8_t
 * @return true if bit is 1 at index i
 */
//constexpr bool bitAt(uint64_t b, uint8_t i) {
//  return (b & (1ULL << i)) != 0;
//}

/**
 * Finds the index of the first active bit, starting from right to left.
 * LSB: Least Significant Bit
 *
 * @param board type::bitboard_t
 * @return index[0, 63], if board is 0, 0 is returned.
 */
::david::type::bitboard_t LSB(::david::type::bitboard_t board) {
#ifdef __linux__
  return (board != 0) ? __builtin_ffsll(board) - 1 : 0ULL;
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

/**
 * Deactivates the smallest active bit, and returns the LSB value.
 *
 * @param board type::bitboard_t
 * @return index[0, 63], if the board is 0, 0 is returned
 */
::david::type::bitboard_t NSB(::david::type::bitboard_t &board) {
  board &= ~(1LL << LSB(board));
  return LSB(board);
}

::david::type::bitboard_t MSB(::david::type::bitboard_t board) {
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

::david::type::bitboard_t NSB_r(::david::type::bitboard_t &board) {
  board &= ~(1LL << MSB(board));
  return MSB(board);
}

// Turns on bit
void flipBit(::david::type::bitboard_t &board, ::david::type::bitboard_t index) {
  board |= (1LL << index);
}

// YEAH tell that bit to flipp off!!!
// Nobody wants you bit... NOBODY WANTS YOU
void flipBitOff(::david::type::bitboard_t &board, ::david::type::bitboard_t index) {
  board &= ~(1ULL << index);
}

// Turns on bit
void flipBitOn(::david::type::bitboard_t &board, ::david::type::bitboard_t index) {
  board |= (1LL << index);
}

int nrOfActiveBits(const ::david::type::bitboard_t b) {
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

bool bitAt(const ::david::type::bitboard_t b, const uint8_t i) {
  return (b & (1ULL << i)) != 0;
}

std::string getAbsoluteProjectPath() {
#ifdef __linux__
  //linux code goes here
  char buffer[PATH_MAX];
  std::string path = getcwd(buffer, PATH_MAX) ? std::string(buffer) : std::string("");

  if (path == "") {
    return "";
  }

  // find string until folder project name
  std::string projectPath = path.substr(0, path.find(::david::engineInformation::PROJECT_NAME)); //hackz

  return projectPath + ::david::engineInformation::PROJECT_NAME;
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

bool perft() {
  return perft(-1);
}

bool perft(const int limit) {
  ::david::type::gameState_t gs;
  ::utils::setDefaultChessLayout(gs);

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

  auto len = limit == -1 ? static_cast<int>(perftScores.size()) : limit;
  bool success = true;

  if (limit != -1) {
    std::cerr << " * perft is depth limited to " << limit << ", max depth is " << (perftScores.size() - 1) << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }


  std::printf("|%7s|%32s|%32s|%10s|%10s|\n",
              "-------",
              "--------------------------------",
              "--------------------------------",
              "----------",
              "----------");
  std::printf("| %5s | %30s | %30s | %8s | %8s |\n",
              "Depth",
              "David MoveGen Result",
              "Expected MoveGen Result",
              "Error %",
              "Seconds");
  std::printf("|%7s|%32s|%32s|%10s|%10s|\n",
              "-------",
              "--------------------------------",
              "--------------------------------",
              "----------",
              "----------");
  for (int i = 0; i < len; i++) {
    // start time
    struct timeval tp, tp2;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    // run perft
    auto moveGenPerft = ::utils::perft(i, gs);

    // time finished
    gettimeofday(&tp2, NULL);
    long int ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000;

    // expect node count
    auto expectedPerft = perftScores[i];

    // generated node error estimation
    const double difference =
        static_cast<double>(std::max(moveGenPerft, expectedPerft) - std::min(moveGenPerft, expectedPerft)) / std::max(moveGenPerft, expectedPerft);

    // print results for perft[i]
    std::printf("| %5i | %30lu | %30lu | %8.1f | %8.2f |\n",
                i,
                moveGenPerft,
                expectedPerft,
                difference * 100.0, // difference
                (ms2 - ms) / 1000.0);

    // if the results are bad, don't continue
    if (moveGenPerft != expectedPerft) {
      success = false;
      break;
    }
  }

  return success;
}

uint64_t perft(const int depth, const ::david::type::gameState_t &gs) {
  if (depth == 0) {
    return 1;
  }


  // create a holder for possible game outputs
  std::vector<::david::type::gameState_t> states;

  {
    ::david::movegen::MoveGenerator gen;
    gen.setGameState(gs);

    // generate possible game outputs
    gen.generateGameStates(states);
  }

  int len = static_cast<int>(states.size());
  uint64_t nodes = 0;

  // calculate move for every move
  for (int i = 0; i < len; i++) {
    nodes += perft(depth - 1, states.at(i));
  }

  return nodes;
}

const std::string getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;
  using ::david::type::bitboard_t;
  bitboard_t a, b;

  if (first.playerColor == WHITE) {
    a = first.whitePieces;
    b = second.whitePieces;
  } else {
    a = first.blackPieces;
    b = second.blackPieces;
  }

  bitboard_t difference = a ^ b;
  bitboard_t from = LSB(a & difference);
  bitboard_t to = LSB(b & difference);

#ifdef DAVID_DEVELOPMENT
  assert(difference != 0ULL);
  assert(from < 64);
  assert(to < 64);
#endif

  std::array<char, 8> indexes = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  std::string EGN = "";

  // from
  EGN += indexes[from % 8];
  EGN += std::to_string((from + 8) / 8);

  // to
  EGN += indexes[to % 8];
  EGN += std::to_string((to + 8) / 8);

  return EGN;
}
void getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second, std::string &EGN) {
  EGN = getEGN(first, second);
}

} // End of utils

