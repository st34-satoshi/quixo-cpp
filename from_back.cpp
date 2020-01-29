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
c++ -std=c++17 -Wall -O3 from_back.cpp 
./a.out
*/


#include "state.cpp"

void init(){
    createCombinations();
    initState();
}

/*
 update all values
*/

bool isLoseState(ll indexState, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAddO){
    // if all next states are win this state is lose
    ll thisState = generateState(indexState, oNumber, xNumber);
    // next states are reverse of o and x.
    auto nextStatesReverse = createNextStates(thisState, /*chooseEmpty*/false);
    for (auto state : nextStatesReverse){
        ll indexNextState = generateIndexNumber(state);
        if(!nextStatesValuesSame->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    // next states are nextState the number of o increase
    auto nextStatesAddO = createNextStates(thisState, true);
    for (auto state : nextStatesAddO){
        ll indexNextState = generateIndexNumber(state);
        if(!nextStatesValuesAddO->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    return true;
}

bool updateValues(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd ){
    bool updated = false;
    for (ull i=0ll;i<values->size()/2ll;i++){
        if (values->at(i*2ll) || values->at(i*2ll+1ll)){
            // lose or win --> skip
            continue;
        }
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
    return updated;
}

void updateValuesFromEnd(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd){
    // find the states which end of the game, and change previous states (to win)
    // oNumber is for values

    for (ull i=0ll;i<nextStatesValuesSame->size()/2ll;i++){
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
                if (nextStatesValuesSame->at(stateI*2ll)){ // this is for debug
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
            // for symmetric states!!
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
    for (ull i=0ll;i<nextStatesValuesAdd->size()/2ll;i++){
        
        if (!nextStatesValuesAdd->at(i*2ll+1ll)){ // lose = 01
            // not lose --> skip
            continue;
        }
        
        ll stateNumber = generateState(i, xNumber, oNumber+1);
        
        // generate previous states, update to win
        for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/true)){
            for(auto s : symmetricAllStates(stateN)){
                ll stateI = generateIndexNumber(s);
                values->at(stateI*2ll) = 1;
            }
            
        }
    }
}

void computeStatesValue(int oNumber, int xNumber){
    // TODO implement: the case no == nx. do not need to use reverse???
    // 00: unknown(draw)
    // 01: lose
    // 10: win
    // 11: ??
    // you need to check only first bit to know the state is win (it is used to check the previous state is lose?) 

    // oNumber != xNumber
    // initialize this state value
    // we need to compute reverse states at the same time. 
    vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> valuesReverse(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    
    // read next state value
    vector<bool> nextValues(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextValues, xNumber, oNumber+1);
    vector<bool> nextReverseValues(getCombination(combinationSize, oNumber)*getCombination(combinationSize-oNumber, xNumber+1) * 2); // next states values of valuesReverse
    readStatesValue(&nextReverseValues, oNumber, xNumber+1);

    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    cout << "staert search" << nextValues.size() << "," << nextReverseValues.size() << endl;
    updateValuesFromEnd(&values, oNumber, xNumber, &valuesReverse, &nextValues);
    updateValuesFromEnd(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues);

    // compute values until no update
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

void computeAllStatesValue(){
    // compute from end(o+x=combinationSize)
    for(int total=combinationSize; total>=0 ;total--){
        cout << "total = " << total << endl;
        for(int oNumber=0;oNumber<=total/2;oNumber++){
            cout << "o number = " << oNumber << endl;
            int xNumber = total - oNumber;
            computeStatesValue(oNumber, xNumber);
        }
    }
}

int main(){
    clock_t start = clock();
    init();
    computeAllStatesValue();
    clock_t end = clock();
    cout << "end : " << (double)(end - start)/ CLOCKS_PER_SEC << " sec" << endl;
    return 0;
}