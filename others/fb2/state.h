#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <bitset>
#include "global.h"

// A state is represented by a 64-bit integer
// As defined in global.h: typedef unsigned long long State;
// 32 bits indicate positions of X tiles
// 32 bits indicate positions of O tiles
// For 4x4: for each 32bits, 16bits are useless and 16bits are useful
// For 5x5: for each 32bits,  7bits are useless and 25bits are useful

// Example: quotes are added to help reading the state (valid in C++ to use these quotes!)
// s = 0b0000000'10001'00101'00000'00101'00000'0000000'00010'00010'00000'11000'11000
// corresponds to the following state:
// X--OX
// --XOX
// -----
// OOX-X
// OO---

// Swap the X and O in the state
inline State swapPlayer(State s) {
  return (s << HALF) | (s >> HALF);
}

// Check if there is a line of X
bool hasLineX(State s) {
  for(int i=0; i<NB_LINES; ++i) {
    if ((s & LINES_X[i]) == LINES_X[i]) return true;
  }
  return false;
}

// Check if there is a line of O
bool hasLineO(State s) {
  for(int i=0; i<NB_LINES; ++i) {
    if ((s & LINES_O[i]) == LINES_O[i]) return true;
  }
  return false;
}

// Pretty printing of a State
void printState(State s) {
  std::bitset<8*sizeof(State)> b(s);
  std::cout << s << " " << b << std::endl;
  int k = SIZE*SIZE-1; // Start from the top left corner
  for(int i=0; i<SIZE; ++i) {
    for(int j=0; j<SIZE; ++j) {
      if (b[k+HALF] and b[k]) { // Only here to debug; should never be true
        std::cout << "BUG with" << s << std::endl;
        exit(0);
      }
      if (b[k+HALF]) std::cout << "X";
      else if (b[k]) std::cout << "O";
      else std::cout << "-";
      k--;
    }
    std::cout << std::endl;
  }
}

// TODO: Need to find a better name here!
// This Order is used as an intermediate encoding between
// - a state and
// - an encoded state; a triplet (nbX, nbO, index)
// TODO: Rewrite code to remove this intermediate step?
struct Order {
  unsigned int X;
  unsigned int O;
};
static_assert (sizeof(Order) == 8, "Incorrect size of Order");

// Convert a State into a pair of 32bits number
// - Position of X
// - Position of O after eliminating X position
// Note: This function id certainly not very clear
//       look at code and try to understand :)
Order computeOrder(State s) {
  Order ord;
  ord.X = s >> HALF; // Keep only the X
  ord.O = 0;
  unsigned int tmp = s; // ugly :p Is it even correct? Is it needed? or can I use directly s? TODO: Change?
  unsigned int mask = 1 << (SIZE*SIZE-1);
  while (mask) {
    if ((tmp & mask) == mask) { // means there is an O. ( == mask is probably useless, TODO: ToRemove?)
      ord.O <<= 1;
      ord.O += 1;
    } else if ((ord.X & mask) != mask) { // means there is no X and no O
      ord.O <<= 1;
    }
    mask >>= 1;
  }
  return ord;
}

State fromOrder(Order ord) {
  State s = ((State) ord.X) << HALF;
  unsigned int maskXO = 1;
  unsigned int maskO = 1;
  while (maskXO != (1 << (SIZE*SIZE))) {
    if ((ord.X & maskXO) != maskXO) {
     if ((ord.O & maskO) == maskO) { // == maskO is probably useless, kept for clarity (TODO?)
      s |= maskXO;
     }
      maskO <<= 1;
    }
    maskXO <<= 1;
  }
  return s;
}

// Function used for debugging
// Should not be used in real computations
void printOrder(State s) {
  Order ord = computeOrder(s);
  std::cout << "order: " << std::bitset<32>(ord.X) << "," << std::bitset<32>(ord.O) << std::endl;
}

// Small struct to store the list of next/previous states of a given state
// Note: Hopefully faster than using a vector/array because it removes need of dynamic allocation
//       StateList objects will be stored on the stack
struct StateList {
  int nb;
  State state[MAX_NB_STATES];
};

// Each move is encoded by a 4-uple (see global.h):
// - Location of the tile taken by the player (indicated on the X marks)
// - Location of the tile taken by the player (indicated on both X and O marks)
// - Location of added tile
// - Mask of tiles to be slided

// Compute the list of next states when taking an empty tile
// The location of tile taken should be neither X nor O
// Note: Differently from Tanaka, I decided to create two different functions
//       Not sure it is better; but it was more clear in my mind to do this
StateList computeNextStatesFromEmpty(State s) {
  StateList l;
  int k = 0;
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_LEFT[i][1]) == 0) // There is neither an O nor an X tile, which means it is empty
      l.state[k++] = (((s & MOVE_LEFT[i][3]) << 1) & MOVE_LEFT[i][3]) | ( s & ~MOVE_LEFT[i][3]) | MOVE_LEFT[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_RIGHT[i][1]) == 0)
      l.state[k++] = (((s & MOVE_RIGHT[i][3]) >> 1) & MOVE_RIGHT[i][3]) | ( s & ~MOVE_RIGHT[i][3]) | MOVE_RIGHT[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_UP[i][1]) == 0)
      l.state[k++] = (((s & MOVE_UP[i][3]) << SIZE) & MOVE_UP[i][3]) | ( s & ~MOVE_UP[i][3]) | MOVE_UP[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_DOWN[i][1]) == 0)
      l.state[k++] = (((s & MOVE_DOWN[i][3]) >> SIZE) & MOVE_DOWN[i][3]) | ( s & ~MOVE_DOWN[i][3]) | MOVE_DOWN[i][2];
  }
  l.nb = k;
  return l;
}

// Compute the list of next states when taking an X tile
// The location of tile taken should be an X
// Note: Differently from Tanaka, I decided to create two different functions
//       Not sure it is better; but it was more clear in my mind to do this
StateList computeNextStatesFromX(State s) {
  StateList l;
  int k = 0;
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    // Can probably remove the != 0... but I find it clearer like that; probably optimized by the compiler anyway
    if ((s & MOVE_LEFT[i][0]) != 0) // There is an X tile
      l.state[k++] = (((s & MOVE_LEFT[i][3]) << 1) & MOVE_LEFT[i][3]) | ( s & ~MOVE_LEFT[i][3]) | MOVE_LEFT[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_RIGHT[i][0]) != 0)
      l.state[k++] = (((s & MOVE_RIGHT[i][3]) >> 1) & MOVE_RIGHT[i][3]) | ( s & ~MOVE_RIGHT[i][3]) | MOVE_RIGHT[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_UP[i][0]) != 0)
      l.state[k++] = (((s & MOVE_UP[i][3]) << SIZE) & MOVE_UP[i][3]) | ( s & ~MOVE_UP[i][3]) | MOVE_UP[i][2];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_DOWN[i][0]) != 0)
      l.state[k++] = (((s & MOVE_DOWN[i][3]) >> SIZE) & MOVE_DOWN[i][3]) | ( s & ~MOVE_DOWN[i][3]) | MOVE_DOWN[i][2];
  }
  l.nb = k;
  return l;
}

// Compute the list of previous states when taking an X tile
// The location of tile taken should be an X
StateList computePreviousStatesFromX(State s) {
  StateList l;
  int k = 0;
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    // Can probably remove the != 0... but I find it clearer like that
    if ((s & MOVE_LEFT[i][2]) != 0) // There is an X tile
      l.state[k++] = (((s & MOVE_LEFT[i][3]) >> 1) & MOVE_LEFT[i][3]) | ( s & ~MOVE_LEFT[i][3]) | MOVE_LEFT[i][0];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_RIGHT[i][2]) != 0)
      l.state[k++] = (((s & MOVE_RIGHT[i][3]) << 1) & MOVE_RIGHT[i][3]) | ( s & ~MOVE_RIGHT[i][3]) | MOVE_RIGHT[i][0];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_UP[i][2]) != 0)
      l.state[k++] = (((s & MOVE_UP[i][3]) >> SIZE) & MOVE_UP[i][3]) | ( s & ~MOVE_UP[i][3]) | MOVE_UP[i][0];
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_DOWN[i][2]) != 0)
      l.state[k++] = (((s & MOVE_DOWN[i][3]) << SIZE) & MOVE_DOWN[i][3]) | ( s & ~MOVE_DOWN[i][3]) | MOVE_DOWN[i][0];
  }
  l.nb = k;
  return l;
};

// Compute the list of next states when taking an empty tile
// The location of tile taken should be empty
StateList computePreviousStatesFromEmpty(State s) {
  StateList l;
  int k = 0;
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    // Can probably remove the != 0... but I find it clearer like that
    if ((s & MOVE_LEFT[i][2]) != 0) // There is an X tile
      l.state[k++] = (((s & MOVE_LEFT[i][3]) >> 1) & MOVE_LEFT[i][3]) | ( s & ~MOVE_LEFT[i][3]);
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_RIGHT[i][2]) != 0)
      l.state[k++] = (((s & MOVE_RIGHT[i][3]) << 1) & MOVE_RIGHT[i][3]) | ( s & ~MOVE_RIGHT[i][3]);
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_UP[i][2]) != 0)
      l.state[k++] = (((s & MOVE_UP[i][3]) >> SIZE) & MOVE_UP[i][3]) | ( s & ~MOVE_UP[i][3]);
  }
  for(int i=0; i<NB_MOVE_PER_DIR; ++i) {
    if ((s & MOVE_DOWN[i][2]) != 0)
      l.state[k++] = (((s & MOVE_DOWN[i][3]) << SIZE) & MOVE_DOWN[i][3]) | ( s & ~MOVE_DOWN[i][3]);
  }
  l.nb = k;
  return l;
}
#endif //STATE_H
