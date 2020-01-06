#include <iostream>
#include <unordered_map>
#include <vector>

// 0: empty, 1: o, 2: x
using namespace std;
typedef long long ll;
typedef unordered_map<ll, int> stateMap;
const int boardSize = 5;
// i=0; bottom
// j=0; right
vector< vector<ll> > cellNumbers;  // it is used to get a cell number.

void init(){
    // initialize cellNumbers
    vector< vector<ll> > cells(boardSize, vector<ll>(boardSize));
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cells[i][j] = 3ll << (i * boardSize + j)*2;
        }
    }
    cellNumbers = cells;
}

ll getCellNumber(int row, int column, ll state){
    return cellNumbers[row][column] & state;
}

int getShiftedCellNumber(int row, int column, ll state){
    ll cellNumber = getCellNumber(row, column, state);
    return int(cellNumber >> (row*boardSize + column)*2);
}

stateMap *createNextStates(int presentState, bool chooseEmpty){
    // if chooseEmpty, increase o number. choose e. turn is o.
    // else, the number of o, x, e are same. choose o.  turn is o.
    // after creating states, swap turn
    // TODO implement soon:
    // choose only first/last row, then rotate and choose first/last row again.
    auto *nextStates = new stateMap;
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            // todo
            if (chooseEmpty && getCellNumber(i, j, presentState)){
                // check this cell is empty?
            }else{
                // check this cell is o
            }
        }
    }
}

stateMap *createSaveStateSet(stateMap *initialStates){
    // create state set from initial states and save them to storage
    // return next initial state map pointer

    stateMap *createdStates = initialStates;
    auto *nextInitialStates = new stateMap;  // next state has 2 types. #o and #x has 2 types
    (*nextInitialStates)[1] = 1;
    while (createdStates != nullptr && !createdStates->empty()){
        // create new states which is reachable from newState
        auto *newStates = new stateMap;
        for(auto itr = createdStates->begin(); itr != createdStates->end(); ++itr){
            // create reachable states
            // todo:
        }
        createdStates = newStates;
    }

    return nextInitialStates;
}

stateMap *createInitialStates(){
    int initialState = 0;
    auto *initialStates = new stateMap;
    (*initialStates)[initialState] = initialState;
    return initialStates;
}

int createTree(){
    // return 0 if success
    // save results to storage.

    stateMap *initialStates = createInitialStates();
    // TODO implement: save initial states to storage

    stateMap *nextInitialStates = createSaveStateSet(initialStates);
    return 0;
}

//int main(){
//    // 4*4
//    cout << "start initialize h" << endl;
//    init();
//    cout << "start creating tree" << endl;
//    return 0;
//}