// #include "global.cpp"
#include "enc_state.cpp"

vector< vector<ll> > cellNumbers;  // it is used to get a cell number.
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
array<array<ll, 4>, (boardSize-2)*2> MoveNextOppRightShift;
array<array<ll, 4>, (boardSize-2)*2> MoveNextOppLeftShift;
// move edge ex) top edge to right
// [l][s][i][j], l is the shift length, s=0 is bottom/right, s=1 is top/left, i is the position, j=0 is C, j=1 is A, j=2 is B, j=3 is the shift length
array<array<array<array<ll, 4>, boardSize-1>, 2>, 2> MovePreviousEdgeRightShift;
array<array<array<array<ll, 4>, boardSize-1>, 2>, 2> MovePreviousEdgeLeftShift;
array<array<array<array<ll, 4>, boardSize-1>, 2>, 2> MoveNextEdgeRightShift;
array<array<array<array<ll, 4>, boardSize-1>, 2>, 2> MoveNextEdgeLeftShift;

void initMovingMasksNext(){
    ll A;
    // o is 1, x is 2. turn is always o. but when creating states, turn is swapped!
    // next = choose x, previous = choose o. (because the turn is swapped)

    // Move Next Opposite
    for(int i=0;i<boardSize-2;i++){
        // C
        MoveNextOppLeftShift[i*2][0] = xMark << ((i+2)*boardSize-1)*2;
        MoveNextOppLeftShift[i*2+1][0] = xMark << (i+1+boardSize*(boardSize-1))*2;
        MoveNextOppRightShift[i*2][0] = xMark << (i+1)*boardSize*2;
        MoveNextOppRightShift[i*2+1][0] = xMark << (i+1)*2;
        // A
        MoveNextOppLeftShift[i*2][1] = ((1ll << boardSize*2) - 1) << ((i+1)*boardSize*2);
        A = 0ll;
        for(int j=0;j<boardSize;j++){
            A = (A << boardSize*2) + (bMark << (i+1)*2); 
        }
        MoveNextOppLeftShift[i*2+1][1] = A;
        MoveNextOppRightShift[i*2][1] = ((1ll << boardSize*2) - 1) << ((i+1)*boardSize*2);
        MoveNextOppRightShift[i*2+1][1] = A;
        // B
        MoveNextOppLeftShift[i*2][2] = xMark << (i+1)*boardSize*2;
        MoveNextOppLeftShift[i*2+1][2] = xMark << (i+1)*2;
        MoveNextOppRightShift[i*2][2] = xMark << ((i+2)*boardSize-1)*2;
        MoveNextOppRightShift[i*2+1][2] = xMark << ((i+1)+boardSize*(boardSize-1))*2;
        // shift length 
        MoveNextOppLeftShift[i*2][3] = 1*2;
        MoveNextOppLeftShift[i*2+1][3] = boardSize*2;
        MoveNextOppRightShift[i*2][3] = 1*2;
        MoveNextOppRightShift[i*2+1][3] = boardSize*2;
    }
     // Move Next Edge
     // l = 0.
    for(int s=0;s<2;s++){
        for(int i=0;i<boardSize-1;i++){
            // C
            MoveNextEdgeLeftShift[0][s][i][0] = (xMark << (i+1)*2) << s*boardSize*(boardSize-1)*2;
            MoveNextEdgeRightShift[0][s][i][0] = (xMark << i*2) << s*boardSize*(boardSize-1)*2;
            // A
            MoveNextEdgeLeftShift[0][s][i][1] = ((1ll << ((i+2)*2)) - 1ll) << s*boardSize*(boardSize-1)*2;
            MoveNextEdgeRightShift[0][s][i][1] = (((1ll << (boardSize-i)*2) - 1ll) << i*2) << s*boardSize*(boardSize-1)*2;
            // B
            MoveNextEdgeLeftShift[0][s][i][2] = xMark << (s*boardSize*(boardSize-1)*2);
            MoveNextEdgeRightShift[0][s][i][2] = (xMark << (boardSize-1)*2) << (s*boardSize*(boardSize-1)*2);
            // shift length
            MoveNextEdgeLeftShift[0][s][i][3] = 1ll*2ll;
            MoveNextEdgeRightShift[0][s][i][3] = 1ll*2ll;
        }
    }
    // l = 1
    for(int s=0;s<2;s++){
        for(int i=0;i<boardSize-1;i++){
            // C
            MoveNextEdgeLeftShift[1][s][i][0] = (xMark << (i+1)*boardSize*2) << s*2*(boardSize-1);
            MoveNextEdgeRightShift[1][s][i][0] = (xMark << i*boardSize*2) << s*2*(boardSize-1);
            // A
            A = 0ll;
            for(int j=0;j<i+2;j++){
                A = (A << boardSize*2) + bMark;
            }
            MoveNextEdgeLeftShift[1][s][i][1] = A << ((boardSize-1)*2*s);
            A = 0ll;
            for(int j=0;j<boardSize-i;j++){
                A = (A << boardSize*2) + bMark;
            }
            MoveNextEdgeRightShift[1][s][i][1] = (A << (boardSize*i*2)) << ((boardSize-1)*2*s);
            // B
            MoveNextEdgeLeftShift[1][s][i][2] = xMark << s*(boardSize-1)*2;
            MoveNextEdgeRightShift[1][s][i][2] = (xMark << boardSize*(boardSize-1)*2) << s*(boardSize-1)*2;
            // shift length
            MoveNextEdgeRightShift[1][s][i][3] = boardSize*2ll;
            MoveNextEdgeLeftShift[1][s][i][3] = boardSize*2ll;
        }
    }
}

void initMovingMasksPrevious(){
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
        for(int i=0;i<boardSize-1;i++){
            // C
            MovePreviousEdgeRightShift[0][s][i][0] = oMark << (s*boardSize*(boardSize-1)*2);
            MovePreviousEdgeLeftShift[0][s][i][0] = (oMark << (boardSize-1)*2) << (s*boardSize*(boardSize-1)*2);
            // A
            MovePreviousEdgeRightShift[0][s][i][1] = ((1ll << ((i+2)*2)) - 1ll) << s*boardSize*(boardSize-1)*2;
            MovePreviousEdgeLeftShift[0][s][i][1] = (((1ll << (boardSize-i)*2) - 1ll) << i*2) << s*boardSize*(boardSize-1)*2;
            // B
            MovePreviousEdgeRightShift[0][s][i][2] = (oMark << (i+1)*2) << s*boardSize*(boardSize-1)*2;
            MovePreviousEdgeLeftShift[0][s][i][2] = (oMark << i*2) << s*boardSize*(boardSize-1)*2;
            // shift length
            MovePreviousEdgeLeftShift[0][s][i][3] = 1ll*2ll;
            MovePreviousEdgeRightShift[0][s][i][3] = 1ll*2ll;
        }
    }
    // l = 1
    for(int s=0;s<2;s++){
        for(int i=0;i<boardSize-1;i++){
            // C
            MovePreviousEdgeLeftShift[1][s][i][0] = (oMark << boardSize*(boardSize-1)*2) << s*(boardSize-1)*2;
            MovePreviousEdgeRightShift[1][s][i][0] = oMark << s*(boardSize-1)*2;
            // A
            A = 0ll;
            for(int j=0;j<boardSize-i;j++){
                A = (A << boardSize*2) + bMark;
            }
            MovePreviousEdgeLeftShift[1][s][i][1] = (A << (boardSize*i*2)) << ((boardSize-1)*2*s);
            A = 0ll;
            for(int j=0;j<i+2;j++){
                A = (A << boardSize*2) + bMark;
            }
            MovePreviousEdgeRightShift[1][s][i][1] = A << ((boardSize-1)*2*s);
            // B
            MovePreviousEdgeLeftShift[1][s][i][2] = (oMark << i*boardSize*2) << s*2*(boardSize-1);
            MovePreviousEdgeRightShift[1][s][i][2] = (oMark << (i+1)*boardSize*2) << s*2*(boardSize-1);
            // shift length
            MovePreviousEdgeRightShift[1][s][i][3] = boardSize*2;
            MovePreviousEdgeLeftShift[1][s][i][3] = boardSize*2;
        }
    }
}

void initMovingMasks(){
    initMovingMasksPrevious();
    initMovingMasksNext();
}

// masks for swap player 
// o is 1, x is 2
// (s & oMask) << 1 | (s & xMask) >> 1
ll XMaskForSwap;
ll OMaskForSwap;

void initSwapMasks(){
    ll o = 0ll;
    ll x = 0ll;
    for(int i=0;i<combinationSize;i++){
        o = (o << 2) + oMark;
        x = (x << 2) + xMark;
    }
    XMaskForSwap = x;
    OMaskForSwap = o;
}

ll swapPlayer(ll state){
    return ((state & OMaskForSwap) << 1) | ((state & XMaskForSwap) >> 1);
}

ll getCellNumber(int row, int column, ll state){
    return cellNumbers[row][column] & state;
}

ll getShiftedCellNumber(int row, int column, ll state){
    ll cellNumber = getCellNumber(row, column, state);
    return cellNumber >> (row*boardSize + column)*2;
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
    cout << bitset<boardSize*boardSize*2+20>(state) << " , n = " << state << endl;
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
    // if there is line of o, win
    // else if there is line of x, lose
    // if there two lines (o and x), o win because the last player(x) loss.
    bool loss = false;
    ll mask;
    for(ull i=0;i<eWinMasks.size();i++){
        mask = state & eWinMasks.at(i);
        if (mask == xWinMasks.at(i)){
            loss = true;
        }
        if (mask == oWinMasks.at(i)){
            return 1;
        }
    }
    if (loss){
        return -1;
    }
    return 0;
}

StateArray createNextStates(ll pres, bool fromEmpty){
    // turn is o. last player is x
    // if fromEmpty, previous mark is -. 
    // else, previous mark is x

    StateArray states;
    // if the previous state is the end of the game avoid the state
    if (isWin(pres)!=0){
        return states;
    }
    // if this state is end of the game (there is line) no next states.
    ll ps = swapPlayer(pres);

    ll newS;
    for(int i=0;i<(boardSize-2)*2;i++){
        if(fromEmpty && ((MoveNextOppRightShift[i][0] >> 1 | MoveNextOppRightShift[i][0]) & ps)){
            // need to select empty but it is o or x
            continue;
        }
        if(!fromEmpty && !(MoveNextOppRightShift[i][0] & ps)){
            // need to select x tile but not x.
            continue;
        }
        newS = (((ps & MoveNextOppRightShift[i][1]) >> MoveNextOppRightShift[i][3]) & MoveNextOppRightShift[i][1]) | (ps & ~MoveNextOppRightShift[i][1]) | MoveNextOppRightShift[i][2];
        states.states[states.count++] = newS;
    }
    for(int i=0;i<(boardSize-2)*2;i++){
        if(fromEmpty && ((MoveNextOppLeftShift[i][0] >> 1 | MoveNextOppLeftShift[i][0]) & ps)){
            // need to select empty but it is o or x
            continue;
        }
        if(!fromEmpty && !(MoveNextOppLeftShift[i][0] & ps)){
            // need to select x tile but not x.
            continue;
        }
        newS = (((ps & MoveNextOppLeftShift[i][1]) << MoveNextOppLeftShift[i][3]) & MoveNextOppLeftShift[i][1]) | (ps & ~MoveNextOppLeftShift[i][1]) | MoveNextOppLeftShift[i][2];
        states.states[states.count++] = newS;
    }
    // bool updated; // avoid the same Next states TODO:
    for(int l=0;l<2;l++){
        for(int s=0;s<2;s++){
            // updated = false;
            for(int i=0;i<boardSize-1;i++){
                if(fromEmpty && ((MoveNextEdgeRightShift[l][s][i][0] >> 1 | MoveNextEdgeRightShift[l][s][i][0]) & ps)){
                    // need to select empty but it is o or x
                    continue;
                }
                if(!fromEmpty && !(MoveNextEdgeRightShift[l][s][i][0] & ps)){
                    // need to select x tile but not x.
                    continue;
                }
                // TODO: if fromEmpty check the [2] is empty?, if fromCircle check the [2] is o!
                // if (MoveNextEdgeRightShift[l][s][i][2] & ps){
                //     if(updated){
                //         continue; // skip. because the next tile is same mark and created the same state.
                //     }
                //     updated = true;
                // }else{
                //     updated = false;
                // }
                newS = (((ps & MoveNextEdgeRightShift[l][s][i][1]) >> MoveNextEdgeRightShift[l][s][i][3]) & MoveNextEdgeRightShift[l][s][i][1]) | (ps & ~MoveNextEdgeRightShift[l][s][i][1]) | MoveNextEdgeRightShift[l][s][i][2];
                states.states[states.count++] = newS;
            }
        }
    }
    for(int l=0;l<2;l++){
        for(int s=0;s<2;s++){
            // updated = false;
            for(int i=0;i<boardSize-1;i++){
                if(fromEmpty && ((MoveNextEdgeLeftShift[l][s][i][0] >> 1 | MoveNextEdgeLeftShift[l][s][i][0]) & ps)){
                    // need to select empty but it is o or x
                    continue;
                }
                if(!fromEmpty && !(MoveNextEdgeLeftShift[l][s][i][0] & ps)){
                    // need to select x tile but not x.
                    continue;
                }
                // if (MoveNextEdgeLeftShift[l][s][i][2] & ps){
                //     if(updated){
                //         continue;
                //     }
                //     updated = true;
                // }else{
                //     updated = false;
                // }
                newS = (((ps & MoveNextEdgeLeftShift[l][s][i][1]) << MoveNextEdgeLeftShift[l][s][i][3]) & MoveNextEdgeLeftShift[l][s][i][1]) | (ps & ~MoveNextEdgeLeftShift[l][s][i][1]) | MoveNextEdgeLeftShift[l][s][i][2];
                states.states[states.count++] = newS;
            }
        }
    }
    return states;
}

StateArray createPreviousStates(ll pres, bool fromEmpty){
    // turn is o. last player is x
    // if fromEmpty, previous mark is -. 
    // else, previous mark is x
    // if the previous state is the end of the game avoid the state
    // before creating states, swap turn

    ll ps = swapPlayer(pres);
    StateArray states;
    ll newS;
    for(int i=0;i<(boardSize-2)*2;i++){
        if (MovePreviousOppRightShift[i][0] & ps){
            newS = (((ps & MovePreviousOppRightShift[i][1]) >> MovePreviousOppRightShift[i][3]) & MovePreviousOppRightShift[i][1]) | (ps & ~MovePreviousOppRightShift[i][1]);
            if (!fromEmpty){
                newS = newS | MovePreviousOppRightShift[i][2];
            }
            if (isWin(newS)==0){
                states.states[states.count++] = newS;
            }
        }
    }
    for(int i=0;i<(boardSize-2)*2;i++){
        if (MovePreviousOppLeftShift[i][0] & ps){
            newS = (((ps & MovePreviousOppLeftShift[i][1]) << MovePreviousOppLeftShift[i][3]) & MovePreviousOppLeftShift[i][1]) | (ps & ~MovePreviousOppLeftShift[i][1]);
            if (!fromEmpty){
                newS = newS | MovePreviousOppLeftShift[i][2];
            }
            if (isWin(newS)==0){
                states.states[states.count++] = newS;
            }
        }
    }
    for(int l=0;l<2;l++){
        for(int s=0;s<2;s++){
            for(int i=0;i<boardSize-1;i++){
                if(MovePreviousEdgeRightShift[l][s][i][0] & ps){
                    newS = (((ps & MovePreviousEdgeRightShift[l][s][i][1]) >> MovePreviousEdgeRightShift[l][s][i][3]) & MovePreviousEdgeRightShift[l][s][i][1]) | (ps & ~MovePreviousEdgeRightShift[l][s][i][1]);
                    if (!fromEmpty){
                        newS = newS | MovePreviousEdgeRightShift[l][s][i][2];
                    }
                    if (isWin(newS)==0){
                        states.states[states.count++] = newS;
                    }
                }
            }
        }
    }
    for(int l=0;l<2;l++){
        for(int s=0;s<2;s++){
            for(int i=0;i<boardSize-1;i++){
                if(MovePreviousEdgeLeftShift[l][s][i][0] & ps){
                    newS = (((ps & MovePreviousEdgeLeftShift[l][s][i][1]) << MovePreviousEdgeLeftShift[l][s][i][3]) & MovePreviousEdgeLeftShift[l][s][i][1]) | (ps & ~MovePreviousEdgeLeftShift[l][s][i][1]);
                    if (!fromEmpty){
                        newS = newS | MovePreviousEdgeLeftShift[l][s][i][2];
                    }
                    if (isWin(newS)==0){
                        states.states[states.count++] = newS;
                    }
                }
            }
        }
    }
    return states;
}

void initState(){
    initEncoding();
    initSwapMasks();
    // initialize cellNumbers
    vector< vector<ll> > cells(boardSize, vector<ll>(boardSize));
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cells[i][j] = 3ll << (i * boardSize + j)*2;
        }
    }
    cellNumbers = cells;

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
