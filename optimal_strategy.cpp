/**
 * find optimal action
 * you can get the record of the perfect player!
 */

#include "state.cpp"

struct StateSet{
    ll state = 0ll;
    int oNumber = 0;
    int xNumber = 0;
};

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

inline bool isNotDefault(vector<bool> *values, ll index){
    return values->at(index*2ll) || values->at(index*2ll + 1ll);
}

StateSet optimalNextState(StateSet presentState){
    // 1: select min next loss, 2: select draw, 3: select max next win
    vector<bool> reverseValues(combinations[combinationSize][presentState.xNumber] * combinations[(combinationSize-presentState.xNumber)][presentState.oNumber] * 2);
    readStatesValue(&reverseValues, presentState.xNumber, presentState.oNumber);
    vector<int> reverseSteps(combinations[combinationSize][presentState.xNumber] * combinations[(combinationSize-presentState.xNumber)][presentState.oNumber]);
    readStatesStep(&reverseSteps, presentState.xNumber, presentState.oNumber);
    vector<bool> nextValues(combinations[combinationSize][presentState.xNumber] * combinations[(combinationSize-presentState.xNumber)][presentState.oNumber+1] * 2);
    readStatesValue(&nextValues, presentState.xNumber, presentState.oNumber+1);
    vector<int> nextSteps(combinations[combinationSize][presentState.xNumber] * combinations[(combinationSize-presentState.xNumber)][presentState.oNumber+1]);
    readStatesStep(&nextSteps, presentState.xNumber, presentState.oNumber+1);

    int stateV = 2; // next state. 0: win, 1: draw, 2:loss
    int stateStep = 0;
    StateSet ss;
    StateArray saAdd = createNextStates(presentState.state, true);
    for(int i=0;i<saAdd.count;i++){
        ll stateI = generateIndexNumber(saAdd.states[i], presentState.xNumber, presentState.oNumber+1);
        if(isLoss(&nextValues, stateI)){
            if(stateV != 0){
                stateV = 0;
                stateStep = nextSteps[stateI];
                ss.state = saAdd.states[i];
                ss.oNumber = presentState.xNumber;
                ss.xNumber = presentState.oNumber+1;
            }else{
                if(stateStep > nextSteps[stateI]){
                    stateStep = nextSteps[stateI];
                    ss.state = saAdd.states[i];
                    ss.oNumber = presentState.xNumber;
                    ss.xNumber = presentState.oNumber+1;
                }
            }
        }else if(isWin(&nextValues, stateI)){
            if(stateV == 2){
                if(stateStep < nextSteps[stateI]){
                    stateStep = nextSteps[stateI];
                    ss.state = saAdd.states[i];
                    ss.oNumber = presentState.xNumber;
                    ss.xNumber = presentState.oNumber+1;
                }
            }
        }else{ 
            // draw
            if(stateV==2){
                stateV = 1;
                ss.state = saAdd.states[i];
                ss.oNumber = presentState.xNumber;
                ss.xNumber = presentState.oNumber+1;
            }
        }
    }
    StateArray saR = createNextStates(presentState.state, false);
    for(int i=0;i<saR.count;i++){
        ll stateI = generateIndexNumber(saR.states[i], presentState.xNumber, presentState.oNumber);
        if(isLoss(&reverseValues, stateI)){
            if(stateV != 0){
                stateV = 0;
                stateStep = reverseSteps[stateI];
                ss.state = saR.states[i];
                ss.oNumber = presentState.xNumber;
                ss.xNumber = presentState.oNumber;
            }else{
                if(stateStep > reverseSteps[stateI]){
                    stateStep = reverseSteps[stateI];
                    ss.state = saR.states[i];
                    ss.oNumber = presentState.xNumber;
                    ss.xNumber = presentState.oNumber;
                }
            }
        }else if(isWin(&reverseValues, stateI)){
            if(stateV == 2){
                if(stateStep < reverseSteps[stateI]){
                    stateStep = reverseSteps[stateI];
                    ss.state = saR.states[i];
                    ss.oNumber = presentState.xNumber;
                    ss.xNumber = presentState.oNumber;
                }
            }
        }else{ 
            // draw
            if(stateV==2){
                stateV = 1;
                ss.state = saR.states[i];
                ss.oNumber = presentState.xNumber;
                ss.xNumber = presentState.oNumber;
            }
        }
    }
    return ss;
}

int main(){
    createCombinations();
    initState();
    initMovingMasks();
    initEncoding();
    StateSet ss; // initial state
    cout << "initial state " << endl;
    printState(ss.state);
    // fight
    int o, x;
    ll s;
    while (true){
        printState(ss.state);
        cout << "type state" << endl;
        cin >> o >> x >> s;
        ss.oNumber = o;
        ss.xNumber = x;
        ss.state = s;
        ss = optimalNextState(ss);
        // printState(swapPlayer(ss.state));
    }

    // create the record of perfect player
    // for(int i=1;i<100;i++){
    //     cout << "step = " << i << endl;
    //     ss = optimalNextState(ss);
    //     if(i%2==0) printState(swapPlayer(ss.state));
    //     else printState(ss.state);
    //     if (isWin(ss.state)!=0){
    //         // end of the game
    //         cout << "reached End! " << endl;
    //         break;
    //     }
    // }
    return 0;
}