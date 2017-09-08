#include "david/MoveGenTest.h"
#include "david/MoveGen.h"
#include "david/utils/utils.h"

namespace david {

MoveGenTest::MoveGenTest(const MoveGen &mg)
    : moveGen(mg)
{}


void MoveGenTest::print() const {
  ::utils::printGameState(moveGen.state);
}


type::bitboard_t MoveGenTest::generateRookAttack(const uint8_t index,
                                                 const type::gameState_t &gs,
                                                 const bool hostilePath) const
{
  return this->moveGen.generateRookAttack(index, gs, hostilePath);
}

type::bitboard_t MoveGenTest::extractLegalSouthPath(const type::bitboard_t psuedoPath,
                                                    const type::bitboard_t friendlyBlockers,
                                                    const type::bitboard_t hostiles,
                                                    const type::bitboard_t southArea,
                                                    const uint8_t offset) const {
  return this->moveGen.extractLegalSouthPath(psuedoPath, friendlyBlockers, hostiles, southArea, offset);
}
type::bitboard_t MoveGenTest::extractLegalNorthPath(const type::bitboard_t psuedoPath,
                                                    const type::bitboard_t friendlyBlockers,
                                                    const type::bitboard_t hostiles,
                                                    const type::bitboard_t northArea,
                                                    const uint8_t offset) const {
  return this->moveGen.extractLegalNorthPath(psuedoPath, friendlyBlockers, hostiles, northArea, offset);
}

}