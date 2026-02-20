#include <iostream>
#include "main.cpp" 

int main(){
    Namespace n=CreateContext();
    doCode(R"(
        importdll("./module.so");
        print("Hello from module: ", sum(1,2,3));
    )",n);
    return 0;
}