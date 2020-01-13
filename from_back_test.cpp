#include <iostream>
#include "from_back.cpp"
using namespace std;

void testCombinations(){
    cout << "test combinations" << endl;
    // createCombinations();
    for(int i=0;i<=boardSize*boardSize;i++){
        cout << "i " << i << ": ";
        for(int j=0;j<=boardSize*boardSize;j++){
            cout << combinations.at(i).at(j) << ", ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]){
    testCombinations();
    return 0;
}