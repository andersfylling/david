#pragma once

// a class for a game state
namespace david {
namespace board {
struct state {
  std::array<uint_fast64_t, 12> bitboards{0ull};
};

constexpr state defaultState{{
    // black == 0
    // white == 1
    
    // black
    ::david::constant::defaultPiecePosition::black::ROOK,
    ::david::constant::defaultPiecePosition::black::KNIGHT,
    ::david::constant::defaultPiecePosition::black::BISHOP,
    ::david::constant::defaultPiecePosition::black::QUEEN,
    ::david::constant::defaultPiecePosition::black::KING,
    ::david::constant::defaultPiecePosition::black::PAWN,
    
    // white
    ::david::constant::defaultPiecePosition::white::ROOK,
    ::david::constant::defaultPiecePosition::white::KNIGHT,
    ::david::constant::defaultPiecePosition::white::BISHOP,
    ::david::constant::defaultPiecePosition::white::QUEEN,
    ::david::constant::defaultPiecePosition::white::KING,
    ::david::constant::defaultPiecePosition::white::PAWN,
    
    // pieces
    ::david::constant::defaultPiecePosition::black::PIECES,
    ::david::constant::defaultPiecePosition::white::PIECES,
    
    // complete board merged
    ::david::constant::defaultPiecePosition::PIECES
}};
}
class Board
{
 public:
  constexpr Board()
      : state(board::defaultState)
  {}
  constexpr Board(board::state s)
      : state(s)
  {}
  
  constexpr void setState(board::state s)
  {
    this->state = s;
  }
  
  constexpr void makeMove(const uint_fast16_t move) {}
  constexpr void unmakeMove(const uint_fast16_t move) {}
  
  constexpr uint_fast64_t getBitboard(const uint_fast8_t index) const
  {
    return state.bitboards[index];
  }
  constexpr uint_fast64_t getBitboard(const uint_fast8_t index, const uint_fast8_t colour) const
  {
    return state.bitboards[index * colour + index];
  }
 
 private:
  board::state state;
};
}