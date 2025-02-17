#include<iostream>
#include<vector>
using namespace std;

class foo{
public:
    foo(int a) : num(a){}
    foo(foo&& A) = default;
    
    int num;
};

int main(){

    foo a(10);

    foo b = move(a);

    cout << a.num << b.num << endl;

    vector<int>&& x = vector{1, 2, 3};
    for(auto it : x){
        cout << it << endl;
    }

    x.push_back(4);
     for(auto it : x){
        cout << it << endl;
    }   

    return 0;
}