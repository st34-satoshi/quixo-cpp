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

void testNextStates(){
    // ll s = 0b00000000'01010101'01010110'01100101'01011001;
    ll s = 0b00000000'00010100'01000110'01000101'01001001;
    cout << "init state" << endl;
    printState(swapPlayer(s));
    auto nS = createNextStates2(s, false);
    cout << "cout = " << nS.count << endl;
    for(int i=0;i<nS.count;i++){
        printState(nS.State_array[i]);
    }
}

void testChar(){
    unsigned char i = 0;
    for(int j=0;j<99;j++){
        i++;
        cout << i << "," << j << endl;
    }

}
int main(){
    cout << "test" << endl;
    createCombinations();
    initState();
    initMovingMasks();
    createCombinations();
    initEncoding();
    // testPreviousStates();
    // testChar();
    testNextStates();

    return 0;
}