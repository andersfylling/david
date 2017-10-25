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


# Incorrect perft scores #
```
+-----------------------------------------------------------------------------+
|            r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |    6 |
+----------+------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok | fail | > perft(5): got 15833006, wants 15833292
+----------+------+------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     5 |                       15833006 |  2046219 |     6512 |        0 |   329456 |   200564 |   200564 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+

+-----------------------------------------------------------------------------+
|            r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 |
+-----------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |    6 |
+----------+------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok | fail | > perft(5): got 15833006, wants 15833292
+----------+------+------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     5 |                       15833006 |  2046219 |     6512 |        0 |   329456 |   200564 |   200564 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
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

# Contributing
Todo

# Thanks to
[Juddperft](https://github.com/jniemann66/juddperft) for finding a bug in castling and promotion.

# Old repository
https://bitbucket.org/sciencefyll/david/src
