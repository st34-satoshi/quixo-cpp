#include <iostream>
#include "moving.cpp"
using namespace std;

void testInitMoving(){
    initMoving();
    for(auto m : nextStatesFromEmpty){
        m.printOut();
    }
}

int main(){
    cout << "test" << endl;
    testInitMoving();
    cout << "end test " << endl;

    return 0;
}