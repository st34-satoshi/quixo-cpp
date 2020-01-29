#include <iostream>
// #include "moving.cpp"
#include "state-ox-different.cpp"
using namespace std;

void testInitMoving(){
    initMoving();
    int i = 0;
    for(auto m : nextStatesFromEmpty){
        cout << "i = " << i++ << endl;
        m.printOut();
        cout << endl;
    }
}

void testCreateNextStates(){
    ll s = 0b110'00000'00000'00000'00000'000'110'110'000;
    cout << "initial state" << endl;
    printState(s);
    auto states = createNextStates(s, false);
    int i = 0;
    for(auto t : states){
        cout << "hh" << endl;
        cout << i++ << endl;
        printState(t);
    }
}

int main(){
    cout << "test" << endl;
    initMoving();
    initState();
    // testInitMoving();
    testCreateNextStates();
    cout << "end test " << endl;

    return 0;
}