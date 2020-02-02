#include "global.cpp"

// TODO remove them??
vector< vector<ll> > cellNumbers;  // it is used to get a cell number.
vector<ll> rowNumbers;  // it is used to get a row numbers.
// use to check win
vector<ll> xWinMasks; // check row, column and diagonal line
vector<ll> oWinMasks;
vector<ll> eWinMasks;

// they are used to create next states and previous states
// C is check mask (the moving piece exist?) turn is always o!!
// (((s & A) >> 1) & A) | (S & ~A) | B

// to opposite.  ex) left to right, top to bottom
// [i][j]: i is the position, j=0 is C, j=1 is A, j=2 is B, j=3 is shift length
array<array<ll, 4>, (boardSize-2)*2> MovePreviousOppRightShift;
array<array<ll, 4>, (boardSize-2)*2> MovePreviousOppLeftShift;

// move edge ex) top edge to right
// [l][s][i][j], l is the shift length, s=0 is bottom/right, s=1 is top/left, i is the position, j=0 is C, j=1 is A, j=2 is B, j=4 is the shift length
array<array<array<array<ll, 4>, boardSize-2>, 2>, 2> MovePreviousEdgeRightShift;
array<array<array<array<ll, 4>, boardSize-2>, 2>, 2> MovePreviousEdgeLeftShift;

void initGlobalState(){
    for(int i=0;i<STATE_COUNT;i++){
        State_array[i] = 0ll;
    }
    STATE_COUNT = 0;
}

void initMovingMasks(){
    ll A;
    // o is 1, x is 2. turn is always o. but creating states turn is swapped!
    // next = choose x, previous = choose o. (because the turn is swapped)

    // Move Previous Opposite
    for(int i=0;i<boardSize-2;i++){
        // C
        MovePreviousOppLeftShift[i*2][0] = oMark << ((i+2)*boardSize-1)*2;
        MovePreviousOppLeftShift[i*2+1][0] = oMark << (i+1+boardSize*(boardSize-1))*2;
        MovePreviousOppRightShift[i*2][0] = oMark << (i+1)*boardSize*2;
        MovePreviousOppRightShift[i*2+1][0] = oMark << (i+1)*2;
        // A
        MovePreviousOppLeftShift[i*2][1] = ((1ll << boardSize*2) - 1) << ((i+1)*boardSize*2);
        A = 0ll;
        for(int j=0;j<boardSize;j++){
            A = (A << boardSize*2) + (bMark << (i+1)*2); 
        }
        MovePreviousOppLeftShift[i*2+1][1] = A;
        MovePreviousOppRightShift[i*2][1] = ((1ll << boardSize*2) - 1) << ((i+1)*boardSize*2);
        MovePreviousOppRightShift[i*2+1][1] = A;
        // B
        MovePreviousOppLeftShift[i*2][2] = oMark << (i+1)*boardSize*2;
        MovePreviousOppLeftShift[i*2+1][2] = oMark << (i+1)*2;
        MovePreviousOppRightShift[i*2][2] = oMark << ((i+2)*boardSize-1)*2;
        MovePreviousOppRightShift[i*2+1][2] = oMark << ((i+1)+boardSize*(boardSize-1))*2;
        // shift length 
        MovePreviousOppLeftShift[i*2][3] = 1*2;
        MovePreviousOppLeftShift[i*2+1][3] = boardSize*2;
        MovePreviousOppRightShift[i*2][3] = 1*2;
        MovePreviousOppRightShift[i*2+1][3] = boardSize*2;
    }
     // Move Previous Edge
     // l = 0.
    for(int s=0;s<2;s++){
        for(int i=0;i<boardSize-2;i++){
            // C
            MovePreviousEdgeLeftShift[0][s][i][0] = (oMark << (boardSize-1)*2) << (s*boardSize*(boardSize-1)*2);
            MovePreviousEdgeRightShift[0][s][i][0] = oMark << (s*boardSize*(boardSize-1)*2);
            // A
            MovePreviousEdgeLeftShift[0][s][i][1] = ((1ll << ((i+2)*2)) - 1ll) << s*boardSize*(boardSize-1)*2;
            MovePreviousEdgeRightShift[0][s][i][1] = (((1ll << (boardSize-i-1)*2) - 1ll) << (i+1)*2) << s*boardSize*(boardSize-1)*2;
            // B
            MovePreviousEdgeLeftShift[0][s][i][2] = (oMark << (i+1)*2) << s*boardSize*(boardSize-1)*2;
            MovePreviousEdgeRightShift[0][s][i][2] = (oMark << (i+1)*2) << s*boardSize*(boardSize-1)*2;
            // shift length
            MovePreviousEdgeRightShift[0][s][i][3] = 1ll*2ll;
            MovePreviousEdgeLeftShift[0][s][i][3] = 1ll*2ll;
        }
    }
    // l = 1
    for(int s=0;s<2;s++){
        for(int i=0;i<boardSize-2;i++){
            // C
            MovePreviousEdgeLeftShift[1][s][i][0] = (oMark << boardSize*2) << s*(boardSize-1)*2;
            MovePreviousEdgeRightShift[1][s][i][0] = oMark << s*(boardSize-1)*2;
            // A
            A = 0ll;
            for(int j=0;j<boardSize-i+1;j++){
                A = (A << boardSize*2) + bMark;
            }
            MovePreviousEdgeLeftShift[1][s][i][1] = (A << (boardSize*(i+1)*2)) << ((boardSize-1)*2*s);
            A = 0ll;
            for(int j=0;j<i+2;j++){
                A = (A << boardSize*2) + bMark;
            }
            MovePreviousEdgeRightShift[1][s][i][1] = A << ((boardSize-1)*2*s);
            // B
            MovePreviousEdgeLeftShift[1][s][i][2] = (oMark << (i+1)*boardSize*2) << s*2*(boardSize-1);
            MovePreviousEdgeRightShift[1][s][i][2] = (oMark << (i+1)*boardSize*2) << s*2*(boardSize-1);
            // shift length
            MovePreviousEdgeRightShift[1][s][i][3] = boardSize*2;
            MovePreviousEdgeLeftShift[1][s][i][3] = boardSize*2;
        }
    }
}

ll moveLeft(ll rowState, int fromI, int toI, ll addMark){
    // move the piece(index) to left
    // rowState is one row state
    // index 0 is right
    ll newRow = 0ll;
    // add cell from right
    for(int j=0;j<=boardSize-1;j++){
        if (j==toI){
            newRow += addMark << toI * 2;
        }else if (j<fromI || j>toI){
            newRow += cellNumbers[0][j] & rowState; // same as base row state
        }else{
            newRow += (cellNumbers[0][j+1] & rowState) >> 2; // 1 bit shift to right
        }
    }
    return newRow;
}

ll moveRight(ll rowState, int fromI, int toI, ll addMark){
    // move the piece(index) from 'fromI' to 'toI'
    // rowState is one row state
    // index 0 is right
    ll newRow = 0;
    // add cell from left
    for(int j=boardSize-1;j>=0;j--){
        if (j==toI){
            newRow += addMark << toI * 2;
        }else if (j>fromI || j<toI){
            newRow += cellNumbers[0][j] & rowState; // same as base row state
        }else{
            newRow += (cellNumbers[0][j-1] & rowState) << 2; // 1 bit shift to left
        }
    }
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

vector<ll> symmetricAllStates(ll state){
    // return all symmetric states
    ll rState = reverseState(state);
    vector<ll> symmetricStates;
    symmetricStates.push_back(state);
    symmetricStates.push_back(rState);
    for(ll s: {state, rState}){
        for(int i=0;i<3;i++){
            s = rotatedState(s);
            symmetricStates.push_back(s);
        }
    }
    return symmetricStates;
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
    for(ull i=0;i<eWinMasks.size();i++){
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
                    newRow = moveLeft(movingRow, j, boardSize-1, 2ll);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    // add to nextStates
                    // TODO: avoid the newStaet which is already in nextStates
                    nextStates.push_back(newState);
                }
                if(j!=0){
                    // move to right
                    movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
                    newRow = moveRight(movingRow, j, 0, 2ll);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    // add to nextStates
                    // TODO: avoid the newStaet which is already in nextStates
                    nextStates.push_back(newState);
                }
            }
        }
    }
    return nextStates;
}
void createP(ll ps, bool fromEmpty){
    ll s = swapPlayer(ps);
    initGlobalState();
    for(int i=0;i<(boardSize-2)*2;i++){
        if (MovePreviousOppRightShift[i][0] & s){
            State_array[STATE_COUNT++] = (((s & MovePreviousOppRightShift[i][1]) >> MovePreviousOppRightShift[i][3]) & MovePreviousOppRightShift[i][1]) | (s & ~MovePreviousOppRightShift[i][1]) | MovePreviousOppRightShift[i][2];
        }
    }
    for(int i=0;i<(boardSize-2)*2;i++){
        if (MovePreviousOppLeftShift[i][0] & s){
            State_array[STATE_COUNT++] = (((s & MovePreviousOppLeftShift[i][1]) << MovePreviousOppLeftShift[i][3]) & MovePreviousOppLeftShift[i][1]) | (s & ~MovePreviousOppLeftShift[i][1]) | MovePreviousOppLeftShift[i][2];
        }
    }
}

vector<ll> createPreviousStates(ll presentState, bool fromEmpty){
    // turn is o. last player is x
    // if fromEmpty, previous mark is -. 
    // else, previous mark is x
    // if the previous state is the end of the game avoid the state
    // before creating states, swap turn
    presentState = swapPlayer(presentState);
    ll previousMark = 1ll;
    if (fromEmpty){
        previousMark = 0ll;
    }
    vector<ll> previousStates;
    ll movingRow, newRow, newState;
    // choose only switch row, then rotate and switch row again.
    // search present state and rotated state.
    for(ll state : {presentState}){
        for(int i=0;i<boardSize;i++){
            movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
            for(int j : {0, boardSize-1}){
                if (getShiftedCellNumber(i, j, state)!=1ll){
                    // need to choose o, but the cell is o.
                    continue;
                }
                if(j == boardSize-1){
                    // previous position is right(small) (without i==0 and i== boardSize-1)
                    newRow = moveRight(movingRow, j, 0, previousMark);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    // newSta RightymmetricState(newState);  // select minimum state in symmetric states.
                    // add to previousStates
                    // avoid the previous state which is at the end of the game
                    // TODO: avoid the newStaet which is already in nextStates. and the state at the end of the game
                    if(isWin(newState)==0){
                        previousStates.push_back(newState);
                    }
                }
                if(j == 0){
                    // previous position is left
                    newRow = moveLeft(movingRow, j, boardSize-1, previousMark);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    if(isWin(newState)==0){
                        previousStates.push_back(newState);
                    }
                }
                if (i == 0 || i == boardSize-1){
                    // previous position is 0<j<boardsize-1
                    for (int k=1;k<boardSize-1;k++){
                        // previous action is k to left
                        if ( j == boardSize - 1){
                            newRow = moveRight(movingRow, boardSize-1, k, previousMark);
                            newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                            if(isWin(newState)==0){
                                previousStates.push_back(newState);
                            }
                        }
                        if ( j == 0){
                            // previous action is k to right
                            newRow = moveLeft(movingRow, 0, k, previousMark);
                            newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                            if(isWin(newState)==0){
                                previousStates.push_back(newState);
                            }
                        }
                    }
                }
            }
        }
    }
    for(ll state : {rotatedState(presentState)}){
        for(int i=0;i<boardSize;i++){
            movingRow = (state & rowNumbers[i]) >> 2*i*boardSize;
            for(int j : {0, boardSize-1}){
                if (getShiftedCellNumber(i, j, state)!=1ll){
                    // need to choose o, but the cell is o.
                    continue;
                }
                if(j == boardSize-1){
                    // previous position is right(small) (without i==0 and i== boardSize-1)
                    newRow = moveRight(movingRow, j, 0, previousMark);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    // newSta RightymmetricState(newState);  // select minimum state in symmetric states.
                    // add to previousStates
                    // avoid the previous state which is at the end of the game
                    // TODO: avoid the newStaet which is already in nextStates. and the state at the end of the game
                    if(isWin(newState)==0){
                        previousStates.push_back(rotatedState(rotatedState(rotatedState(newState))));
                    }
                }
                if(j == 0){
                    // previous position is left
                    newRow = moveLeft(movingRow, j, boardSize-1, previousMark);
                    newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                    if(isWin(newState)==0){
                        previousStates.push_back(rotatedState(rotatedState(rotatedState(newState))));
                    }
                }
                if (i == 0 || i == boardSize-1){
                    // previous position is 0<j<boardsize-1
                    for (int k=1;k<boardSize-1;k++){
                        // previous action is k to left
                        if ( j == boardSize - 1){
                            newRow = moveRight(movingRow, boardSize-1, k, previousMark);
                            newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                            if(isWin(newState)==0){
                                previousStates.push_back(rotatedState(rotatedState(rotatedState(newState))));
                            }
                        }
                        if ( j == 0){
                            // previous action is k to right
                            newRow = moveLeft(movingRow, 0, k, previousMark);
                            newState = (state & ~rowNumbers[i]) | (newRow << 2*i*boardSize);
                            if(isWin(newState)==0){
                                previousStates.push_back(rotatedState(rotatedState(rotatedState(newState))));
                            }
                        }
                    }
                }
            }
        }
    }
    return previousStates;
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

/*
 *  encoding
*/

// TODO: if possible, create a better algorithm to change state <--> index
ll generateMark(int spaceNumber, ll *indexNumber, int oNumber, int xNumber){
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
    ll indexNumber = 0;
    int oNumber = 0;
    int xNumber = 0;
    ll mark;
    for(int i=0;i<combinationSize;i++){
        mark = getRightMark(stateNumber);
        stateNumber = stateNumber >> 2;
        if(mark == 1ll){
            oNumber++;
        }else if (mark == 2ll){
            xNumber++;
            // not o. if it is possible to select o, add the number of patterns of next states.
            if (oNumber > 0){
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
        }else if (mark == 0ll){
            if (oNumber > 0){
                indexNumber += getPatterns(i, oNumber-1, xNumber);
            }
            if (xNumber > 0){
                indexNumber += getPatterns(i, oNumber, xNumber-1);
            }
        }
    }

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
