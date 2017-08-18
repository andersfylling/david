#include "david/utils/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "david/types.h"
#include "david/MoveGen.h"

#ifdef __linux__
//linux code goes here
#include <david/environment.h>
#include <david/ChessEngine.h>
#include <assert.h>
#include <david/MoveGeneration.h>
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

  // take all the data from the new fields and apply them to the old
  int w = 0;
  int b = 1;

  if (!n.isWhite) {
    w = 1;
    b = 0;
  }

  n.WhiteBishop = n.bishops[w];
  n.WhiteKing   = n.kings[w];
  n.WhiteKnight = n.knights[w];
  n.WhitePawn   = n.pawns[w];
  n.WhiteQueen  = n.queens[w];
  n.WhiteRook   = n.rooks[w];

  n.BlackBishop = n.bishops[b];
  n.BlackKing   = n.kings[b];
  n.BlackKnight = n.knights[b];
  n.BlackPawn   = n.pawns[b];
  n.BlackQueen  = n.queens[b];
  n.BlackRook   = n.rooks[b];

  n.whitePieces = n.piecess[w];
  n.blackPieces = n.piecess[b];

  n.pieces = n.combinedPieces;

  n.playerColor = n.isWhite ? WHITE : BLACK;

  // TODO: to use a uint8 for castling or not? seems slower, but less memory..
  
}

}

bool isHalfMove(const ::david::type::gameState_t &parent, const ::david::type::gameState_t &child) {
  // This counter is reset after captures or pawn moves, and incremented otherwise.
  // Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  // including castling itself, increment the Halfmove Clock.
  // However, those moves are irreversible in the sense to reverse the same rights -
  // since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

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
std::string generateFen(const ::david::type::gameState_t& node) {
  using ::david::type::bitboard_t;

  int w = 0;
  int b = 1;
  
  if (!node.isWhite) {
    w = 1;
    b = 0;
  }

  std::array<bitboard_t, 12> boards = {
      node.bishops[w],
      node.kings[w],
      node.knights[w],
      node.pawns[w],
      node.queens[w],
      node.rooks[w],
      
      node.bishops[b],
      node.kings[b],
      node.knights[b],
      node.pawns[b],
      node.queens[b],
      node.rooks[b]
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
  fen += node.isWhite ? 'w' : 'b';
  fen += ' '; // spacing

  // missing castling verification support
  fen += "-";
  fen += ' '; // spacing

  // missing passant target verification support
  fen += "-";
  fen += ' '; // spacing

  // missing halfmove verification support
  fen += std::to_string(node.halfMoves);
  fen += ' '; // spacing

  // missing fullmove verification support
  fen += std::to_string(node.fullMoves);

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
 // TODO: convert to the new node setup
std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(const ::david::type::gameState_t& node) {
//  movegen::MoveGenerator gen;
//  gen.setGameState(node);
//  gen.generateAttacks(node.playerColor);
//
//  // These are used to define whats benefitial and negative inputs
//  auto attacks = gen.getAttackState();

  ::david::environment::Environment env{node.playerColor}; // TODO: REMOVE ASAP
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

  std::array<float, ::david::constant::nn::INPUTSIZE> boardInfo = {
      static_cast<float>(node.isWhite ? 1.0 : -1.0),

      static_cast<float>(nrOfBlackBishop  < 0.1 ? 1.0 : -1.0),
      static_cast<float>(nrOfBlackKing    < 0.1 ? 1.0 : -1.0),
      static_cast<float>(nrOfBlackKnight  < 0.1 ? 1.0 : -1.0),
      static_cast<float>(nrOfBlackPawn    < 0.1 ? 1.0 : -1.0),
      static_cast<float>(nrOfBlackQueen   < 0.1 ? 1.0 : -1.0),
      static_cast<float>(nrOfBlackRook    < 0.1 ? 1.0 : -1.0),

      static_cast<float>(nrOfBlackBishop  < 0.1 ? 0.0 : nrOfBlackBishop / 100.0),
      static_cast<float>(nrOfBlackKing    < 0.1 ? 0.0 : nrOfBlackKing   / 100.0),
      static_cast<float>(nrOfBlackKnight  < 0.1 ? 0.0 : nrOfBlackKnight / 100.0),
      static_cast<float>(nrOfBlackPawn    < 0.1 ? 0.0 : nrOfBlackPawn   / 100.0),
      static_cast<float>(nrOfBlackQueen   < 0.1 ? 0.0 : nrOfBlackQueen  / 100.0),
      static_cast<float>(nrOfBlackRook    < 0.1 ? 0.0 : nrOfBlackRook   / 100.0),

      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackBishop) & whitePieces)  / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackKing) & whitePieces)    / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackKnight) & whitePieces)  / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackPawn) & whitePieces)    / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackQueen) & whitePieces)   / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.BlackRook) & whitePieces)    / 100.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackBishop))  == 0 ? -1.0 : 1.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackKing))    == 0 ? -1.0 : 1.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackKnight))  == 0 ? -1.0 : 1.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackPawn))    == 0 ? -1.0 : 1.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackQueen))   == 0 ? -1.0 : 1.0),
      static_cast<float>((combinedWhiteAttacks & (*attacks.BlackRook))    == 0 ? -1.0 : 1.0),


      // does the piece exist?
      static_cast<float>(nrOfWhiteBishop  < 0.1 ? -1.0 : 1.0),
      static_cast<float>(nrOfWhiteKing    < 0.1 ? -1.0 : 1.0),
      static_cast<float>(nrOfWhiteKnight  < 0.1 ? -1.0 : 1.0),
      static_cast<float>(nrOfWhitePawn    < 0.1 ? -1.0 : 1.0),
      static_cast<float>(nrOfWhiteQueen   < 0.1 ? -1.0 : 1.0),
      static_cast<float>(nrOfWhiteRook    < 0.1 ? -1.0 : 1.0),
      // How many of that piece type exists?
      static_cast<float>(nrOfWhiteBishop  < 0.1 ? 0.0 : nrOfWhiteBishop / 100.0),
      static_cast<float>(nrOfWhiteKing    < 0.1 ? 0.0 : nrOfWhiteKing   / 100.0),
      static_cast<float>(nrOfWhiteKnight  < 0.1 ? 0.0 : nrOfWhiteKnight / 100.0),
      static_cast<float>(nrOfWhitePawn    < 0.1 ? 0.0 : nrOfWhitePawn   / 100.0),
      static_cast<float>(nrOfWhiteQueen   < 0.1 ? 0.0 : nrOfWhiteQueen  / 100.0),
      static_cast<float>(nrOfWhiteRook    < 0.1 ? 0.0 : nrOfWhiteRook   / 100.0),
      // How many black pieces can each type attack?
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhiteBishop) & blackPieces) / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhiteKing)   & blackPieces) / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhiteKnight) & blackPieces) / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhitePawn)   & blackPieces) / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhiteQueen)  & blackPieces) / 100.0),
      static_cast<float>(utils::nrOfActiveBits((*attacks.WhiteRook)   & blackPieces) / 100.0),
      // Are any of the piece types safe?
      static_cast<float>((combinedBlackAttacks & (*attacks.WhiteBishop))  == 0 ? 1.0 : -1.0),
      static_cast<float>((combinedBlackAttacks & (*attacks.WhiteKing))    == 0 ? 1.0 : -1.0),
      static_cast<float>((combinedBlackAttacks & (*attacks.WhiteKnight))  == 0 ? 1.0 : -1.0),
      static_cast<float>((combinedBlackAttacks & (*attacks.WhitePawn))    == 0 ? 1.0 : -1.0),
      static_cast<float>((combinedBlackAttacks & (*attacks.WhiteQueen))   == 0 ? 1.0 : -1.0),
      static_cast<float>((combinedBlackAttacks & (*attacks.WhiteRook))    == 0 ? 1.0 : -1.0),

      static_cast<float>(utils::nrOfActiveBits(whitePieces) / 100.0), // is never 0
      static_cast<float>(utils::nrOfActiveBits(blackPieces) / 100.0), // is never 0
      static_cast<float>(utils::nrOfActiveBits(whitePieces | blackPieces) / 100.0),

      static_cast<float>(utils::nrOfActiveBits(combinedBlackAttacks & whitePieces) == 0 ? 1.0 : -1.0
          * (utils::nrOfActiveBits(combinedBlackAttacks & whitePieces) / 100.0)),
      static_cast<float>(utils::nrOfActiveBits(combinedWhiteAttacks & blackPieces) == 0 ? -1.0 :
      utils::nrOfActiveBits(combinedWhiteAttacks & blackPieces) / 100.0),

      static_cast<float>(node.blackQueenCastling  ? -1.0 :  1.0),
      static_cast<float>(node.blackKingCastling   ? -1.0 :  1.0),
      static_cast<float>(node.whiteQueenCastling  ?  1.0 : -1.0),
      static_cast<float>(node.whiteKingCastling   ?  1.0 : -1.0),

      static_cast<float>(100 - node.halfMoves / 100.0),
      static_cast<float>(50 - node.fullMoves / 100.0),

      // if the color playing is not yours, and the number here is high, it should not be a good thing.
      static_cast<float>(10/*node.children.size()*/ / 100.0), // index 60
      // will always be 0 unless children are generated before comparing score.

      // needs to be filled in afterwards
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // index = 70
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // index = 80
      0, // 81
      0  // 82
  };

  unsigned int index = 61;

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

      boardInfo[index++] = (i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 1);

    // fill in missing pieces
    for (; prog < 1; prog++) {
      boardInfo[index++] = (-1.0f);
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

      boardInfo[index++] = (i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 2);

    // fill in missing pieces
    for (; prog < 2; prog++) {
      boardInfo[index++] = (-1.0f);
    }
  }
  for (auto b : boards8) {
    //auto ba = std::bitset<64>(b);
    //double arr[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    auto prog = 0;
    do {
      auto i = LSB(b);
      flipBitOff(b, i);

      boardInfo[index++] = (i == 0 ? 0.0f : i / 100.0f);
      prog += 1;
    } while (b != 0ULL && prog < 8);

    // fill in missing pieces
    for (; prog < 8; prog++) {
      boardInfo[index++] = (-1.0f);
    }
  }

  return boardInfo;
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
  using ::david::bitboard::COLOR::WHITE;


  // set the new array data
  n.piecesArr[0] = {
      ::david::constant::defaultPiecePosition::white::PAWN,
      ::david::constant::defaultPiecePosition::black::PAWN
  };
  n.piecesArr[1] = {
      ::david::constant::defaultPiecePosition::white::ROOK,
      ::david::constant::defaultPiecePosition::black::ROOK
  };
  n.piecesArr[2] = {
      ::david::constant::defaultPiecePosition::white::KNIGHT,
      ::david::constant::defaultPiecePosition::black::KNIGHT
  };
  n.piecesArr[3] = {
      ::david::constant::defaultPiecePosition::white::BISHOP,
      ::david::constant::defaultPiecePosition::black::BISHOP
  };
  n.piecesArr[4] = {
      ::david::constant::defaultPiecePosition::white::QUEEN,
      ::david::constant::defaultPiecePosition::black::QUEEN
  };
  n.piecesArr[5] = {
      ::david::constant::defaultPiecePosition::white::KING,
      ::david::constant::defaultPiecePosition::black::KING
  };


  // set the new array data
  n.pawns   = {
      ::david::constant::defaultPiecePosition::white::PAWN,
      ::david::constant::defaultPiecePosition::black::PAWN
  };
  n.rooks   = {
      ::david::constant::defaultPiecePosition::white::ROOK,
      ::david::constant::defaultPiecePosition::black::ROOK
  };
  n.queens  = {
      ::david::constant::defaultPiecePosition::white::QUEEN,
      ::david::constant::defaultPiecePosition::black::QUEEN
  };
  n.knights = {
      ::david::constant::defaultPiecePosition::white::KNIGHT,
      ::david::constant::defaultPiecePosition::black::KNIGHT
  };
  n.kings   = {
      ::david::constant::defaultPiecePosition::white::KING,
      ::david::constant::defaultPiecePosition::black::KING
  };
  n.bishops = {
      ::david::constant::defaultPiecePosition::white::BISHOP,
      ::david::constant::defaultPiecePosition::black::BISHOP
  };

  n.piecess = {
      ::david::constant::defaultPiecePosition::white::PIECES,
      ::david::constant::defaultPiecePosition::black::PIECES
  };

  n.combinedPieces = ::david::constant::defaultPiecePosition::PIECES;

  n.blackKingCastling   = true;
  n.blackQueenCastling  = true;
  n.whiteKingCastling   = true;
  n.whiteQueenCastling  = true;
  n.castling = 15; //utils::stringTo8bitArray("00001111");

  n.isWhite = true;

  n.halfMoves = 0;
  n.fullMoves = 1;

  n.possibleSubMoves = 20;


  // add data to old fields
  ::utils::convert::legacyGameStateUpdate(n);
}

inline namespace print {
inline namespace cout {
/**
 * Print out the chess board based on a gameState node
 * @param gs type::gameState_t&
 */
void printGameState(const ::david::type::gameState_t &gs) {

  int w = 0;
  int b = 1;
  if (!gs.isWhite) {
    w = 1;
    b = 0;
  }

  std::map<const char, ::david::type::bitboard_t> links = {
      {'b', gs.bishops[b]},
      {'k', gs.kings[b]},
      {'n', gs.knights[b]},
      {'p', gs.pawns[b]},
      {'q', gs.queens[b]},
      {'r', gs.rooks[b]},

      {'B', gs.bishops[w]},
      {'K', gs.kings[w]},
      {'N', gs.knights[w]},
      {'P', gs.pawns[w]},
      {'Q', gs.queens[w]},
      {'R', gs.rooks[w]}
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
void printBoard(::david::type::bitboard_t bb) {
  std::string board(64, '-');

  while (bb != 0ULL) {
    const auto i = LSB(bb);
    flipBitOff(bb, i);
    board.at(i) = 'X';
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

  int w = 0;
  int b = 1;
  if (!gs.isWhite) {
    w = 1;
    b = 0;
  }

  std::map<const char, ::david::type::bitboard_t> links = {
      {'b', gs.bishops[b]},
      {'k', gs.kings[b]},
      {'n', gs.knights[b]},
      {'p', gs.pawns[b]},
      {'q', gs.queens[b]},
      {'r', gs.rooks[b]},

      {'B', gs.bishops[w]},
      {'K', gs.kings[w]},
      {'N', gs.knights[w]},
      {'P', gs.pawns[w]},
      {'Q', gs.queens[w]},
      {'R', gs.rooks[w]}
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
      gs.isWhite = gs.playerColor == WHITE;

      i += 2; // skip char and space afterwards
      index += 1;
    }

      // castling
    else if (index == 65) {
      for (; i < len && fen[i] != ' '; i += 1) {
        // TODO: Implement castling for fen string
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
        // TODO: what to do about en passant
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

      gs.fullMoves = ::utils::stoi(fm);
      break; // finished reading FEN string
    }
  }
  gs.piecess = {
      gs.pawns[0] | gs.rooks[0] | gs.queens[0] | gs.knights[0] | gs.kings[0] | gs.bishops[0],
      gs.pawns[1] | gs.rooks[1] | gs.queens[1] | gs.knights[1] | gs.kings[1] | gs.bishops[1]
  };
  gs.combinedPieces = gs.piecess[0] | gs.piecess[1];

  // LEGACY SUPPORT
  ::utils::convert::legacyGameStateUpdate(gs);
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

// TODO: support new gameNode type
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
  if (!bitAt(gs.combinedPieces, origin)) {
    std::cerr << "ORIGIN " << origin << " AT THE FOLLOWING BITBOARD DOES NOT EXIST!!" << std::endl;
    ::utils::printGameState(gs);
    return;
  }
  
  for (int i = 0; i < 2; i++) {
    if (bitAt(gs.piecess[i], origin)) {
      if (bitAt(gs.pawns[i], origin)) {
        movePiece(gs.pawns[i], origin, destination);
      }
      else if (bitAt(gs.rooks[i], origin)) {
        movePiece(gs.rooks[i], origin, destination);
      }
      else if (bitAt(gs.bishops[i], origin)) {
        movePiece(gs.bishops[i], origin, destination);
      }
      else if (bitAt(gs.kings[i], origin)) {
        movePiece(gs.kings[i], origin, destination);
      }
      else if (bitAt(gs.queens[i], origin)) {
        movePiece(gs.queens[i], origin, destination);
      }
      else if (bitAt(gs.knights[i], origin)) {
        movePiece(gs.knights[i], origin, destination);
      }
    }
  }

  gs.isWhite = !gs.isWhite;


  // LEGACY SUPPORT
  ::utils::convert::legacyGameStateUpdate(gs);
  
}

void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest) {
  flipBitOff(board, orig);
  flipBitOn(board, dest);
}

/**
 * Finds the index of the first active bit, starting from right to left.
 * LSB: Least Significant Bit
 *
 * @param board type::bitboard_t
 * @return index[0, 63], if board is 0, 0 is returned.
 */
uint8_t LSB(::david::type::bitboard_t board) {
#ifdef __linux__
  return (board > 0) ? __builtin_ffsl(board) - 1 : 0; // ffsl is long
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
  for (int i = 0; i <= len; i++) {
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
      //break;
    }
  }

  return success;
}

uint64_t perft(const int depth, const ::david::type::gameState_t &gs) {
  if (depth == 0) {
    return 1;
  }

  // create a holder for possible game outputs
  bool newMoveGen = false; // MoveGen.cpp/h or MoveGenerator.cpp/h
  if (newMoveGen) {
    ::david::MoveGen moveGen{gs};

    std::array<::david::type::gameState_t, ::david::constant::MAXMOVES> states;
    const uint16_t len = moveGen.template generateGameStates<::david::constant::MAXMOVES>(states);
    uint64_t nodes = 0;

    // calculate move for every move
    const auto nextDepth = depth - 1;
    for (unsigned long i = 0; i < len; i++) {
      nodes += perft(nextDepth, states[i]);
    }

    return nodes;
  }
  else {
    std::vector<::david::type::gameState_t> states;

    // generate possible game outputs
    ::david::movegen::MoveGenerator gen{};
    gen.setGameState(gs);
    gen.generateGameStates(states);

    const auto len = states.size();
    uint64_t nodes = 0;

    // calculate move for every move
    for (unsigned long i = 0; i < len; i++) {
      nodes += perft(depth - 1, states.at(i));
    }

    return nodes;
  }
}

const std::string getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;
  using ::david::type::bitboard_t;

  // 0 == 1 if the colour has changed. aka new step has been made.
  bitboard_t a = first.piecess[0]; // this can be either black or white, doesnt matter
  bitboard_t b = second.piecess[1]; // but it's always the opposite of this

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

