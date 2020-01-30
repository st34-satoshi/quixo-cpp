// change i <--> state
// o state and x state. (101010 for o, 00101 for x)
#include "global.cpp"

class Encoder{
private:
    // array<ll, 0> oStates;
    // array<ll, 0> xStates;
    ll xMask;
    unordered_map<ll, ll> oStates; // key=index, value=state
    unordered_map<ll, ll> xStates;
    unordered_map<ll, ll> oStatesR; // key=state, value=index
    unordered_map<ll, ll> xStatesR;
    int oNumber, xNumber;
    ll xPatternNumber;

    void initState(bool selected[], int selectedSize, int remain, ll* ind, int selectedI, bool o){
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
                oStatesR.insert(pair(s, *ind));
            }else{
                xStates.insert(pair(*ind, s));
                xStatesR.insert(pair(s, *ind));
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
    Encoder(int oN, int xN){
        // initialize
        oNumber = oN;
        xNumber = xN;
        xPatternNumber = combinations[combinationSize-oNumber][xNumber];
        xMask = (1LL << combinationSize) - 1ll;
        bool selectedO[combinationSize];
        for(int i=0;i<combinationSize;i++){
            selectedO[i] = false;
        }
        ll indexO = 0ll;
        initState(selectedO, combinationSize, oNumber, &indexO, -1, true);
        bool selectedX[combinationSize-oNumber];
        for(int i=0;i<combinationSize-oNumber;i++){
            selectedO[i] = false;
        }
        ll indexX = 0ll;
        initState(selectedX, combinationSize-oNumber, xNumber, &indexX, -1, false);
    }

    ll generateState(ll index){
        ll oState = oStates[index / xPatternNumber];
        ll xStateTmp = xStates[index % xPatternNumber];
        ll xState = 0ll;
        int xI = 0;
        int created = 0;
        for(int i=0;i<combinationSize;i++){
            if(oState & (1ll << i)){
                continue;
            }
            if(xStateTmp & (1ll << xI)){
                xState += 1ll << i;
                created++;
            }
            xI++;
            if(created == xNumber){
                break;
            }
        }
        return (oState << stateLengthHalf) | xState;
    }

    ll generateIndex(ll state){
        // at first create o index
        // then create x index
        // then o * nCr + x.   n=boardSize-oNumber, r means x number
        ll x = 0ll;
        ll presentX = state & xMask;
        ll presentO = state >> stateLengthHalf;
        ll indexO = oStatesR.at(presentO); // TODO debug??
        ll mask;
        int created = 0;
        int creatingI = 0;
        for(int i=0;i<combinationSize;i++){
            mask = 1ll << i;
            if(presentO & mask){
                continue;
            }
            if (presentX & mask){
                x += 1ll << creatingI;
                created++;
            }
            creatingI++;
            if(created == xNumber){
                break;
            }
        }
        ll indexX = xStatesR.at(x);
        cout << "o index " << indexO << endl;
        cout << "x index " << indexX << endl;
        return indexO*xPatternNumber + indexX;
    }

    void printOut(){
        cout << "o states" << endl;
        for(auto p : oStates){
            cout << "i, s = " << p.first << ", " << p.second << endl;
            cout << bitset<9>(p.second) << endl;
        }
        cout << endl;
        cout << "x states" << endl;
        for(auto p : xStates){
            cout << "i, s = " << p.first << ", " << p.second << endl;
            cout << bitset<9>(p.second) << endl;
        }
    }
};

int main(){
    cout << "test" << endl;
    createCombinations();
    Encoder e = Encoder(2, 4);
    e.printOut();
    ll s = 0b110'00000'00000'00000'00000'000'110'110'000;
    ll in = e.generateIndex(s);
    cout << in << endl;
    cout << "s = " << bitset<64>(e.generateState(in)) << endl;
    cout << "end test " << endl;

    return 0;
}