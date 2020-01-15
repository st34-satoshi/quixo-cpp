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
#include <fstream>
#include <sstream>


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

string fileName(int oNumber, int xNumber){ostringstream osO, osX;
    osO << oNumber;
    osX << xNumber;
    return "results/testo"+osO.str()+"x"+osX.str()+".bin";
}

void writeStatesValue(vector<bool> *values, int oNumber, int xNumber){
    // TODO if the file already exist, do not overwrite
    ofstream fout(fileName(oNumber, xNumber), ios::out | ios::binary);
    if(!fout.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    for(auto t: *values){
        fout.put(t);
    }
    fout.close();
}

void readStatesValue(vector<bool> *values, int oNumber, int xNumber){
    ifstream fin(fileName(oNumber, xNumber), ios::in | ios::binary);
    if(!fin.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    char data;
    ll i = 0ll;
    while (fin.get(data)){
        values->at(i++) = data;
    }
    fin.close();
}

/*
 update all values
*/

bool isLoseState(ll indexState, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAddO){
    // cout << "start isLoseState" << endl;
    // if all next states are win this state is lose
    ll thisState = generateState(indexState, oNumber, xNumber);
    // next states are reverse of o and x.
    auto nextStatesReverse = createNextStates(thisState, /*chooseEmpty*/false);
    // cout << "start isLoseState same" << endl;
    for (auto state : nextStatesReverse){
        ll indexNextState = generateIndexNumber(state);
        if(!nextStatesValuesSame->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    // cout << "start isLoseState add o" << endl;
    // next states are nextState the number of o increase
    auto nextStatesAddO = createNextStates(thisState, true);
    for (auto state : nextStatesAddO){
        ll indexNextState = generateIndexNumber(state);
        // cout << indexNextState << endl;
        if(!nextStatesValuesAddO->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    return true;
}

bool updateValues(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd ){
    cout << "start update values" << endl;
    bool updated = false;
    for (ll i=0ll;i<values->size()/2ll;i++){
        if (values->at(i*2ll) || values->at(i*2ll+1ll)){
            // lose or win --> skip
            continue;
        }
        // TODO refactor !!! isLoseState
        if (isLoseState(i, oNumber, xNumber, nextStatesValuesSame, nextStatesValuesAdd)){
            // update this state to lose and change previous states to win
            updated = true;
            ll stateNumber = generateState(i, oNumber, xNumber);
            // update all symmetric states
            for(auto stateN : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(stateN);
                values->at(stateI*2ll+1ll) = 1; // 00 --> 01 (draw --> lose)
            }
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, false)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    nextStatesValuesSame->at(stateI*2ll) = 1;
                    nextStatesValuesSame->at(stateI*2ll+1ll) = 0;
                }
                
            }
        }
    }
    cout << "end update values" << endl;
    return updated;
}

void updateValuesFromEnd(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd){
    // cout << "start update values from end" << endl;
    // find the states which end of the game, and change previous states (to win)
    // oNumber is for values
    for (ll i=0ll;i<nextStatesValuesSame->size()/2ll;i++){
        if (nextStatesValuesSame->at(i*2ll) || nextStatesValuesSame->at(i*2ll+1ll)){
            // lose or win --> skip. this precious states already updated
            continue;
        }
        ll stateNumber = generateState(i, xNumber, oNumber);
        int win = isWin(stateNumber); // win:1, lose:-1, draw:0
        if (win == -1){
            // update this state to lose and change previous states to win
            // update all symmetric states
            for(auto stateN : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(stateN);
                if (nextStatesValuesSame->at(stateI*2ll)){ // TODO remove this is for debug
                    cout << "strange bug this state already decided" << endl;
                    cout << nextStatesValuesSame->at(stateI*2ll) << nextStatesValuesSame->at(stateI*2ll+1ll) << endl;
                    printState(stateN);
                }
                nextStatesValuesSame->at(stateI*2ll+1ll) = 1; // 00 --> 01 (draw --> lose)
            }
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/false)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    if (values->at(stateI*2ll+1ll)){
                        cout << "Strange bug. this" << endl;
                    }
                    values->at(stateI*2ll) = 1;
                }
            }
        }else if (win == 1){
            if (nextStatesValuesSame->at(i*2ll+1ll)){
                cout << "strange bug to win" << endl;
            }
            // TODO for symmetric states!!
            for(auto state : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(state);
                if (nextStatesValuesSame->at(stateI*2ll+1ll)){
                    cout << "Strange bug. this state win but it is lose." << endl;
                }
                nextStatesValuesSame->at(stateI*2ll) = 1; // 00 --> 10 (draw --> win)
            }
        }
    }
    // find next lose state update this state to win
    for (ll i=0ll;i<nextStatesValuesAdd->size()/2ll;i++){
        if (!nextStatesValuesAdd->at(i*2ll+1ll)){ // lose = 01
            // not lose --> skip
            continue;
        }
        ll stateNumber = generateState(i, xNumber, oNumber);
        // generate previous states, update to win
        for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/true)){
            for(auto s : symmetricAllStates(stateN)){
                ll stateI = generateIndexNumber(s);
                values->at(stateI*2ll) = 1;
                // values->at(stateI*2ll+1ll) = 0;
            }
            
        }
    }

    // cout << "end update values from end" << endl;
}

void computeStatesValue(int oNumber, int xNumber){
    // TODO implement: the case no == nx. do not need to use reverse
    // 00: unknown(draw)
    // 01: lose
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
        vector<bool> nv(combinations[combinationSize][xNumber+1] * combinations[(combinationSize-xNumber-1)][oNumber] * 2);
        readStatesValue(&nv, xNumber+1, oNumber);
        nextValues = nv;
        vector<bool> nrv(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber+1] * 2);
        readStatesValue(&nrv, oNumber, xNumber+1);
        nextReverseValues = nrv;
    }

    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    updateValuesFromEnd(&values, oNumber, xNumber, &valuesReverse, &nextValues);
    updateValuesFromEnd(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues);

    // calculate values until no update
    bool updated = true;
    while (updated){
        updated = false;
        // check all states
        updated = updateValues(&values, oNumber, xNumber, &valuesReverse, &nextValues);
        updated = updateValues(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues) || updated;
    }
    // save resutl to strage
    writeStatesValue(&values, oNumber, xNumber);
    writeStatesValue(&valuesReverse, xNumber, oNumber);
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