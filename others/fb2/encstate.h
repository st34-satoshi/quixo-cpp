#ifndef ENCSTATE_H
#define ENCSTATE_H

#include <iostream>
#include <array>
#include <utility> // for std::pair
#include <vector>

#include "global.h"
#include "state.h"

// Note: I do not expect this file to be understandable :)
// TODO: NEED TO USE BETTER NAMES HERE!
//       NOT VERY CLEAR FILE AT THE MOMENT!!!
//


// (kind-of-)dict that associates a pos of X (or O) to a pair
// - number of elements
// - index of this pos among all positions of same number of elements
std::array<std::pair<unsigned int, unsigned int>, NB_COMB> combValues;
//TODO: Is it slow to use pair here? Probably not... But I did not check...

// Reverse of the previous dict:
// Associate a pair (number of elements, index) to a position of X
std::array<std::vector<unsigned long long>, SIZE*SIZE+1> revCombValues;

// TODO: Explain what it is... But I do not know how to explain...
std::array<std::array<unsigned long long, SIZE*SIZE+1>, SIZE*SIZE+1> offset;

// Initialize all previously-defined global constants :)
void initEncState() {
  std::cout << "Initialization of combValues:" << std::endl;
  std::array<unsigned int, SIZE*SIZE+1> combCount {0};
  for(unsigned int comb=0; comb<NB_COMB; ++comb) {
    // Not sure this line will compile on Mac; it compiles at least with GCC. Otherwise, you will need to implement popcount by yourself :D
    unsigned int pop = __builtin_popcount(comb);
    //std::cout << comb << " " << pop << " " << combCount[pop] << std::endl; // To debug
    combValues[comb] = std::make_pair(pop, combCount[pop]);
    combCount[pop]++;
  }

  std::cout << "Initialization of revCombValues" << std::endl;
  for(int i=0; i<=SIZE*SIZE; ++i) {
    std::cout << i << " " << combCount[i] << std::endl;
    revCombValues[i].resize(combCount[i]);
  }
  for(unsigned int comb=0; comb<NB_COMB; ++comb) {
    revCombValues[combValues[comb].first][combValues[comb].second] = comb;
  }

  std::cout << "Initialization of offset" << std::endl;
  for(int nbX=0; nbX<=SIZE*SIZE; ++nbX) {
    for(int nbO=0; nbO<=SIZE*SIZE-nbX; ++nbO) {
      int indexCombO = ((1 << nbO) - 1) << (SIZE*SIZE - nbX - nbO);
      offset[nbX][nbO] = combValues[indexCombO].second + 1;
      //std::cout << nbX << "'" << nbO << " : " << offset[nbX][nbO] << std::endl;
    }
  }
}


//TODO: Merge with Order (see in state.h)
//TODO: Rewrite code so that we dont need to keep nbX and nbO
//      (using only two vector values and valuesReverse as in Tanaka's code)
struct EncodedState {
  int nbX;
  int nbO;
  unsigned long long index;
};

EncodedState encode(State s) {
  Order ord = computeOrder(s);
  EncodedState enc;
  enc.nbX = combValues[ord.X].first;
  enc.nbO = combValues[ord.O].first;
  enc.index = combValues[ord.X].second * offset[enc.nbX][enc.nbO] + combValues[ord.O].second;
  return enc;
}

State decode(EncodedState enc) {
  Order ord;
  unsigned int tmp = enc.index / offset[enc.nbX][enc.nbO];
  ord.X = revCombValues[enc.nbX][tmp];
  ord.O = revCombValues[enc.nbO][enc.index - tmp * offset[enc.nbX][enc.nbO]];
  return fromOrder(ord);
}



#endif //ENCSTATE_H
