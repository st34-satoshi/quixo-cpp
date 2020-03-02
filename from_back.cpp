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

// global variables. this is reseted at the start of computation of the states set()
struct StatesValue{
    vector<bool> statesValue;

    void initSize(){
        statesValue.resize(MAX_STATES_VALUE*2ll);
    }

    void initValues(int oNumber, int xNumber){
        for(ll i=0ll;i<combinations[combinationSize][oNumber]*combinations[combinationSize-oNumber][xNumber]*2ll;i++){
            statesValue[i] = false;
        }
    }
    bool isLoss(ll index){
        return statesValue[index*2ll] && statesValue[index*2ll + 1ll];
    }
    bool isDraw(ll index){
        // default or winOrDraw
        return !(statesValue[index*2ll + 1ll]);
    }
    bool read(int oNumber, int xNumber){
        ifstream fin(fileName(oNumber, xNumber, "Value"), ios::in | ios::binary);
        if(!fin.is_open()){
            cout << "cannot open file 1" << endl;
            return false;
        }
        unsigned char data;
        ll number = combinations[combinationSize][oNumber]*combinations[combinationSize-oNumber][xNumber]*2ll;
        ll i = 0ll;
        for (;i<number/8;i++){
            data = fin.get();
            ll mask = 1ll << 7;
            for(ll j=0;j<8;j++){
                statesValue[i*8+j] = data & mask;
                mask = mask >> 1;
            }
        }
        ll r = number % 8;
        if (r != 0){
            data = fin.get();
            ll mask = 1 << (r - 1);
            for(ll j=0;j<r;j++){
                statesValue[i*8+j] = data & mask;
                mask = mask >> 1;
            }
        }
        fin.close();
        return true;
    }
};

StatesValue statesValue, reverseStatesValue, nextStatesValue; // gloval values!

void initResizeGloval(){
    statesValue.initSize();
    reverseStatesValue.initSize();
    nextStatesValue.initSize();
}

void init(){
    createCombinations();
    initState();
    initMovingMasks();
    initEncoding();
    initResizeGloval();
}

/*
 update all values
*/

void initGloval(int oNumber, int xNumber){
    statesValue.initValues(oNumber, xNumber);
    nextStatesValue.initValues(xNumber, oNumber);
}

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

inline bool isLoss(vector<bool> *values, ll index){ // TODO: remove
    return values->at(index*2ll) && values->at(index*2ll + 1ll);
}

inline bool isLossState(array<bool, MAX_STATES_VALUE> *statesValue, ll index){
    return statesValue->at(index*2ll) && statesValue->at(index*2ll + 1ll);
}

inline bool isWinOrDraw(vector<bool> *values, ll index){
    return values->at(index*2ll) && !(values->at(index*2ll + 1ll));
}

inline bool isDefault(vector<bool> *values, ll index){
    return !(values->at(index*2ll)) && !(values->at(index*2ll + 1ll));
}

inline bool isDraw(vector<bool> *values, ll index){// TODO: remove
    // default or winOrDraw
    return !(values->at(index*2ll + 1ll));
}

inline bool isNotDefault(vector<bool> *values, ll index){
    return values->at(index*2ll) || values->at(index*2ll + 1ll);
}

bool isLoseState(ll indexState, int oNumber, int xNumber, vector<bool> *reverseStatesValues){
    // if all next states are win this state is lose, no next state, it is loss because the all next states(creating o) are win(if not win this state value is not default)
    ll thisState = generateState(indexState, oNumber, xNumber);
    // next states are reverse of o and x.
    StateArray sa = createNextStates(thisState, /*chooseEmpty*/false);
    for(int i=0;i<sa.count;i++){
    // for (auto state : nextStatesReverse){
        ll indexNextState = generateIndexNumber(sa.states[i], xNumber, oNumber);
        if(!isWin(reverseStatesValues, indexNextState)){
            // at least 1 next state is not win. this state is not lose
            return false;
        }
    }
    return true;
}

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
            auto symStates = symmetricAllStates(stateNumber);
            for(int j=0;j<symStates.size;j++){
                ll stateI = generateIndexNumber(symStates.states[j], oNumber, xNumber);
                updateToLoss(values, stateI);
            }
            // generate previous states, update to win
            StateArray sa = createPreviousStates(stateNumber, false);
            for(int j=0;j<sa.count;j++){
                ll stateN = sa.states[j];
                auto symStates = symmetricAllStates(stateN);
                for(int k=0;k<symStates.size;k++){
                    ll stateI = generateIndexNumber(symStates.states[k], xNumber, oNumber);
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
    // nextStatesValue.initValues(xNumber, oNumber+1);

    // read next states from the file
    if(!nextStatesValue.read(xNumber, oNumber+1)){
        return;
    }
    ull nextStatesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber+1];
    cout << "size = " << nextStatesSize << endl;
    // ull nextStatesSize = getCombination(combinationSize, xNumber)*getCombination(combinationSize-xNumber, oNumber+1) * 2ll;

    for (ull i=0ll;i<nextStatesSize;i++){
        // cout << "i=" << i << endl;
        if(nextStatesValue.isLoss(i)){ // TODO: change vector --> array at once is hard. make the function for array, do not use the function for vector, remove the old functions
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int j=0;j<sa.count;j++){
                stateN = sa.states[j];
                stateI = generateIndexNumber(stateN, oNumber, xNumber);
                updateToWin(values, stateI);
            }
        }else if(nextStatesValue.isDraw(i)){
            // default or winOrDraw
            ll stateNumber = generateState(i, xNumber, oNumber+1);
            // generate previous states, update to winOrDraw(not loss)
            StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
            ll stateN, stateI;
            for(int j=0;j<sa.count;j++){
                stateN = sa.states[j];
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
            auto symStates = symmetricAllStates(stateNumber);
            for(int j=0;j<symStates.size;j++){
                ll stateI = generateIndexNumber(symStates.states[j], xNumber, oNumber);
                updateToLoss(reverseStatesValues, stateI);
            }
            // generate previous states, update to win
            StateArray sa = createPreviousStates(stateNumber, false);
            ll stateN, stateI;
            for(int j=0;j<sa.count;j++){
                stateN = sa.states[j];
                auto symStates = symmetricAllStates(stateN);
                for(int k=0;k<symStates.size;k++){
                    stateI = generateIndexNumber(symStates.states[k], oNumber, xNumber);
                    updateToWin(values, stateI);
                }
            }
        }else if (win == 1){
            auto symStates = symmetricAllStates(stateNumber);
            for(int j=0;j<symStates.size;j++){
                ll stateI = generateIndexNumber(symStates.states[j], xNumber, oNumber);
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
    // TODO: remove comments out
    // initGloval(oNumber, xNumber);  // initialize gloval variables, SatesValue, ReverseStatesValue.
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

bool needCompute(int oNumber, int xNumber){
    string type = "Value";
    return !(isExistFile(oNumber, xNumber, type) && isExistFile(xNumber, oNumber, type));
}

void computeAllStatesValue(){
    // compute from end(o+x=combinationSize)
    for(int total=combinationSize; total>=0;total--){
        cout << "total = " << total << endl;
        for(int oNumber=0;oNumber<=total/2;oNumber++){
            cout << "o number = " << oNumber << endl;
            int xNumber = total - oNumber;
            // if (!needCompute(oNumber, xNumber)) continue;
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