#include <unistd.h>
#include <string>
#include <map>
#include "david/types.h"
#include "david/utils/utils.h"

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

 * @param node
 * @param whiteMovesNext
 * @return
 * @Deprecated needs to use gameState_t not _ptr
 */
std::string generateFen(const ::david::type::gameState_t& node) {
  using ::david::type::bitboard_t;


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
  int pos = 1;
  std::string fen = "";

  for (uint8_t i = 0; i < 64; i++) {
    if (pos == 9) {
      pos = 1;
      fen += '/';
    }

    char p = ' ';
    for (uint8_t j = 0; j < 12; j++) {
      const uint64_t board = node.piecesArr[j % 6][j / 6];

      if (utils::bitAt(board, i)) {
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
      {'b', gs.piecesArr[gs.iBishops][b]},
      {'k', gs.piecesArr[gs.iKings][b]},
      {'n', gs.piecesArr[gs.iKnights][b]},
      {'p', gs.piecesArr[gs.iPawns][b]},
      {'q', gs.piecesArr[gs.iQueens][b]},
      {'r', gs.piecesArr[gs.iRooks][b]},

      {'B', gs.piecesArr[gs.iBishops][w]},
      {'K', gs.piecesArr[gs.iKings][w]},
      {'N', gs.piecesArr[gs.iKnights][w]},
      {'P', gs.piecesArr[gs.iPawns][w]},
      {'Q', gs.piecesArr[gs.iQueens][w]},
      {'R', gs.piecesArr[gs.iRooks][w]}
  };

  std::string board(64, '-');

  for (auto entry : links) {
    const char piece = entry.first;
    auto bitboard = entry.second;

    while (bitboard != 0ULL) {
      auto i = LSB(bitboard);
      flipBitOff(bitboard, i);
      board.at(board.size() - 1 - i) = piece;
    }
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
void generateFromFEN(::david::type::gameState_t& gs, const std::string &fen) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;

  int w = 0;
  int b = 1;
  if (!gs.isWhite) {
    w = 1;
    b = 0;
  }

  std::map<const char, ::david::type::bitboard_t> links = {
      {'b', gs.piecesArr[gs.iBishops][b]},
      {'k', gs.piecesArr[gs.iKings][b]},
      {'n', gs.piecesArr[gs.iKnights][b]},
      {'p', gs.piecesArr[gs.iPawns][b]},
      {'q', gs.piecesArr[gs.iQueens][b]},
      {'r', gs.piecesArr[gs.iRooks][b]},

      {'B', gs.piecesArr[gs.iBishops][w]},
      {'K', gs.piecesArr[gs.iKings][w]},
      {'N', gs.piecesArr[gs.iKnights][w]},
      {'P', gs.piecesArr[gs.iPawns][w]},
      {'Q', gs.piecesArr[gs.iQueens][w]},
      {'R', gs.piecesArr[gs.iRooks][w]}
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
      gs.isWhite = fen[i + 1] == 'w';

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

  for (int c = 0; c < 2; c++) {
    for (int i = 0; i < 6; i++) {
      gs.piecess[c] |= gs.piecesArr[i][c];
    }
  }
  gs.combinedPieces = gs.piecess[0] | gs.piecess[1];

} // generateBoardFromFen(...)



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
 * @deprecated
 * @param gs
 * @param EGN
 */
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
      for (int j = 0; j < 6; j++) {
        if (bitAt(gs.piecesArr[j][i], origin)) {
          movePiece(gs.piecesArr[j][i], origin, destination);
        }
      }

    }
  }

  gs.isWhite = !gs.isWhite;
}

/**
 *
 * @deprecated
 * @param board
 * @param orig
 * @param dest
 */
void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest) {
  flipBitOff(board, orig);
  flipBitOn(board, dest);
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

