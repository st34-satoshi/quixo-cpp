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
    for(int l=0;l<2;l++){
        cout << "l " << l << endl;
        for(int s=0;s<2;s++){
            cout << "s " << s << endl;
            for(int i=0;i<boardSize-2;i++){
                cout << "i " << i << endl;
                for(int j=0;j<4;j++){
                    cout << j << ": "<< bitset<32>(MovePreviousEdgeLeftShift[l][s][i][j]) << ", ";
                }
                cout << endl;
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