// change i <--> state
// o state and x state. (101010 for o, 00101 for x)
#include "global.cpp"

class Encoder{
private:
    // array<ll, 0> oStates;
    // array<ll, 0> xStates;
    ll xMask;
    unordered_map<int, ll> oStates; // key=index, value=state
    unordered_map<int, ll> xStates;

    void initState(bool selected[], int selectedSize, int remain, int* ind, int selectedI, bool o){
        //
        if(remain == 0){
            ll s = 0;
            for(int i=0;i<selectedSize;i++){
                if (selected[i]){
                    s += 1ll << i;
                }
            }
            if(o){
                oStates.insert(pair(*ind, s));
            }else{
                xStates.insert(pair(*ind, s));
            }
            *ind += 1;
            return;
        }
        for(int i=selectedI+1;i<=selectedSize-remain;i++){
            selected[i] = true;
            initState(selected, selectedSize, remain-1, ind, i, o);
            selected[i] = false;
        }

    }
public:
    Encoder(int oNumber, int xNumber){
        // initialize
        xMask = (1LL << combinationSize) - 1ll;
        // oStates.resize(combinations[combinationSize][oNumber]);
        // xStates.resize(combinations[combinationSize-oNumber][xNumber]);
        // array<bool, combinationSize> selectedO;
        bool selectedO[combinationSize];
        for(int i=0;i<combinationSize;i++){
            selectedO[i] = false;
        }
        int indexO = 0;
        initState(selectedO, combinationSize, oNumber, &indexO, -1, true);
    }
    void printOut(){
        cout << "o states" << endl;
        for(auto p : oStates){
            cout << "i, s = " << p.first << ", " << p.second << endl;
            cout << bitset<9>(p.second) << endl;
        }
    }
};

int main(){
    cout << "test" << endl;
    Encoder e = Encoder(2, 4);
    e.printOut();
    cout << "end test " << endl;

    return 0;
}