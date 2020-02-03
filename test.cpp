#include <iostream>
// #include "enc_state.cpp"
#include "state.cpp"
using namespace std;

void testGenerateStateFromI(){
    for(int i=0;i<10;i++){
        cout << "i=" << i << endl;
        ll s = generateStateFromIndex(i, 6, 3);
        printState(s);
    }
        
}

void t(){
    //
    cout << StateArrayFromI.size() << endl;
    cout << StateArrayFromI[16].size() << endl;
    cout << StateArrayFromI[16][6].size() << endl;
    for(int i=0;i<combinationSize+1;i++){
        for(int j=0;j<combinationSize+1;j++){
            cout << i << "," << j << " size = " << StateArrayFromI[i][j].size() << endl;
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
    // testInitMovingMask();
    // testInitOpp();
    // testCreatePreviousState();
    createCombinations();
    initEncoding();
    // t();
    testGenerateStateFromI();

    return 0;
}