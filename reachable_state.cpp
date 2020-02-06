/*
compute the state is reachable or not
*/

#include "state.cpp"

vector<bool> statesReachable; // 2*i: false is not reachable, true: reachable, 2*i+1: false: not searched, true: searched
vector<bool> reverseStatesReachable;
int oNumber, xNumber;

void init(){
    createCombinations();
    initState();
    initMovingMasks();
    initEncoding();
}

void writeStatesReachable(vector<bool> *states, int oN, int xN){
    // TODO if the file already exist, do not overwrite
    ofstream fout(fileName(oN, xN, "Reachable"), ios::out | ios::binary);
    if(!fout.is_open()){
        cout << "cannot open file reachable" << endl;
        return;
    }
    int bitCounter = 0;
    unsigned char c = 0;
    bool t;
    for(ll i=0ll;i<states->size()/2ll;i++){
        t = states->at(i*2ll);
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

void readStatesReachable(vector<bool> *values, int oN, int xN){
    ifstream fin(fileName(oN, xN, "Reachable"), ios::in | ios::binary);
    if(!fin.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    unsigned char data;
    ll number = combinations[combinationSize][oN]*combinations[combinationSize-oN][xN];
    ll i = 0ll;
    for (;i<number/8;i++){
        data = fin.get();
        ll mask = 1 << 7;
        for(ll j=0;j<8;j++){
            values->at((i*8ll+j)*2ll) = data & mask;
            mask = mask >> 1;
        }
    }
    ll r = number % 8;
    if (r != 0){
        data = fin.get();
        ll mask = 1 << (r - 1);
        for(ll j=0;j<r;j++){
            values->at(i*8+j) = data & mask;
            mask = mask >> 1;
        }
    }
    fin.close();
}

bool updateReachable(){
    bool updated = false;
    for (ull i=0ll;i<statesReachable.size()/2ll;i++){
        if (statesReachable[i*2ll+1ll]) continue;
        if (!statesReachable[i*2ll]) continue;
        // reachable state and not searched yet
        updated = true;
        ll stateNumber = generateState(i, oNumber, xNumber);
        StateArray sa = createNextStates(stateNumber, false);
        for(int j=0;j<sa.count;j++){
            ll stateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
            reverseStatesReachable[stateI*2ll] = true; // update to reachable. I do not think about the 2*i+1. 2*i+1 updates in next loop
        }
        statesReachable[i*2ll+1ll] = true;
    }
    for (ull i=0ll;i<reverseStatesReachable.size()/2ll;i++){
        if (reverseStatesReachable[i*2ll+1ll]) continue;
        if (!reverseStatesReachable[i*2ll]) continue;
        // reachable state and not searched yet
        updated = true;
        ll stateNumber = generateState(i, xNumber, oNumber);
        StateArray sa = createNextStates(stateNumber, false);
        for(int j=0;j<sa.count;j++){
            ll stateI = generateIndexNumber(sa.states[j], oNumber, xNumber);
            statesReachable[stateI*2ll] = true; // update to reachable. I do not think about the 2*i+1. 2*i+1 updates in next loop
        }
        reverseStatesReachable[i*2ll+1ll] = true;
    }
    return updated;
}

void updateFromPrevious(){
    // read previous states from the file
    vector<bool> previousStatesReachable(getCombination(combinationSize, xNumber-1)*getCombination(combinationSize-(xNumber-1), oNumber) * 2);
    readStatesReachable(&previousStatesReachable, xNumber-1, oNumber);

    for (ull i=0ll;i<previousStatesReachable.size()/2ll;i++){
        if (!previousStatesReachable[i*2ll]) continue;
        // reachable state
        ll stateNumber = generateState(i, xNumber-1, oNumber);
        StateArray sa = createNextStates(stateNumber, true);
        for(int j=0;j<sa.count;j++){
            ll stateI = generateIndexNumber(sa.states[j], oNumber, xNumber);
            statesReachable[stateI*2ll] = true; // update to reachable. 
        }
    }
    // reverse
    // cout << "resize" << endl;
    previousStatesReachable.resize(getCombination(combinationSize, oNumber-1)*getCombination(combinationSize-(oNumber-1), xNumber) * 2);
    readStatesReachable(&previousStatesReachable, oNumber-1, xNumber);
    // cout << "resize" << oNumber << ", " << xNumber << endl;
    // cout << previousStatesReachable.size() << endl;
    for (ull i=0ll;i<previousStatesReachable.size()/2ll;i++){
        // cout << "ll" << endl;
        if (!previousStatesReachable[i*2ll]) continue;
        // cout << "ll11" << endl;
        // reachable state
        ll stateNumber = generateState(i, oNumber-1, xNumber);
        // cout << "ll 2" << endl;
        StateArray sa = createNextStates(stateNumber, true);
        // cout << "count " << sa.count << endl;
        for(int j=0;j<sa.count;j++){
            // cout << "j = " << j << endl;
            ll stateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
            // if(oNumber == 1 && xNumber == 4){
            //     cout << "o, x = " << oNumber << ", " << xNumber << endl;
            //     cout << stateI << endl;
            //     printState(sa.states[j]);
            // }
            reverseStatesReachable[stateI*2ll] = true; // update to reachable. 
            // cout << "hh" << endl;
        }
    }
}

void computeStatesReachable(int oN, int xN){
    // initialize this state value
    // we need to compute reverse states at the same time. 
    // cout << "start computing" << endl;
    statesReachable.resize(combinations[combinationSize][oN] * combinations[(combinationSize-oN)][xN] * 2);
    reverseStatesReachable.resize(combinations[combinationSize][xN] * combinations[(combinationSize-xN)][oN] * 2);
    // cout << "start computing" << endl;
    // cout << "1 total states size = " << statesReachable.size() << ", " << reverseStatesReachable.size() << ", " << combinations[combinationSize][oN] * combinations[(combinationSize-oN)][xN] * 2 <<endl;
    for(ull i=0;i<statesReachable.size();i++){
        statesReachable[i] = false;
    }
    for(ull i=0;i<reverseStatesReachable.size();i++){
        reverseStatesReachable[i] = false;
    }
    // statesReachable = values;
    // reverseStatesReachable = valuesReverse;
    oNumber = oN;
    xNumber = xN;
    // cout << "update from previous " << endl;
    updateFromPrevious();
    // cout << "update reachable" << endl;
    // compute all states reachable until no update
    while (updateReachable());
    // save resutl to strage
    // cout << "result manage" << endl;
    // int oCount = 0, xCount = 0;
    // for(int i=0;i<statesReachable.size()/2ll;i++){
    //     if(statesReachable[i*2]){
    //         oCount++;
    //     }
    // }
    // for(int i=0;i<reverseStatesReachable.size()/2ll;i++){
    //     if(reverseStatesReachable[i*2]){
    //         xCount++;
    //     }
    // }
    // cout << "o, x = " << oNumber << ", " << xNumber << endl;
    // cout << "total states size = " << statesReachable.size() << ", " << reverseStatesReachable.size() << ", " << combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2 <<endl;
    // cout << "reachable number = " << oCount << ", " << xCount << endl;
    writeStatesReachable(&statesReachable, oN, xN);
    writeStatesReachable(&reverseStatesReachable, xN, oN);
}

void computeInitialStateReachable(){
    vector<bool> stateReachable = {true, true};
    writeStatesReachable(&stateReachable, 0, 0);
}

void computeAllStatesReachable(){
    cout << "start initial " << endl;
    computeInitialStateReachable();
    cout << "end initial " << endl;
    for(int total=1; total<=combinationSize;total++){
        cout << "total = " << total << endl;
        for(int oN=0;oN<=total/2;oN++){
            cout << "o number = " << oN << endl;
            int xN = total - oN;
            computeStatesReachable(oN, xN);
        }
    }
}

int main(){
    clock_t start = clock();
    init();
    computeAllStatesReachable();
    clock_t end = clock();
    cout << "end : " << (double)(end - start)/ CLOCKS_PER_SEC << " sec" << endl;
    return 0;
}