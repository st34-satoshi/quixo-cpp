#include <iostream>
#include <unordered_map>
#include <vector>

// 0: empty, 1: o, 2: x
using namespace std;
typedef long long ll;
typedef unordered_map<ll, int> stateMap; // element is 1. no meaning
const int boardSize = 3;
// i=0; bottom
// j=0; right
vector< vector<ll> > cellNumbers;  // it is used to get a cell number.
vector<ll> rowNumbers;  // it is used to get a row numbers.

// TODO: remove this. it should be save to storage. if 5 by 5 not enough memory.
vector<stateMap> allStateSet;

bool contains(stateMap *base, ll state){
    return base->find(state) != base->end();
}

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
    int newRow = 0;
    // add cell from right
    for(int j=0;j<boardSize-1;j++){
        if (j<i){
            newRow += int(cellNumbers[0][j]) & rowState; // same as base row state
        }else{
            newRow += (int(cellNumbers[0][j+1]) & rowState) >> 2; // 1 bit shift to right
        }
    }
    newRow += 2 << (boardSize-1)*2; // the left is x (2). turn is always x(because the turn already changed)
    return newRow;
}

int moveRight(int rowState, int i){
    // move the piece(index) to right
    // rowState is one row state
    // index 0 is right
    int newRow = 0;
    // add cell from left
    for(int j=boardSize-1;j>0;j--){
        if (j>i){
            newRow += int(cellNumbers[0][j]) & rowState; // same as base row state
        }else{
            newRow += (int(cellNumbers[0][j-1]) & rowState) << 2; // 1 bit shift to left
        }
    }
    newRow += 2; // the right is x.
    return newRow;
}

ll getCellNumber(int row, int column, ll state){
    return cellNumbers[row][column] & state;
}

int getShiftedCellNumber(int row, int column, ll state){
    ll cellNumber = getCellNumber(row, column, state);
    return int(cellNumber >> (row*boardSize + column)*2);
}

ll swapPlayer(ll state){
    ll newState = 0ll;
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

ll reverseState(ll state){
    // return reverse (mirror) state
    ll newState = 0ll;
    int n;
    for(int i=boardSize-1;i>=0;i--){
        for(int j=boardSize-1;j>=0;j--){
            newState = newState << 2;
            newState += ll(getShiftedCellNumber(i, boardSize - j - 1, state));
        }
    }
    return newState;
}

ll rotatedState(ll state){
    // {1, 2, 3,
    //  4, 5, 6
    //  7, 8, 9}
    // to
    // {3, 6, 9,
    //  2, 5, 8
    //  1, 4, 7}
    ll newState = 0;
    for(int i=boardSize-1;i>=0;i--){
        for(int j=boardSize-1;j>=0;j--){
            newState = newState << 2;
            newState += ll(getShiftedCellNumber(j, boardSize - i - 1, state));
        }
    }
    return newState;
}

ll symmetricState(ll state){
    // return the minimum state of all symmetric states
    ll minState = state;
    vector<ll> searchingStates = {state, reverseState(state)};
    for(ll s: searchingStates){
        for(int i=0;i<3;i++){
            s = rotatedState(s);
            minState = min(minState, s);
        }
    }
    return minState;
}

stateMap *createNextStates(ll presentState, bool chooseEmpty){
    // if chooseEmpty, increase o number. choose e. turn is o.
    // else, the number of o, x, e are same. choose o.  turn is o.
    // before creating states, swap turn
    presentState = swapPlayer(presentState);

    auto *nextStates = new stateMap;
    int movingRow, newRow;
    ll newState;
    // choose only switch row, then rotate and switch row again.
    // search present state and rotated state.
    vector<ll> searchingStates = {presentState, rotatedState(presentState)};
    for(ll state : searchingStates){
        for(int i=0;i<boardSize;i++){
            for(int j=0;j<boardSize;j++){
                if(0<i && i<boardSize-1 && 0<j && j<boardSize-1){
                    // not edge
                    continue;
                }
                if (chooseEmpty && getShiftedCellNumber(i, j, state)!=0){
                    // need to choose empty but the cell is not empty.
                    continue;
                }
                if (!chooseEmpty && getShiftedCellNumber(i, j, state)!=2){
                    // need to choose x but the cell is not x. turn is already changed.
                    continue;
                }
                // TODO: refactor. move right and move left are similar. make it simple.
                if(j!=boardSize-1){
                    // move to left
                    movingRow = int((state & rowNumbers[i]) >> 2*i*boardSize);
                    newRow = moveLeft(movingRow, j);
                    newState = (state & ~rowNumbers[i]) | (ll(newRow) << 2*i*boardSize);
                    newState = symmetricState(newState);  // select minimum state in symmetric states.
                    // add to nextStates
                    if (nextStates->find(newState) == nextStates->end()){
                        (*nextStates)[newState] = 1;
                    }
                }
                if(j!=0){
                    // move to right
                    movingRow = int((state & rowNumbers[i]) >> 2*i*boardSize);
                    newRow = moveRight(movingRow, j);
                    newState = (state & ~rowNumbers[i]) | (ll(newRow) << 2*i*boardSize);
                    newState = symmetricState(newState);  // select minimum state in symmetric states.
                    // add to nextStates
                    if (nextStates->find(newState) == nextStates->end()){
                        (*nextStates)[newState] = 1;
                    }
                }
            }
        }
    }
    return nextStates;
}

stateMap *createSaveStateSet(stateMap *initialStates){
    // create state set from initial states and save them to storage
    // return next initial state map pointer

    stateMap *createdStates = initialStates;
    auto *nextInitialStates = new stateMap;  // next state has 2 types. #o and #x
    auto *presentStates = new stateMap;
    while (createdStates != nullptr && !createdStates->empty()){
        // create new states which is reachable from createdState
        // save present state
        auto *newStates = new stateMap;
        for(auto stateItr = createdStates->begin(); stateItr != createdStates->end(); ++stateItr){
            // save
            (*presentStates)[stateItr->first] = 1;

            // create reachable states
            // choose empty. add to nextInitialStates
            auto nextStates = createNextStates(stateItr->first, true); 
            for(auto itr=nextStates->begin();itr!=nextStates->end();itr++){
                if(nextInitialStates->find(itr->first) == nextInitialStates->end()){
                    (*nextInitialStates)[itr->first] = 1;
                }
            }
            // choose circle. add to newStates
            nextStates = createNextStates(stateItr->first, false); 
            for(auto itr=nextStates->begin();itr!=nextStates->end();itr++){
                if(!contains(newStates, itr->first) && !contains(presentStates, itr->first) && !contains(createdStates, itr->first)){
                    (*newStates)[itr->first] = 1;
                }
            }
        }
        createdStates = newStates;
    }
    // TODO: save present states
    allStateSet.push_back(*presentStates);

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
    // TODO: implement: save initial states to storage
    allStateSet.push_back(*initialStates);

    stateMap *nextInitialStates;
    int counter = 0;
    while (initialStates != nullptr && !initialStates->empty()){
        // repeat until nextInitialStates is null
        initialStates = createSaveStateSet(initialStates);
        cout << ++counter << endl;
    }
    
    return 0;
}

// TODO: output state . You can understand easily
void printState(ll state){
    cout << "print state" << endl;
    int n;
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            n = getShiftedCellNumber(i, j, state);
            if(n==0){
                cout << "-";
            }else if (n==1){
                cout << "o";
            }else if (n==2){
                cout << "x";
            }
        }
        cout << endl;
    }
    cout << endl;
}

//int main(){
//    // 4*4
//    cout << "start initialize h" << endl;
//    init();
//    cout << "start creating tree" << endl;
//    return 0;
//}