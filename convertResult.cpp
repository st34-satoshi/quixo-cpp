#include "state.cpp"

void convertResultsTOXO(){
    int countState = 0;
    unsigned int win=0, draw=0, loss=0;
    for(int total=combinationSize; total>=0 ;total--){
        cout << "total = " << total << endl;
        for(int oNumber=0;oNumber<=total;oNumber++){
            int xNumber = total - oNumber;
            vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
            readStatesValue(&values, oNumber, xNumber);
            // TODO save to file
            for(ull i=0; i<values.size()/2ll;i++){
                // ll s = generateState(i, oNumber, xNumber);
                // cout << "value = " << values.at(i*2) << values.at(i*2+1) << endl;
                // printState(s);
                countState++;
                if (values.at(i*2)) win++;
                else if (values.at(i*2+1)) loss++;
                else draw++;
            }
        }
    }
    cout << "total states = " << countState << endl;
    cout << "win states = " << win << endl;
    cout << "loss states = " << loss << endl;
    cout << "draw states = " << draw << endl;
}

int main(int argc, char *argv[]){
    createCombinations();
    initState();
    convertResultsTOXO();
}