/**
 * after computing all states value, compute the step to win.
 * the winner tries to win in minimum steps.
 * the loser tries to lose in max steps.
 * 
 */

#include "state.cpp"

const int DEFAULT_STEP = 200; // TODO: select the good number

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

void updateStepsFromNext(vector<int> *steps, vector<bool> *values, int oNumber, int xNumber){
    // if next state is loss, this state step --> min(this step, next step+1)
    // if next state is win, this state step --> if this step is Default, nextS+1, else max(thisS, nextS)

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
                steps->at(stateI) = min(steps->at(stateI), nextStatesStep[i]+1);
            }
        }else if(isWin(&nextStatesValue, i)){
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int j=0;j<sa.count;j++){
                stateN = sa.states[j];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                if(isLoss(values, stateI)){
                    if(steps->at(stateI) == DEFAULT_STEP){
                        steps->at(stateI) = nextStatesStep[i]+1;
                    }else{
                        steps->at(stateI) = max(steps->at(stateI), nextStatesStep[i]+1);
                    }
                }
                // debug
                if(stateN == 104998){
                    cout << "wrong !!" << steps->at(stateI) << endl;
                    printState(stateNumber);
                    cout << i << "l " << nextStatesStep[i] << endl;
                    printState(stateN);
                    // exit(0);
                }
            }
        }
    }
}

void updateStepFromEndStates(vector<int> *statesStep, int oNumber, int xNumber, vector<bool> *reverseStatesValue, vector<int> *reverseStatesStep){
    // find the states which end of the game in reverseStates, update the states step to 0
    for (ull i=0ll;i<reverseStatesValue->size()/2ll;i++){
        if (isDraw(reverseStatesValue, i)){
            continue; // not the end of the game
        }
        ll stateNumber = generateState(i, xNumber, oNumber);
        int win = isWin(stateNumber); // win:1, lose:-1, draw:0
        if (win != 0){
            // update this state step to 0
            reverseStatesStep->at(i) = 0;
        }
    }
}

bool updateStatesStep(vector<bool> *statesValue, vector<int> *statesStep, vector<int> *statesStepTmp, vector<bool> *reverseStatesValue, vector<int> *reverseStatsStep, int oNumber, int xNumber, int presentStep){
    // if the max next states step == present step -1, then update the state step
    bool continueSearching = false;
    for (ull i=0ll;i<statesStep->size();i++){
        if (isDraw(statesValue, i)){
            continue; // not loss
        }
        if(statesStep->at(i) < presentStep){
            continue; // this state step already decided!
        }
        continueSearching = true;
        // check all next states step
        if (isWin(statesValue, i)){
            // debug
            if(157081 == generateState(i, oNumber, xNumber)){
                cout << "here" << endl;
                // exit(0);
            }
            if(statesStepTmp->at(i) == presentStep){
                statesStep->at(i) = presentStep;
                if(157081 == generateState(i, oNumber, xNumber)){
                    cout << "here2" << endl;
                    exit(0);
                }
                continue;
            }
            // at least one next loss state step is present step-1, update 
            StateArray sa = createNextStates(generateState(i, oNumber, xNumber), false);
            for(int j=0;j<sa.count;j++){
                ll nextStateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
                if(isLoss(reverseStatesValue, nextStateI) && reverseStatsStep->at(nextStateI) == presentStep-1){
                    statesStep->at(i) = presentStep;
                    break;
                }
            }
            if(157081 == generateState(i, oNumber, xNumber)){
                cout << "here23 " << statesStep->at(i) <<  endl;
                // exit(0);
            }
        }else if(isLoss(statesValue, i)){
            // if next max step == present step -1, update
            int nextMaxStep = 0;
            if(statesStepTmp->at(i) != DEFAULT_STEP){
                nextMaxStep = statesStepTmp->at(i) - 1;
            }
            StateArray sa = createNextStates(generateState(i, oNumber, xNumber), false);
            for(int j=0;j<sa.count;j++){
                ll nextStateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
                nextMaxStep = max(nextMaxStep, reverseStatsStep->at(nextStateI));
            }
            if(nextMaxStep == presentStep-1){
                statesStep->at(i) = presentStep;
            }
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
    vector<int> statesStepsTmp(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber], DEFAULT_STEP);
    vector<int> reverseStatesStepsTmp(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber], DEFAULT_STEP);

    updateStepsFromNext(&statesStepsTmp, &values, oNumber, xNumber);
    updateStepsFromNext(&reverseStatesStepsTmp, &valuesReverse, xNumber, oNumber);

    updateStepFromEndStates(&statesSteps, oNumber, xNumber, &valuesReverse, &reverseStatesSteps);
    updateStepFromEndStates(&reverseStatesSteps, xNumber, oNumber, &values, &statesSteps);

    // compute steps until no update
    int i = 1;
    for(;i<=DEFAULT_STEP;i++){
        if(i==DEFAULT_STEP){
            cout << "ERROR: reached default step!" << endl;
            exit(0);
        }
        bool continueSearching = false;
        // // check all states
        // if(oNumber==xNumber){
        //     continueSearching = updateStatesStep(&values, &statesSteps, &statesSteps, oNumber, xNumber, i);
        // }else{
        continueSearching = updateStatesStep(&values, &statesSteps, &statesStepsTmp, &valuesReverse, &reverseStatesSteps, oNumber, xNumber, i);
        continueSearching = updateStatesStep(&valuesReverse, &reverseStatesSteps, &reverseStatesStepsTmp, &values, &statesSteps, xNumber, oNumber, i) || continueSearching;
        // }
        if (!continueSearching){
            break;
        }
    }
    cout << "loop count " << i << endl;

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