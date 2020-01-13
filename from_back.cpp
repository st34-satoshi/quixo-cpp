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

using namespace std;

int main(){
    cout << "hello" << endl;
    for(int i=0;i<10;i++){
        // bitset<i> hoge;
        cout << i << endl;
    }
    const int t = 10;
    bitset<t> tset;
    return 0;
}