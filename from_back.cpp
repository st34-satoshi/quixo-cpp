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
    bool isDefault(ll index){
        return !(statesValue[index*2ll]) && !(statesValue[index*2ll + 1ll]);
    }
    bool isNotDefault(ll index){
        return statesValue[index*2ll] || statesValue[index*2ll + 1ll];
    }
    void updateToWin(ll index){
        statesValue[index*2ll] = false;
        statesValue[index*2ll + 1ll] = true;
    }
    void updateToWinOrDraw(ll index){
        statesValue[index*2ll] = true;
        statesValue[index*2ll + 1ll] = false;
    }
    void updateToLoss(ll index){
        statesValue[index*2ll] = true;
        statesValue[index*2ll + 1ll] = true;
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
    void writeStatesValue(int oNumber, int xNumber){
        ofstream fout(fileName(oNumber, xNumber, "Value"), ios::out | ios::binary);
        if(!fout.is_open()){
            cout << "cannot open file" << endl;
            return;
        }
        int bitCounter = 0;
        unsigned char c = 0;
        ull statesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber]*2ll;
        for(ull i=0ll;i<statesSize;i++){
            bool t = statesValue[i];
            c = c << 1;
            c += t;
            bitCounter++;
            if (bitCounter == 8){
                fout.put(c);
                c = 0;
                bitCounter = 0;
            }
        }
        if (bitCounter != 0){
            fout.put(c);
        }
        fout.close();
    }

};

StatesValue nextStatesValue; // gloval values!

inline bool isWin(vector<bool> *values, ll index){
    return (!values->at(index*2ll) && values->at(index*2ll + 1ll));
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


struct PresentStatesValue: StatesValue{

    void updateValuesFromNext(int oNumber, int xNumber){
        // if next state is loss, this state --> win
        // if next state is draw(or winOrDraw) --> winOrDraw
        // oNumber is for values

        // read next states from the file. save to the gloval vralue.
        if(!nextStatesValue.read(xNumber, oNumber+1)){
            return;
        }
        ull nextStatesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber+1];
        for (ull i=0ll;i<nextStatesSize;i++){
            if(nextStatesValue.isLoss(i)){ 
                ll stateNumber = generateState(i, xNumber, oNumber+1);
                StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
                ll stateN, stateI;
                for(int j=0;j<sa.count;j++){
                    stateN = sa.states[j];
                    stateI = generateIndexNumber(stateN, oNumber, xNumber);
                    updateToWin(stateI);
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
                    if (isDefault(stateI)){
                        updateToWinOrDraw(stateI);
                    }
                }
            }
        }
    }

    void updateValuesFromEndStates(int oNumber, int xNumber, PresentStatesValue *reverseSV){
        // find the states which end of the game, update the state and update previous states (to win)
        // oNumber is for values
        // update this struct using reverseStatesValues, but update the reverseStatesValue which is end of the game(has a line).

        ull statesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber];
        for (ull i=0ll;i<statesSize;i++){
            if (reverseSV->isNotDefault(i)){
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
                    reverseSV->updateToLoss(stateI);
                }
                // generate previous states, update to win
                StateArray sa = createPreviousStates(stateNumber, false);
                ll stateN, stateI;
                for(int j=0;j<sa.count;j++){
                    stateN = sa.states[j];
                    auto symStates = symmetricAllStates(stateN);
                    for(int k=0;k<symStates.size;k++){
                        stateI = generateIndexNumber(symStates.states[k], oNumber, xNumber);
                        updateToWin(stateI);
                    }
                }
            }else if (win == 1){
                auto symStates = symmetricAllStates(stateNumber);
                for(int j=0;j<symStates.size;j++){
                    ll stateI = generateIndexNumber(symStates.states[j], xNumber, oNumber);
                    reverseSV->updateToWin(stateI);
                }
            }
        }
    }
    bool updateValues(int oNumber, int xNumber, PresentStatesValue *reverseSV){
        bool updated = false;
        ull statesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber];
        for (ull i=0ull;i<statesSize;i++){
            if (isNotDefault(i)){
                // lose or win --> skip
                continue;
            }
            if (isLoseState(i, oNumber, xNumber, &reverseSV->statesValue)){
                // update this state to lose and change previous states to win
                updated = true;
                ll stateNumber = generateState(i, oNumber, xNumber);
                // update all symmetric states
                auto symStates = symmetricAllStates(stateNumber);
                for(int j=0;j<symStates.size;j++){
                    ll stateI = generateIndexNumber(symStates.states[j], oNumber, xNumber);
                    updateToLoss(stateI);
                }
                // generate previous states, update to win
                StateArray sa = createPreviousStates(stateNumber, false);
                for(int j=0;j<sa.count;j++){
                    ll stateN = sa.states[j];
                    auto symStates = symmetricAllStates(stateN);
                    for(int k=0;k<symStates.size;k++){
                        ll stateI = generateIndexNumber(symStates.states[k], xNumber, oNumber);
                        reverseSV->updateToWin(stateI);
                    }
                }
            }
        }
        return updated;
    }
};

PresentStatesValue statesValue, reverseStatesValue; // gloval values!

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

void computeStatesValue(int oNumber, int xNumber){
    // we need to compute reverse states at the same time. 

    // initialize this state value
    statesValue.initValues(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValue.initValues(xNumber, oNumber);
    
    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    statesValue.updateValuesFromNext(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValue.updateValuesFromNext(xNumber, oNumber);

    if(oNumber==xNumber) {
        statesValue.updateValuesFromEndStates(oNumber, xNumber, &statesValue);
    }else{
        statesValue.updateValuesFromEndStates(oNumber, xNumber, &reverseStatesValue);
        reverseStatesValue.updateValuesFromEndStates(xNumber, oNumber, &statesValue);
    }

    // compute values until no update
    bool updated = true;
    while (updated){
        updated = false;
        // check all states
        if(oNumber==xNumber){
            updated = statesValue.updateValues(oNumber, xNumber, &statesValue);
        }else{
            updated = statesValue.updateValues(oNumber, xNumber, &reverseStatesValue);
            updated = reverseStatesValue.updateValues(xNumber, oNumber, &statesValue) || updated;
        }
    }
    // save resutl to strage
    statesValue.writeStatesValue(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValue.writeStatesValue(xNumber, oNumber);
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
            if (!needCompute(oNumber, xNumber)) continue;
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