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

int main(){
    cout << "test" << endl;
    init();
    testInitialize();
//    testGetCellNumber();

    return 0;
}