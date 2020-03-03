#include <vector>
#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
#include <time.h>
#include <array>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;
typedef long long ll;
typedef unsigned long long ull;

const int boardSize = 4;
const int combinationSize = boardSize * boardSize;
const int MAX_STATES = (boardSize-2)*4*3 + 4*2; // max size of next states
const ull MAX_STATES_VALUE = 2018016; // 4*4
// const ull MAX_STATES_VALUE = 26293088250; // TODO: this is 25C16*16C8. if you change the boardSize you should change this number. It is better to create this number automatically!
const int THREADS_NUMBER = 32;
const int MUTEX_NUMBER = 1 << 15;

vector< vector<ll>> combinations;  // nCr, (n<boardSize^2)

const ll oMark = 1ll;
const ll xMark = 2ll;
const ll bMark = 3ll;

struct StateArray{
    int count = 0;
    array<ll, MAX_STATES> states;
};

struct SymmetricStates{
    static const int size = 8;
    array<ll, size> states;
};

struct RotateStates{
    array<ll, 4> states;
};

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

inline ll getPatterns(int spaceNumber, int oNumber, int xNumber){
    return combinations.at(spaceNumber).at(oNumber) * combinations.at(spaceNumber-oNumber).at(xNumber);
}

ll getCombination(int x, int y){
    // return xCy
    // this function is called when x and y sometimes invalid value (ex. <0, >combinationSize)
    if (x < 0 || x > combinationSize || y < 0 || y > combinationSize){
        return 0ll;
    }
    return combinations[x][y];
}
