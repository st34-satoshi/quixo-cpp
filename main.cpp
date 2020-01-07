#include <iostream>
#include <unordered_map>
#include <vector>

// 0: empty, 1: o, 2: x
using namespace std;
typedef long long ll;
typedef unordered_map<ll, int> stateMap; // element is 1. no meaning
const int boardSize = 5;
// i=0; bottom
// j=0; right
vector< vector<ll> > cellNumbers;  // it is used to get a cell number.
vector<ll> rowNumbers;  // it is used to get a row numbers.

void init(){
    // initialize cellNumbers
    vector< vector<ll> > cells(boardSize, vector<ll>(boardSize));
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cells[i][j] = 3ll << (i * boardSize + j)*2;
        }
    }
    cellNumbers = cells;

    // initialize rowNumbers
    vector<ll> rows(boardSize);
    // create base number: 1111111111 (binary number)
    ll baseNumber = 3ll;
    for(int i=0;i<boardSize-1;i++){
        baseNumber = (baseNumber<<2) + 3ll;
    }
    for(int i=0;i<boardSize;i++){
        rows[i] = baseNumber << i*boardSize*2;
    }
    rowNumbers = rows;
}

int moveLeft(int rowState, int i){
    // move the piece(index) to left
    // rowState is one row state
    // index 0 is right
    int movingPiece = (int(cellNumbers[0][i]) & rowState) >> i*2;
    int newRow = 0;
    // add cell from right
    for(int j=0;j<boardSize-1;j++){
        if (j<i){
            newRow += int(cellNumbers[0][j]) & rowState; // same as base row state
        }else{
            newRow += (int(cellNumbers[0][j+1]) & rowState) >> 2; // 1 bit shift to right
        }
    }
    newRow += movingPiece << (boardSize-1)*2; // the left is moving piece
    return newRow;
}

int moveRight(int rowState, int i){
    // move the piece(index) to right
    // rowState is one row state
    // index 0 is right
    int movingPiece = (int(cellNumbers[0][i]) & rowState) >> i*2;
    int newRow = 0;
    // add cell from left
    for(int j=boardSize-1;j>0;j--){
        if (j>i){
            newRow += int(cellNumbers[0][j]) & rowState; // same as base row state
        }else{
            newRow += (int(cellNumbers[0][j-1]) & rowState) << 2; // 1 bit shift to left
        }
    }
    newRow += movingPiece; // the right is moving piece
    return newRow;
}

ll getCellNumber(int row, int column, ll state){
    return cellNumbers[row][column] & state;
}

int getShiftedCellNumber(int row, int column, ll state){
    ll cellNumber = getCellNumber(row, column, state);
    return int(cellNumber >> (row*boardSize + column)*2);
}

int swapPlayer(int state){
    ll newState = 0;
    int n;
    for(int i=boardSize-1;i>=0;i--){
        for(int j=boardSize-1;j>=0;j--){
            newState = newState << 2;
            n = getShiftedCellNumber(i, j, state);
            // swap 1 and 2 (o and x)
            if (n == 1){
                newState += 2ll;
            }else if (n == 2){
                newState += 1ll;
            }
        }
    }
    return newState;
}

stateMap *createNextStates(int presentState, bool chooseEmpty){
    // if chooseEmpty, increase o number. choose e. turn is o.
    // else, the number of o, x, e are same. choose o.  turn is o.
    // before creating states, swap turn
    // TODO implement soon: swap turn

    auto *nextStates = new stateMap;
    // choose only switch row, then rotate and switch row again.
    vector<int> columns = {0, boardSize-1};  // search only top and bottom
    // todo rotate state.

    int movingRow, newRow;
    ll newState;
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            if(0<i && i<boardSize-1 && 0<j && j<boardSize-1){
                // not edge
                continue;
            }
            if (chooseEmpty && getShiftedCellNumber(i, j, presentState)!=0){
                // need to choose empty but the cell is not empty.
                continue;
            }
            if (!chooseEmpty && getShiftedCellNumber(i, j, presentState)!=2){
                // need to choose o(circle) but the cell is not o.
                continue;
            }
            if(j==0){
                // move to left
                movingRow = int((presentState & rowNumbers[i]) >> 2*i);
                newRow = moveLeft(movingRow, j);
                newState = (presentState & ~rowNumbers[i]) | (ll(newRow) << 2*i);
                // TODO implement: symmetric
                // add to nextStates
                if (nextStates->find(newState) == nextStates->end()){
                    (*nextStates)[newState] = 1;
                }
                // todo
            }
            // todo: j==boardSize, else
        }
    }
    return nextStates;
}

stateMap *createSaveStateSet(stateMap *initialStates){
    // create state set from initial states and save them to storage
    // return next initial state map pointer

    stateMap *createdStates = initialStates;
    auto *nextInitialStates = new stateMap;  // next state has 2 types. #o and #x has 2 types
    auto *presentStateSet = new stateMap;
//    (*nextInitialStates)[1] = 1;
    while (createdStates != nullptr && !createdStates->empty()){
        // create new states which is reachable from createdState
        // save present state
        auto *newStates = new stateMap;
        for(auto itr = createdStates->begin(); itr != createdStates->end(); ++itr){
            // save
            // for debug. TODO: remove
            if (presentStateSet->find(itr->first) != presentStateSet->end()){
                cout << "strange Error: present states has already have this state" << endl;
            }
            (*presentStateSet)[itr->first] = 1;
            // create reachable states
            // todo: add to present and next
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
    // todo: repeat until nextInitialStates is null
    return 0;
}

//int main(){
//    // 4*4
//    cout << "start initialize h" << endl;
//    init();
//    cout << "start creating tree" << endl;
//    return 0;
//}