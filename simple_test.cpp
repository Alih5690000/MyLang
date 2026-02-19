#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    doCode(R"(
        class Foo{
            fn __constructor__(self,val){
                self.value=val;
            };
        };
        f=Foo(42);
    )",n);
    return 0;
}
