#include <iostream>
// #include "moving.cpp"
#include "state-ox-different.cpp"
using namespace std;

void testInitMoving(){
    initMoving();
    int i = 0;
    for(auto m : previousStatesToEmpty){
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

void testCreatePreviousStates(){
    // ll s = 0b110'00000'00000'00000'00000'000'110'110'000;
    ll s = 0b0000000000000000000000000111001100000000000000000000000110001100;
    cout << "initial state" << endl;
    printState(s);
    auto states = createPreviousStates(s, true);
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
    testInitMoving();
    // testCreateNextStates();
    testCreatePreviousStates();
    cout << "end test " << endl;

    return 0;
}