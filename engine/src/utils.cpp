#include "chess_ann/utils.h"

#ifdef __linux__
//linux code goes here
#include <unistd.h>
#include <chess_ann/GameTree.h>
#include <chess_ann/environment.h>
#elif _WIN32
// windows code goes here
#endif

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

bool utils::isHalfMove(::gameTree::nodePtr parent, ::gameTree::nodePtr child) {
  using ::bitboard::COLOR::WHITE;
  using ::bitboard::COLOR::BLACK;
  using ::bitboard::bitboard_t;

  ::environment::Environment env1(parent->playerColor);
  ::environment::Environment env2(child->playerColor);

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
std::string utils::generateFen(::gameTree::nodePtr node) {
  using ::bitboard::COLOR::WHITE;
  using ::bitboard::COLOR::BLACK;
  using ::bitboard::bitboard_t;

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
      if (::utils::bitAt(boards[j], i)) {
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
  return (b & (1 << i)) != 0;
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
  std::string projectPath = path.substr(0, path.find(::engineInformation::PROJECT_NAME)); //hackz

  return projectPath + ::engineInformation::PROJECT_NAME;
#elif _WIN32
  // windows code goes here
  return "";
#else
  // idk
  std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
  return "";
#endif
}

