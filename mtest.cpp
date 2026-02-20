#include <iostream>
#include "main.cpp" 

int main(){
    Namespace n=CreateContext();
    doCode(R"(
        a=map();
        a["key"]=123;
        print(a["key"]);
        importdll("./module.so");
        print("Module loaded successfully ",a);
    )",n);
    return 0;
}