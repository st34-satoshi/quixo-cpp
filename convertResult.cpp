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
                if (!values.at(i*2ll)&&values.at(i*2ll+1ll)) win++;
                else if (values.at(i*2ll+1ll) && values.at(i*2ll+1ll)) loss++;
                else draw++;
            }
        }
    }
    cout << "total states = " << countState << endl;
    cout << "win states = " << win << endl;
    cout << "loss states = " << loss << endl;
    cout << "draw states = " << draw << endl;
}

void outputStepResult(){
    int maxStep = 50; // TODO: you need to select the good number
    ull win=0ll, draw=0ll, loss=0ll, all = 0ll;
    vector<int> winSteps(maxStep);
    vector<int> lossSteps(maxStep);
    int step;
    // for(int total=combinationSize; total>=0 ;total--){
    for(int total=combinationSize; total>=16 ;total--){
        cout << "total = " << total << endl;
        // for(int oNumber=0;oNumber<=total;oNumber++){
        for(int oNumber=8;oNumber<=8;oNumber++){
            int xNumber = total - oNumber;
            vector<int> statesStep(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber]);
            readStatesStep(&statesStep, oNumber, xNumber);
            vector<bool> values(combinations[combinationSize][oNumber] * combinations[(combinationSize-oNumber)][xNumber] * 2);
            readStatesValue(&values, oNumber, xNumber);
            // TODO save to file
            for(ull i=0ull;i<statesStep.size();i++){
                // TODO: if you want to see the result of state(win, loss or draw and step), please remove the "//"
                // ll s = generateState(i, oNumber, xNumber);
                step = statesStep.at(i);
                if(step > 40 && values.at(i*2ll+1ll) && values.at(i*2ll)){
                    cout << "strange " << step << endl;
                    cout << values.at(2ll*i) << endl;
                    printState(generateState(i, oNumber, xNumber));
                    exit(0);
                }
                // cout << "step = " << step << endl;
                if(!values.at(i*2ll+1ll)){
                    //  cout << "v = draw" << endl; 
                     draw++;
                }else if(values.at(i*2ll)){
                    //  cout << "v = loss" << endl; 
                     loss++;
                     lossSteps[step]++;
                }else{
                    // cout << "v = win" << endl; 
                    win++;
                    winSteps[step]++;
                }
                // printState(s);
                all++;

            }
        }
    }
    cout << "total states = " << all << endl;
    cout << "win states = " << win << endl;
    cout << "loss states = " << loss << endl;
    cout << "draw states = " << draw << endl;
    cout << endl;
    cout << "steps to win" << endl;
    for(int i=0;i<maxStep;i++){
        cout << "step=" << i << ", the number of states = " << winSteps[i] << endl;
    }
    cout << endl;
    cout << "steps to loss" << endl;
    for(int i=0;i<maxStep;i++){
        cout << "step=" << i << ", the number of states = " << lossSteps[i] << endl;
    }
    cout << endl;
    cout << "total " << endl;
    int t = 0;
    for(int i=0;i<maxStep;i++){
        t += lossSteps[i] + winSteps[i];
        cout << i << " ; " << t << endl;
    }
}

int main(int argc, char *argv[]){
    createCombinations();
    initState();
    // convertResultsTOXO();
    outputStepResult();
}