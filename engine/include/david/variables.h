#pragma once

//
// Const
//
const int MAX_DEPTH = 10;


//
// Board sides, used in search
//
/*enum Side {WHITE, BLACK};*/

//
// Enums for game phase, if we want to include that
//


//
// Enum value is used to store known values, such as winning value, draw, mate, piece value etc.
//
// Mainly taken from stockfish, may need some tweaking
//
enum Values {
    VALUE_ZERO      = 0,
    VALUE_DRAW      = 0,
    VALUE_KNOWN_WIN = 10000,
    VALUE_MATE = 32000,
    VALUE_INFINITE  = 32001,

    PawnValueMg   = 198,   PawnValueEg   = 258,
    KnightValueMg = 817,   KnightValueEg = 846,
    BishopValueMg = 836,   BishopValueEg = 857,
    RookValueMg   = 1270,  RookValueEg   = 1278,
    QueenValueMg = 2521, QueenValueEg = 2558,
};

/*enum PieceType {
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
};*/

enum Piece {
    NO_PIECE,
    W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
};


//
// variables.h must have it's own return functions, to return spesific types.
//