
#include "david/utils/neuralNet.h"
#include "david/utils/utils.h"
#include "david/MoveGen.h"

#include <string>
#include <map>

namespace utils {
namespace neuralNet {

/**
 * Issue, what if there is more pieces of one type than the normal in a chess game?
 * Issue, should be floats and not doubles.
 *
 * @param node
 * @param player
 * @return
 */
// TODO: convert to the new node setup
std::array<float, ::david::constant::nn::INPUTSIZE> convertGameStateToInputs(::david::type::gameState_t& gs) {
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

  std::array<::david::type::bitboard_t, 2> boards1 = {
      gs.piecesArr[5][b],
      gs.piecesArr[5][w]
  };
  std::array<::david::type::bitboard_t, 8> boards2 = {
      gs.piecesArr[::david::constant::index::bishop][b],
      gs.piecesArr[::david::constant::index::knight][b],
      gs.piecesArr[::david::constant::index::queen][b],
      gs.piecesArr[::david::constant::index::rook][b],
      gs.piecesArr[::david::constant::index::bishop][w],
      gs.piecesArr[::david::constant::index::queen][w],
      gs.piecesArr[::david::constant::index::knight][w],
      gs.piecesArr[::david::constant::index::rook][w]
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


} // neuralNet
} // utils