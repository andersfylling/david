#pragma once

#include "david/david.h"

namespace david {

using ::david::constant::FLAG_SPECIAL0;
using ::david::constant::FLAG_SPECIAL1;
using ::david::constant::FLAG_PROMOTION;
using ::david::constant::FLAG_CAPTURE;
using ::david::constant::RANGE_FLAG;
using ::david::constant::RANGE_TO;
using ::david::constant::RANGE_FROM;

class Move
{
  constexpr Move(const uint_fast8_t from, const uint_fast8_t to, const uint_fast8_t flags)
    : move(((flags & 0xf)<<12) | ((from & 0x3f)<<6) | (to & 0x3f))
  {}
  constexpr Move(const uint_fast16_t m)
      : move(m)
  {}
  constexpr void operator=(Move m)
  {
    this->move = m;
  }
  constexpr void setMove(const uint_fast16_t m)
  {
    this->move = m;
  }
  
  constexpr uint_fast8_t getTo() const
  {
    return this->move & RANGE_TO;
  }
  constexpr uint_fast8_t getFrom() const
  {
    return this->move & RANGE_FROM;
  }
  constexpr uint_fast8_t getFlags() const
  {
    return this->move & RANGE_FLAG;
  }
  constexpr uint_fast16_t getMove() const
  {
    return this->move;
  }
  
  
  constexpr void setTo(const uint_fast8_t to)
  {
    this->move &= ~constant::RANGE_TO;
    this->move |= to & RANGE_TO;
  }
  constexpr void setFrom(const uint_fast8_t from)
  {
    this->move &= ~RANGE_FROM;
    this->move |= from & RANGE_FROM;
  }
  constexpr void setFlags(const uint_fast8_t flags)
  {
    this->move &= ~RANGE_FLAG;
    this->move |= flags & RANGE_FLAG;
  }
  
  constexpr bool hasPromotion() const
  {
    return (this->move & FLAG_PROMOTION) != 0;
  }
  
  constexpr bool hasCapture() const
  {
    return (this->move & FLAG_CAPTURE) != 0;
  }
  
  constexpr bool hasSpecial1() const
  {
    return (this->move & FLAG_SPECIAL1) != 0;
  }
  
  constexpr bool hasSpecial0() const
  {
    return (this->move & FLAG_SPECIAL0) != 0;
  }
  
  constexpr bool isQuietMoves() const
  {
    return (this->move & RANGE_FLAG) == 0;
  }
  
  constexpr bool isDoublePawnPush() const
  {
    return ((this->move & RANGE_FLAG) == FLAG_SPECIAL0);
  }
  
  constexpr bool isKingCastle() const
  {
    return ((this->move & RANGE_FLAG) == FLAG_SPECIAL1);
  }
  
  constexpr bool isQueenCastle() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_SPECIAL0 | FLAG_SPECIAL1));
  }
  
  constexpr bool isEPCapture() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_SPECIAL0 | FLAG_CAPTURE));
  }
  
  constexpr bool isKnightPromotion() const
  {
    return ((this->move & RANGE_FLAG) == FLAG_PROMOTION);
  }
  
  constexpr bool isBishopPromotion() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_SPECIAL0));
  }
  
  constexpr bool isRookPromotion() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_SPECIAL1));
  }
  
  constexpr bool isQueenPromotion() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_SPECIAL1 | FLAG_SPECIAL0));
  }
  
  constexpr bool isKnightPromoCapture() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_CAPTURE));
  }
  
  constexpr bool isBishopPromoCapture() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_CAPTURE | FLAG_SPECIAL0));
  }
  
  constexpr bool isRookPromoCapture() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_CAPTURE | FLAG_SPECIAL1));
  }
  
  constexpr bool isQueenPromoCapture() const
  {
    return ((this->move & RANGE_FLAG) == (FLAG_PROMOTION | FLAG_CAPTURE | FLAG_SPECIAL1 | FLAG_SPECIAL0));
  }
  
  constexpr uint_fast16_t getButterflyIndex() const
  {
    return this->move & (RANGE_FROM & RANGE_TO);
  }
  constexpr bool operator==(Move a) const
  {
    return this->move == a.getMove();
  }
  constexpr bool operator!=(Move a) const
  {
    return this->move != a.getMove();
  }

 private:
  constexpr uint_fast16_t move;
};
}