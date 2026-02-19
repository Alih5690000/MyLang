#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    doCode(R"(
        a=list();
        a.push_back([1,2,3]);
        print(a);
    )",n);
}