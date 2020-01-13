#include <iostream>
#include "from_back.cpp"
using namespace std;

void testCombinations(){
    cout << "test combinations" << endl;
    // createCombinations();
    for(int i=0;i<=boardSize*boardSize;i++){
        cout << "i " << i << ": ";
        for(int j=0;j<=boardSize*boardSize;j++){
            cout << combinations[i][j] << ", ";
        }
        cout << endl;
    }
}

void testGenerateState(){
    cout << "start test generate state" << endl;
    for(int i=50;i<1000;i+=5){
        cout << "i=" << i << endl;
        ll state = generateState(i, 2, 4);
        ll indexNumber = generateIndexNumber(state);
        if (indexNumber != i){
            cout << "not same" << endl;
            cout << i << ", " << indexNumber << ", " << state << endl;
            printState(state);
        }else{
            cout << "same state !!!" << endl;
        }
    }
}

void testGenerateIndexNumber(){
    //
}

int main(int argc, char* argv[]){
    cout << "start test !!" << endl;
    init();
    // testCombinations();
    testGenerateState();
    cout << "end test !!" << endl;
    return 0;
}