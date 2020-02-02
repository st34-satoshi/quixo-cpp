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
    // initMovingMasks();
    for(int l=0;l<2;l++){
        cout << "l " << l << endl;
        for(int s=0;s<2;s++){
            cout << "s " << s << endl;
            for(int i=0;i<boardSize-1;i++){
                cout << "i " << i << endl;
                for(int j=0;j<4;j++){
                    cout << j << ": "<< bitset<32>(MovePreviousEdgeLeftShift[l][s][i][j]) << ", ";
                    printState(MovePreviousEdgeLeftShift[l][s][i][j]);
                }
                cout << endl;
            }
            cout << endl;
        }
        cout << endl;
    }
}

void testInitOpp(){
    for(int i=0;i<(boardSize-2)*2;i++){
        cout << "i " << i; 
        for(int j=0;j<4;j++){
            cout << j << ": " << bitset<32>(MovePreviousOppLeftShift[i][j]) << ", " << endl;
            printState(MovePreviousOppRightShift[i][j]);
        }
        cout << endl;
    }
}

void testCreatePreviousState(){
    ll bs = 0b10100001'01100110'10001001'10000010;
    cout << "initial state !" << endl;
    printState(bs);
    auto ss = createPreviousStates(bs, false);
    // for(auto s: ss){
    //     cout << "state = " << bitset<32>(s) << endl;
    //     printState(s);
    // }
    createP(bs, false);
    cout << "swap" << endl;
    printState(swapPlayer(bs));
    for(auto s: State_array){
        // printState(s);
        if (!s){
            break;
        }
        bool ok = false;
        for(auto ans: ss){
            if (s == ans){
                ok = true;
            }
            if (ok){
                break;
            }
        }
        if(!ok){
            cout << "Error: not ok" << endl;
            printState(s);
        }
    }
    for(auto s: ss){
        bool ok = false;
        for(auto ans: State_array){
            if (s == ans){
                ok = true;
            }
            if (ok){
                break;
            }
        }
        if(!ok){
            cout << "Error: not ok not created" << endl;
            printState(s);
        }
    }

}


int main(){
    cout << "test" << endl;
    createCombinations();
    initState();
    initMovingMasks();
    // init();
    // testPrintState();
    // testWinMasks();
    // testIsWin();
    // printAllStates();
    testInitMovingMask();
    // testInitOpp();
    // testCreatePreviousState();

    return 0;
}