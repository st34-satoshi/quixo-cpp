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
}

ll generateStateFromIndex(ll indexState, int oNumber, int xNumber){
    cout << "start enc" << endl;
    int xPatterns = combinations[combinationSize-oNumber][xNumber];
    // at first decide o
    vector<bool> oStateArray = StateArrayFromI[combinationSize][oNumber][indexState/xPatterns];
    vector<bool> xStateArray = StateArrayFromI[combinationSize-oNumber][xNumber][indexState%xPatterns];
    ll newS = 0ll;
    int xI = 0;
    for(int i=0;i<combinationSize;i++){
        if(oStateArray[i]){
            newS += oMark << i*2;
            continue;
        }
        if(xStateArray[xI]){
            newS += xMark << i*2;
        }
        xI++;
    }
    return newS;
}