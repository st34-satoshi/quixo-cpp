/**
 * after computing all states value, compute the step to win.
 * the winner tries to win in minimum steps.
 * the loser tries to lose in max steps.
 * 
 */

#include "state.cpp"

const int DEFAULT_STEP = 100; // TODO: select the good number

void init(){
    createCombinations();
    initState();
    initMovingMasks();
    initEncoding();
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

inline void updateWinStep(vector<int> *stateStep, int step, ll i){
    stateStep->at(i) = min(step, stateStep->at(i));
}

inline void updateLossStep(vector<int> *stateStep, int step, ll i){
    if(stateStep->at(i) == DEFAULT_STEP){
        stateStep->at(i) = step;
    }else{
        stateStep->at(i) = max(step, stateStep->at(i));
    }
}

void updateStepsFromNext(vector<int> *steps, int oNumber, int xNumber){
    // if next state is loss, this state step --> min(this step, next step+1)
    // if next state is win, this state step --> it this step is Default, nextS+1, else max(thisS, nextS)

    // read next states from the file
    vector<bool> nextStatesValue(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextStatesValue, xNumber, oNumber+1);
    // read next states step from file
    vector<int> nextStatesStep(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1));  // next states values of values
    readStatesStep(&nextStatesStep, xNumber, oNumber+1);

    for (ull i=0ll;i<nextStatesStep.size();i++){
        if(isLoss(&nextStatesValue, i)){
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateI;
            for(int j=0;j<sa.count;j++){
                stateI = generateIndexNumber(sa.states[j], oNumber, xNumber);
                updateWinStep(steps, nextStatesStep[i]+1, stateI);
            }
        }else if(isWin(&nextStatesValue, i)){
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateLossStep(steps, nextStatesStep[i]+1, stateI);
            }
        }
    }
}

void updateStepFromEndStates(vector<int> *statesStep, int oNumber, int xNumber, vector<bool> *reverseStatesValue, vector<int> *reverseStatesStep){
    // find the states which end of the game in reverseStates, update the states step to 0
    // and if the end state is loss, update previous states step to 1
    // oNumber is for values

    for (ull i=0ll;i<reverseStatesValue->size()/2ll;i++){
        if (isDraw(reverseStatesValue, i)){
            continue; // not the end of the game
        }
        ll stateNumber = generateState(i, xNumber, oNumber);
        int win = isWin(stateNumber); // win:1, lose:-1, draw:0
        if (win == -1){
            // update this state step to 0 and change previous states step to 1
            reverseStatesStep->at(i) = 0;
            // generate previous states, update to 1
            StateArray sa = createPreviousStates(stateNumber, false);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateWinStep(statesStep, 1, stateI);
            }
        }else if (win == 1){
            reverseStatesStep->at(i) = 0;
            StateArray sa = createPreviousStates(stateNumber, false);
            ll stateN, stateI;
            for(int i=0;i<sa.count;i++){
                stateN = sa.states[i];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateLossStep(statesStep, 1, stateI);
            }
        }
    }
}

bool updateStatesStep(vector<bool> *statesValue, vector<int> *statesStep, vector<int> *reverseStatsStep, int oNumber, int xNumber, int presentStep){
    // when loss state updated, the previous states step update
    // the state is loss and max next states step < i, and this loss state step == i or default, update the state step
    bool continueSearching = false;
    for (ull i=0ll;i<statesStep->size();i++){
        if (!isLoss(statesValue, i)){
            continue; // not loss
        }
        if(statesStep->at(i) < presentStep){
            continue; // this state step already decided!
        }
        continueSearching = true;
        // check all next states step
        StateArray sa = createNextStates(generateState(i, oNumber, xNumber), false);
        int nextMaxStep = 0;
        for(int j=0;j<sa.count;j++){
            ll nextStateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
            nextMaxStep = max(nextMaxStep, reverseStatsStep->at(nextStateI));
        }
        if(nextMaxStep < presentStep){
            if(statesStep->at(i) == presentStep || statesStep->at(i) == DEFAULT_STEP){
                updateLossStep(statesStep, presentStep, i);
                StateArray stateA = createPreviousStates(generateState(i, oNumber, xNumber), false);
                for(int j=0;j<stateA.count;j++){
                    ll pStateI = generateIndexNumber(stateA.states[j], xNumber, oNumber);
                    updateWinStep(reverseStatsStep, presentStep+1, pStateI);
                }
            }
        }else{
            updateLossStep(statesStep, presentStep+1, i); // temporary update.
        }
    }
    return continueSearching;
}

void computeStatesStep(int oNumber, int xNumber){
    // we need to compute reverse states at the same time. 
    // read the value from file
    vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> valuesReverse(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    readStatesValue(&values, oNumber, xNumber);
    readStatesValue(&valuesReverse, xNumber, oNumber);
    // initialize steps
    vector<int> statesSteps(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber], DEFAULT_STEP);
    vector<int> reverseStatesSteps(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber], DEFAULT_STEP);

    updateStepsFromNext(&statesSteps, oNumber, xNumber);
    updateStepsFromNext(&reverseStatesSteps, xNumber, oNumber);

    updateStepFromEndStates(&statesSteps, oNumber, xNumber, &valuesReverse, &reverseStatesSteps);
    updateStepFromEndStates(&reverseStatesSteps, xNumber, oNumber, &values, &statesSteps);

    // compute steps until no update
    for(int i=1;i<=DEFAULT_STEP;i++){
        if(i==DEFAULT_STEP){
            cout << "ERROR: reached default step!" << endl;
            exit(0);
        }
        bool continueSearching = false;
        // // check all states
        if(oNumber==xNumber){
            continueSearching = updateStatesStep(&values, &statesSteps, &statesSteps, oNumber, xNumber, i);
        }else{
            continueSearching = updateStatesStep(&values, &statesSteps, &reverseStatesSteps, oNumber, xNumber, i);
            continueSearching = updateStatesStep(&valuesReverse, &reverseStatesSteps, &statesSteps, xNumber, oNumber, i) || continueSearching;
        }
        if (!continueSearching){
            break;
        }
    }

    // save resutl to strage
    writeStatesSteps(&statesSteps, oNumber, xNumber);
    writeStatesSteps(&reverseStatesSteps, xNumber, oNumber);
}

void computeAllStatesStep(){
    // compute from end(o+x=combinationSize)
    for(int total=combinationSize; total>=0;total--){
        cout << "total = " << total << endl;
        for(int oNumber=0;oNumber<=total/2;oNumber++){
            cout << "o number = " << oNumber << endl;
            int xNumber = total - oNumber;
            computeStatesStep(oNumber, xNumber);
        }
    }
}


int main(){
    clock_t start = clock();
    init();
    computeAllStatesStep();
    clock_t end = clock();
    cout << "end : " << (double)(end - start)/ CLOCKS_PER_SEC << " sec" << endl;
    return 0;
}