```
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
See https://sciencefyll.github.io/david/html/ for generated Doxygen docs.

# Results #
MoveGen accuracy:
```
+------------------------------------------------------------------------+
|               rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 |
+------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok |   ok |   ok |   ok |
+----------+------+------+------+------+------+------+
+------------------------------------------------------------------------+
|       r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - |
+------------------------------------------------------------------------+
| perft(n) |    0 |    1 |    2 |    3 |    4 |    5 |
+----------+------+------+------+------+------+------+
|   result |   ok |   ok |   ok | fail |
+----------+------+------+------+------+

Detected failure. Doing advanced perft check. Please verify results.
FEN string: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
| Depth |                          Nodes | Captures |     E.P. |  Castles |   Promos |   Checks | Checkm's |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
|     3 |                         97,731 |   17,222 |       45 |    3,128 |        0 |      992 |      992 |
+-------+--------------------------------+----------+----------+----------+----------+----------+----------+
```

This is without caching.
MoveGen speed: `~35,471,675.5moves/second`

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

# Old repository
https://bitbucket.org/sciencefyll/david/src

# Contributors
 * [Klyngen](https://github.com/klyngen)
 * [Markus Jacobsen](https://bitbucket.org/MarkusJacobsen/)

# Thanks to
[Juddperft](https://github.com/jniemann66/juddperft) for finding a bug in castling and promotion.
