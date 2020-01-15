#include <vector>
#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>

using namespace std;
typedef long long ll;

const int boardSize = 3;
const int combinationSize = boardSize * boardSize;

vector< vector<ll>> combinations;  // nCr, (n<boardSize^2)

void createCombinations(){
    int combSize = combinationSize+ 1;
    vector< vector<ll> > comb(combSize, vector<ll>(combSize));
    for(int i=0;i<combSize;i++){
        comb.at(i).at(0) = 1;
        comb.at(i).at(i) = 1;
    }
    for(int i=1;i<combSize;i++){
        for(int j=1;j<i;j++){
            comb.at(i).at(j) = comb.at(i-1).at(j-1) + comb.at(i-1).at(j); // nCr = (n-1)C(r-1)+(n-1)Cr
        }
    }
    combinations = comb;
}

ll getPatterns(int spaceNumber, int oNumber, int xNumber){
    // cout << "caculate patterns " << spaceNumber << ", " << oNumber << ", " << xNumber << endl;
    return combinations.at(spaceNumber).at(oNumber) * combinations.at(spaceNumber-oNumber).at(xNumber);
}

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