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
// #include "gg.h"

// global variables. this is reseted at the start of computation of the states set()
struct StatesValue{
    array<vector<bool>, MUTEX_NUMBER> statesValue;
    array<mutex, MUTEX_NUMBER> mutexes;

    inline bool getStateValue(ll i){
        // mutexes[i%MUTEX_NUMBER].lock();
        // bool b = statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER];
        // mutexes[i%MUTEX_NUMBER].unlock();
        // return b;
        return statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER];
    }
    // inline bool getStateValueWithoutLock(ll i){
    //     return statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER];
    // }
    inline void setStateValue(ll i, bool b){
        // mutexes[i%MUTEX_NUMBER].lock();
        // statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER] = b;
        // mutexes[i%MUTEX_NUMBER].unlock();
        statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER] = b;
    }
    // inline void setStateValueWithoutLock(ll i, bool b){
    //     statesValue[i%MUTEX_NUMBER][i/MUTEX_NUMBER] = b;
    // }

    void initSize(){
        for(int i=0;i<MUTEX_NUMBER;i++){
            statesValue[i].resize(MAX_STATES_VALUE*2ll/MUTEX_NUMBER + 1); // round up
        }
    }
    void initValues(int oNumber, int xNumber){
        for(ll i=0ll;i<combinations[combinationSize][oNumber]*combinations[combinationSize-oNumber][xNumber]*2ll;i++){
            setStateValue(i, false);
        }
    }

    // check the state
    inline bool isLoss(ll index){
        return getStateValue(index*2ll) && getStateValue(index*2ll + 1ll);
    }
    inline bool isDraw(ll index){
        // default or winOrDraw
        return !(getStateValue(index*2ll + 1ll));
    }
    inline bool isDefault(ll index){
        return !(getStateValue(index*2ll)) && !(getStateValue(index*2ll + 1ll));
    }
    inline bool isNotDefaultLock(ll index){
        mutexes[index%MUTEX_NUMBER].lock();
        bool b = getStateValue(index*2ll) || getStateValue(index*2ll + 1ll);
        mutexes[index%MUTEX_NUMBER].unlock();
        return b;
    }
    inline bool isWinState(ll index){
        return (!getStateValue(index*2ll) && getStateValue(index*2ll + 1ll));
    }

    bool isLossState(ll indexState, int oNumber, int xNumber){
        // this state is reverse state. oN and xN is alse reverse.
        // check if the reverse state is loss?
        // if all next states are win this state is lose, no next state, it is loss because the all next states(creating o) are win(if not win this state value is not default)
        ll thisState = generateState(indexState, oNumber, xNumber);
        // next states are reverse of o and x.
        StateArray sa = createNextStates(thisState, /*chooseEmpty*/false);
        for(int i=0;i<sa.count;i++){
            ll indexNextState = generateIndexNumber(sa.states[i], xNumber, oNumber);
            if(!isWinState(indexNextState)){
                // at least 1 next state is not win. this state is not lose
                return false;
            }
        }
        return true;
    }

    // update
    inline void updateToWinLock(ll index){
        mutexes[index%MUTEX_NUMBER].lock();
        setStateValue(index*2ll, false);
        setStateValue(index*2ll + 1ll, true);
        mutexes[index%MUTEX_NUMBER].unlock();
    }
    inline void updateToWinOrDrawLock(ll index){
        // check this state is default, then update
        mutexes[index%MUTEX_NUMBER].lock();
        if(isDefault(index)){
            setStateValue(index*2ll, true);
            setStateValue(index*2ll + 1ll, false);
        }
        mutexes[index%MUTEX_NUMBER].unlock();
    }
    inline void updateToLoss(ll index){
        setStateValue(index*2ll, true);
        setStateValue(index*2ll + 1ll, true);
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
                setStateValue(i*8+j, data & mask);
                mask = mask >> 1;
            }
        }
        ll r = number % 8;
        if (r != 0){
            data = fin.get();
            ll mask = 1 << (r - 1);
            for(ll j=0;j<r;j++){
                setStateValue(i*8+j, data & mask);
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
            bool t = getStateValue(i);
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

struct PresentStatesValue: StatesValue{
    thread threads[THREADS_NUMBER];
    bool updatedGloval; // this is used for updateValues
    mutex update_mutex;

    void updateValuesFromNextThread(int oNumber, int xNumber, ll startI, ll endI){
        // if next state is loss, this state --> win
        // if next state is draw(or winOrDraw) --> winOrDraw
        // oNumber is for values
        for (ll i=startI;i<endI;i++){
            if(nextStatesValue.isLoss(i)){ 
                ll stateNumber = generateState(i, xNumber, oNumber+1);
                StateArray sa = createPreviousStates(stateNumber, /*fromEmpty*/true);
                ll stateN, stateI;
                for(int j=0;j<sa.count;j++){
                    stateN = sa.states[j];
                    stateI = generateIndexNumber(stateN, oNumber, xNumber);
                    updateToWinLock(stateI);
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
                    // if (isDefault(stateI)){
                    //     updateToWinOrDrawLock(stateI);
                    // }
                    updateToWinOrDrawLock(stateI); // check this state is defalut then update.
                }
            }
        }
    }
    void updateValuesFromNext(int oNumber, int xNumber){
        // read next states from the file. save to the gloval vralue.
        if(!nextStatesValue.read(xNumber, oNumber+1)){
            return;
        }
        ull nextStatesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber+1];
        ull statesSizePerThread = nextStatesSize / THREADS_NUMBER;
        ull i=0;
        for(;i<THREADS_NUMBER-1;i++) threads[i] = thread([this, oNumber, xNumber, i, statesSizePerThread]{updateValuesFromNextThread(oNumber, xNumber, i*statesSizePerThread, (i+1ll)*statesSizePerThread);});
        threads[i] = thread([this, oNumber, xNumber, i, statesSizePerThread, nextStatesSize]{updateValuesFromNextThread(oNumber, xNumber, i*statesSizePerThread, nextStatesSize);});
        for(int j=0;j<THREADS_NUMBER;j++){
            threads[j].join();
        }
    }

    void updateValuesFromEndStates(int oNumber, int xNumber, PresentStatesValue *reverseSV){
        // find the states which end of the game, update the state and update previous states (to win)
        // oNumber is for values
        // update this struct using reverseStatesValues, but update the reverseStatesValue which is end of the game(has a line).

        ull statesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber];
        for (ull i=0ll;i<statesSize;i++){
            if (reverseSV->isNotDefaultLock(i)){
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
                        updateToWinLock(stateI);
                    }
                }
            }else if (win == 1){
                auto symStates = symmetricAllStates(stateNumber);
                for(int j=0;j<symStates.size;j++){
                    ll stateI = generateIndexNumber(symStates.states[j], xNumber, oNumber);
                    reverseSV->updateToWinLock(stateI);
                }
            }
        }
    }
    void updateValuesThread(int oNumber, int xNumber, bool reverse, ll startI, ll endI);
    bool updateValues(int oNumber, int xNumber, bool reverse);
};

PresentStatesValue statesValueGloval, reverseStatesValueGloval; // gloval values!

// void PresentStatesValue::huga(){
//     cout << "hello" << endl;
// }
bool PresentStatesValue::updateValues(int oNumber, int xNumber, bool reverse){
    updatedGloval = false;
    ull statesSize = combinations[combinationSize][xNumber]*combinations[combinationSize-xNumber][oNumber];
    ull statesSizePerThread = statesSize / THREADS_NUMBER;
    ull i=0;
    for(;i<THREADS_NUMBER-1;i++) threads[i] = thread([this, oNumber, xNumber, i, reverse, statesSizePerThread]{updateValuesThread(oNumber, xNumber, reverse, i*statesSizePerThread, (i+1ll)*statesSizePerThread);});
    threads[i] = thread([this, oNumber, xNumber, i, reverse, statesSizePerThread, statesSize]{updateValuesThread(oNumber, xNumber, reverse, i*statesSizePerThread, statesSize);});
    for(int j=0;j<THREADS_NUMBER;j++){
        threads[j].join();
    }
    return updatedGloval;
}
void PresentStatesValue::updateValuesThread(int oNumber, int xNumber, bool reverse, ll startI, ll endI){
    bool updated = false;
    for (ll i=startI;i<endI;i++){
        if (isNotDefaultLock(i)){
            // lose or win --> skip
            continue;
        }
        bool isLoss;
        if (!reverse){
            isLoss = reverseStatesValueGloval.isLossState(i, oNumber, xNumber);
        }else{
            isLoss = statesValueGloval.isLossState(i, oNumber, xNumber);
        }
        if (isLoss){
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
                    if (!reverse){
                        reverseStatesValueGloval.updateToWinLock(stateI);
                    }else{
                        statesValueGloval.updateToWinLock(stateI);
                    }
                }
            }
        }
    }
    if(updated){
        update_mutex.lock();
        updatedGloval = true;
        update_mutex.unlock();
    }
}

void initResizeGloval(){
    statesValueGloval.initSize();
    reverseStatesValueGloval.initSize();
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
    statesValueGloval.initValues(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValueGloval.initValues(xNumber, oNumber);
    
    // at first find next lose states and update this values to win
    // find the states which end of the game, if it is lose update previous state to win
    statesValueGloval.updateValuesFromNext(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValueGloval.updateValuesFromNext(xNumber, oNumber);

    if(oNumber==xNumber) {
        statesValueGloval.updateValuesFromEndStates(oNumber, xNumber, &statesValueGloval);
    }else{
        statesValueGloval.updateValuesFromEndStates(oNumber, xNumber, &reverseStatesValueGloval);
        reverseStatesValueGloval.updateValuesFromEndStates(xNumber, oNumber, &statesValueGloval);
    }

    // compute values until no update
    bool updated = true;
    while (updated){
        updated = false;
        // check all states
        if(oNumber==xNumber){
            updated = statesValueGloval.updateValues(oNumber, xNumber, true);
        }else{
            updated = statesValueGloval.updateValues(oNumber, xNumber, false);
            updated = reverseStatesValueGloval.updateValues(xNumber, oNumber, true) || updated;
        }
    }
    // save resutl to strage
    statesValueGloval.writeStatesValue(oNumber, xNumber);
    if(oNumber!=xNumber) reverseStatesValueGloval.writeStatesValue(xNumber, oNumber);
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
            // if (!needCompute(oNumber, xNumber)) continue; // skip the computed files
            computeStatesValue(oNumber, xNumber);
        }
    }
}

int main(){
    clock_t start = clock();
    time_t start_time = time(NULL);
    init();
    computeAllStatesValue();
    clock_t end = clock();
    time_t end_time = time(NULL);
    cout << "end : " << (double)(end - start)/ CLOCKS_PER_SEC << " sec" << endl;
    cout << "time = " << end_time - start_time << endl;
    return 0;
}