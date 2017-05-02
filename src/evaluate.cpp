//
// Created by markus on 4/24/17.
//

#include "../include/evaluate.h"



//Need access to some bitboard


int evaluate(Enviornment * node) {
    int score = 0;
    //
    // Evaluate material score
    //
    score += materialEvaluation(node);


    //
    // Evaluate mobility
    //

    //
    // To be continued
    //


}

Values materialEvaluation(Enviornment * node) {
    Values materialScore;
    materialScore = VALUE_ZERO;
    //
    // Basic piece value
    // Other factors that affect material evaluation might be:
    //    *Bonus for the bishop pair (bishops complement each other, controlling squares of different color)
    //    *Penalty for the rook pair (Larry Kaufman called it "principle of redundancy")
    //    *Penalty for the knight pair (as two knights are less successful against the rook than any other pair of minor pieces)
    //    *decreasing the value of the rook pawns and increasing the value of the central pawns (though this can be done in the piece-square tables as well)
    //    *Trade down bonus that encourages the winning side to trade pieces but no pawns
    //    *Penalty for having no pawns, as it makes it more difficult to win the endgame
    //    *Bad trade penalty as proposed by Robert Hyatt, that is penalizing the material imbalances that are disadvantageous like having three pawns for a piece or a rook for two minors.
    //    *Elephantiasis effect as suggested by Harm Geert Muller (meaning that stronger pieces lose part of their value in presence of weaker pieces)
    //


    //
    // Basic piece value
    //
    materialScore += PawnValueEg * (node->numberOfPieces(WP) - node->numberOfPieces(BP))
    + BishopValueEg * (node->numberOfPieces(WR) - node->numberOfPieces(BR))
    + KnightValueEg * (node->numberOfPieces(WN) - node->numberOfPieces(BN))
    + RookValueEg * (node->numberOfPieces(WC) - node->numberOfPieces(BC))
    + QueenValueEg * (node->numberOfPieces(WQ) - node->numberOfPieces(BQ));


    return materialScore;
}

Values mobilityEvaluation(){
   // mobilityScore = mobilityWt * (wMobility-bMobility)
}





