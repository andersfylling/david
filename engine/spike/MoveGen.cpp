#include "david/MoveGen.h"

#include <david/bitboard.h>

namespace david {

/**
 * Constructor
 * @param gs non-mutable gameState instance
 */
MoveGen::MoveGen(const type::gameState_t& gs)
    : state(gs)
    , index_gameStates(0)
{}

//TODO: ((friendly ^ pieces) & hostileAttacks) > 0 => the piece cannot be moved away from the attack path. check.

void MoveGen::runAllMoveGenerators() {
  // generate all the bitboards
  this->generatePawnMoves();
  this->generateRookMoves();
  this->generateKnightMoves();
  this->generateBishopMoves();
  this->generateQueenMoves();
  this->generateKingMoves();

  //generate gameStates based on moves
  const auto nrOfPieceTypes = this->moves.size();
  for (unsigned long pieceType = 0; pieceType < nrOfPieceTypes; pieceType++) {

    // for every pieceType
    for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {
      type::gameState_t gs = this->state;

      // swap the active piece side
      gs.directions[0] *= -1;
      gs.directions[1] *= -1;
      gs.isWhite = !this->state.isWhite;

      // reverse the pieces to respect the active player change
      const auto nrOfPieces = this->state.piecesArr.size();
      for (uint8_t i = 0; i < nrOfPieces; i++) {
        gs.piecesArr[i][0] = this->state.piecesArr[i][1];
        gs.piecesArr[i][1] = this->state.piecesArr[i][0];
      }
      gs.piecesArr[pieceType][1] = this->moves[pieceType][board];       // the colour that just moved. now opponent.

      // Check for capture, and destroy captured piece!
      if ((this->moves[pieceType][board] & this->state.piecess[1]) > 0) {
        const uint8_t attackedPiecePosition = ::utils::LSB(this->moves[pieceType][board]);

        for (auto& bbArr : gs.piecesArr) {
          // since gs has the opposite indexes, use 0 in stead of 1.
          if ((bbArr[0] & attackedPiecePosition) > 0) {
            ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
            break;
          }
        }
      }

      // compile the new pieces
      gs.piecess[0] = gs.piecess[1] = 0; // reset
      for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 6; j++) {
          gs.piecess[i] |= gs.piecesArr[j][i];
        }
      }

      // complete board merge
      gs.combinedPieces = gs.piecess[0] | gs.piecess[1];

      // generate attack paths of enemy pieces and check for check / check mate
      // TODO: check check
      type::bitboard_t attacks = 0ULL;
      //attacks |= this->generatePawnAttack();

      if ((attacks & gs.piecesArr[gs.iKings][0]) > 0) {
        continue;
      }


      // store the completed gamestate
      // TODO: halfstep, fullstep, etc.
      this->gameStates[this->index_gameStates++] = gs;
    }
  }


}

/**
 * Generate all legal moves for pawns
 *
 * TODO: Implement attacking
 * TODO-2: Implement en passant
 */
void MoveGen::generatePawnMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iPawns][0];

  // first lets just move every pawn up/down once
  uint8_t i = ::utils::LSB(que);
  do {
    uint8_t positionLimit = 1; // should the pawn only be move up/down one step?

    const auto resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iPawns][0], i);

    // check if its a start position.
    uint8_t row = i / 8;
    if ((this->state.isWhite && row == 1) || (!this->state.isWhite && row == 6)) {
      positionLimit += 1;
    }

    for (unsigned long j = 1; j <= positionLimit; j++) {
      unsigned long to = i + (this->state.directions[0] * 8 * j); // * j to move two pos up/down
      // its important that this only functions when the start position is at row 2 for white
      // or row 7 for black

      if (!::utils::bitAt(pieces, to)) {
        // store results here
        type::bitboard_t result = resultCache;
        ::utils::flipBitOn(result, to);

        // store the new board layout
        this->moves[this->state.iPawns][index++] = result;
      }
    }

    // remove this piece from the que
    ::utils::flipBitOff(que, i);

    // go to the next piece
    i = ::utils::LSB(que);
  } while(i != 0);

  this->index_moves[this->state.iPawns] = index;
}

/**
 * Generate legal moves for rook
 */
void MoveGen::generateRookMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iRooks][0];

  // directions a rook can move
  std::array<int8_t, 4> directions = {/*up*/ +8, /*right*/ -1, /*down*/ -8, /*left*/ +1};

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {

    auto resultCache = this->state.piecesArr[this->state.iRooks][0];
    ::utils::flipBitOff(resultCache, i);

    // the different board limits related to directions
    uint8_t row = i / 8;
    uint8_t col = i % 8;
    std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

    for (uint8_t direction = 0; direction < 4; direction++) {
      for (unsigned long j = 1; j <= limitations[direction]; j++) {
        unsigned long to = i + (directions[direction] * j);
        // its important that this only functions when the start position is at row 2 for white
        // or row 7 for black

        if (::utils::bitAt(friendly, to)) {
          break;
        }

        // check if it hit a hostile piece
        // cause if so then this is the last check in this direction
        if (::utils::bitAt(hostiles, to)) {
          j = 64; // simple way to cancel the loop
          // TODO: set max loop length to a const.
          // TODO: should this have a way to say if an attack has happened?
        }

        // store results here
        type::bitboard_t result = this->state.piecesArr[this->state.iRooks][0];
        ::utils::flipBitOn(result, to);

        // store the new board layout
        this->moves[this->state.iRooks][index++] = result;
      }
    }

    // remove this piece from the que
    ::utils::flipBitOff(que, i);

    // go to the next piece
    i = ::utils::LSB(que);
  } while(i != 0);

  this->index_moves[this->state.iRooks] = index;
}

/**
 * Generate legal moves for knights
 */
void MoveGen::generateKnightMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iKnights][0];

  // directions a rook can move
  std::array<int8_t, 4> directions = {/*up*/ +8, /*right*/ -1, /*down*/ -8, /*left*/ +1};

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {

    type::bitboard_t resultCache = ::utils::flipBitOffCopy(this->state.piecesArr[this->state.iKnights][0], i);

    // the different board limits related to directions
    uint8_t row = i / 8;
    uint8_t col = i % 8;
    std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

    for (const auto direction : this->state.directions) {
      // up / down
      for (uint8_t y = 1; y < 3; y++) {
        // make sure the knight doesnt go too high or too "low"
        const auto heightPush = row + (direction * y);
        if (heightPush > 7 || heightPush < 0) {
          continue;
        }

        // how far to the side should the knight move, based on its desired height move
        uint8_t colOffset = 3 - y; // if y == 1, then move 2 pos to the side, y == 2 move 1 pos to the side

        // make sure it doesn't go too far to the sides
        const auto sidePush = col + (direction * colOffset);
        if (sidePush > 7 || sidePush < 0) {
          continue;
        }

        // left && right
        for (uint8_t x = 0; x < 2; x++) {
          uint8_t to = i + (direction * colOffset) + (direction * 8 * y);

          // Make sure position can be used
          if (::utils::bitAt(friendly, to)) {
            continue;
          }

          // store results here
          type::bitboard_t result = resultCache;
          ::utils::flipBitOn(result, to);

          // store the new board layout
          this->moves[this->state.iKnights][index++] = result;
        }
      }
    }

    // remove this piece from the que
    ::utils::flipBitOff(que, i);

    // go to the next piece
    i = ::utils::LSB(que);
  } while(i != 0);

  this->index_moves[this->state.iKnights] = index;
}

/**
 * Generate legal moves for bishop
 * TODO: doesnt affect perft at depth 3...
 *
 */
void MoveGen::generateBishopMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  unsigned long index = 0;

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iBishops][0];

  // directions a rook can move
  std::array<int8_t, 4> directions = {
      /*up.right*/    +8 - 1,
      /*right.down*/  -1 - 8,
      /*down.left*/   -8 + 1,
      /*left.up*/     +1 + 8
  };

  // For every rook, see how far you can move it until it hits a piece
  // If it hits a friendly, don't add that position
  // If it hits an enemy then the position can be added
  uint8_t i = ::utils::LSB(que);
  do {
    type::bitboard_t resultCache = this->state.piecesArr[this->state.iBishops][0];
    ::utils::flipBitOff(resultCache, i);

    // the different board limits related to directions
    uint8_t row = i / 8;
    uint8_t col = i % 8;
    std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

    for (uint8_t direction = 0; direction < 4; direction++) {
      for (unsigned long j = 1; j <= limitations[direction] && j <= limitations[(direction + 1) % 4]; j++) {
        unsigned long to = i + (directions[direction] * j);
        // its important that this only functions when the start position is at row 2 for white
        // or row 7 for black

        if (::utils::bitAt(friendly, to)) {
          break;
        }

        // check if it hit a hostile piece
        // cause if so then this is the last check in this direction
        if (::utils::bitAt(hostiles, to)) {
          j = 64; // simple way to cancel the loop
          // TODO: set max loop length to a const.
          // TODO: should this have a way to say if an attack has happened?
        }

        // store the new board layout
        this->moves[this->state.iBishops][index++] = ::utils::flipBitOnCopy(resultCache, to);

      }
    }

    // remove this piece from the que
    ::utils::flipBitOff(que, i);

    // go to the next piece
    i = ::utils::LSB(que);
  } while(i != 0);

  this->index_moves[this->state.iBishops] = index;
}


/**
 * Generate legal moves for queen
 */
void MoveGen::generateQueenMoves() {
  const auto friendly = this->state.piecess[0];
  const auto hostiles = this->state.piecess[1];
  const auto pieces = this->state.combinedPieces;
  unsigned long index = 0;

  // stolen from rook
  {
    // stack over pieces to handle
    auto que = this->state.piecesArr[this->state.iQueens][0];

    // directions a rook can move
    std::array<int8_t, 4> directions = {/*up*/ +8, /*right*/ -1, /*down*/ -8, /*left*/ +1};

    // For every rook, see how far you can move it until it hits a piece
    // If it hits a friendly, don't add that position
    // If it hits an enemy then the position can be added
    uint8_t i = ::utils::LSB(que);
    do {
      type::bitboard_t resultCache = this->state.piecesArr[this->state.iQueens][0];
      ::utils::flipBitOff(resultCache, i);

      // the different board limits related to directions
      uint8_t row = i / 8;
      uint8_t col = i % 8;
      std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

      for (uint8_t direction = 0; direction < 4; direction++) {
        for (unsigned long j = 1; j <= limitations[direction]; j++) {
          unsigned long to = i + (directions[direction] * j);
          // its important that this only functions when the start position is at row 2 for white
          // or row 7 for black

          if (::utils::bitAt(friendly, to)) {
            break;
          }

          // check if it hit a hostile piece
          // cause if so then this is the last check in this direction
          if (::utils::bitAt(hostiles, to)) {
            j = 64; // simple way to cancel the loop
            // TODO: set max loop length to a const.
            // TODO: should this have a way to say if an attack has happened?
          }

          // store the new board layout
          this->moves[this->state.iQueens][index++] = ::utils::flipBitOnCopy(resultCache, to);

        }
      }

      // remove this piece from the que
      ::utils::flipBitOff(que, i);

      // go to the next piece
      i = ::utils::LSB(que);
    } while (i != 0);
  }

  // stolen from bishop
  {
    // stack over pieces to handle
    auto que = this->state.piecesArr[this->state.iQueens][0];

    // directions a rook can move
    std::array<int8_t, 4> directions = {
        /*up.right*/    +8 - 1,
        /*right.down*/  -1 - 8,
        /*down.left*/   -8 + 1,
        /*left.up*/     +1 + 8
    };

    // For every rook, see how far you can move it until it hits a piece
    // If it hits a friendly, don't add that position
    // If it hits an enemy then the position can be added
    uint8_t i = ::utils::LSB(que);
    do {
      type::bitboard_t resultCache = this->state.piecesArr[this->state.iQueens][0];
      ::utils::flipBitOff(resultCache, i);

      // the different board limits related to directions
      uint8_t row = i / 8;
      uint8_t col = i % 8;
      std::array<uint8_t, 4> limitations = {/*up*/7 - row, /*right*/col, /*down*/row, /*left*/7 - col};

      for (uint8_t direction = 0; direction < 4; direction++) {
        for (unsigned long j = 1; j <= limitations[direction] && j <= limitations[(direction + 1) % 4]; j++) {
          unsigned long to = i + (directions[direction] * j);
          // its important that this only functions when the start position is at row 2 for white
          // or row 7 for black

          if (::utils::bitAt(friendly, to)) {
            break;
          }

          // check if it hit a hostile piece
          // cause if so then this is the last check in this direction
          if (::utils::bitAt(hostiles, to)) {
            j = 64; // simple way to cancel the loop
            // TODO: set max loop length to a const.
            // TODO: should this have a way to say if an attack has happened?
          }

          // store the new board layout
          this->moves[this->state.iQueens][index++] = ::utils::flipBitOnCopy(resultCache, to);

        }
      }

      // remove this piece from the que
      ::utils::flipBitOff(que, i);

      // go to the next piece
      i = ::utils::LSB(que);
    } while(i != 0);
  }

  this->index_moves[this->state.iQueens] = index;
}


/**
 * Generate legal moves for king
 */
void MoveGen::generateKingMoves() {
  unsigned long index = 0;
  // TODO: castling
  const auto friendly = this->state.piecess[0];

  // stack over pieces to handle
  auto que = this->state.piecesArr[this->state.iKings][0];

  // <---
  uint8_t maskRowOffset = 3; // offset from row 0
  const std::array<type::bitboard_t, 8> masks = {
      12918652928,
      30149115904,
      60298231808,
      120596463616,
      241192927232,
      482385854464,
      964771708928,
      825720045568
  };

  uint8_t i = ::utils::LSB(que);

  uint8_t row = i / 8;
  type::bitboard_t mask = masks[i % 8];
  type::bitboard_t rMask = 0ULL;

  if (row > maskRowOffset) {
    rMask = mask << (8 * (row - maskRowOffset));
  }
  else {
    rMask = mask >> (8 * (maskRowOffset - row));
  }

  // legal moves, not checked if in check mate or smth
  que = (rMask & friendly) ^ rMask;

  i = ::utils::LSB(que);
  while (i != 0) {
    // store results here
    type::bitboard_t result = ::utils::indexToBitboard(i);

    // store the new board layout
    this->moves[this->state.iKnights][index++] = result;

    // remove this piece from the que
    que ^= result;

    // go to the next piece
    i = ::utils::LSB(que);
  }

  this->index_moves[this->state.iKings] = index;
}


}