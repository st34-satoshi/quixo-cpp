/*
Next player is o.
states are represented by the number of index(i).
bitset saves 2 bits.
00: lose
01: unknown(draw)
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
    // return mark and reduce number
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
        if (*indexNumber < patternsSelectedO){
            return 1ll;
        }
        *indexNumber -= patternsSelectedO;
    }
    if (xNumber > 0){
        ll patternsSelectedX = getPatterns(spaceNumber-1, oNumber, xNumber-1);
        if (*indexNumber < patternsSelectedX){
            return 2ll;
        }
        *indexNumber -= patternsSelectedX;
    }
    return 0ll;
}

ll generateState(ll indexNumber, int oNumber, int xNumber){
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
    }
    return newState;
}

ll generateIndexNumber(ll stateNumber){
    ll indexNumber = 0;
    int oNumber = 0;
    int xNumber = 0;
    ll mark;
    for(int i=0;i<combinationSize;i++){
        mark = getRightMark(stateNumber);
        stateNumber = stateNumber >> 2;
        if(mark == 1ll){
            oNumber++;
        }else if (mark == 2ll){
            xNumber++;
            // not o. if it is possible to select o, add the number of patterns of next states.
            if (oNumber > 0){
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
        }else if (mark == 0ll){
            if (oNumber > 0){
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
            if (xNumber > 0){
                indexNumber += getPatterns(i, oNumber, xNumber-1);
            }

        }
        
    }
    return indexNumber;
}

void computeStatesValue(int oNumber, int xNumber){
    // TODO implement: the case no == nx. do not need to use reverse
    // 00: lose
    // 01: unknown(draw)
    // 10: win
    // 11: ??

    // TODO implement:
    // oNumber != xNumber
    // initialize this state value
    // int eNumber = combinationSize - (oNumber + xNumber);
    // we need to compute reverse states at the same time. 
    vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> valuesReverse(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    
    // read next state value
    vector<bool> nextValues;  // next states values of values
    vector<bool> nextReverseValues; // next states values of valuesReverse
    // TODO implement
    if (oNumber + xNumber < combinationSize){
        // read next state values. o+1 and x+1
        // compute values from next state values
        // TODO implement
    }

    // compute values until no update
    bool updated = true;
    while (updated){
        updated = false;
        // check all states
        for (ll i=0ll;i<values.size();i++){
            if (!values.at(i*2+1)){
                // lose or win --> skip
                continue;
            }
            // if all next states are win this state is lose
            bool lose = true;
            ll thisState = generateState(i, oNumber, xNumber);
            // next states are reverse
            auto nextStatesReverse = createNextStates(thisState, true);
            for (auto state : nextStatesReverse){
                ll indexState = generateIndexNumber(state);
                if(!valuesReverse.at(indexState*2)){
                    // not win (lose or draw)
                    // at least 1 next state is not win. this state is not lose
                    lose = false;
                    break;
                }
            }
            if (!lose){
                continue;
            }
            // next states are nextState the number of o increase
            auto nextStatesAddO = createNextStates(thisState, false);
            for (auto state : nextStatesAddO){
                ll indexState = generateIndexNumber(state);
                if(!nextValues.at(indexState*2)){
                    // not win (lose or draw)
                    // at least 1 next state is not win. this state is not lose
                    lose = false;
                    break;
                }
            }
            if (lose){
                // update to lose and change previous states to win
                // TODO implement
                updated = true;
                values.at(i*2+1) = 0; // 01 --> 00
                // TODO generate previous states, update to win
            }
        }
        // TODO valuesReverse
    }

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