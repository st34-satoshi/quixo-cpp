/*
compute the state is reachable or not
*/

#include "state.cpp"

void init(){
    createCombinations();
    initState();
    initMovingMasks();
    initEncoding();
}

vector<bool> statesReachable; // 2*i: false is not reachable, true: reachable, 2*i+1: false: not searched, true: searched
vector<bool> reverseStatesReachable;

bool updateValues(vector<bool> *values, int oNumber, int xNumber, vector<bool> *reverseStatesValues){
    bool updated = false;
    for (ull i=0ll;i<values->size()/2ll;i++){
        if (isNotDefault(values, i)){
        // if (values->at(i*2ll) || values->at(i*2ll+1ll)){
            // lose or win --> skip
            continue;
        }
        if (isLoseState(i, oNumber, xNumber, reverseStatesValues)){
            // update this state to lose and change previous states to win
            updated = true;
            ll stateNumber = generateState(i, oNumber, xNumber);
            // update all symmetric states
            for(auto stateN : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateToLoss(values, stateI);
            }
            // generate previous states, update to win
            StateArray sa = createPreviousStates(stateNumber, false);
            for(int i=0;i<sa.count;i++){
                ll stateN = sa.states[i];
                for(auto s : symmetricAllStates(stateN)){
                    ll stateI = generateIndexNumber(s, xNumber, oNumber);
                    updateToWin(reverseStatesValues, stateI);
                }
            }
        }
    }
    return updated;
}

void updateValuesFromNext(vector<bool> *values, int oNumber, int xNumber){
    // if next state is loss, this state --> win
    // if next state is draw(or winOrDraw) --> winOrDraw
    // oNumber is for values

    // read next states from the file
    vector<bool> nextStatesValues(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextStatesValues, xNumber, oNumber+1);

    for (ull i=0ll;i<nextStatesValues.size()/2ll;i++){
        if(isLoss(&nextStatesValues, i)){
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateToWin(values, stateI);
            }
        }else if(isDraw(&nextStatesValues, i)){
            // default or winOrDraw
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            // generate previous states, update to winOrDraw(not loss)
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                if (isDefault(values, stateI)){
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
                ll stateI = generateIndexNumber(stateN, xNumber, oNumber);
                updateToLoss(reverseStatesValues, stateI);
            }
            // generate previous states, update to win
            StateArray sa = createPreviousStates(stateNumber, false);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                for(auto s : symmetricAllStates(stateN)){
                    stateI = generateIndexNumber(s, oNumber, xNumber);
                    updateToWin(values, stateI);
                }
            }
        }else if (win == 1){
            for(auto state : symmetricAllStates(stateNumber)){
                ll stateI = generateIndexNumber(state, xNumber, oNumber);
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
    
    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    updateValuesFromNext(&values, oNumber, xNumber);
    updateValuesFromNext(&valuesReverse, xNumber, oNumber);

    updateValuesFromEndStates(&values, oNumber, xNumber, &valuesReverse);
    updateValuesFromEndStates(&valuesReverse, xNumber, oNumber, &values);

    // compute values until no update
    bool updated = true;
    while (updated){
        updated = false;
        // check all states
        updated = updateValues(&values, oNumber, xNumber, &valuesReverse);
        updated = updateValues(&valuesReverse, xNumber, oNumber, &values) || updated;
    }
    // save resutl to strage
    writeStatesValue(&values, oNumber, xNumber);
    writeStatesValue(&valuesReverse, xNumber, oNumber);
}

void computeAllStatesValue(){
    // compute from end(o+x=combinationSize)
    for(int total=combinationSize; total>=0;total--){
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