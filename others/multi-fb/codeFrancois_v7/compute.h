#ifndef COMPUTE_H
#define COMPUTE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>

#include <array>
#include <vector>

#include <atomic>
#include <mutex>
#include <thread>

#include "global.h"
#include "state.h"
#include "encstate.h"
#include "tools.h"



// outcome store the outcome of all states
// it is a 3D `array':
// - first dimension is the number of X
// - second dimension is the number of O
// - third dimension is the index of the state
std::array<std::array<std::vector<bool>, SIZE*SIZE+1>, SIZE*SIZE+1> outcome;
std::array<std::array<unsigned long long, SIZE*SIZE+1>, SIZE*SIZE+1> outcomeSize;

// Mutex to access the vector bool
const int NBBUCKETS = 1 << 10; // nb of subdivisions per vector to try to limit concurrency
std::array<std::array<std::array<std::mutex, NBBUCKETS>, SIZE*SIZE+1>, SIZE*SIZE+1> mutex;
inline int computeBucket(int nbX, int nbO, unsigned long long index) { // The *2 of below is because we access 2*i and 2*i+1... Not very beautiful but it is ok for now...
  unsigned long long reducedSize = outcomeSize[nbX][nbO] / (128*2);    // 128 is conservative to take 16B at the same time. I do not expect compiler/processer to use more than 8B, but just in case :)
  unsigned long long bucketSize = (reducedSize / NBBUCKETS) + 1;       // +1 is needed when NBBUCKET > reducedSize (and also to avoid oversize last bucket)
  unsigned long long eltsPerBucket = bucketSize * 128;
  unsigned long long t = (index / eltsPerBucket);
  //if(t < 0 or t>= NBBUCKETS) {
  //std::cout << "NOT GOOD" << std::endl;
  //std::cout << nbX << " " << nbO << " " << index << " " << outcomeSize[nbX][nbO] << std::endl;
  //exit(0);
  //}
  return t;
}

// Data to store the number of win/loss
// Use atomic variables for multithread
std::array<std::array<std::array<std::atomic_ullong, 2>, SIZE*SIZE+1>, SIZE*SIZE+1> partialStat;
std::array<std::atomic_ullong, 2> totalStat;

void initOutcomeSize() {
  for(int nbX=0; nbX<=SIZE*SIZE; ++nbX) {
    // Funny trick below, probably not very clear what I am doing :)
    int indexCombX = ((1 << nbX) - 1) << (SIZE*SIZE - nbX);
    for(int nbO=0; nbO<=SIZE*SIZE-nbX; ++nbO) {
      int indexCombO = ((1 << nbO) - 1) << (SIZE*SIZE - nbX - nbO);
      unsigned long long nbPos = ((unsigned long long) combValues[indexCombX].second + 1) * ((unsigned long long) combValues[indexCombO].second + 1); // ugly casts (probably) needed to avoid overflow for SIZE == 5
      outcomeSize[nbX][nbO] = 2*nbPos; // 2bits per state; 00 unknown (or DRAW), 01 WINorDRAW (aka noLOSS), 10 WIN,  11 LOSS
    }                                  // Note: Do not change the meaning 00, 01, 10, 11
  }                                    //       Code use the second bit to check if WINorLOSS
}

// Given a LOSS State, update previous state to WIN
void updatePreviousFromLoss(State s) {
  StateList p = computePreviousStatesFromX(swapPlayer(s));
  for(int j=0; j<p.nb; ++j) {
    EncodedState p_enc = encode(p.state[j]);
    int bucket = computeBucket(p_enc.nbX, p_enc.nbO, p_enc.index);
    mutex[p_enc.nbX][p_enc.nbO][bucket].lock();
    if (not outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) {     // If value is already decided (LOSS); must NOT override it
      outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = true;      // otherwise previous state is a WIN = true-false
      outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = false;
      partialStat[p_enc.nbX][p_enc.nbO][0]++;
    }
    mutex[p_enc.nbX][p_enc.nbO][bucket].unlock();
  }
}

void computeImmediateOutcomeThread(int nbX, int nbO, unsigned long long startI, unsigned long long endI) {
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=startI; i<endI; ++i) {
    enc.index = i;
    State s = decode(enc);
    if (hasLineX(s)) { // WIN
      int bucket = computeBucket(nbX, nbO, i);
      mutex[nbX][nbO][bucket].lock();
      if (not outcome[nbX][nbO][2*i]) { // to avoid double-counting
        outcome[nbX][nbO][2*i] = true;
        outcome[nbX][nbO][2*i+1] = false; // Not needed here but let's keep it for safety :)
        partialStat[nbX][nbO][0]++;
      }
      mutex[nbX][nbO][bucket].unlock();
    } else if (hasLineO(s)) { // LOSS
      int bucket = computeBucket(nbX, nbO, i);
      mutex[nbX][nbO][bucket].lock();
      if (outcome[nbX][nbO][2*i]) {      // This state was (incorrectly) considered a WIN; need to revert the stat :)
        partialStat[nbX][nbO][0]--;      // A bit ugly, but that's OK
      }
      outcome[nbX][nbO][2*i] = true;     // This may override previously computed values
      outcome[nbX][nbO][2*i+1] = true;   // Hence we need to keep this line
      mutex[nbX][nbO][bucket].unlock();
      partialStat[nbX][nbO][1]++;
      updatePreviousFromLoss(s);
    }
  }
}


// Compute outcome for all states that have a line
// Priority is given to line of X
// (if lineO and lineX, it is a LOSS from previous player ==> a WIN for next player)
// If we find a LOSS, we update all previous States to WIN
// CAREFUL: There can be some incorrectly marked WIN states => overide needed :)
//          Also need to revert the stats
void computeImmediateOutcome(int nbX, int nbO) {
  std::thread t[NBTHREADS];
  unsigned long long perThread = (outcome[nbX][nbO].size() / 2) / NBTHREADS;
  for(int i=0; i<NBTHREADS-1; ++i) t[i] = std::thread(computeImmediateOutcomeThread, nbX, nbO, i*perThread, (i+1)*perThread);
  t[NBTHREADS-1] = std::thread(computeImmediateOutcomeThread, nbX, nbO, (NBTHREADS-1)*perThread, outcome[nbX][nbO].size() / 2);
  
  for(int i=0; i<NBTHREADS; ++i) t[i].join();
}

// Look at all *already-computed* nextStates and update current state to
// - WIN if a nextState is a LOSS
// - WINorDRAW if the nextState is a DRAW
// Note: This function is called with nbX,nbO+1 wrt current states; not very clear :)
void computeFromNextOutcomeThread(int nbX, int nbO, unsigned long long startI, unsigned long long endI) {
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=startI; i<endI; ++i) {
    if (outcome[nbX][nbO][2*i]) {     // It is a WIN or LOSS state
      if (outcome[nbX][nbO][2*i+1]) { // It is a LOSS state
        enc.index = i;
        State s = decode(enc);
        StateList p = computePreviousStatesFromEmpty(swapPlayer(s));  // Previous from Empty!
        for(int j=0; j<p.nb; ++j) {
          EncodedState p_enc = encode(p.state[j]);
          int bucket = computeBucket(p_enc.nbX, p_enc.nbO, p_enc.index);
          mutex[p_enc.nbX][p_enc.nbO][bucket].lock();
          if (not outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) {     // If value is already decided; must not override it
            outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = true;      // Previous state is a WIN = true-false
            outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = false;   // Cannot be removed; it may be true before update (if it was a WINorDRAW)
            partialStat[p_enc.nbX][p_enc.nbO][0]++;
          }
          mutex[p_enc.nbX][p_enc.nbO][bucket].unlock();
        }
      }
    } else {                          // It is a DRAW state
      enc.index = i;
      State s = decode(enc);
      StateList p = computePreviousStatesFromEmpty(swapPlayer(s));
      for(int j=0; j<p.nb; ++j) {
        EncodedState p_enc = encode(p.state[j]);
        int bucket = computeBucket(p_enc.nbX, p_enc.nbO, p_enc.index);
        mutex[p_enc.nbX][p_enc.nbO][bucket].lock();
        if (not outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index]) {    // If value is already decided; must not override it
          //outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index] = false;  // Not needed here, since it was already false
          outcome[p_enc.nbX][p_enc.nbO][2*p_enc.index+1] = true;   // Previous state is a WIN or DRAW = false-true
        }
        mutex[p_enc.nbX][p_enc.nbO][bucket].unlock();
      }
    }
  }
}


void computeFromNextOutcome(int nbX, int nbO) {
  // Need to load already computed values:
  outcome[nbX][nbO].resize(outcomeSize[nbX][nbO], false);
  std::string filename ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbX) + "_" + std::to_string(nbO));
  bool readOK = readFromFile(outcome[nbX][nbO], filename);
  if (not readOK) {
    std::cout << "There is a bug somewhere in reading already computed files " << filename << std::endl;
    exit(0);
  }

  std::thread t[NBTHREADS];
  unsigned long long perThread = (outcome[nbX][nbO].size() / 2) / NBTHREADS;
  for(int i=0; i<NBTHREADS-1; ++i) t[i] = std::thread(computeFromNextOutcomeThread, nbX, nbO, i*perThread, (i+1)*perThread);
  t[NBTHREADS-1] = std::thread(computeFromNextOutcomeThread, nbX, nbO, (NBTHREADS-1)*perThread, outcome[nbX][nbO].size() / 2);
  
  for(int i=0; i<NBTHREADS; ++i) t[i].join();
  
  // We can "release" memory here. No need to keep it 
  std::vector<bool>().swap(outcome[nbX][nbO]);
}

// Shared boolean for computeIterativelyOutcome
std::atomic<bool> sharedBool;

// Compute outcome iteratively by computing children in the same class
void computeIterativelyOutcomeThread(int nbX, int nbO, unsigned long long startI, unsigned long long endI) {
  EncodedState enc {nbX, nbO, 0};
  for(unsigned long long i=startI; i<endI; ++i) {
    int bucket = computeBucket(nbX, nbO, i);
    mutex[nbX][nbO][bucket].lock();
    bool alreadyComputed = outcome[nbX][nbO][2*i] or outcome[nbX][nbO][2*i+1]; // State is known or WorD. Look only for Unknown states = false-false
    mutex[nbX][nbO][bucket].unlock();
    
    if (not alreadyComputed) {
      enc.index = i;
      State s = decode(enc);
      StateList next = computeNextStatesFromX(s); // Only next from X
      int j;
      for(j=0; j<next.nb; ++j) {
        EncodedState next_enc = encode(swapPlayer(next.state[j]));
        int next_bucket = computeBucket(next_enc.nbX, next_enc.nbO, next_enc.index);
        mutex[next_enc.nbX][next_enc.nbO][next_bucket].lock();
        bool notWin = not outcome[next_enc.nbX][next_enc.nbO][2*next_enc.index] or outcome[next_enc.nbX][next_enc.nbO][2*next_enc.index+1];
        mutex[next_enc.nbX][next_enc.nbO][next_bucket].unlock();
        if (notWin) { // child is *not* a WIN
          break; // No need to explore other nextStates
        }
      }
      if (j == next.nb) { // all children are WIN
        sharedBool = true;               // At least one update is made in this iteration
        mutex[nbX][nbO][bucket].lock();
        outcome[nbX][nbO][2*i] = true;   // LOSS = true-true
        outcome[nbX][nbO][2*i+1] = true; // LOSS
        mutex[nbX][nbO][bucket].unlock();
        partialStat[nbX][nbO][1]++;
        updatePreviousFromLoss(s);
      }
    }
  }
}

bool computeIterativelyOutcome(int nbX, int nbO) {
  sharedBool = false;
  
  std::thread t[NBTHREADS];
  unsigned long long perThread = (outcome[nbX][nbO].size() / 2) / NBTHREADS;
  for(int i=0; i<NBTHREADS-1; ++i) t[i] = std::thread(computeIterativelyOutcomeThread, nbX, nbO, i*perThread, (i+1)*perThread);
  t[NBTHREADS-1] = std::thread(computeIterativelyOutcomeThread, nbX, nbO, (NBTHREADS-1)*perThread, outcome[nbX][nbO].size() / 2);
  
  for(int i=0; i<NBTHREADS; ++i) t[i].join();

  return sharedBool;
}



void printStat(int nbX, int nbO, auto start) {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> d = now-start;
  if (nbX == nbO)
    std::cout << std::setw(9) << d.count() << "  " << std::setw(12) << partialStat[nbX][nbO][0] << " " << std::setw(12) << partialStat[nbX][nbO][1] << std::endl;
  else
    std::cout << std::setw(9) << d.count() << "  " << std::setw(12) << partialStat[nbX][nbO][0] << " " << std::setw(12) << partialStat[nbX][nbO][1] << "  " << std::setw(12) << partialStat[nbO][nbX][0] << " " << std::setw(12) << partialStat[nbO][nbX][1] << std::endl;
}

// Compute all outcome using backward induction !
// NOTE: This function is ugly; it should be refactored...
void computeOutcome() {
  for(int nbXO=SIZE*SIZE; nbXO>=0; --nbXO) { // total number of tiles X or O
    for(int nbX=0; nbX<=nbXO/2; ++nbX) {     // Need to do at the same time the pairs (nbX, nbXO-nbX) and (nbXO-nbX, nbX)
      int nbO = nbXO - nbX;
      std::cout << "Starting nbXO: nbX,nbO = " << nbXO << ": (" << nbX << "," << nbO << ")" << std::endl;

      // Read-Write to file
      std::string filename1 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbX) + "_" + std::to_string(nbO));
      std::string filename2 ("./result/outcome" + std::to_string(SIZE) + "_" + std::to_string(nbO) + "_" + std::to_string(nbX));
      bool alreadyComputed = existingFile(filename1);
      if (nbX != nbO) alreadyComputed = alreadyComputed and existingFile(filename2); // probably useless
      
      if (not alreadyComputed) {    // Values for nbX,nbO are not yet computed
        // Initialize stats and resize arrays
        partialStat[nbX][nbO][0] = 0;
        partialStat[nbX][nbO][1] = 0;
        outcome[nbX][nbO].resize(outcomeSize[nbX][nbO], false);
        if (nbX != nbO) {
          partialStat[nbO][nbX][0] = 0;
          partialStat[nbO][nbX][1] = 0;
          outcome[nbO][nbX].resize(outcomeSize[nbO][nbX], false);
        }

        auto start = std::chrono::system_clock::now();
        // Compute outcome for all boards that have a line (of X or O)
        computeImmediateOutcome(nbX, nbO);
        if (nbX != nbO) computeImmediateOutcome(nbO, nbX);
        printStat(nbX, nbO, start);
      
        if (nbXO < SIZE*SIZE) {
          // Compute WIN states that have a LOSS child in already comptued values (ie when adding a tile)
          computeFromNextOutcome(nbX, nbO+1);
          if (nbX != nbO) computeFromNextOutcome(nbO, nbX+1);
        }
        
        // Compute the remaining values with a long loop
        // TODO: Keep track of which states have been updated to WIN
        //       and look only among their previousStates
        //       maybe faster for 5x5 and long vector<bool>
        //       but not sure :)
        bool progress = true;
        while (progress) {
          printStat(nbX, nbO, start);
          progress = computeIterativelyOutcome(nbX, nbO);
          if (nbX != nbO) progress = computeIterativelyOutcome(nbO, nbX) || progress;
        }

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> d = now-start;
        std::cout << std::setw(9) << d.count() << "  Start writing to file" << std::endl;
  
        // Finish computation, store to file
        writeToFile(outcome[nbX][nbO], filename1);
        totalStat[0] += partialStat[nbX][nbO][0];
        totalStat[1] += partialStat[nbX][nbO][1];
        if (nbX != nbO) {
          writeToFile(outcome[nbO][nbX], filename2);
          totalStat[0] += partialStat[nbO][nbX][0];
          totalStat[1] += partialStat[nbO][nbX][1];
        }

        now = std::chrono::system_clock::now();
        d = now-start;
        std::cout << std::setw(9) << d.count() << "  End writing to file" << std::endl;
      }
      
      // VERY UGLY CODE HERE!!!
      // NEED TO FIND SOMETHING BETTER
      // TODO: One possible solution would be to allocate the largest possible frmo the beginning
      //       and then only reinitialize needed values to false-false
      std::vector<bool>().swap(outcome[nbX][nbO]);
      if (nbX != nbO) std::vector<bool>().swap(outcome[nbO][nbX]);
    }
  }
  std::cout << "May be invalid if data read from files:" << std::endl;
  std::cout << "Final stats: " << totalStat[0] << " " << totalStat[1] << std::endl;
}




#endif //COMPUTE_H
