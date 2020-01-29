#include <iostream>
#include "moving.cpp"
using namespace std;

void testInitMoving(){
    initMoving();
}

int main(){
    cout << "test" << endl;
    testInitMoving():
    cout << "end test " << endl;

    return 0;
}

// class MyClass
// {
// private:
//     int hoge;
// public:
//     int ge;
// };
// int main()
// {
//     MyClass a;    //  MyClass 型のオブジェクト a を生成
//     // ↓ 間違った例
//     MyClass b();  //  最後に () を付けると、MyClass オブジェクトを返す関数宣言になってしまうので注意
//     return 0;
// }