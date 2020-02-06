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
    previousStatesReachable.resize(getCombination(combinationSize, oNumber-1)*getCombination(combinationSize-(oNumber-1), xNumber) * 2);
    readStatesReachable(&previousStatesReachable, oNumber-1, xNumber);
    // cout << "resize" << oNumber << ", " << xNumber << endl;
    // cout << previousStatesReachable.size() << endl;
    for (ull i=0ll;i<previousStatesReachable.size()/2ll;i++){
        if (!previousStatesReachable[i*2ll]) continue;
        // reachable state
        ll stateNumber = generateState(i, oNumber-1, xNumber);
        StateArray sa = createNextStates(stateNumber, true);
        for(int j=0;j<sa.count;j++){
            ll stateI = generateIndexNumber(sa.states[j], xNumber, oNumber);
            reverseStatesReachable[stateI*2ll] = true; // update to reachable. 
        }
    }
}

void computeStatesReachable(int oN, int xN){
    // initialize this state value
    // we need to compute reverse states at the same time. 
    statesReachable.resize(combinations[combinationSize][oN] * combinations[(combinationSize-oN)][xN] * 2);
    reverseStatesReachable.resize(combinations[combinationSize][xN] * combinations[(combinationSize-xN)][oN] * 2);
    for(ull i=0;i<statesReachable.size();i++){
        statesReachable[i] = false;
    }
    for(ull i=0;i<reverseStatesReachable.size();i++){
        reverseStatesReachable[i] = false;
    }
    oNumber = oN;
    xNumber = xN;
    updateFromPrevious();
    // compute all states reachable until no update
    while (updateReachable());
    // save resutl to strage
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