/*
Next player is o.
states are represented by the ninber of index(i).
bitset saves 2 bits.
00: unknown(draw)
01: lose
10: win
11: ??

you can change size by boardSize.

Run
c++ -std=c++17 -O3 from_back.cpp 
./a.out
*/

#include <iostream>
#include <bitset>
#include <vector>

using namespace std;

const int boardSize = 5;
vector< vector<int>> combinations;  // nCr, (n<boardSize^2)

void creatCombinations(){
    int combinationSize = boardSize * boardSize + 1;
    vector< vector<int> > comb(combinationSize, vector<int>(combinationSize));
    for(int i=0;i<combinationSize;i++){
        comb.at(i).at(0) = 1;
        comb.at(i).at(i) = 1;
    }
    for(int i=1;i<combinationSize;i++){
        for(int j=1;j<i;j++){
            comb.at(i).at(j) = comb.at(i-1).at(j-1) + comb.at(i-1).at(j); // nCr = (n-1)C(r-1)+(n-1)Cr
        }
    }
    combinations = comb;
}

void init(){
    // initialize
}

// int main(){
//     cout << "hello" << endl;
//     for(int i=0;i<10;i++){
//         // bitset<i> hoge;
//         cout << i << endl;
//     }
//     const int t = 10;
//     bitset<t> tset;
//     return 0;
// }