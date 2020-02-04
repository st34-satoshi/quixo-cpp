/**
 * after computing all states value, compute the step to win.
 * the winner tries to win in minimum steps.
 * the loser tries to lose in max steps.
 * 
 */

#include "state.cpp"

const int DEFAULT_STEP = 1000;

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

inline void updateWinStep(vector<int> *stateStep, int step, int i){
    stateStep->at(i) = min(step, stateStep->at(i));
}

inline void updateLossStep(vector<int> *stateStep, int step, int i){
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

    // for (ull i=0ll;i<nextStatesValue.size();i++){
    //     if(isLoss(&nextStatesValue, i)){
    //         ll stateNumber = generateState(i, xNumber, oNumber+1);
    //         StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
    //         ll stateN, stateI;
    //         for(int i=0;i<sa.count;i++){
    //             stateN = sa.states[i];
    //             stateI = generateIndexNumber(stateN, oNumber, xNumber);
    //             updateToWin(values, stateI);
    //         }
    //     }else if(isDraw(&nextStatesValues, i)){
    //         // default or winOrDraw
    //         ll stateNumber = generateState(i, xNumber, oNumber+1);
    //         // generate previous states, update to winOrDraw(not loss)
    //         StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
    //         ll stateN, stateI;
    //         for(int i=0;i<sa.count;i++){
    //             stateN = sa.states[i];
    //             stateI = generateIndexNumber(stateN, oNumber, xNumber);
    //             if (isDefault(values, stateI)){
    //                 updateToWinOrDraw(values, stateI);
    //             }
    //         }
    //     }
    // }
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

    // // at first find next lose states and update this values to win
    // // find the states which end of the game, if it is lose update previous state to win
    // updateStepsFromNext(&values, oNumber, xNumber);
    // updateValuesFromNext(&valuesReverse, xNumber, oNumber);

    // updateValuesFromEndStates(&values, oNumber, xNumber, &valuesReverse);
    // updateValuesFromEndStates(&valuesReverse, xNumber, oNumber, &values);

    // // compute values until no update
    // bool updated = true;
    // while (updated){
    //     updated = false;
    //     // check all states
    //     updated = updateValues(&values, oNumber, xNumber, &valuesReverse);
    //     updated = updateValues(&valuesReverse, xNumber, oNumber, &values) || updated;
    // }
    // // save resutl to strage
    // writeStatesValue(&values, oNumber, xNumber);
    // writeStatesValue(&valuesReverse, xNumber, oNumber);
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