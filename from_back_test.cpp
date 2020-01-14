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
    bool ok = true;
    for(int i=0;i<860;i+=1){
        cout << "i=" << i << endl;
        ll state = generateState(i, 2, 4);
        ll indexNumber = generateIndexNumber(state);
        if (indexNumber != i){
            cout << "not same" << endl;
            cout << i << ", " << indexNumber << ", " << state << endl;
            printState(state);
            ok = false;
        }else{
            cout << "same state !!!" << endl;
        }
    }
    cout << "re " << ok << endl;
}

void testVectorSize(){
    vector<bool> testv(999999999999999999);
    cout << testv.size() * 400 << endl;
}

vector<ll> testVector(){
    vector<ll> tt(3, 4);
    return tt;
}

int main(int argc, char* argv[]){
    cout << "start test !!" << endl;
    init();
    // testCombinations();
    // testGenerateState();
    // testVectorSize();
    auto tv = testVector();
    cout << tv.at(0) << tv.at(2);
    cout << "end test !!" << endl;
    return 0;
}