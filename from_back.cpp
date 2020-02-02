/*
Next player is o.
states are represented by the number of index(i).
bitset saves 2 bits.

00: default(draw)
01: win or draw (later draw)
10: win, 2*i=0 2*i+1=1
11: loss

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

inline void updateToWin(vector<bool> *values, ll index){
    values->at(index*2ll) = false;
    values->at(index*2ll + 1ll) = true;
}

inline void updateToLoss(vector<bool> *values, ll index){
    values->at(index*2ll) = true;
    values->at(index*2ll + 1ll) = true;
}

inline void updateToWinOrDraw(vector<bool> *values, ll index){
    values->at(index*2ll) = true;
    values->at(index*2ll + 1ll) = false;
}

inline bool isWin(vector<bool> *values, ll index){
    return (!values->at(index*2ll) && values->at(index*2ll + 1ll));
}

inline bool isLoss(vector<bool> *values, ll index){
    return values->at(index*2ll) && values->at(index*2ll + 1ll);
}

inline bool isWinOrDraw(vector<bool> *values, ll index){
    return values->at(index*2ll) && !(values->at(index*2ll + 1ll));
}

inline bool isDefault(vector<bool> *values, ll index){
    return !(values->at(index*2ll)) && !(values->at(index*2ll + 1ll));
}

inline bool isDraw(vector<bool> *values, ll index){
    // default or winOrDraw
    return !(values->at(index*2ll + 1ll));
}

inline bool isNotDefault(vector<bool> *values, ll index){
    return values->at(index*2ll) || values->at(index*2ll + 1ll);
}

bool isLoseState(ll indexState, int oNumber, int xNumber, vector<bool> *nextStatesValuesSame, vector<bool> *nextStatesValuesAddO){
    // if all next states are win this state is lose
    ll thisState = generateState(indexState, oNumber, xNumber);
    // next states are reverse of o and x.
    auto nextStatesReverse = createNextStates(thisState, /*chooseEmpty*/false);
    for (auto state : nextStatesReverse){
        ll indexNextState = generateIndexNumber(state);
        if(!isWin(nextStatesValuesSame, indexNextState)){
        // if(!nextStatesValuesSame->at(indexNextState*2ll)){
            // not win (lose or draw)
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    // next states are nextState the number of o increase
    auto nextStatesAddO = createNextStates(thisState, true);
    for (auto state : nextStatesAddO){
        ll indexNextState = generateIndexNumber(state);
        if(!isWin(nextStatesValuesAddO, indexNextState)){
        // if(!nextStatesValuesAddO->at(indexNextState*2ll)){
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
        if (isNotDefault(values, i)){
        // if (values->at(i*2ll) || values->at(i*2ll+1ll)){
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
                updateToLoss(values, stateI);
            }
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, false)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    updateToWin(nextStatesValuesSame, stateI);
                }
                
            }
        }
    }
    return updated;
}

void updateValuesFromNext(vector<bool> *values, int oNumber, int xNumber, vector<bool> *nextStatesValues){
    // if next state is loss, this state --> win
    // if next state is draw(or winOrDraw) --> winOrDraw
    // oNumber is for values
    for (ull i=0ll;i<nextStatesValues->size()/2ll;i++){
        if(isLoss(nextStatesValues, i)){
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/true)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    updateToWin(values, stateI);
                }
            }
        }else if(isDraw(nextStatesValues, i)){
            // default or winOrDraw
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            // generate previous states, update to winOrDraw(not loss)
            for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/true)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    updateToWinOrDraw(values, stateI);
                }
            }
        }
    }
}

void updateValuesFromEndStates(vector<bool> *values, int oNumber, int xNumber, vector<bool> *reverseStatesValues){
    // find the states which end of the game, update the state and update previous states (to win)
    // oNumber is for values

    for (ull i=0ll;i<reverseStatesValues->size()/2ll;i++){
        if (isNotDefault(reverseStatesValues, i)){
            // loss, win or winOrDraw --> skip. this is not the end of the game
            continue;
        }
        ll stateNumber = generateState(i, xNumber, oNumber);
        int win = isWin(stateNumber); // win:1, lose:-1, draw:0
        if (win == -1){
            // update this state to lose and change previous states to win
            for(auto stateN : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(stateN);
                updateToLoss(reverseStatesValues, stateI);
            }
            // generate previous states, update to win
            for( auto stateN : createPreviousStates(stateNumber, /*fromEmpty*/false)){
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s);
                    updateToWin(values, stateI);
                }
            }
        }else if (win == 1){
            for(auto state : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(state);
                updateToWin(reverseStatesValues, stateI);
            }
        }
    }
}

void computeStatesValue(int oNumber, int xNumber){
    // TODO implement: the case no == nx. do not need to use reverse???
    // oNumber != xNumber
    // initialize this state value
    // we need to compute reverse states at the same time. 
    vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> valuesReverse(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    
    // read next state value
    // TODO remove this, read in the function
    vector<bool> nextValues(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextValues, xNumber, oNumber+1);
    vector<bool> nextReverseValues(getCombination(combinationSize, oNumber)*getCombination(combinationSize-oNumber, xNumber+1) * 2); // next states values of valuesReverse
    readStatesValue(&nextReverseValues, oNumber, xNumber+1);

    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    cout << "staert search" << nextValues.size() << "," << nextReverseValues.size() << endl;
    updateValuesFromNext(&values, oNumber, xNumber, &nextValues);
    updateValuesFromNext(&valuesReverse, xNumber, oNumber, &nextReverseValues);

    updateValuesFromEndStates(&values, oNumber, xNumber, &valuesReverse);
    updateValuesFromEndStates(&valuesReverse, xNumber, oNumber, &values);

    // updateValuesFromEnd(&values, oNumber, xNumber, &valuesReverse, &nextValues);
    // updateValuesFromEnd(&valuesReverse, xNumber, oNumber, &values, &nextReverseValues);

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