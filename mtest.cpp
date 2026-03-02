#include <iostream>
#include "main.cpp" 

int main(){
    Namespace n=CreateContext();
    // g++ mtest.cpp -o mtest && ./mtest
    doCode(R"(
        a=input();
        b=input();
        if (a==b){
            println("They are same");
        };
        if (a!=b){
            println("They are not same");
        };
    )",n);
    return 0;
}