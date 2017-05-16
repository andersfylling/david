```
┌────────────────────┐
│░█▀▄░█▀█░█░█░▀█▀░█▀▄│
│░█░█░█▀█░▀▄▀░░█░░█░█│
│░▀▀░░▀░▀░░▀░░▀▀▀░▀▀░│
└────────────────────┘
```

# David #
David is a university project that tries to implement a Artificial Neural Network to evaluate board score.
This project is written in C++14 and is built using CMake 2.6, with the compiler gcc version 6.2.

# Trello
[Trello board for project progress and todo list](https://trello.com/b/j2LpVuZV/chess-ann)


### How do I get set up? ###

* `git clone https://sciencefyll@bitbucket.org/sciencefyll/chess_ann.git && cd chess_ann && ./update.sh`

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