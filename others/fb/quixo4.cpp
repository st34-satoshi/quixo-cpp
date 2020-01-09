/*
Next player to play: X
Previous player: O

A board is a 16-tuple/list of 0,1,2
e.g.: (0,0,0,1, 1,2,0,0, 0,1,2,0, 2,1,2,1) means that the board is
    ...X
    XO..
    .XO.
    OXOX

   0  1  2  3
   4  5  6  7
   8  9 10 11
  12 13 14 15
*/

#include <iostream>
#include "global4.h"
#include "state4.h"
#include <unordered_set>

void countReachable() {
  std::unordered_set<ShortState> allStates;
  std::unordered_set<ShortState> newStates;
  std::unordered_set<ShortState> newNewStates;

  State init(0);
  ShortState init_ss = init.convert();
  allStates.insert(init_ss);
  newStates.insert(init_ss);

  int i = 0;
  while (not newStates.empty()) {
    i++;
    std::cout << i << " " << allStates.size() << " " << newStates.size() << std::endl;
    newNewStates.clear();

    for(auto ss : newStates) {
      State s(ss);
      ChildrenList cl = s.computeChildren();
      for(int n=0; n<cl.nbChildren; ++n) {
        ShortState ssc = cl.children[n].convert();

        if (cl.children[n].checkLines() != 0) {
          allStates.insert(ssc);
        } else {
          if (allStates.find(ssc) == allStates.end()) {
            newNewStates.insert(ssc);
            allStates.insert(ssc);
          }
        }
      }
    }

    newStates = newNewStates;
  }

  std::cout << "There are " << allStates.size() << " reachable states" << std::endl;
  return;
}  

int main() {
  countReachable();
  return 0;
}
  
