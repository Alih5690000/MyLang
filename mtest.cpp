#include <iostream>
#include "main.cpp" 

int main(){
    Namespace n=CreateContext();
    // g++ mtest.cpp -o mtest && ./mtest
    doCode(R"(
        importdll("./module.so");
        a=BankAccount();
        println("a.type ",typeNo(a));
        println("Created");
        println(a.m);
        a.inc(12);
    )",n);
    return 0;
}