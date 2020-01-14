/*
Next player is o.
states are represented by the number of index(i).
bitset saves 2 bits.
00: unknown(draw)
01: lose
10: win
11: ??

but it is difficult to create next states and find cymmetric states using indexNumber.
we use this state.
states are represented by long long.
0:empty, 1:o, 2:x
ox-
o-o
--x
is
011000_010001_000010

you can change size by boardSize.

Run
c++ -std=c++17 -O3 from_back.cpp 
./a.out
*/

#include <iostream>
#include <bitset>
#include <vector>

#include "state.cpp"

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

void init(){
    createCombinations();
    initState();
}

ll getPatterns(int spaceNumber, int oNumber, int xNumber){
    // cout << "caculate patterns " << spaceNumber << ", " << oNumber << ", " << xNumber << endl;
    return combinations.at(spaceNumber).at(oNumber) * combinations.at(spaceNumber-oNumber).at(xNumber);
}

ll generateMark(int spaceNumber, ll *indexNumber, int oNumber, int xNumber){
    // TODO implement: return mark and reduce number
    // cout << "generate mark" << endl;
    // 1=o, 2=x, 0=no mark
    // at the end
    if (spaceNumber == oNumber){
        return 1ll;
    }
    if (spaceNumber == xNumber){
        return 2ll;
    }
    if (oNumber == 0 && xNumber== 0){
        return 0ll;
    }
    // not at the end
    if (oNumber > 0){
        ll patternsSelectedO = getPatterns(spaceNumber-1, oNumber-1, xNumber);
        // cout << "patterns selected o " << patternsSelectedO << endl;
        if (*indexNumber < patternsSelectedO){
            return 1ll;
        }
        *indexNumber -= patternsSelectedO;
    }
    if (xNumber > 0){
        ll patternsSelectedX = getPatterns(spaceNumber-1, oNumber, xNumber-1);
        // cout << "patterns selected x " << patternsSelectedX << endl;
        if (*indexNumber < patternsSelectedX){
            return 2ll;
        }
        *indexNumber -= patternsSelectedX;
    }
    return 0ll;
}

ll generateState(ll indexNumber, int oNumber, int xNumber){
    // cout << "generate state" << endl;
    // change to state from indexNumber
    // it is possible to represent state using indexNumber but it is difficult to find symmetric states using indexNumber.
    ll remainingIndexNumber = indexNumber;
    int remainingONumber = oNumber;
    int remainingXNumber = xNumber;
    ll newState = 0ll;
    for(int i=combinationSize;i>0;i--){
        ll mark = generateMark(i, &remainingIndexNumber, remainingONumber, remainingXNumber);
        newState = (newState << 2) + mark;
        if (mark == 1ll){
            remainingONumber--;
        }else if (mark == 2ll){
            remainingXNumber--;
        }
        // cout << "hhoo" << mark << ", " << remainingIndexNumber << ", " << remainingONumber << ", " << remainingXNumber << endl;
        
    }
    // cout << "end generate state" << endl;
    return newState;
}

ll generateIndexNumber(ll stateNumber){
    // TODO implement:
    // cout << "generate index number" << endl;
    ll indexNumber = 0;
    int oNumber = 0;
    int xNumber = 0;
    ll mark;
    for(int i=0;i<combinationSize;i++){
        mark = getRightMark(stateNumber);
        // cout << "mark = " << mark << endl;
        // cout << indexNumber << ", " << oNumber << ", " << xNumber << endl;
        stateNumber = stateNumber >> 2;
        if(mark == 1ll){
            oNumber++;
        }else if (mark == 2ll){
            xNumber++;
            // not o. if it is possible to select o, add the number of patterns of next states.
            if (oNumber > 0){
                // cout << "oo " << i << "," << oNumber << " " << xNumber << endl; 
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
        }else if (mark == 0ll){
            if (oNumber > 0){
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
            if (xNumber > 0){
                // cout << "xx " << i << "," << oNumber << " " << xNumber << endl; 
                indexNumber += getPatterns(i, oNumber, xNumber-1);
            }

        }
        
    }
    // cout << "end generate index number" << endl;
    return indexNumber;
}

void computeStatesValue(int oNumber, int xNumber){
    // TODO implement: the case no == nx. do not need to use reverse
    // TODO implement:
    // oNumber != xNumber
    // initialize this state value
    // int eNumber = combinationSize - (oNumber + xNumber);
    // we need to compute reverse states at the same time. 
    vector<bool> valaues(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> valauesReverse(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    // read next state value
    // TODO implement

    // compute values from next state values
    // compute values until no update

}

// int main(){
//     cout << "hello" << endl;
//     for(int i=0;i<10;i++){
//         // bitset<i> hoge;
//         cout << i << endl;
//     }
//     const int t = 10;
//     bitset<t> tset;
//     return 0;
// }