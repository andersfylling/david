//
// Created by markus on 4/24/17.
//

#ifndef CHESS_ANN_VARIABLES_H
#define CHESS_ANN_VARIABLES_H


//
// Board sides, used in search
//
enum Side {WHITE, BLACK};

//
// Enums for game phase, if we want to include that
//


//
// Enum value is used to store known values, such as winningvalue, draw, mate, piece value etc.
//
enum Value {
    VALUE_ZERO      = 0,
    VALUE_DRAW      = 0,
    VALUE_KNOWN_WIN = 10000,
    VALUE_MATE = 32000,

    PawnValueMg   = 198,   PawnValueEg   = 258,
    KnightValueMg = 817,   KnightValueEg = 846,
    BishopValueMg = 836,   BishopValueEg = 857,
    RookValueMg   = 1270,  RookValueEg   = 1278,
    QueenValueMg = 2521, QueenValueEg = 2558,
};

#endif //CHESS_ANN_VARIABLES_H
