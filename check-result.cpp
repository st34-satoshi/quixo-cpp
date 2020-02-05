/**
 * after computing all states value and compute the step to win, check al result.
 * this check algorithm should be wrong!
 * 
 */

#include "state.cpp"


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

void checkStates(vector<int> *statesStep, vector<bool> *statesValue, vector<int> *reverseStatesStep, vector<bool> *reverseStatesValue, int oNumber, int xNumber){
    // read next states from the file
    vector<bool> nextStatesValue(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2);  // next states values of values
    readStatesValue(&nextStatesValue, xNumber, oNumber+1);
    vector<int> nextStatesStep(getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1));  // next states values of values
    readStatesStep(&nextStatesStep, xNumber, oNumber+1);

    for (ull i=0ll;i<statesStep->size();i++){
        if(isLoss(statesValue, i)){
            // all next states are win, the next max steps is this step -1
            ll stateNumber = generateState(i, oNumber, xNumber);
            if(isWin(stateNumber)==-1){
                if(statesStep->at(i)==0){
                    continue;
                }else{
                    cout << "this loss state is at the end but step is not 0" << endl;
                    exit(0);
                }
            }
            StateArray saAdd = createNextStates(stateNumber, /*fromEmpty*/true);
            ll stateI;
            int nextMaxStep = 0;
            for(int j=0;j<saAdd.count;j++){
                stateI = generateIndexNumber(saAdd.states[j], xNumber, oNumber+1);
                if(!isWin(&nextStatesValue, stateI)){
                    cout << "this loss state has not win next state (add) !" << endl;
                    exit(0);
                }
                nextMaxStep = max(nextMaxStep, nextStatesStep[stateI]);
            }
            StateArray saSame = createNextStates(stateNumber, /*fromEmpty*/false);
            for(int j=0;j<saSame.count;j++){
                stateI = generateIndexNumber(saSame.states[j], xNumber, oNumber);
                if(!isWin(reverseStatesValue, stateI)){
                    cout << "this loss state has not win next state (same) !" << endl;
                    exit(0);
                }
                nextMaxStep = max(nextMaxStep, reverseStatesStep->at(stateI));
            }
            if(nextMaxStep != statesStep->at(i)-1){
                cout << "this loss state step is wrong " << nextMaxStep << "," << statesStep->at(i) << endl;
                printState(stateNumber);
                exit(0);
            }
        }else if(isWin(statesValue, i)){
            // at least one next state is loss
            ll stateNumber = generateState(i, oNumber, xNumber);
            if(isWin(stateNumber)==1){
                if(statesStep->at(i)==0){
                    continue;
                }else{
                    cout << "this win state is at the end but step is not 0" << endl;
                    exit(0);
                }
            }
            StateArray saAdd = createNextStates(stateNumber, /*fromEmpty*/true);
            ll stateI;
            int nextMinStep = 1000; // TODO: large number enough
            for(int j=0;j<saAdd.count;j++){
                stateI = generateIndexNumber(saAdd.states[j], xNumber, oNumber+1);
                if(isLoss(&nextStatesValue, stateI)){
                    nextMinStep = min(nextMinStep, nextStatesStep[stateI]);
                }
            }
            StateArray saSame = createNextStates(stateNumber, /*fromEmpty*/false);
            for(int j=0;j<saSame.count;j++){
                stateI = generateIndexNumber(saSame.states[j], xNumber, oNumber);
                if(isLoss(reverseStatesValue, stateI)){
                    nextMinStep = min(nextMinStep, reverseStatesStep->at(stateI));
                }
            }
            if(nextMinStep != statesStep->at(i)-1){
                cout << "this win state step is wrong " << nextMinStep << "," << statesStep->at(i) << "," << i << endl;
                exit(0);
            }
        }else{
            // draw, no next loss states, and at least one draw state
            ll stateNumber = generateState(i, oNumber, xNumber);
            bool ok = false;
            StateArray saAdd = createNextStates(stateNumber, /*fromEmpty*/true);
            ll stateI;
            for(int j=0;j<saAdd.count;j++){
                stateI = generateIndexNumber(saAdd.states[j], xNumber, oNumber+1);
                if(isLoss(&nextStatesValue, stateI)){
                    cout << "this draw state has next loss state " << endl;
                    exit(0);
                }else if(isDraw(&nextStatesValue, stateI)){
                    ok = true;
                    break;
                }
            }
            StateArray saSame = createNextStates(stateNumber, /*fromEmpty*/false);
            for(int j=0;j<saSame.count;j++){
                stateI = generateIndexNumber(saSame.states[j], xNumber, oNumber);
                if(isLoss(reverseStatesValue, stateI)){
                    cout << "this draw state has next loss state (same)" << endl;
                    exit(0);
                }else if(isDraw(reverseStatesValue, stateI)){
                    ok = true;
                    break;
                }
            }
        }
    }
}

void check(int oNumber, int xNumber){
    // read the value from file
    vector<bool> statesValue(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
    vector<bool> reverseStatesValue(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber] * 2);
    readStatesValue(&statesValue, oNumber, xNumber);
    readStatesValue(&reverseStatesValue, xNumber, oNumber);
    // initialize steps
    vector<int> statesStep(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber]);
    vector<int> reverseStatesStep(combinations[combinationSize][xNumber] * combinations[(combinationSize-xNumber)][oNumber]);
    readStatesStep(&statesStep, oNumber, xNumber);
    readStatesStep(&reverseStatesStep, xNumber, oNumber);

    checkStates(&statesStep, &statesValue, &reverseStatesStep, &reverseStatesValue, oNumber, xNumber);

}

void checkAllStates(){
    // compute from end(o+x=combinationSize)
    for(int total=combinationSize; total>=0;total--){
        cout << "total = " << total << endl;
        for(int oNumber=0;oNumber<=total/2;oNumber++){
            cout << "o number = " << oNumber << endl;
            int xNumber = total - oNumber;
            check(oNumber, xNumber);
        }
    }
}


int main(){
    clock_t start = clock();
    init();
    checkAllStates();
    clock_t end = clock();
    cout << "end : " << (double)(end - start)/ CLOCKS_PER_SEC << " sec" << endl;
    return 0;
}