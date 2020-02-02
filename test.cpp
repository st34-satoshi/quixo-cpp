#include <iostream>
#include "state.cpp"
using namespace std;

void testPrintState(){
    ll t = 0b011000010001000010;
    printState(t);
}

void testWinMasks(){
    for(auto n: eWinMasks){
        printState(n);
    }
}

void testIsWin(){
    ll t = 0b010100101010000000;
    printState(t);
    cout << isWin(t) << endl;
    t = 0b010100100010000000;
    printState(t);
    cout << isWin(t) << endl;
    t = 0b010100100110000001;
    printState(t);
    cout << isWin(t) << endl;
}

void testInitMovingMask(){
    initMovingMasks();
    for(int i=0;i<2;i++){
        cout << "i " << i << endl;
        for(int j=0;j<boardSize-2;j++){
            cout << "j " << j << endl;
            for(int k=0;k<3;k++){
                cout << k << ": "<< bitset<32>(MoveOppToRight[i][j][k]);
            }
            cout << endl;
        }
        cout << endl;
    }
}


int main(){
    cout << "test" << endl;
    // init();
    // testPrintState();
    // testWinMasks();
    // testIsWin();
    // printAllStates();
    testInitMovingMask();

    return 0;
}