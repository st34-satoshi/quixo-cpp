#include <iostream>
#include "main.cpp"
using namespace std;

void testInitialize(){
    cout << "test initialize" << endl;
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cout << bitset<50>(cellNumbers.at(i).at(j)) << ",";
        }
    }
    cout << endl;

    cout << "hoge" << endl;
    for(int i=0;i<boardSize;i++){
        cout << bitset<50>(rowNumbers[i]) << ",";
    }
    cout << "end initialize" << endl;
}

void testGetCellNumber(){
    ll test;
    test = 7ll; // 7 = 111
    cout << endl;
    cout << getCellNumber(0, 0, test) << endl; // 11 -> 3
    cout << getCellNumber(0, 1, test) << endl; // 100 -> 4
    cout << getShiftedCellNumber(0, 0, test) << endl; // 11 -> 3
    cout << getShiftedCellNumber(0, 1, test) << endl; // 01 -> 1
}

void testMoveLeft(){
    cout << "start move left test" << endl;
    int r;
    for(int i=0;i<510;i++){
        for (int j=0;j<boardSize;j++){
            r = moveLeft(i, j);
            cout << bitset<10>(i) << ", " << j << ", " << bitset<10>(r) << endl;
        }
    }
}

void testMoveRight(){
    cout << "start move right test" << endl;
    int r;
    for(int i=0;i<510;i++){
        for (int j=0;j<boardSize;j++){
            r = moveRight(i, j);
            cout << bitset<10>(i) << ", " << j << ", " << bitset<10>(r) << endl;
        }
    }
}

void testSwap(){
    cout << "start test swap" << endl;
    for(int i=0;i<510;i++){
        cout << bitset<10>(i) << "sst " << bitset<10>(swapPlayer(i)) << endl;
    }
}

void testRotate(){
    cout << "start test rotate" << endl;
    ll t = 0b010100101010000000;
    cout << bitset<18 >(rotatedState(t)) << endl;
}

void testReverseState(){
    cout << "start test reverse state" << endl;
    ll t = 0b010100101010000000;
    cout << bitset<18 >(reverseState(t)) << endl;
}

void testSymmetricState(){
    cout << "start test symmetric state" << endl;
    ll t = 0b010100101010000000;
    cout << bitset<18 >(symmetricState(t)) << endl;
    cout << bitset<18 >(symmetricState(symmetricState(t))) << endl;
}

void testCreateNextState(){
    cout << "start test create next state" << endl;
    ll t = 0b010100101010000000;
    auto nextStates = createNextStates(t, true);
    for(auto itr = nextStates->begin(); itr != nextStates->end(); ++itr){
        cout << bitset<18>(itr->first) << endl;
    }
    cout << "choose o (1)" << endl;
    nextStates = createNextStates(t, false);
    for(auto itr = nextStates->begin(); itr != nextStates->end(); ++itr){
        cout << bitset<18>(itr->first) << endl;
    }
}

int main(){
    cout << "test" << endl;
    init();
//    testInitialize();
//    testGetCellNumber();
//    testMoveLeft();
//    testMoveRight();
    // testSwap();
    // testRotate();
    // testReverseState();
    // testSymmetricState();
    testCreateNextState();

    return 0;
}