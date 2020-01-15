#include "state.cpp"

void convertResultsTOXO(){
    for(int total=combinationSize; total>=0 ;total--){
        cout << "totol = " << total << endl;
        for(int oNumber=0;oNumber<=total;oNumber++){
            int xNumber = total - oNumber;
            cout << fileName(2, 3);
            vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
            readStatesValue(&values, oNumber, xNumber);
            for(auto v : values){
                cout << v;
            }
        }
    }
}

int main(int argc, char *argv[]){
    createCombinations();
    initState();
    convertResultsTOXO();
}