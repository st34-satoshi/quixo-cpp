#include <vector>
#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <time.h>

using namespace std;
typedef long long ll;
typedef unsigned long long ull;

const int boardSize = 3;
const int combinationSize = boardSize * boardSize;

vector< vector<ll>> combinations;  // nCr, (n<boardSize^2)

void createCombinations(){
    int combSize = combinationSize+ 1;
    vector< vector<ll> > comb(combSize, vector<ll>(combSize));
    for(int i=0;i<combSize;i++){
        comb.at(i).at(0) = 1;
        comb.at(i).at(i) = 1;
    }
    for(int i=1;i<combSize;i++){
        for(int j=1;j<i;j++){
            comb.at(i).at(j) = comb.at(i-1).at(j-1) + comb.at(i-1).at(j); // nCr = (n-1)C(r-1)+(n-1)Cr
        }
    }
    combinations = comb;
}

ll getPatterns(int spaceNumber, int oNumber, int xNumber){
    return combinations.at(spaceNumber).at(oNumber) * combinations.at(spaceNumber-oNumber).at(xNumber);
}