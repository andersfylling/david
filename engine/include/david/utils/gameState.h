#pragma once

#include <unistd.h>
#include <string>
#include "david/types.h"
#include "david/utils/utils.h"

namespace utils {

//! Utilities specific to the gameState struct.
namespace gameState {

std::string generateFen(const ::david::type::gameState_t& node);



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
void print(const ::david::type::gameState_t& gs);

/**
 * Get a shared pointer of a fresh gameState based on a fen string.
 * This assumes the fen string is correct before parsing.
 *
 * Warning: this ignores, castling, passant, halfmove and fullmove.
 *
 * @param gs gameState_t&
 * @param fen const std::string&, must be correctly formatted (!)
 */
void generateFromFEN(::david::type::gameState_t& gs, const std::string &fen);


constexpr bool isHalfMove(
    const uint64_t parentActivePieces,
    const uint64_t childActivePieces,
    const uint64_t parentActivePawns,
    const uint64_t parentOtherPawns,
    const uint64_t childActivePawns,
    const uint64_t childOtherPawns
) {
  // This counter is reset after captures or pawn moves, and incremented otherwise.
  // Also moves which lose the castling rights, that is rook- and king moves from their initial squares,
  // including castling itself, increment the Halfmove Clock.
  // However, those moves are irreversible in the sense to reverse the same rights -
  // since once a castling right is lost, it is lost forever, as considered in detecting repetitions.

  // check if parent or child is overlapping, AKA. a piece has been captured.
  if ((parentActivePieces & childActivePieces) != 0ULL) {
    return false;
  }

  // check if the ---- pawns has moved
  if ((parentActivePawns | childOtherPawns) != 0ULL) {
    return false;
  }

  // check if !---- pawns has moved
  //if ((parent.WhitePawn | child.WhitePawn) != 0ULL) {
  //  return false;
  //}
  //
  // no rules of half moving were broken
  //return true;
  return (parentOtherPawns | childActivePawns) != 0ULL;
}

const std::string getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second);
void getEGN(const ::david::type::gameState_t &first, const ::david::type::gameState_t &second, std::string &EGN);
void generateMergedBoardVersion(::david::type::gameState_t& gs);

/**
 * Set default board values
 * @param node gameState_t&
 */
void setDefaultChessLayout(::david::type::gameState_t &gs);
} // ::utils::gameState
} // End of utils

