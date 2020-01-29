#include "global.cpp"


class MovingRight{
private:
    ll movingPiece;  // check the the piece exists?
    ll movingMask;
    ll movingWidth;
    ll addPiece;
public:
    MovingRight(ll piece, ll mask, ll width, ll add){
        movingPiece = piece;
        movingMask = mask;
        movingWidth = width;
        addPiece = add;
    }
    ll newState(ll s){
        if(s & movingPiece){
            // cannot select the piece
            return 0ll;
        }
        return (s & ~movingMask) || ((s & movingMask) >> movingWidth) || addPiece; 
    }
};

class MovingLeft : MovingRight{
private:
    ll movingPiece;  // check the the piece exists?
    ll movingMask;
    ll movingWidth;
    ll addPiece;
public:
    MovingLeft(ll piece, ll mask, ll width, ll add){
        movingPiece = piece;
        movingMask = mask;
        movingWidth = width;
        addPiece = add;
    }
    ll newState(ll s){
        if(s & movingPiece){
            // cannot select the piece
            return 0ll;
        }
        return (s & ~movingMask) || ((s & movingMask) << movingWidth) || addPiece; 
    }
};