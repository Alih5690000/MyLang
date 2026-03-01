#include <iostream>
#include "main.cpp" 

int main(){
    Namespace n=CreateContext();
    // g++ mtest.cpp -o mtest && ./mtest
    doCode(R"(
        class A{
            fn __constructor__(self){
                self.x=0;
                println("A ctr");
                return(null);
            };
        };
        class B:A{
            fn __constructor__(self){
                viewFunc(super(self).__constructor__);
                super(self).__constructor__();
                println("B ctor");
                return(null);
            }
        };
        a=B();
        println(a.x);
    )",n);
    return 0;
}