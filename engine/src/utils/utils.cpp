#include "david/utils/utils.h"
#include "fann/floatfann.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "david/types.h"
#include "david/MoveGen.h"
#include <david/ChessEngine.h>

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
std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(const ::david::type::gameState_t& gs) {
   std::array<float, ::david::constant::nn::INPUTSIZE> boardInfo{};

   uint8_t w = 1;
   uint8_t b = 0;
   int offset = 0;

   if (gs.isWhite) {
     w = 0;
     b = 1;
   }

   ::david::MoveGen moveGen{gs};

   // empty game state that holds all the attacks
   ::david::type::gameState_t attacks = moveGen.generateAttacks();


   // which colour is the active / currently playing
   boardInfo[offset++] = gs.isWhite ? 1 : -1;

   //
   // Black pieces, then white pieces
   //
   for (uint8_t c = b, ii = 0; ii < 2; ii++, c = static_cast<uint8_t>((c + 1) % 2) ) {
     uint8_t co = static_cast<uint8_t>((c + 1) % 2);
     // Does every black piece type exist?
     auto len = gs.piecesArr.size();
     for (uint8_t i = 0; i < len; i++) {
       boardInfo[i + offset] = static_cast<float>(gs.piecesArr[i][c] > 0 ? 1.0 : -1.0);
     }
     offset += len;

     // nr of each black piece type
     for (uint8_t i = 0; i < len; i++) {
       boardInfo[i + len] = static_cast<float>(gs.piecesArr[i][c] / 100.0);
     }
     offset += len;

     // How many white pieces can each black chess type attack?
     for (uint8_t i = 0; i < len; i++) {
       boardInfo[i + len] = static_cast<float>(utils::nrOfActiveBits(attacks.piecesArr[i][c] & gs.piecess[co])  / 100.0);
     }
     offset += len;

     // Are any of the piece types safe?
     for (uint8_t i = 0; i < len; i++) {
       boardInfo[i + len] = static_cast<float>(utils::nrOfActiveBits(attacks.piecesArr[i][c] & gs.piecess[co])  / 100.0);
     }
     offset += len;
   }

   // how many pieces are there?
   boardInfo[offset++] = static_cast<float>(::utils::nrOfActiveBits(gs.piecess[b]) / 100.0);
   boardInfo[offset++] = static_cast<float>(::utils::nrOfActiveBits(gs.piecess[w]) / 100.0);
   boardInfo[offset++] = static_cast<float>(::utils::nrOfActiveBits(gs.combinedPieces) / 100.0);

   // how many pieces can colour attack?
   boardInfo[offset++] = static_cast<float>(::utils::nrOfActiveBits(attacks.piecess[b] & gs.piecess[w]) / 100.0);
   boardInfo[offset++] = static_cast<float>(::utils::nrOfActiveBits(attacks.piecess[w] & gs.piecess[b]) / 100.0);

   // castling
   boardInfo[offset++] = static_cast<float>(gs.queenCastlings[b]  ? 1.0 : -1.0);
   boardInfo[offset++] = static_cast<float>(gs.kingCastlings[b]   ? 1.0 : -1.0);
   boardInfo[offset++] = static_cast<float>(gs.queenCastlings[w]  ? 1.0 : -1.0);
   boardInfo[offset++] = static_cast<float>(gs.kingCastlings[w]   ? 1.0 : -1.0);

   // game progress
   boardInfo[offset++] = static_cast<float>(100 - gs.halfMoves / 100.0);
   boardInfo[offset++] = static_cast<float>(50 - gs.fullMoves  / 100.0);

   // how many possible moves from this game state
   boardInfo[offset++] = static_cast<float>(moveGen.nrOfPossibleMoves() / 100.0); // is this smart??

  // old version

   unsigned int index = 61;

   std::array<::david::type::bitboard_t, 2> boards1 = {
       gs.piecesArr[5][b],
       gs.piecesArr[5][w]
   };
   std::array<::david::type::bitboard_t, 8> boards2 = {
       gs.piecesArr[gs.iBishops][b],
       gs.piecesArr[gs.iKnights][b],
       gs.piecesArr[gs.iQueens][b],
       gs.piecesArr[gs.iRooks][b],
       gs.piecesArr[gs.iBishops][w],
       gs.piecesArr[gs.iQueens][w],
       gs.piecesArr[gs.iKnights][w],
       gs.piecesArr[gs.iRooks][w]
   };
   std::array<::david::type::bitboard_t, 2> boards8 = {
       gs.piecesArr[0][b],
       gs.piecesArr[0][w]
   };

   // generate inputs
   for (auto b : boards1) {
     //auto ba = std::bitset<64>(b);
     //double arr[1] = {-1.0};
     auto prog = 0;
     do {
       auto i = LSB(b);
       flipBitOff(b, i);

       boardInfo[offset++] = (i == 0 ? 0.0f : i / 100.0f);
       prog += 1;
     } while (b != 0ULL && prog < 1);

     // fill in missing pieces
     for (; prog < 1; prog++) {
       boardInfo[offset++] = (-1.0f);
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

       boardInfo[offset++] = (i == 0 ? 0.0f : i / 100.0f);
       prog += 1;
     } while (b != 0ULL && prog < 2);

     // fill in missing pieces
     for (; prog < 2; prog++) {
       boardInfo[offset++] = (-1.0f);
     }
   }
   for (auto b : boards8) {
     //auto ba = std::bitset<64>(b);
     //double arr[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
     auto prog = 0;
     do {
       auto i = LSB(b);
       flipBitOff(b, i);

       boardInfo[offset++] = (i == 0 ? 0.0f : i / 100.0f);
       prog += 1;
     } while (b != 0ULL && prog < 8);

     // fill in missing pieces
     for (; prog < 8; prog++) {
       boardInfo[offset++] = (-1.0f);
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


  // add data to old fields
  ::utils::convert::legacyGameStateUpdate(n);
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
void printGameState(const ::david::type::gameState_t &gs) {

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

  std::string board(64, '-');

  for (auto entry : links) {
    const char piece = entry.first;
    auto bitboard = entry.second;

    while (bitboard != 0ULL) {
      auto i = LSB(bitboard);
      flipBitOff(bitboard, i);
      board.at(63 - i) = piece;
    }
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
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param gs gameState_t&
 * @param fen const std::string&, must be correctly formatted (!)
 */
// TODO the colour can swap the chess pieces 0.o
void generateBoardFromFen(::david::type::gameState_t& gs, const std::string fen) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;

  // reset
  gs.enPassant = 0;
  gs.enPassantPawn = 0;
  gs.passant = false;
  gs.kingCastlings = {0, 0};
  gs.queenCastlings = {0, 0};

  int w = 0;
  int b = 1;
  if (!gs.isWhite) {
    w = 1;
    b = 0;
  }

  // if its a small letter, use black. otherwise use white.
  std::map<const char, ::david::type::bitboard_t&> links = {
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
        if (fen[i] == 'K') {
          gs.kingCastlings[w] = true;
        }
        else if (fen[i] == 'Q') {
          gs.queenCastlings[w] = true;
        }
        else if (fen[i] == 'k') {
          gs.kingCastlings[b] = true;
        }
        else if (fen[i] == 'q') {
          gs.queenCastlings[b] = true;
        }
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
    gs.piecess[c] = 0; // reset
    for (int i = 0; i < 6; i++) {
      gs.piecess[c] |= gs.piecesArr[i][c]; // save
    }
  }
  gs.combinedPieces = gs.piecess[0] | gs.piecess[1];
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

void movePiece(::david::type::bitboard_t& board, uint8_t orig, uint8_t dest) {
  flipBitOff(board, orig);
  flipBitOn(board, dest);
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
  ::utils::setDefaultChessLayout(gs);
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

bool perft_debug(const int limit, const uint8_t startDepth) {
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
bool perft_debug_advanced(const ::david::type::gameState_t& gs, const uint8_t start) {
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
              "Castling",
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
  for (uint8_t depth = start; depth <= 6; depth++) {
    // run perft
    std::array<int, 6> perftResults = {}; // zero initialization
    auto moveGenPerft = ::utils::perft_debug(depth, gs, perftResults);

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

const std::string getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second) {
  using ::david::bitboard::COLOR::WHITE;
  using ::david::bitboard::COLOR::BLACK;
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


#ifdef DAVID_DEVELOPMENT
  assert(difference != 0ULL);
  assert(from < 64);
  assert(to < 64);
#endif

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
    // now it just shows that the king jumps 2 or 3 moves.
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

} // End of utils

