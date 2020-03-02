#include "global.cpp"
/*
encoding
i <--> state
*/

// nCr, vector of the state. if o or x exist 1, else o
// r <= n
// example. n=9, r=4, i=0,  StateArrayFromI[n][r][i] = {0,0,0,0,0,1,1,1,1}
//                    i=1,                             {0,0,0,0,1,0,1,1,1}
array<array<vector<vector<bool>>, combinationSize+1>, combinationSize+1> StateArrayFromI;
// example. n=9, r=4, s(state only of x)=000001111,  IndexArrayFromState[n][r][s] = 0
//                    s(state only of x)=000010111,                               = 1
array<array<vector<ll>, combinationSize+1>, combinationSize+1> IndexArrayFromState;

void initEncodingArray(bool selectedStates[], int selectedStateSize, int markNumber, int remain, ll* ind, int selectedMaxI){
    if (remain==0){
        // initialize StateArrayFromI and IndexArrayFromState
        for(int i=0;i<selectedStateSize;i++){
            StateArrayFromI[selectedStateSize][markNumber].at(*ind).at(i) = selectedStates[i];
        }
        ll s = 0ll;
        for(int i=0;i<selectedStateSize;i++){
            if(selectedStates[i]){
                s += 1ll << (selectedStateSize - 1 - i);
            }
        }
        IndexArrayFromState[selectedStateSize][markNumber].at(s) = *ind;
        *ind += 1;
        return;
    }
    for(int i=selectedMaxI+1;i<=selectedStateSize-remain;i++){
        selectedStates[i] = true;
        initEncodingArray(selectedStates, selectedStateSize, markNumber, remain - 1, ind, i);
        selectedStates[i] = false;
    }
}

void initEncoding(){
    for(int i=1;i<=combinationSize;i++){
        vector<vector<bool>> w(1, vector<bool>(i));
        StateArrayFromI[i][0] = w; // j=0
        vector<ll> v(1, 0ll);
        IndexArrayFromState[i][0] = v;
        for(int j=1;j<=i;j++){
            // resize the vector
            vector<vector<bool>> w(combinations[i][j], vector<bool>(i));
            StateArrayFromI[i][j] = w;
            vector<ll> v(1ll << i);
            IndexArrayFromState[i][j] = v;

            ll ind = 0ll;
            bool selectedStates[i];
            for(int k=0;k<i;k++){ selectedStates[k]=false; }
            initEncodingArray(selectedStates, i, j, j, &ind, -1);
        }
    }
    for(int j=0;j<=combinationSize;j++){
        vector<vector<bool>> w(1, vector<bool>(1));
        StateArrayFromI[0][j] = w;
        vector<ll> v(1, 0ll);
        IndexArrayFromState[0][j] = v;
    }
}

ll generateState(ll indexState, int oNumber, int xNumber){
    int xPatterns = combinations[combinationSize-oNumber][xNumber];
    // at first decide o
    ll newS = 0ll;
    int xI = 0;
    for(int i=0;i<combinationSize;i++){
        if(StateArrayFromI[combinationSize][oNumber][indexState/xPatterns][i]){
            newS += oMark << (combinationSize - i -1)*2;
            continue;
        }
        if(StateArrayFromI[combinationSize-oNumber][xNumber][indexState%xPatterns][xI]){
            newS += xMark << (combinationSize - i - 1)*2;
        }
        xI++;
    }
    return newS;
}

ll generateIndexNumber(ll state, int oNumber, int xNumber){
    // generate index from state
    int oState = 0;
    int xState = 0;  // without o tile space
    int n;
    int xI = 0;
    for(int i=0;i<combinationSize;i++){
        n = state % 4; // right 2 bit
        state = state >> 2;
        if(n == oMark){
            oState = oState + (1 << i);
            continue;
        }
        if (n == xMark){
            xState = xState + (1 << xI);
        }
        xI++;
    }
    ll oIndex = IndexArrayFromState[combinationSize][oNumber][oState];
    ll xIndex = IndexArrayFromState[combinationSize-oNumber][xNumber][xState];
    int xPatterns = combinations[combinationSize-oNumber][xNumber];
    return oIndex * ll(xPatterns) + xIndex;
}