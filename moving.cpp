#include "global.cpp"

class Moving{
private:
    ll movingPiece;  // check the the piece exists?
    ll movingMask;
    int movingWidth;
    ll addPiece;
    bool right;
    bool fromEmpty;
public:
    Moving(ll piece, ll mask, int width, ll add, bool r, bool from){
        movingPiece = piece;
        movingMask = mask;
        movingWidth = width;
        addPiece = add;
        right = r;
        fromEmpty = from;
    }
    Moving(){
        movingPiece = 0ll;
        movingMask = 0ll;
        movingWidth = 0;
        addPiece = 0ll;
        right = true;
        fromEmpty = true;
    }
    ll newState(ll s){
        if (fromEmpty){
            if(s & movingPiece){
                // cannot select the piece
                return 0ll;
            }
        }else{
            // from x
            if((s & movingPiece) == 0){
                // cannot select the piece
                return 0ll;
            }
        }
        if (right){
           return (s & ~movingMask) || (((s & movingMask) >> movingWidth) & movingMask) || addPiece;  
        }
        return (s & ~movingMask) || (((s & movingMask) << movingWidth) & movingMask) || addPiece; 
    }
    void printOut(){
        cout << "print moving" << endl;
        cout << "p = " << bitset<64>(movingPiece) << "" << endl;
        cout << "mask = " << bitset<64>(movingMask) << "" << endl;
        cout << "w = " << movingWidth << endl;
        cout << "add = " << bitset<64>(addPiece) << endl;
        cout << "r = " << right << " fromEmpty = " << fromEmpty << endl;
    }
};

array<Moving, 4*2 + (boardHeight-2)*3*2 + (boardWidth-2)*3*2> nextStatesFromEmpty;  // corner*4, + ...
array<Moving, 4*2 + (boardHeight-2)*3*2 + (boardWidth-2)*3*2> nextStatesFromX;
array<Moving, 4*(boardWidth+boardHeight-2 + (boardHeight-2)*2 + (boardWidth-2)*2)> previousStatesToEmpty; 
array<Moving, 4*(boardWidth+boardHeight-2 + (boardHeight-2)*2 + (boardWidth-2)*2)> previousStatesToO; 

void initMoving(){
    // // (0, 0) is row down

    array<array<ll, boardWidth>, boardHeight> xPiece;
    for(int i=0;i<boardHeight;i++){
        for(int j=0;j<boardWidth;j++){
            xPiece[i][j] = 1ll << (i*boardWidth + j);
        }
    }

    int i = 0;

    ll mask;
    // move row 
    for(int c=0;c<boardWidth;c++){
        // move to right
        if(c!=0){
            for(int row : {0, boardHeight-1}){
                mask = ((1ll << (c+1)) - 1) << (row * boardWidth);
                mask = (mask << stateLengthHalf) | mask;
                nextStatesFromEmpty[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][0], true, true);
                nextStatesFromX[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][0], true, false);
                i++;
            }
        }
        // move to left
        if(c!=boardWidth-1){
            for(int row : {0, boardHeight-1}){
                mask = ((1ll << (boardWidth - c)) - 1) << (c + row * boardWidth); //todo
                mask = (mask << stateLengthHalf) | mask;
                nextStatesFromEmpty[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], false, true); 
                nextStatesFromX[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], false, false);
                i++;
            }
        }
    }
    for(int j=1;j<boardHeight-1;j++){
        // move to right
        mask = ((1ll << boardWidth) - 1ll) << (j*boardWidth);
        mask = (mask << stateLengthHalf) | mask;
        nextStatesFromEmpty[i] = Moving(xPiece[j][boardWidth-1], mask, 1, xPiece[j][0], true, true); 
        nextStatesFromX[i] = Moving(xPiece[j][boardWidth-1], mask, 1, xPiece[j][0], true, false);
        i++;
        // move to left
        nextStatesFromEmpty[i] = Moving(xPiece[j][0], mask, 1, xPiece[j][boardWidth-1], false, true); 
        nextStatesFromX[i] = Moving(xPiece[j][0], mask, 1, xPiece[j][boardWidth-1], false, false);
        i++;
    }
    // move column
    for(int r=0;r<boardHeight;r++){
        // move to up
        if(r!=boardHeight-1){
            for(int c : {0, boardWidth-1}){
                mask = (1ll << c) << (r*boardWidth);
                for(int k=r;k<boardHeight-1;k++){
                    mask = (mask << boardWidth) | mask;
                }
                mask = (mask << stateLengthHalf) | mask;
                nextStatesFromEmpty[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[boardHeight-1][c], false, true); 
                nextStatesFromX[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[boardHeight-1][c], false, false);
                i++;
            }   
        }
        // move to down
        if(r!=0){
            for(int c : {0, boardWidth-1}){
                mask = (1ll << c);
                for(int k=0;k<r;k++){
                    mask = (mask << boardWidth) | mask;
                }
                mask = (mask << stateLengthHalf) | mask;
                nextStatesFromEmpty[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[0][c], true, true); 
                nextStatesFromX[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[0][c], true, false);
                i++;
            }   
        }
    }
    for(int c=1;c<boardHeight-1;c++){
        // move to up
        mask = 1ll << c;
        for(int k=0;k<boardHeight-1;k++){
            mask = mask | (mask << boardWidth);
        }
        mask = (mask << stateLengthHalf) | mask;
        nextStatesFromEmpty[i] = Moving(xPiece[0][c], mask, boardWidth, xPiece[boardHeight-1][c], false, true); 
        nextStatesFromX[i] = Moving(xPiece[0][c], mask, boardWidth, xPiece[boardHeight-1][c], false, false);
        i++;
        // move to down
        nextStatesFromEmpty[i] = Moving(xPiece[boardHeight-1][c], mask, boardWidth, xPiece[0][c], true, true); 
        nextStatesFromX[i] = Moving(xPiece[boardHeight-1][c], mask, boardWidth, xPiece[0][c], true, false);
        i++;
    }
}
