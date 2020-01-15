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

void testCreatePreviousState(){
    cout << "start test create previous state" << endl;
    ll t = 0b010100101010000000;
    printState(t);
    auto previousStates = createPreviousStates(t, true);
    for(auto state : previousStates){
        printState(state);
    }
    cout << "from empty" << endl;
    previousStates = createPreviousStates(t, false);
    for(auto state : previousStates){
        printState(state);
    }
    cout << "initial state" << endl;
    t = 0ll;
    previousStates = createPreviousStates(t, false);
    for(auto state : previousStates){
        printState(state);
    } 
}

void testBool(){
    vector<bool> testB;
    testB.push_back(1);
    testB.push_back(3);
    testB.push_back(true);
    testB.push_back(0);
    for(auto t : testB){
        cout << t << endl;
    }
}

void testComputeStatesValue(){
    computeStatesValue(5, 4);
}

int main(int argc, char* argv[]){
    cout << "start test !!" << endl;
    init();
    // testCombinations();
    // testGenerateState();
    // testVectorSize();
    // testCreatePreviousState();
    // testBool();
    testComputeStatesValue();
    cout << "end of test !!" << endl;
    return 0;
}