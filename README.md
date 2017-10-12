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
|     3 |                          97731 |    17222 |       45 |     3128 |        0 |      992 |      992 |
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
|     3 |                           8902 |     0.00 |
|     4 |                         197281 |     0.01 |
|     5 |                        4865609 |     0.15 |
|     6 |                      119060324 |     3.22 |
|     7 |                     3195901860 |    84.74 |
+-------+--------------------------------+----------+
```


# Trello
[Trello board for project progress and todo list](https://trello.com/b/j2LpVuZV/chess-ann) This is outdated..


### How do I get set up? ###

* `git clone https://sciencefyll@bitbucket.org/sciencefyll/david.git && cd david && ./update.sh`

* How to run tests: See https://github.com/philsquared/Catch

* Use CMake to compile

Please see develop branch for latest releases, master is way behind.
The executable "chess_ann_source" is the only one that compiles at the moment.

To use the latest version in branch:develop
`git clone https://sciencefyll@bitbucket.org/sciencefyll/chess_ann.git && cd chess_ann && git checkout develop && ./cmake.sh`
You should then find the executables in /engine/cmake/build/bin/. However note that some of the executables might not compile due to in development issues. 
The only needed executable should be the chess_ann_anntraining and chess_ann_src depending on your goal.

### Contribution guidelines ###
The style guide: https://google.github.io/styleguide/cppguide.html

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact