#include <vector>

using namespace std;
typedef long long ll;

const int boardSize = 3;

vector< vector<ll> > cellNumbers;  // it is used to get a cell number.
vector<ll> rowNumbers;  // it is used to get a row numbers.
// use to check win
vector<ll> xWinMasks; // check row, column and diagonal line
vector<ll> oWinMasks;
vector<ll> eWinMasks;

ll moveLeft(ll rowState, int i){
    // move the piece(index) to left
    // rowState is one row state
    // index 0 is right
    ll newRow = 0ll;
    // add cell from right
    for(int j=0;j<boardSize-1;j++){
        if (j<i){
            newRow += cellNumbers[0][j] & rowState; // same as base row state
        }else{
            newRow += (cellNumbers[0][j+1] & rowState) >> 2; // 1 bit shift to right
        }
    }
    newRow += 2ll << (boardSize-1)*2; // the left is x (2). turn is always x(because the turn already changed)
    return newRow;
}

ll moveRight(ll rowState, int i){
    // move the piece(index) to right
    // rowState is one row state
    // index 0 is right
    ll newRow = 0;
    // add cell from left
    for(int j=boardSize-1;j>0;j--){
        if (j>i){
            newRow += cellNumbers[0][j] & rowState; // same as base row state
        }else{
            newRow += (cellNumbers[0][j-1] & rowState) << 2; // 1 bit shift to left
        }
    }
    newRow += 2ll; // the right is x.
    return newRow;
}

ll getRightMark(ll state){
    return state & 3ll;
}

ll getCellNumber(int row, int column, ll state){
    return cellNumbers[row][column] & state;
}

ll getShiftedCellNumber(int row, int column, ll state){
    ll cellNumber = getCellNumber(row, column, state);
    return cellNumber >> (row*boardSize + column)*2;
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
    ll rState = reverseState(state);
    ll minState = min(state, rState);
    vector<ll> searchingStates = {state, rState};
    for(ll s: searchingStates){
        for(int i=0;i<3;i++){
            s = rotatedState(s);
            minState = min(minState, s);
        }
    }
    return minState;
}

void printState(ll state){
    cout << "print state" << endl;
    cout << bitset<boardSize*boardSize*2>(state) << endl;
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
            }else if (n==3){
                cout << "e";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int isWin(ll state){
    // return 0:not decided, 1:win, -1:lose
    // turn is o;
    // if there is line of x, lose
    // else if there is line of o, win
    bool win = false;
    ll mask;
    for(int i=0;i<eWinMasks.size();i++){
        mask = state & eWinMasks.at(i);
        if (mask == xWinMasks.at(i)){
            return -1;
        }
        if (mask == oWinMasks.at(i)){
            win = true;
        }
    }
    if (win){
        return 1;
    }
    return 0;
}

vector<ll> createNextStates(ll presentState, bool chooseEmpty){
    // if chooseEmpty, increase o number. choose e. turn is o.
    // else, the number of o, x, e are same. choose o.  turn is o.
    // before creating states, swap turn
    presentState = swapPlayer(presentState);

    vector<ll> nextStates;
    // if this state is end of the game (there is line) no next states.
    if (isWin(presentState)!=0){
        return nextStates;
    }

    ll movingRow, newRow, newState;
    // choose only switch row, then rotate and switch row again.
    // search present state and rotated state.
    for(ll state : {presentState, rotatedState(presentState)}){
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
                    movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
                    newRow = moveLeft(movingRow, j);
                    newState = (state & ~rowNumbers[i]) | (ll(newRow) << 2*i*boardSize);
                    newState = symmetricState(newState);  // select minimum state in symmetric states.
                    // add to nextStates
                    // TODO: avoid the newStaet which is already in nextStates
                    nextStates.push_back(newState);
                }
                if(j!=0){
                    // move to right
                    movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
                    newRow = moveRight(movingRow, j);
                    newState = (state & ~rowNumbers[i]) | (ll(newRow) << 2*i*boardSize);
                    newState = symmetricState(newState);  // select minimum state in symmetric states.
                    // add to nextStates
                    // TODO: avoid the newStaet which is already in nextStates
                    nextStates.push_back(newState);
                }
            }
        }
    }
    return nextStates;
}

void initState(){
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

    // make masks to check win
    ll oWin = 0ll;
    ll xWin = 0ll;
    ll eWin = 0ll;
    // diagonal
    for(int i=0;i<boardSize;i++){
        oWin += 1ll << (i*boardSize+i)*2;
        xWin += 2ll << (i*boardSize+i)*2;
        eWin += 3ll << (i*boardSize+i)*2;
    }
    oWinMasks.push_back(oWin);
    xWinMasks.push_back(xWin);
    eWinMasks.push_back(eWin);
    oWinMasks.push_back(reverseState(oWin));
    xWinMasks.push_back(reverseState(xWin));
    eWinMasks.push_back(reverseState(eWin));
    // row
    oWin = 1ll;
    xWin = 2ll;
    eWin = 3ll;
    for(int i=0;i<boardSize-1;i++){
        oWin = oWin << 2;
        xWin = xWin << 2;
        eWin = eWin << 2;
        oWin += 1ll;
        xWin += 2ll;
        eWin += 3ll;
    }
    for(int i=0;i<boardSize;i++){
        oWinMasks.push_back(oWin<<(2*i*boardSize));
        xWinMasks.push_back(xWin<<(2*i*boardSize));
        eWinMasks.push_back(eWin<<(2*i*boardSize));
    }
    // column
    oWin = 1ll;
    xWin = 2ll;
    eWin = 3ll;
    for(int i=0;i<boardSize-1;i++){
        oWin = oWin << 2*boardSize;
        xWin = xWin << 2*boardSize;
        eWin = eWin << 2*boardSize;
        oWin += 1ll;
        xWin += 2ll;
        eWin += 3ll;
    }
    for(int i=0;i<boardSize;i++){
        oWinMasks.push_back(oWin<<(i*2));
        xWinMasks.push_back(xWin<<(i*2));
        eWinMasks.push_back(eWin<<(i*2));
    }

}