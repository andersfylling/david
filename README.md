```markdown
┌────────────────────┐
│░█▀▄░█▀█░█░█░▀█▀░█▀▄│
│░█░█░█▀█░▀▄▀░░█░░█░█│
│░▀▀░░▀░▀░░▀░░▀▀▀░▀▀░│
└────────────────────┘
```

# David #
David is a university project that tries to implement a Artificial Neural Network to evaluate board score.
This project is written in C++17 and is built using CMake 3.8, with the compiler gcc7+.

# Docs #
See https://andersfylling.github.io/david/html/ for generated Doxygen docs.

# TO-DO #
## Move Generation (Legal Moves) ##
 - [ ] X-ray for check
 - [ ] Encoding Moves
 - [ ] No errors on all chessprogramming positions (1-6)
 - [x] Generate gamestates for each piece type
 - [x] Be able to add this to a stack / pre allocated array.
 - [x] Perft test suite
 - [x] Perft
 - [ ] 100% documentation
 - [ ] Staged move generation
 - [x] Precalculated psuedo moves for knight, diagonals, horizontal and vertical directions.
 - [ ] Magic bitboards. (Maybe ?)
 - [ ] Transposition tables
 - [ ] Threading (is it worth it?)
 
 ## Search (Selective) ##
  - [ ] Replace recursion with iteration,
  - [ ] Improve sorting algorithm.
  - [ ] Quiescence Search
  - [x] Iterative deepening
  - [x] Negamax
  - [x] Alpha-Beta pruning
  - [x] Aspiration Windows (Needs revision)
  - [ ] Threaded search (Thread pool to seperate branches for quicker searching)


# Results #
MoveGen accuracy:
```
+-----------------------------------------------------------------------------+
|                    rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok |   ok |
+----------+------+------+------+------+------+------+
+-----------------------------------------------------------------------------+
|            r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok | fail | > perft(4): got 4085750, wants 4085603
+----------+------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     4 |                        4085750 |   757163 |     1929 |   128096 |    15172 |    25523 |    25523 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+

+-----------------------------------------------------------------------------+
|                                       8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |    6 |
+----------+------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok |   ok |   ok |
+----------+------+------+------+------+------+------+------+
+-----------------------------------------------------------------------------+
|            r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |    6 |
+----------+------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok | fail | > perft(5): got 15832226, wants 15833292
+----------+------+------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     5 |                       15832226 |  2046061 |     6512 |        0 |   328676 |   200174 |   200174 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+

+-----------------------------------------------------------------------------+
|            r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |    6 |
+----------+------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok |   ok | fail | > perft(6): got 706082478, wants 706045033
+----------+------+------+------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     6 |                      706082478 | 210367872 |      212 | 10910767 | 81102984 | 26974006 | 26974006 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+

+-----------------------------------------------------------------------------+
|                   rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok | fail | > perft(3): got 62402, wants 62379
+----------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     3 |                          62402 |     8517 |        0 |     1107 |     5068 |     1201 |     1201 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+

+-----------------------------------------------------------------------------+
|    r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok |   ok |
+----------+------+------+------+------+------+------+
```

This is without caching.
MoveGen speed: `~35,471,675.5moves/second`. Note that a move here is a complete game state generated.

Perft(0..7) results:
```
+-------+--------------------------------+----------+
| Depth |                          Nodes |     Time |
+-------+--------------------------------+----------+
|     0 |                              1 |     0.00 |
|     1 |                             20 |     0.00 |
|     2 |                            400 |     0.00 |
|     3 |                          8,902 |     0.00 |
|     4 |                        197,281 |     0.01 |
|     5 |                      4,865,609 |     0.15 |
|     6 |                    119,060,324 |     3.22 |
|     7 |                  3,195,901,860 |    84.74 |
+-------+--------------------------------+----------+
```

# Contributors
 * [andersfylling](https://github.com/andersfylling)
 * [Klyngen](https://github.com/klyngen)
 * [MarkusAJacobsen](https://github.com/MarkusAJacobsen)

# Thanks to
[Juddperft](https://github.com/jniemann66/juddperft) for finding a bug in castling and promotion.

# Old repository
https://bitbucket.org/sciencefyll/david/src
