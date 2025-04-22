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


    string a = "0123456";
    string b = "7891011";
    a.append(b, 3);
    cout << a << endl;
    return 0;
}