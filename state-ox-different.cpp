#include "global.cpp"

// TODO: remove not used functions and variables. create state example (only necessary class)
// state is [space, for o, space, for x]
vector< vector<ll> > cellMasksO;  // it is used to get a cell number.
vector< vector<ll> > cellMasksX; 
// vector<ll> rowNumbers;  // it is used to get a row numbers.
// use to check win
vector<ll> xWinMasks; // check row, column and diagonal line
vector<ll> oWinMasks;

void initWinMasksDiagonal(){
    ll winMask = 0ll;
    for(int i=0;i<boardWidth;i++){
        winMask += 1ll << (i*boardWidth+i);
    }
    oWinMasks.push_back(winMask << stateLengthHalf);
    xWinMasks.push_back(winMask);
    // diagonal reverse
    winMask = 0ll;
    for(int i=0;i<boardSize;i++){
        winMask += 1ll << (i*boardSize+boardWidth-1-i);
    }
    oWinMasks.push_back(winMask << stateLengthHalf);
    xWinMasks.push_back(winMask);
}

void initState(){
    // initialize cellMasks O and X
    vector< vector<ll> > cellsO(boardSize, vector<ll>(boardSize));
    vector< vector<ll> > cellsX(boardSize, vector<ll>(boardSize));
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cellsO[i][j] = 1ll << (i * boardSize + j + stateLengthHalf);
            cellsX[i][j] = 1ll << (i * boardSize + j);
        }
    }
    cellMasksO = cellsO;
    cellMasksX = cellsX;

    // make masks to check win
    // diagonal
    if (boardWidth == boardHeight){
        initWinMasksDiagonal();
    }
    
    // row
    ll winMask = 0ll;
    for(int i=0;i<boardWidth;i++){
        winMask += 1ll << i;
    }
    for(int i=0;i<boardHeight;i++){
        oWinMasks.push_back(winMask<<(i*boardWidth)<<stateLengthHalf);
        xWinMasks.push_back(winMask<<(i*boardWidth));
    }
    // column
    winMask = 0ll;
    for(int i=0;i<boardHeight;i++){
        winMask += 1ll << i * boardWidth;
    }
    for(int i=0;i<boardHeight;i++){
        oWinMasks.push_back(winMask<<i<<stateLengthHalf);
        xWinMasks.push_back(winMask<<i);
    }
}

// ll moveLeft(ll rowState, int fromI, int toI, ll addMark){
//     // move the piece(index) to left
//     // rowState is one row state
//     // index 0 is right
//     ll newRow = 0ll;
//     // add cell from right
//     for(int j=0;j<=boardSize-1;j++){
//         if (j==toI){
//             newRow += addMark << toI * 2;
//         }else if (j<fromI || j>toI){
//             newRow += cellNumbers[0][j] & rowState; // same as base row state
//         }else{
//             newRow += (cellNumbers[0][j+1] & rowState) >> 2; // 1 bit shift to right
//         }
//     }
//     return newRow;
// }

ll moveRight(ll rowState, int fromI, int toI, ll addMark){
    // TODO:
    // move the piece(index) from 'fromI' to 'toI'
    // rowState is one row state
    // index 0 is right
    ll newRow = 0;
    // // add cell from left
    // for(int j=boardSize-1;j>=0;j--){
    //     if (j==toI){
    //         newRow += addMark << toI * 2;
    //     }else if (j>fromI || j<toI){
    //         newRow += cellNumbers[0][j] & rowState; // same as base row state
    //     }else{
    //         newRow += (cellNumbers[0][j-1] & rowState) << 2; // 1 bit shift to left
    //     }
    // }
    return newRow;
}

// ll getRightMark(ll state){
//     return state & 3ll;
// }

ll getCellNumber(int row, int column, ll state){
    if (cellMasksO[row][column] & state){
        return 1ll;
    }else if(cellMasksX[row][column] & state){
        return 2ll;
    }
    return 0ll;
}

// ll getShiftedCellNumber(int row, int column, ll state){
//     ll cellNumber = getCellNumber(row, column, state);
//     return cellNumber >> (row*boardSize + column)*2;
// }

ll swapPlayer(ll state){
    return state << stateLengthHalf || state >> stateLengthHalf;
}

void printState(ll state){
    cout << "print state" << endl;
    cout << bitset<stateLengthHalf*2>(state) << endl;
    int n;
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            n = getCellNumber(i, j, state);
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
    for(auto m : xWinMasks){
        if (m == (state & m)){
            return -1;
        }
    }
    for(auto m : oWinMasks){
        if (m == (state & m)){
            return 1;
        }
    }
    return 0;
}

vector<ll> createNextStates(ll presentState, bool chooseEmpty){
    // TODO: implement
    vector<ll> hoge;
    return hoge;
    // // if chooseEmpty, increase o number. choose e. turn is o.
    // // else, the number of o, x, e are same. choose o.  turn is o.
    // // before creating states, swap turn
    // presentState = swapPlayer(presentState);

    // vector<ll> nextStates;
    // // if this state is end of the game (there is line) no next states.
    // if (isWin(presentState)!=0){
    //     return nextStates;
    // }

    // ll movingRow, newRow, newState;
    // // choose only switch row, then rotate and switch row again.
    // // search present state and rotated state.
    // for(ll state : {presentState, rotatedState(presentState)}){
    //     for(int i=0;i<boardSize;i++){
    //         for(int j=0;j<boardSize;j++){
    //             if(0<i && i<boardSize-1 && 0<j && j<boardSize-1){
    //                 // not edge
    //                 continue;
    //             }
    //             if (chooseEmpty && getShiftedCellNumber(i, j, state)!=0){
    //                 // need to choose empty but the cell is not empty.
    //                 continue;
    //             }
    //             if (!chooseEmpty && getShiftedCellNumber(i, j, state)!=2){
    //                 // need to choose x but the cell is not x. turn is already changed.
    //                 continue;
    //             }
    //             // TODO: refactor. move right and move left are similar. make it simple.
    //             if(j!=boardSize-1){
    //                 // move to left
    //                 movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
    //                 newRow = moveLeft(movingRow, j, boardSize-1, 2ll);
    //                 newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                 newState = symmetricState(newState);  // select minimum state in symmetric states.
    //                 // add to nextStates
    //                 // TODO: avoid the newStaet which is already in nextStates
    //                 nextStates.push_back(newState);
    //             }
    //             if(j!=0){
    //                 // move to right
    //                 movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
    //                 newRow = moveRight(movingRow, j, 0, 2ll);
    //                 newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                 newState = symmetricState(newState);  // select minimum state in symmetric states.
    //                 // add to nextStates
    //                 // TODO: avoid the newStaet which is already in nextStates
    //                 nextStates.push_back(newState);
    //             }
    //         }
    //     }
    // }
    // return nextStates;
}

vector<ll> createPreviousStates(ll presentState, bool fromEmpty){
    // TODO: implement
    vector<ll> hoge;
    return hoge;
    // // turn is o. last player is x
    // // if fromEmpty, previous mark is -. 
    // // else, previous mark is x
    // // if the previous state is the end of the game avoid the state
    // // before creating states, swap turn
    // presentState = swapPlayer(presentState);
    // ll previousMark = 1ll;
    // if (fromEmpty){
    //     previousMark = 0ll;
    // }
    // vector<ll> previousStates;
    // ll movingRow, newRow, newState;
    // // choose only switch row, then rotate and switch row again.
    // // search present state and rotated state.
    // for(ll state : {presentState, rotatedState(presentState)}){
    //     for(int i=0;i<boardSize;i++){
    //         movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
    //         for(int j : {0, boardSize-1}){
    //             if (getShiftedCellNumber(i, j, state)!=1ll){
    //                 // need to choose o, but the cell is o.
    //                 continue;
    //             }
    //             if(j == boardSize-1){
    //                 // previous position is right(small) (without i==0 and i== boardSize-1)
    //                 newRow = moveRight(movingRow, j, 0, previousMark);
    //                 newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                 // newSta RightymmetricState(newState);  // select minimum state in symmetric states.
    //                 // add to previousStates
    //                 // avoid the previous state which is at the end of the game
    //                 // TODO: avoid the newStaet which is already in nextStates. and the state at the end of the game
    //                 if(isWin(newState)==0){
    //                     previousStates.push_back(newState);
    //                 }
    //             }
    //             if(j == 0){
    //                 // previous position is left
    //                 newRow = moveLeft(movingRow, j, boardSize-1, previousMark);
    //                 newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                 if(isWin(newState)==0){
    //                     previousStates.push_back(newState);
    //                 }
    //             }
    //             if (i == 0 || i == boardSize-1){
    //                 // previous position is 0<j<boardsize-1
    //                 for (int k=1;k<boardSize-1;k++){
    //                     // previous action is k to left
    //                     if ( j == boardSize - 1){
    //                         newRow = moveRight(movingRow, boardSize-1, k, previousMark);
    //                         newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                         if(isWin(newState)==0){
    //                             previousStates.push_back(newState);
    //                         }
    //                     }
    //                     if ( j == 0){
    //                         // previous action is k to right
    //                         newRow = moveLeft(movingRow, 0, k, previousMark);
    //                         newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
    //                         if(isWin(newState)==0){
    //                             previousStates.push_back(newState);
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    // return previousStates;
}

// TODO: if possible, create a better algorithm to change state <--> index
ll generateMark(int spaceNumber, ll *indexNumber, int oNumber, int xNumber){
    // TODO:
    // return mark and reduce number
    // 1=o, 2=x, 0=no mark
    // at the end
    if (spaceNumber == oNumber){
        return 1ll;
    }
    if (spaceNumber == xNumber){
        return 2ll;
    }
    if (oNumber == 0 && xNumber== 0){
        return 0ll;
    }
    // not at the end
    if (oNumber > 0){
        ll patternsSelectedO = getPatterns(spaceNumber-1, oNumber-1, xNumber);
        if (*indexNumber < patternsSelectedO){
            return 1ll;
        }
        *indexNumber -= patternsSelectedO;
    }
    if (xNumber > 0){
        ll patternsSelectedX = getPatterns(spaceNumber-1, oNumber, xNumber-1);
        if (*indexNumber < patternsSelectedX){
            return 2ll;
        }
        *indexNumber -= patternsSelectedX;
    }
    return 0ll;
}

ll generateState(ll indexNumber, int oNumber, int xNumber){
    // TODO:
    // change to state from indexNumber
    // it is possible to represent state using indexNumber but it is difficult to find symmetric states using indexNumber. todo create a better algorithm
    ll remainingIndexNumber = indexNumber;
    int remainingONumber = oNumber;
    int remainingXNumber = xNumber;
    ll newState = 0ll;
    for(int i=combinationSize;i>0;i--){
        ll mark = generateMark(i, &remainingIndexNumber, remainingONumber, remainingXNumber);
        newState = (newState << 2) + mark;
        if (mark == 1ll){
            remainingONumber--;
        }else if (mark == 2ll){
            remainingXNumber--;
        }
    }
    return newState;
}

ll generateIndexNumber(ll stateNumber){
    // TODO:
    ll indexNumber = 0;
    // int oNumber = 0;
    // int xNumber = 0;
    // ll mark;
    // for(int i=0;i<combinationSize;i++){
    //     mark = getRightMark(stateNumber);
    //     stateNumber = stateNumber >> 2;
    //     if(mark == 1ll){
    //         oNumber++;
    //     }else if (mark == 2ll){
    //         xNumber++;
    //         // not o. if it is possible to select o, add the number of patterns of next states.
    //         if (oNumber > 0){
    //             indexNumber += getPatterns(i, oNumber-1, xNumber);
    //         }
    //     }else if (mark == 0ll){
    //         if (oNumber > 0){
    //             indexNumber += getPatterns(i, oNumber-1, xNumber);
    //         }
    //         if (xNumber > 0){
    //             indexNumber += getPatterns(i, oNumber, xNumber-1);
    //         }
    //     }
    // }

    return indexNumber;
}

string fileName(int oNumber, int xNumber){
    ostringstream osO, osX;
    osO << oNumber;
    osX << xNumber;
    return "results/statesValueo"+osO.str()+"x"+osX.str()+".bin";
}

void writeStatesValue(vector<bool> *values, int oNumber, int xNumber){
    // TODO if the file already exist, do not overwrite
    ofstream fout(fileName(oNumber, xNumber), ios::out | ios::binary);
    if(!fout.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    int bitCounter = 0;
    unsigned char c = 0;
    for(auto t: *values){
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

void readStatesValue(vector<bool> *values, int oNumber, int xNumber){
    ifstream fin(fileName(oNumber, xNumber), ios::in | ios::binary);
    if(!fin.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    unsigned char data;
    ll number = combinations[combinationSize][oNumber]*combinations[combinationSize-oNumber][xNumber]*2;
    ll i = 0ll;
    for (;i<number/8;i++){
        data = fin.get();
        ll mask = 1 << 7;
        for(ll j=0;j<8;j++){
            values->at(i*8+j) = data & mask;
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
