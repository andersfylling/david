#include <unistd.h>
#include <string>
#include <map>
#include "david/types.h"
#include "david/david.h"
#include "david/utils/utils.h"
#include "david/utils/gameState.h"

namespace utils {
namespace gameState {


/**
 * A FEN record contains six fields. The separator between fields is a space. The fields are:

    1. Piece placement (from white's perspective). Each rank is described, starting with rank 8 and ending with rank 1; within each rank, the contents of each square are described from file "a" through file "h". Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter taken from the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K").[1] White pieces are designated using upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Empty squares are noted using digits 1 through 8 (the number of empty squares), and "/" separates ranks.

    2. Active color. "w" means White moves next, "b" means Black.

    3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more letters: "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can castle kingside), and/or "q" (Black can castle queenside).

    4. En passant target square in algebraic notation. If there's no en passant target square, this is "-". If a pawn has just made a two-square move, this is the position "behind" the pawn. This is recorded regardless of whether there is a pawn in position to make an en passant capture.[2]

    5. Halfmove clock: This is the number of halfmoves since the last capture or pawn advance. This is used to determine if a draw can be claimed under the fifty-move rule.

    6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.

 * @param gs
 * @param whiteMovesNext
 * @return
 * @Deprecated needs to use gameState_t not _ptr
 */
std::string generateFen(const ::david::type::gameState_t& gs) {
  using ::david::type::bitboard_t;

  int w = 0;
  int b = 1;
  if (!gs.isWhite) {
    w = 1;
    b = 0;
  }

  std::array<char, 12> symbols = {
      'p',
      'r',
      'n',
      'b',
      'q',
      'k',

      'P',
      'R',
      'N',
      'B',
      'Q',
      'K'
  };

  char spaces[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8'};

  int spacing = 0;
  int pos = 0; // fenstring position
  std::string fen = "";

  for (uint8_t i = 0; i < 64; i++) {
    if (pos == 8) {
      pos = 0;
      fen += '/';
    }

    // find the most significant bit, or chess pieces from left top to right bottom
    char p = ' ';
    for (uint8_t j = 0; j < gs.piecesArr.size(); j++) {
      const auto boards = gs.piecesArr[j];

      // for each colour
      for (uint8_t c = 0; c < 2; c++) {
        const bool white = (c == 0 && gs.isWhite) || (c == 1 && !gs.isWhite); // check if teh current piece is white

        if (utils::bitAt(boards[c], 63 - i)) {
          p = symbols[j + (white ? 6 : 0)];
          break;
        }
      }
    }

    if (p == ' ') {
      spacing += 1;
    }

    if (pos == 7 || p != ' ') {
      if (spacing > 0) {
        fen += spaces[spacing];
      }
      spacing = 0;
    }

    if (p != ' ') {
      fen += p;
    }

    pos += 1;
  }

  fen += ' '; // spacing

  // who is the active player
  fen += gs.isWhite ? 'w' : 'b';
  fen += ' '; // spacing

  // Castling
  if (gs.queenCastlings[w] || gs.queenCastlings[b] || gs.kingCastlings[w] || gs.kingCastlings[b]) {
    // white king
    if (gs.kingCastlings[w]) {
      fen += 'K';
    }

    // white queen
    if (gs.queenCastlings[w]) {
      fen += 'Q';
    }

    // black king
    if (gs.kingCastlings[b]) {
      fen += 'k';
    }

    // black queen
    if (gs.queenCastlings[b]) {
      fen += 'q';
    }
  }
  else {
    fen += '-';
  }
  fen += ' '; // spacing

  // encode en passant position
  if (gs.enPassant != 0) {
    char col = 'h' - (gs.enPassant % 8);
    int row = gs.enPassant / 8;
    fen += col;
    fen += std::to_string(1 + row);
  }
  else {
    fen += "-";
  }
  fen += ' '; // spacing

  // missing halfmove verification support
  fen += std::to_string(gs.halfMoves);
  fen += ' '; // spacing

  // missing fullmove verification support
  fen += std::to_string(gs.fullMoves);

  return fen;
}


/**
 * Print out the chess board based on a gameState node
 * @param gs type::gameState_t&
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
 */
void print(const ::david::type::gameState_t& gs) {
  int w = gs.isWhite ? 0 : 1;
  int b = gs.isWhite ? 1 : 0;

  std::map<const char, ::david::type::bitboard_t> links = {
      {'b', gs.piecesArr[::david::constant::index::bishop][b]},
      {'k', gs.piecesArr[::david::constant::index::king][b]},
      {'n', gs.piecesArr[::david::constant::index::knight][b]},
      {'p', gs.piecesArr[::david::constant::index::pawn][b]},
      {'q', gs.piecesArr[::david::constant::index::queen][b]},
      {'r', gs.piecesArr[::david::constant::index::rook][b]},

      {'B', gs.piecesArr[::david::constant::index::bishop][w]},
      {'K', gs.piecesArr[::david::constant::index::king][w]},
      {'N', gs.piecesArr[::david::constant::index::knight][w]},
      {'P', gs.piecesArr[::david::constant::index::pawn][w]},
      {'Q', gs.piecesArr[::david::constant::index::queen][w]},
      {'R', gs.piecesArr[::david::constant::index::rook][w]}
  };

  std::string board(64, '-');

  for (auto entry : links) {
    const char piece = entry.first;
    auto bitboard = entry.second;

    while (bitboard != 0ULL) {
      auto i = ::utils::LSB(bitboard);
      flipBitOff(bitboard, i);
      board.at(63 - i) = piece;
    }
  }

  if (gs.enPassant > 0) {
    board.at(63 - gs.enPassant) = 'e';
  }

  std::cout << "\n ";
  std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  for (uint8_t i = 0; i < 8; i++) {
    std::cout << 8 - i << " | ";
    for (uint8_t j = 0; j < 8; j++) {
      std::cout << board.at((i * 8) + j) << " | ";
    }
    std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
  }
  std::cout << "  ";
  for (int i = 'A'; i < 'A' + 8; i++) {
    std::cout << "  " << (char) i << " ";
  }

  std::cout << std::endl;
} // print(...)


/**
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param gs gameState_t&
 * @param fen const std::string&, must be correctly formatted (!)
 */
void generateFromFEN(::david::type::gameState_t& gameState, const std::string &fen) {
  ::david::type::gameState_t gs;

  // reset
  gs.enPassant = 0;
  gs.enPassantPawn = 0;
  gs.passant = false;
  gs.kingCastlings = {{ false }};
  gs.queenCastlings = {{ false }};



  int w = 0;
  int b = 1;


  // if its a small letter, use black. otherwise use white.
  std::map<const char, ::david::type::bitboard_t&> links = {
      {'b', gs.piecesArr[::david::constant::index::bishop][b]},
      {'k', gs.piecesArr[::david::constant::index::king][b]},
      {'n', gs.piecesArr[::david::constant::index::knight][b]},
      {'p', gs.piecesArr[::david::constant::index::pawn][b]},
      {'q', gs.piecesArr[::david::constant::index::queen][b]},
      {'r', gs.piecesArr[::david::constant::index::rook][b]},

      {'B', gs.piecesArr[::david::constant::index::bishop][w]},
      {'K', gs.piecesArr[::david::constant::index::king][w]},
      {'N', gs.piecesArr[::david::constant::index::knight][w]},
      {'P', gs.piecesArr[::david::constant::index::pawn][w]},
      {'Q', gs.piecesArr[::david::constant::index::queen][w]},
      {'R', gs.piecesArr[::david::constant::index::rook][w]}
  };


  uint8_t index = 0; //64 = space, 66 = space,
  auto len = static_cast<uint8_t>(fen.length());
  for (uint8_t i = 0; i < len; i++) {
    //::utils::gameState::generateMergedBoardVersion(gs);
    const auto &c = fen[i];

    // piece positions
    if (index < 64) {
      if (c == '/') {
        continue;
      }

      // check if the char is a piece type
      if (links.count(c) > 0) {
        // it's a piece type
        // since this index starts at the top of the string, we need to start at index 63.
        flipBit(links.at(c), 63 - index); // set bit at correct index on correct board
        index += 1;
      } else {
        // assumption: it's a number
        // update index with this number
        index += ctoi(c);
      }
    }

      // color
    else if (index == 64) {
      gs.isWhite = fen[i + 1] != 'b';

      i += 2; // skip char and space afterwards
      index += 1;
    }

      // castling
    else if (index == 65) {
      for (; i < len && fen[i] != ' '; i += 1) {
        char fenC = fen[i];
        if (fenC == 'K') {
          gs.kingCastlings[w] = true;
        }
        else if (fenC == 'Q') {
          gs.queenCastlings[w] = true;
        }
        else if (fenC == 'k') {
          gs.kingCastlings[b] = true;
        }
        else if (fenC == 'q') {
          gs.queenCastlings[b] = true;
        }
      }

      //i += 1; // skip space
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
        gs.enPassant = ::utils::LSB(::utils::chessIndexToBitboard(pos));
      }

      //i += 1; // skip space
      index += 1;
    }

      // half move
    else if (index == 67) {
      std::string hm = "";
      for (; i < len && fen[i] != ' '; i += 1) {
        hm += fen[i];
      }

      gs.halfMoves = ::utils::stoi(hm);

      //i += 1; // skip space
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

  gameState = gs;

  // if its blacks turn then reverse the colour index
  if (!gs.isWhite) {
    for (uint8_t i = 0; i < 6; i++) {
      gameState.piecesArr[i][0] = gs.piecesArr[i][1];
      gameState.piecesArr[i][1] = gs.piecesArr[i][0];
    }
    // castling rights
    gameState.queenCastlings[0] = gs.queenCastlings[1];
    gameState.queenCastlings[1] = gs.queenCastlings[0];
    gameState.kingCastlings[0] = gs.kingCastlings[1];
    gameState.kingCastlings[1] = gs.kingCastlings[0];
  }

  // en passant
  if (gs.enPassant > 0) {
    gameState.enPassantPawn = gs.isWhite ? gs.enPassant - 8 : gs.enPassant + 8;
  }

  // fix piecess and combinedPieces
  ::utils::gameState::generateMergedBoardVersion(gameState);
} // generateFromFEN



const std::string getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second) {
  using ::david::type::bitboard_t;

  const ::std::array<char, 4> pieceTypes = {'r', 'n', 'b', 'q'};
  const std::array<char, 8> indexes = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
  const std::array<char, 8> ints = {'1', '2', '3', '4', '5', '6', '7', '8'};
  std::string EGN = "----";
  bool castling = false;

  // 0 == 1 if the colour has changed. aka new step has been made.
  bitboard_t a = first.piecess[0];
  bitboard_t b = second.piecess[1];
  bitboard_t difference = a ^ b;
  bitboard_t from;
  bitboard_t to;

  // castling
  if (::utils::nrOfActiveBits(difference) == 4) {
    castling = true;

    // if there's more than one king per colour, use the commented out solution.
    from = ::utils::LSB(first.piecesArr[5][0]); //::utils::LSB(first.piecesArr[5][0] & difference);
    to = ::utils::LSB(first.piecesArr[5][1]); //::utils::LSB(second.piecesArr[5][1] & difference);
  }
  else {
    from = LSB(a & difference);
    to = LSB(b & difference);
  }

  // from
  EGN.at(0) = indexes[from % 8];
  EGN.at(1) = ints[from / 8];

  // to
  EGN.at(2) = indexes[to % 8];
  EGN.at(3) = ints[to / 8];

  // check for promotion
  if (!castling && (first.piecesArr[0][0] & from) > 0 && (second.piecesArr[0][1] & to) == 0) {
    // find out which piece type
    for (uint8_t i = 1; i < 5; i++) {
      if ((second.piecesArr[i][1] & to) > 0) {
        // found piece: i
        EGN += pieceTypes[i];
        break;
      }
    }
  }
  else if (castling) {
    // add the castling char?
    // now it just shows that the king jumps 2 moves.
  }

  return EGN;
}
void getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second, std::string &EGN) {
  EGN = getEGN(first, second);
}

void generateMergedBoardVersion(::david::type::gameState_t& gs) {
  gs.piecess[0] = gs.piecess[1] = 0;
  // compile the new pieces
  for (uint8_t i = 0; i < 2; i++) {
    for (uint8_t j = 0; j < 6; j++) {
      gs.piecess[i] |= gs.piecesArr[j][i];
    }
  }

  // complete board merge
  gs.combinedPieces = gs.piecess[0] | gs.piecess[1];
}

/**
 * Set default board values
 * @param node gameState_t&
 */
void setDefaultChessLayout(::david::type::gameState_t &n) {

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
  n.castling = 15; //utils::stringTo8bitArray("00001111");
  n.kingCastlings = {true, true};
  n.queenCastlings = {true, true};

  n.piecess = {
      ::david::constant::defaultPiecePosition::white::PIECES,
      ::david::constant::defaultPiecePosition::black::PIECES
  };

  n.combinedPieces = ::david::constant::defaultPiecePosition::PIECES;

  n.isWhite = true;

  n.halfMoves = 0;
  n.fullMoves = 1;

  n.possibleSubMoves = 20;
}



} // ::utils::gameState
} // End of utils

