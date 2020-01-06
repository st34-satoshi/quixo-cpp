#include <iostream>
#include "main.cpp"
using namespace std;

void outputCellNumbers(){
    for(int i=0;i<boardSize;i++){
        for(int j=0;j<boardSize;j++){
            cout << bitset<50>(cellNumbers.at(i).at(j)) << ",";
        }
    }
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
    testGetCellNumber();

    return 0;
}