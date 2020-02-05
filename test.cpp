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

// void testNextStates(){
//     // ll s = 0b00000000'01010101'01010110'00100101'01011001;
//     ll s = 0b00000000'00010100'01000100'01000101'00001001;
//     cout << "init state !!!!!" << endl;
//     printState(swapPlayer(s));
//     auto nS = createNextStates2(s, true);
//     cout << "cout = " << nS.count << endl;
//     for(int i=0;i<nS.count;i++){
//         printState(nS.State_array[i]);
//     }
// }

void testMoveNextEdge(){
    for(int s=0;s<2;s++){
        cout << "s = " << s << endl;
        for(int i=0;i<boardSize-1;i++){
            cout << "i = " << i << endl;
            for(int j=0;j<4;j++){
                cout << "j=" << j << ", " << MoveNextEdgeRightShift[0][s][i][j] << ", " << endl;
                printState(MoveNextEdgeRightShift[0][s][i][j]);
            }
        }
    }
}

void testChar(){
    unsigned char i = 0;
    for(int j=0;j<99;j++){
        i++;
        cout << i << "," << j << endl;
    }
}

void testREADWrite(){
    vector<int> tt;
    for(int i=0;i<100;i++){
        tt.push_back(i);
    }
    writeStatesSteps(&tt, 13, 1);
    vector<int> rr(10000);
    readStatesStep(&rr, 13, 1);
    for(int i=0;i<100;i++){
        cout << i << ",";
    }
}

void testCheckWin(){
    ll s = 157081ll;
    printState(s);
    cout << isWin(s) << endl;
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
    // testNextStates();
    // testMoveNextEdge();
    // testREADWrite();
    testCheckWin();

    return 0;
}