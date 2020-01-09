#ifndef STATE4_H
#define STATE4_H

#include <iostream>
#include "global4.h"

// An 'int' would be sufficient here for 4x4
// But a 'long' will be necessary for 5x5
// (assuming int=4B and long=8B)
typedef unsigned long ShortState;

enum Lines { none, onlyX, onlyO, both };

struct ChildrenList;

class State {
 public:
  char board[SIZE*SIZE];

  // Initialize an empty board
  State() {
    for(int i=0; i<SIZE*SIZE; ++i) {
      board[i] = 0;
    }
  }

  // Initialize from an existing board
  State(const State& s) {
    for(int i=0; i<SIZE*SIZE; ++i) {
      board[i] = s.board[i];
    }
  }

  // Initialize from a ShortState
  State(ShortState ss) {
    for(int i=15; i>=0; --i) {
      board[i] = ss%3;
      ss /= 3;
    }
  }

  // Flip the board (along the first diagonal)
  void flip() {
    State s(*this); // To make it simple, let's make a copy
    int i = 0;
    for(int j=0; j<SIZE; ++j) {
      for(int k=0; k<SIZE; ++k) {
        board[i] = s.board[j+SIZE*k];
        i += 1;
      }
    }
  }

  // Rotate the board (counter-clockwise)
  void rot90() {
    State s(*this); // To make it simple, let's make a copy
    int i = 0;
    for(int j=SIZE-1; j>=0; --j) {
      for(int k=0; k<SIZE; ++k) {
        board[i] = s.board[j+SIZE*k];
        i += 1;
      }
    }
  }

  void reverse() {
    for(int i=0; i<SIZE*SIZE; ++i) {
      if (board[i] != 0) {
        board[i] = 3 - board[i];
      }
    }
  }

  // Canonize the board
  void canonize() {
    State s1(*this);
    s1.rot90();
    if (s1 > *this) *this = s1;
    s1.rot90();
    if (s1 > *this) *this = s1;
    s1.rot90();
    if (s1 > *this) *this = s1;
    s1.flip();
    if (s1 > *this) *this = s1;
    s1.rot90();
    if (s1 > *this) *this = s1;
    s1.rot90();
    if (s1 > *this) *this = s1;
    s1.rot90();
    if (s1 > *this) *this = s1;
  }

  // TODO; Improve this function; kind of ugly now :)
  Lines checkLines() {
    bool lineX = false;
    for(int i=0; i<NB_LINES; ++i) {
      int j = 0;
      for(; j<SIZE; ++j) {
        if (board[LINES[i][j]] != 1) break;
      }
      if (j == SIZE) {
        lineX = true;
        break;
      }
    }

    bool lineO = false;
    for(int i=0; i<NB_LINES; ++i) {
      int j = 0;
      for(; j<SIZE; ++j) {
        if (board[LINES[i][j]] != 2) break;
      }
      if (j == SIZE) {
        lineO = true;
        break;
      }
    }

    if (lineX) {
      if (lineO) return both;
      return onlyX;
    }
    if (lineO) return onlyO;
    return none;
  }

  ChildrenList computeChildren() const;
  
  State& operator=(const State& s) {
    for(int i=0; i<SIZE*SIZE; ++i) {
      board[i] = s.board[i];
    }
    return *this;
  }
  
  bool operator<(const State& s) {
    for(int i=0; i<SIZE*SIZE; ++i) {
      if (board[i] < s.board[i]) return true;
      if (board[i] > s.board[i]) return false;
    }
    return false;
  }

  bool operator>(const State& s) {
    for(int i=0; i<SIZE*SIZE; ++i) {
      if (board[i] < s.board[i]) return false;
      if (board[i] > s.board[i]) return true;
    }
    return false;
  }

  bool operator==(const State& s) {
    for(int i=0; i<SIZE*SIZE; ++i) {
      if (board[i] != s.board[i]) return false;
    }
    return true;
  }

  // Convert to a ShortState
  ShortState convert() const {
    ShortState ss = 0;
    for(int i=0; i<SIZE*SIZE; ++i) {
      ss = 3*ss + board[i];
    }
    return ss;
  }

  // Display the board
  void print(bool reverse=false) const {
    for(int i=0; i<SIZE*SIZE; ++i) {
      switch (board[i]) {
      case 0: std::cout << "."; break;
      case 1: std::cout << (reverse?"O":"X"); break;
      case 2: std::cout << (reverse?"X":"O"); break;
      }
      if (i%SIZE == SIZE-1) std::cout << std::endl;
    }
  }
  
};

struct ChildrenList {
  int nbChildren;
  State children[MAX_CHILDREN];
};

// EXTREMELY UGLY CODE HERE
// TODO: WRITE THIS PROPERLY !!!!!!
const int INDEX[12] =    {0, 1, 2, 3, 7,11,15,14,13,12, 8, 4};
const int NB_MOVES[12] = {2, 3, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3};
const int MOVES[12][3][16] = {
  {{1, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {4, 1, 2, 3, 8, 5, 6, 7, 12, 9, 10, 11, -1, 13, 14, 15}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
  {{-1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 5, 2, 3, 4, 9, 6, 7, 8, 13, 10, 11, 12, -1, 14, 15}},
  {{-1, 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 3, -1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 6, 3, 4, 5, 10, 7, 8, 9, 14, 11, 12, 13, -1, 15}},
  {{-1, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 2, 7, 4, 5, 6, 11, 8, 9, 10, 15, 12, 13, 14, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
  {{0, 1, 2, 3, -1, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 2, -1, 4, 5, 6, 3, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 2, 3, 4, 5, 6, 11, 8, 9, 10, 15, 12, 13, 14, -1}},
  {{0, 1, 2, 3, 4, 5, 6, 7, -1, 8, 9, 10, 12, 13, 14, 15}, {0, 1, 2, -1, 4, 5, 6, 3, 8, 9, 10, 7, 12, 13, 14, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 12, 13, 14, -1}},
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, 12, 13, 14}, {0, 1, 2, -1, 4, 5, 6, 3, 8, 9, 10, 7, 12, 13, 14, 11}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, 12, 13, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, -1}, {0, 1, -1, 3, 4, 5, 2, 7, 8, 9, 6, 11, 12, 13, 10, 15}},
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, 12, 14, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, -1}, {0, -1, 2, 3, 4, 1, 6, 7, 8, 5, 10, 11, 12, 9, 14, 15}},
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, -1}, {-1, 1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
  {{0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, -1, 12, 13, 14, 15}, {-1, 1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 12, 9, 10, 11, -1, 13, 14, 15}},
  {{0, 1, 2, 3, 5, 6, 7, -1, 8, 9, 10, 11, 12, 13, 14, 15}, {-1, 1, 2, 3, 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {0, 1, 2, 3, 8, 5, 6, 7, 12, 9, 10, 11, -1, 13, 14, 15}}
};


ChildrenList State::computeChildren() const {
  ChildrenList result;
  result.nbChildren = 0;

  State s(*this);
  s.reverse();
  State child;
  
  for(int i=0; i<12; ++i) {
    if (s.board[INDEX[i]] == 1) continue;
    for(int j=0; j<NB_MOVES[i]; ++j) {
      for(int k=0; k<SIZE*SIZE; ++k) {
        int m = MOVES[i][j][k];
        if (m == -1) child.board[k] = 2;
        else child.board[k] = s.board[m];
      }
      child.canonize();
      int n = 0;
      for(; n<result.nbChildren; ++n) {
        if (result.children[n] == child) break;
      }
      if (n == result.nbChildren) {
        result.children[n] = child;
        result.nbChildren++;
      }
    }
  }

  return result;
}


#endif //STATE4_H
