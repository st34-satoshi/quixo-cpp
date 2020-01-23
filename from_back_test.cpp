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
    cout << "special state" << endl;
    t = 0b010101101010010110;
    printState(t);
    cout << "previous" << endl;
    previousStates = createPreviousStates(t, true);
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

void testBinaryFileWrite(){
    cout << "start binary" << endl;
    vector<bool> testB(10);
    testB.push_back(2);
    for(auto t : testB){
        cout << t;
    }
    ofstream fout("results/test1.bin", ios::out | ios::binary);
    if(!fout.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    for(auto t: testB){
        fout.put(t);
    }
    fout.close();
    cout << "end binary" << endl;
}

void testBinaryFileRead(){
    cout << "start binary read" << endl;
    vector<bool> testB;
    ifstream fin("results/test1.bin", ios::in | ios::binary);
    if(!fin.is_open()){
        cout << "cannot open file" << endl;
        return;
    }
    char data;
    while (fin.get(data)){
        testB.push_back(data);
    }
    for(auto t: testB){
        cout << t;
    }
    cout << endl;
    fin.close();
    cout << "end binary read" << endl;

}

void testComputeAllStatesValue(){
    computeAllStatesValue();
}

// void testMoveLeft(){
//     cout << "start move left test" << endl;
//     int r;
//     for(int i=0;i<510;i++){
//         for (int j=0;j<boardSize;j++){
//             r = moveLeft(i, j);
//             cout << bitset<10>(i) << ", " << j << ", " << bitset<10>(r) << endl;
//         }
//     }
// }

void testMoveRight(){
    cout << "start move right test" << endl;
    int r;
    for(int i=0;i<30;i++){
        for (int j=0;j<boardSize;j++){
            r = moveRight(i, j, 1, 0ll);
            cout << bitset<10>(i) << ", " << j << ", " << bitset<10>(r) << endl;
        }
    }
}

void testWriteReadStates(){
    // TODO make test. same states can read!
    // states is vector<bool>
    vector<bool> states(100);
    // create states randomly?
    for (int i=0;i<states.size();i++){
        if(i%3==0){
            states[i] = 1;
        }
    }
    writeStatesValue(&states, 0, 0);
    vector<bool> check(100);
    readStatesValue(&check, 0, 0);
    int errorCount = 0;
    for (int i=0;i<states.size();i++){
        if (states[i] != check[i]){
            cout << "Error: not same value " << i << states[i] << check[i] << endl; 
            errorCount++;
        }
    }
    cout << "OK " << errorCount << endl;
}

int main(int argc, char* argv[]){
    cout << "start test !!" << endl;
    init();
    // testCombinations();
    // testGenerateState();
    // testVectorSize();
    // testCreatePreviousState();
    // testBool();
    // testComputeStatesValue();
    // testBinaryFileWrite();
    // testBinaryFileRead();
    // computeAllStatesValue();
    // testMoveRight();
    testWriteReadStates();

    cout << "end of test !!" << endl;
    return 0;
}