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

int main(){
    cout << "test" << endl;
    init();
//    testInitialize();
//    testGetCellNumber();
    testMoveLeft();

    return 0;
}