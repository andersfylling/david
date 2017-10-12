#pragma once

#ifdef DAVID_TEST

#include "david/types.h"

namespace david {

class MoveGen;


class MoveGenTest {
 private:
  const MoveGen& moveGen;

 public:
  MoveGenTest(const MoveGen& mg);

  void print() const;

  type::bitboard_t extractLegalSouthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t southArea,
      const uint8_t offset = 7
  ) const;

  type::bitboard_t extractLegalNorthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t northArea,
      const uint8_t offset = 7
  ) const;

  uint64_t generateRookAttack (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const;




};


}

#endif