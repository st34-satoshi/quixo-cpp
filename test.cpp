#include <iostream>
// #include "enc_state.cpp"
#include "state.cpp"
using namespace std;

// void testGenerateStateFromI(){
//     for(int i=0;i<10;i++){
//     // for(int i=280;i<350;i++){
//         cout << "i=" << i << endl;
//         ll s = generateStateFromIndex(i, 3, 3);
//         printState(s);
//         cout << generateIndexFromState(s, 3, 3) << endl;
//     }
        
// }

void testInitEncoding(){
    //
    // cout << StateArrayFromI.size() << endl;
    // cout << StateArrayFromI[16].size() << endl;
    // cout << StateArrayFromI[16][6].size() << endl;
    // for(int i=0;i<combinationSize+1;i++){
    //     for(int j=0;j<combinationSize+1;j++){
    //         cout << i << "," << j << " size = " << StateArrayFromI[i][j].size() << endl;
    //     }
    // }
    cout << IndexArrayFromState.size() << endl;
    cout << IndexArrayFromState[16].size() << endl;
    // for(int i=0;i<combinationSize+1;i++){
    //     for(int j=0;j<combinationSize+1;j++){
    //         cout << i << "," << j << " size = " << IndexArrayFromState[i][j].size() << endl;
    //     }
    // }
    for(int i=0;i<100;i++){
        cout << i << ", " << IndexArrayFromState[16][3][i] << endl;
    }
}

void testPreviousStates(){
    ll s = 0b00000000'01010101'01010110'01100101'01011001;
    cout << "init state" << endl;
    printState(swapPlayer(s));
    auto preS = createPreviousStates(swapPlayer(s), true);
    for(auto i:preS){
        printState(i);
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
    // testInitEncoding();
    // testGenerateStateFromI();
    testPreviousStates();

    return 0;
}