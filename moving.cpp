#include "global.cpp"

// class Moving{
// private:
//     ll movingPiece;  // check the the piece exists?
//     ll movingMask;
//     int movingWidth;
//     ll addPiece;
//     bool right;
//     bool fromEmpty;
// public:
//     Moving(ll piece, ll mask, int width, ll add, bool r, bool from){
//         movingPiece = piece;
//         movingMask = mask;
//         movingWidth = width;
//         addPiece = add;
//         right = r;
//         fromEmpty = from;
//     }
//     ll newState(ll s){
//         if (fromEmpty){
//             if(s & movingPiece){
//                 // cannot select the piece
//                 return 0ll;
//             }
//         }else{
//             // from x
//             if((s & movingPiece) == 0){
//                 // cannot select the piece
//                 return 0ll;
//             }
//         }
//         if (right){
//            return (s & ~movingMask) || (((s & movingMask) >> movingWidth) & movingMask) || addPiece;  
//         }
//         return (s & ~movingMask) || (((s & movingMask) << movingWidth) & movingMask) || addPiece; 
//     }
// };

class Moving{
};


Moving nextStatesFromEmpty;
// array<Moving, 4*2 + (boardHeight-2)*3*2 + (boardWidth-2)*3*2> nextStatesFromEmpty;  // corner*4, + ...
array<Moving, 4*2 + (boardHeight-2)*3*2 + (boardWidth-2)*3*2> nextStatesFromX;
array<Moving, 4*(boardWidth+boardHeight-2 + (boardHeight-2)*2 + (boardWidth-2)*2)> previousStatesToEmpty; 
array<Moving, 4*(boardWidth+boardHeight-2 + (boardHeight-2)*2 + (boardWidth-2)*2)> previousStatesToO; 

void initMoving(){
    // (0, 0) is row down
    // create masks
    array<ll, boardHeight> rowMasks;
    array<ll, boardWidth> columnMasks;
    ll eachMask; // o,x mask
    for(int i=0;i<boardHeight;i++){
        eachMask = ((1ll << boardWidth) - 1ll) << i*boardWidth; // 0000-0011111
        rowMasks[i] = (eachMask << stateLengthHalf) & eachMask; // 0-01111100-0011111
    }
    for(int i=0;i<boardWidth;i++){
        eachMask = 0ll; // 00001'00001'00001'00001'00001
        for(int j=0;j<boardHeight;j++){
            eachMask = (eachMask << boardWidth) + 1ll;
        }
        rowMasks[i] = (eachMask << stateLengthHalf) & eachMask;
    }

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
                mask = ((1ll << c) - 1) << (row * boardWidth);
                nextStatesFromEmpty[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], true, true); 
                nextStatesFromX[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], true, false);
                i++;
            }
        }
        // move to left
        if(c!=boardWidth-1){
            for(int row : {0, boardHeight-1}){
                mask = ((1ll << c) - 1) << (row * boardWidth); //todo
                nextStatesFromEmpty[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], false, true); 
                nextStatesFromX[i] = Moving(xPiece[row][c], mask, 1, xPiece[row][boardWidth-1], false, false);
                i++;
            }
        }
    }
    for(int j=1;j<boardHeight-1;j++){
        // move to right
        nextStatesFromEmpty[i] = Moving(xPiece[j][0], rowMasks[j], 1, xPiece[j][boardWidth-1], true, true); 
        nextStatesFromX[i] = Moving(xPiece[j][0], rowMasks[j], 1, xPiece[j][boardWidth-1], true, false);
        i++;
        // move to left
        nextStatesFromEmpty[i] = Moving(xPiece[j][boardWidth-1], rowMasks[row], 1, xPiece[j][0], false, true); 
        nextStatesFromX[i] = Moving(xPiece[j][boardWidth-1], rowMasks[row], 1, xPiece[j][0], false, false);
        i++;
    }
    // move column
    for(int c=0;c<boardHeight;c++){
        // move to up
        if(c!=boardHeight-1){
            for(int r : {0, boardWidth-1}){
                mask = (1ll << r) << (c*boardWidth);
                for(int k=c;k<boardHeight-1;k++){
                    mask = (mask << boardWidth) | mask;
                }
                nextStatesFromEmpty[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[r][boardHeight-1], false, true); 
                nextStatesFromX[i] = Moving(xPiece[r][c], mask, boardWidth, xPiece[r][boardHeight-1], false, false);
                i++;
            }   
        }
        // move to down
        if(c!=0){
            for(int r : {0, boardWidth-1}){
                mask = (1ll << r);
                for(int k=c;k<boardHeight-1;k++){
                    mask += mask << boardWidth;
                }
                nextStatesFromEmpty[i] = Moving(xPiece[r][c], columnMasks[c], boardWidth, xPiece[0][r], true, true); 
                nextStatesFromX[i] = Moving(xPiece[r][c], columnMasks[c], boardWidth, xPiece[0][r], true, false);
                i++;
            }   
        }
    }
    for(int c=1;c<boardHeight-1;c++){
        // move to up
        nextStatesFromEmpty[i] = Moving(xPiece[0][c], columnMasks[c], boardWidth, xPiece[boardHeight-1][c], false, true); 
        nextStatesFromX[i] = Moving(xPiece[0][c], columnMasks[c], boardWidth, xPiece[boardHeight-1][c], false, false);
        i++;
        // move to down
        nextStatesFromEmpty[i] = Moving(xPiece[boardHeight-1][c], columnMasks[c], boardWidth, xPiece[0][c], true, true); 
        nextStatesFromX[i] = Moving(xPiece[boardHeight-1][c], columnMasks[c], boardWidth, xPiece[0][c], true, false);
        i++;
    }
}
