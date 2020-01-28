#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "global.h"


// Maybe ugly code, let me know if you know how to improve it :)
// I am not expert with file I/O
// Adapted from https://stackoverflow.com/questions/4666450/how-does-one-store-a-vectorbool-or-a-bitset-into-a-file-but-bit-wise
void writeToFile(const std::vector<bool> &in, std::string filename) {
  std::ofstream out(filename, std::ios::out | std::ios::binary);
  unsigned long long i = 0;  // the current bit index
  unsigned char c = 0; // the current byte
  short bits = 0;      // to process next byte
  while(i < in.size()) {
    c = c << 1;       //
    if(in[i]) ++c; // adding 1 if bit is true
    ++bits;
    if(bits == 8){
      out.put((char) c);
      c = 0;
      bits = 0;
    }
    ++i;
  }
  // dump remaining
  if(bits != 0) {
    // pad the byte so that first bits are in the most significant positions.
    while(bits != 8) {
      c = c << 1;
      ++bits;
    }
    out.put((char) c);
  }
  out.close();
}

// TODO: Do something better :)
// return true if success
// return false otherwise
// crash if bug
bool readFromFile(std::vector<bool> &out, std::string filename) {
  std::ifstream in(filename, std::ios::in | std::ios::binary);

  if (not in.good()) { // Probably file does not exist
    return false;
  }
  
  unsigned long long i = 0;    // current bit index
  unsigned char mask = 0x80;   // current byte mask (==128==0b10000000)
  unsigned char c = 0;         // current byte in stream
  while(in.good() && (i < out.size())) {
    if((i%8) == 0) {         // retrieve next character
      c = in.get();
      mask = 0x80;
    } else {
      mask = mask >> 1;   // shift mask
    }
    out[i] = (c & mask);
    ++i;
  }
  if (i != out.size()) {
    std::cout << "SOME BUG SOMEWHERE :)" << std::endl;
    exit(0);
  }
  in.close();
  return true;
}



#endif //TOOLS_H
