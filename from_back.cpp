/*
Next player is o.
states are represented by the number of index(i).
bitset saves 2 bits.
00: unknown(draw)
01: lose
10: win
11: ??

but it is difficult to create next states and previous states using indexNumber.
we use this state.
states are represented by long long.
at first 32bit is for o, and last(small) 32bit is for x 
0:empty, 1:exist
ox-
o-o
--x
is
0*(32-9)_100_101_000_0*(32-9)_010_000_001

you can change size by boardSize.

Run
c++ -std=c++17 -Wall -O3 from_back.cpp 
./a.out
*/


#include "state-ox-different.cpp"
#include "encode.cpp"

void init(){
    createCombinations();
    initState();
    initMoving();
}

bool isLoseState(ll indexState, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAddO, Encoder encT, Encoder encR, Encoder encN){
    // Encoder encT = Encoder(oNumber, xNumber);
    // Encoder encR = Encoder(xNumber, oNumber);
    // Encoder encN = Encoder(xNumber, oNumber+1);
    // if all next states are win this state is lose
    ll thisState = encT.generateState(indexState);
    // next states are reverse of o and x.
    auto nextStatesReverse = createNextStates(thisState, /*chooseEmpty*/false);
    for (auto state : nextStatesReverse){
        // cout << "46  //////" << endl;
        // checkStateError(state, xNumber, oNumber);
        ll indexNextState = encR.generateIndex(state);
        if(!nextStatesValuesSame->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    // next states are nextState the number of o increase
    auto nextStatesAddO = createNextStates(thisState, true);
    for (auto state : nextStatesAddO){
        // cout << "58  //////" << endl;
        // checkStateError(state, xNumber, oNumber+1);
        ll indexNextState = encN.generateIndex(state);
        if(!nextStatesValuesAddO->at(indexNextState*2)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    return true;
}

bool updateValues(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd ){
    // TODO refactor not need to initialize here?
    Encoder encT = Encoder(oNumber, xNumber);
    Encoder encR = Encoder(xNumber, oNumber);
    Encoder encN = Encoder(xNumber, oNumber+1);
    cout << "created encoder" << endl;
    bool updated = false;
    for (ull i=0ll;i<values->size()/2ll;i++){
        if (values->at(i*2ll) || values->at(i*2ll+1ll)){
            // lose or win --> skip
            continue;
        }
        if (isLoseState(i, oNumber, xNumber, nextStatesValuesSame, nextStatesValuesAdd, encT, encR, encN)){
            // update this state to lose and change previous states to win
            updated = true;
            ll stateNumber = encT.generateState(i);
            // update all symmetric states
            // TODO refactor, needless exchange!
            ll stateI = encT.generateIndex(stateNumber);
            values->at(stateI*2ll+1ll) = 1; // 00 --> 01 (draw --> lose)
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, false)){
                // cout << "91  //////" << endl;
                // printState(stateNumber);
                // printState(stateN);
                // checkStateError(stateN, xNumber, oNumber);
                ll stateI = encR.generateIndex(stateN);
                nextStatesValuesSame->at(stateI*2ll) = 1;
                nextStatesValuesSame->at(stateI*2ll+1ll) = 0;
            }
        }
    }
    return updated;
}

void updateValuesFromEnd(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAdd){
    // find the states which end of the game, and change previous states (to win)
    // oNumber is for values
    // cout << "start update value from end" << endl;
    Encoder encT = Encoder(oNumber, xNumber);
    Encoder encR = Encoder(xNumber, oNumber);
    Encoder encN = Encoder(xNumber, oNumber+1);
    // cout << "gg" << endl;

    for (ull i=0ll;i<nextStatesValuesSame->size()/2ll;i++){
        if (nextStatesValuesSame->at(i*2ll) || nextStatesValuesSame->at(i*2ll+1ll)){
            // lose or win --> skip. this precious states already updated
            continue;
        }
        ll stateNumber = encR.generateState(i);
        checkStateError(stateNumber, xNumber, oNumber);
        int win = isWin(stateNumber); // win:1, lose:-1, draw:0
        if (win == -1){
            // update this state to lose and change previous states to win
            // cout << "121  //////" << endl;
            // checkStateError(stateNumber, xNumber, oNumber);
            ll stateI = encR.generateIndex(stateNumber);
            if (nextStatesValuesSame->at(stateI*2ll)){ // this is for debug
                cout << "strange bug this state already decided" << endl;
                cout << nextStatesValuesSame->at(stateI*2ll) << nextStatesValuesSame->at(stateI*2ll+1ll) << endl;
                printState(stateNumber);
            }
            nextStatesValuesSame->at(stateI*2ll+1ll) = 1; // 00 --> 01 (draw --> lose)

            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/false)){
                // cout << "133  //////" << endl;
                // checkStateError(stateN, oNumber, xNumber);
                ll stateI = encT.generateIndex(stateN);
                if (values->at(stateI*2ll+1ll)){
                    cout << "Strange bug. this" << endl;
                }
                values->at(stateI*2ll) = 1;
            }
        }else if (win == 1){
            if (nextStatesValuesSame->at(i*2ll+1ll)){
                cout << "strange bug to win" << endl;
            }
            // cout << "145  //////" << endl;
            // checkStateError(stateNumber, xNumber, oNumber);
            ll stateI = encR.generateIndex(stateNumber);
            if (nextStatesValuesSame->at(stateI*2ll+1ll)){
                cout << "Strange bug. this state win but it is lose." << endl;
            }
            nextStatesValuesSame->at(stateI*2ll) = 1; // 00 --> 10 (draw --> win)
        }
    }

    // find next lose state update this state to win
    for (ull i=0ll;i<nextStatesValuesAdd->size()/2ll;i++){
        
        if (!nextStatesValuesAdd->at(i*2ll+1ll)){ // lose = 01
            // not lose --> skip
            continue;
        }
        
        ll stateNumber = encN.generateState(i);
        checkStateError(stateNumber, xNumber, oNumber+1);
        
        // generate previous states, update to win
        for( auto stateN : createPreviousStates(stateNumber, true)){
            // cout << "168  //////" << endl;
            // printState(stateNumber);
            // printState(stateN);
            // checkStateError(stateN, oNumber, xNumber);
            ll stateI = encT.generateIndex(stateN);
            values->at(stateI*2ll) = 1;
        }
    }
}

void computeStatesValue(int oNumber, int xNumber){
    // cout << "start computing" << endl;
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
    
    cout << "read values from file" << endl;
    // read next state value
    vector<bool> nextValues(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextValues, xNumber, oNumber+1);
    vector<bool> nextReverseValues(getCombination(combinationSize, oNumber)*getCombination(combinationSize-oNumber, xNumber+1) * 2); // next states values of valuesReverse
    readStatesValue(&nextReverseValues, oNumber, xNumber+1);

    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    // cout << "staert search" << nextValues.size() << "," << nextReverseValues.size() << endl;
    cout << "update from end" << endl; 
    updateValuesFromEnd(&values, oNumber, xNumber, &valuesReverse, &nextValues);
    updateValuesFromEnd(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues);

    cout << "update loop" << endl;
    // compute values until no update
    bool updated = true;
    int c = 0;
    while (updated){
        cout << "loop count = " << c++ << endl;
        updated = false;
        // check all states
        // cout << "start update values" << endl;
        updated = updateValues(&values, oNumber, xNumber, &valuesReverse, &nextValues);
        updated = updateValues(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues) || updated;
    }
    cout << "save to file" << endl;
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