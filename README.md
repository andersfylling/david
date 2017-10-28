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


# Perft for debugging (perft perft) #
```
Checking perft(5)[                  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ] => OK
Checking perft(5)[          r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ] => OK
Checking perft(6)[                                     8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ] => OK
Checking perft(6)[          r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1 ] => OK
Checking perft(6)[          r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ] => OK
Checking perft(5)[                 rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ] => OK
Checking perft(5)[  r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ] => OK
Checking perft(6)[                                         3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1 ] => OK
Checking perft(6)[                                         2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1 ] => OK
Checking perft(6)[                                            4k3/1P6/8/8/8/8/K7/8 w - - 0 1 ] => OK
Checking perft(6)[                                             8/P1k5/K7/8/8/8/8/8 w - - 0 1 ] => OK
Checking perft(5)[                                       8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1 ] => OK
Checking perft(5)[                                             K1k5/8/P7/8/8/8/8/8 w - - 0 1 ] => OK
Checking perft(7)[                                            8/k1P5/8/1K6/8/8/8/8 w - - 0 1 ] => OK
Checking perft(4)[                                         8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1 ] => OK
Checking perft(3)[                                                r3k3/3N4/8/8/8/8/8/4K3 b q ] => OK
```

# Perft speed #
This is without caching.
MoveGen speed: `~35,471,675.5 legal moves/second`.
Note that a move here is a complete game state generated and
some are removed at runtime as they are evaluated as illegal.
This means there isn't a correct nodes per second counter yet.

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
