#include <iostream>
#include <array>
#include <string>
#include <utility> // for std::pair
#include <vector>

#include "global.h"
#include "state.h"
#include "encstate.h"
#include "tools.h"

// outcome store the outcome of all states
// it is a 3D `array':
// - first dimension is the number of X
// - second dimension is the number of O
// - third dimension is the index of the state
// 
std::array<std::array<std::vector<bool>, SIZE*SIZE+1>, SIZE*SIZE+1> outcome;
std::array<std::array<unsigned long long, SIZE*SIZE+1>, SIZE*SIZE+1> outcomeSize;

 
void initOutcomeSize() {
  for(int nbX=0; nbX<=SIZE*SIZE; ++nbX) {
    // Funny trick below, probably not very clear what I am doing :)
    int indexCombX = ((1 << nbX) - 1) << (SIZE*SIZE - nbX);
    for(int nbO=0; nbO<=SIZE*SIZE-nbX; ++nbO) {
      int indexCombO = ((1 << nbO) - 1) << (SIZE*SIZE - nbX - nbO);
      unsigned long long nbPos = ((unsigned long long) combValues[indexCombX].second + 1) * ((unsigned long long) combValues[indexCombO].second + 1); // ugly casts (probably) needed to avoid overflow for SIZE == 5
      outcomeSize[nbX][nbO] = 2*nbPos; // 2bits per state; 00 unknown (or DRAW), 01 WINorDRAW (aka noLOSS), 10 WIN,  11 LOSS
    }
  }
}

// Store numbers of Win, Draw, and Loss
// TODO: Rename "Stat" is too close to "State"
struct Stat {
  unsigned long long nbW;
  unsigned long long nbL;

  Stat& operator+=(const Stat& rhs) {
    nbW += rhs.nbW;
    nbL += rhs.nbL;
    return *this;
  }
};

// Compute outcome for all states that have a line
// Priority is given to line of O
// (if lineO and lineX, it is a LOSS from rules)
Stat computeImmediateOutcome(int nbX, int nbO) {
  Stat count {0, 0};
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=0; i<outcome[nbX][nbO].size() / 2; ++i) {
    enc.index = i;
    State s = decode(enc);
    if (hasLineO(s)) { // LOSS
      outcome[nbX][nbO][2*i] = true;
      outcome[nbX][nbO][2*i+1] = true;
      count.nbL++;
    } else if (hasLineX(s)) { // WIN
      outcome[nbX][nbO][2*i] = true;
      //outcome[nbX][nbO][2*i+1] = false; //Not needed here
      count.nbW++;
    }
  }
  return count;
}

// Look for all *immediate* LOSS (computed with previous function)
// and update all the previous states to WIN (only not yet computed states!)
// Note (TODO): Could be included in the previous function
//              But need to be careful due to updated a state to WIN instead of an immediate LOSS
Stat computeImmediateOutcomeBis(int nbX, int nbO) {   // TODO: Obviously need to find a better name for this function...
  Stat count {0, 0};
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=0; i<outcome[nbX][nbO].size() / 2; ++i) {
    if (outcome[nbX][nbO][2*i] and outcome[nbX][nbO][2*i+1]) { // It is a LOSS state
      enc.index = i;
      State s = decode(enc);
      StateList p = computePreviousStatesFromX(swapPlayer(s));
      for(int j=0; j<p.nb; ++j) {
        EncodedState p_enc = encode(p.state[j]);
        if (outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) continue; // Value is already decided; must not override it
        outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = true;        // otherwise previous state is a WIN = true-false
        //outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = false;   // Not needed here since it was already false
        count.nbW++;
      }
    }
  }
  return count;
}


// Look at all *already-computed* nextStates and update current state to
// - WIN if a nextState is a LOSS
// - WINorDRAW if the nextState is a DRAW
// Note: This function is called with nbX,nbO+1 wrt current states; not very clear :)
Stat computeFromNextOutcome(int nbX, int nbO) {
  Stat count {0, 0};
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=0; i<outcome[nbX][nbO].size() / 2; ++i) {
    if (outcome[nbX][nbO][2*i]) {     // It is a WIN or LOSS state
      if (outcome[nbX][nbO][2*i+1]) { // It is a LOSS state
        enc.index = i;
        State s = decode(enc);
        StateList p = computePreviousStatesFromEmpty(swapPlayer(s));  // Previous from Empty!
        for(int j=0; j<p.nb; ++j) {
          EncodedState p_enc = encode(p.state[j]);
          if (outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) continue; // Value is already decided; must not override it
          outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = true;        // Previous state is a WIN = true-false
          outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = false;     // Cannot be removed; it may be true before update (if it was a WINorDRAW)
          count.nbW++;
        }
      }
    } else {                          // It is a DRAW state
      enc.index = i;
      State s = decode(enc);
      StateList p = computePreviousStatesFromEmpty(swapPlayer(s));
      for(int j=0; j<p.nb; ++j) {
        EncodedState p_enc = encode(p.state[j]);
        if (outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) continue; // Value is already decided; must not override it
        //outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = false;     // Not needed here, since it was already false
        outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = true;      // Previous state is a WIN or DRAW = false-true
      }
    }
  }
  return count;
}


      
// Compute outcome iteratively by computing children in the same class
Stat computeIterativelyOutcome(int nbX, int nbO) {
  Stat count {0, 0};
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=0; i<outcome[nbX][nbO].size() / 2; ++i) {
    if (outcome[nbX][nbO][2*i] or outcome[nbX][nbO][2*i+1]) continue; // State is known or WorD. Look only for Unknown states = false-false
    enc.index = i;
    State s = decode(enc);
    StateList next = computeNextStatesFromX(s); // Only next from X
    int j;
    for(j=0; j<next.nb; ++j) {
      EncodedState next_enc = encode(swapPlayer(next.state[j]));
      if (not outcome[next_enc.nbX][next_enc.nbO][2*next_enc.index] or outcome[next_enc.nbX][next_enc.nbO][2*next_enc.index+1]) { // child is *not* a WIN
        break; // No need to explore other nextStates
      }
    }
    if (j == next.nb) { // all children are WIN
      outcome[nbX][nbO][2*i] = true;   // LOSS = true-true
      outcome[nbX][nbO][2*i+1] = true; // LOSS
      count.nbL++;
      
      StateList prevStates = computePreviousStatesFromX(swapPlayer(s));  // Only prev from X
      for(int k=0; k<prevStates.nb; ++k) {
        EncodedState prev_enc = encode(prevStates.state[k]);
        if (outcome[prev_enc.nbX][prev_enc.nbO][2*prev_enc.index]) continue;
        outcome[prev_enc.nbX][prev_enc.nbO][2*prev_enc.index] = true;    // WIN = true-false
        outcome[prev_enc.nbX][prev_enc.nbO][2*prev_enc.index+1] = false; // WIN
        count.nbW++;
      }
    }
  }
  return count;
}

Stat totalStat = {0, 0};
// Compute all outcome using backward induction !
// NOTE: This function is ugly; it should be refactored...
void computeOutcome() {
  for(int nbXO=SIZE*SIZE; nbXO>=0; --nbXO) { // total number of tiles X or O
    for(int nbX=0; nbX<=nbXO/2; ++nbX) {     // Need to do at the same time the pairs (nbX, nbXO-nbX) and (nbXO-nbX, nbX)
      std::cout << "Starting nbXO,nbX = " << nbXO << "," << nbX << std::endl;
      int nbO = nbXO - nbX;
      
      outcome[nbX][nbO].resize(outcomeSize[nbX][nbO], false); // could maybe use reserve here to avoid long initialization (and do initialization in computeImmediateOutcome
      if (nbX != nbO) outcome[nbO][nbX].resize(outcomeSize[nbO][nbX], false);
            
      // Read-Write to file
      std::string filename1 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbX) + "_" + std::to_string(nbO));
      std::string filename2 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbO) + "_" + std::to_string(nbX));
      bool readOK = readFromFile(outcome[nbX][nbO], filename1);
      if (nbX != nbO) readOK = readOK and readFromFile(outcome[nbO][nbX], filename2);
      
      if (not readOK) {    // Values for nbX,nbO are not yet computed 
        if (nbXO < SIZE*SIZE) {
          // Need to load already computed values:
		  // TODO: No need to load both of them at the same time
		  //       It would be enough to load one by one and then remove from memory
          outcome[nbX][nbO+1].resize(outcomeSize[nbX][nbO+1], false);
          std::string filename3 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbX) + "_" + std::to_string(nbO+1));
          bool readOK3 = readFromFile(outcome[nbX][nbO+1], filename3);
          if (not readOK3) {
            std::cout << "There is a bug somewhere in reading already computed files " << filename3 << std::endl;
            exit(0);
          }
          if (nbX != nbO) {
            outcome[nbO][nbX+1].resize(outcomeSize[nbO][nbX+1], false);
            std::string filename4 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbO) + "_" + std::to_string(nbX+1));
            bool readOK4 = readFromFile(outcome[nbO][nbX+1], filename4);
            if (not readOK4) {
              std::cout << "There is a bug somewhere in reading already computed files " << filename4 << std::endl;
              exit(0);
            }
          }
        }
        
        Stat stat = {0, 0}; // To track progress

        // Compute outcome for all boards that have a line (of X or O)
        stat += computeImmediateOutcome(nbX, nbO);
        if (nbX != nbO)  stat += computeImmediateOutcome(nbO, nbX);
        std::cout << "  " << stat.nbW << " " << stat.nbL << std::endl;

        // Compute WIN states that have an immediate LOSS child
        stat += computeImmediateOutcomeBis(nbX, nbO);
        if (nbX != nbO)  stat += computeImmediateOutcomeBis(nbO, nbX);
        std::cout << "  " << stat.nbW << " " << stat.nbL << std::endl;

        // Compute WIN states that have a LOSS child in already comptued values (ie when adding a tile)
        stat += computeFromNextOutcome(nbX, nbO+1);
        if (nbX != nbO)  stat += computeFromNextOutcome(nbO, nbX+1);

        // Compute the remaining values with a long loop
        // TODO: Keep track of which states have been updated to WIN
        //       and look only among their previousStates
        //       maybe faster for 5x5 and long vector<bool>
        //       but not sure :)
        bool progress = true;
        while (progress) {
          std::cout << "  " << stat.nbW << " " << stat.nbL << std::endl;
          progress = false;
          Stat tmp = computeIterativelyOutcome(nbX, nbO);
          stat += tmp;
          if (tmp.nbW + tmp.nbL > 0) progress = true;
          
          if (nbX != nbO) {
            tmp = computeIterativelyOutcome(nbO, nbX);
            stat += tmp;
            if (tmp.nbW + tmp.nbL > 0) progress = true;
          }
        }
        
        writeToFile(outcome[nbX][nbO], filename1);
        if (nbX != nbO) writeToFile(outcome[nbO][nbX], filename2);

        totalStat += stat;
      }
      
      // VERY UGLY CODE HERE!!!
      // NEED TO FIND SOMETHING BETTER
      // TODO: One possible solution would be to allocate the largest possible frmo the beginning
      //       and then only reinitialize needed values to false-false
      std::vector<bool>().swap(outcome[nbX][nbO]);
      std::vector<bool>().swap(outcome[nbX][nbO+1]);
      if (nbX != nbO) {
        std::vector<bool>().swap(outcome[nbO][nbX]);
        std::vector<bool>().swap(outcome[nbO][nbX+1]);
      }
    }
  }
  std::cout << "May be invalid if data read from files:" << std::endl;
  std::cout << "Final stats: " << totalStat.nbW << " " << totalStat.nbL << std::endl;
}

// CAUTION: Currently does not work
//          Need to read from files before accessing outcome[][]
void printStats() {
  int nbWin = 0;
  int nbLoss = 0;
  int nbDraw = 0;
  for(int nbX=0; nbX<=SIZE*SIZE; ++nbX) {
    for(int nbO=0; nbO<=SIZE*SIZE-nbX; ++nbO) {
      for(unsigned long long i=0; i<outcome[nbX][nbO].size() / 2; ++i) {
        if (outcome[nbX][nbO][2*i] and outcome[nbX][nbO][2*i+1]) nbLoss++;
        else if (outcome[nbX][nbO][2*i]) nbWin++;
        else nbDraw++;
      }
    }
  }
  std::cout << "nbWin, nbLoss, nbDraw: " << nbWin << "," << nbLoss << "," << nbDraw << std::endl;
  std::cout << "From initial config: " << outcome[0][0][0] << outcome[0][0][1] << std::endl;
}

int main() {
  initEncState();
  initOutcomeSize();
  computeOutcome();
  
  return 0;
}
  
