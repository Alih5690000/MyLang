#include <iostream>
#include "main.cpp"

int main(){
    FunctionObject* f=new FunctionObject({"x"},"x+5");
    Namespace n=Namespace();
    n["f"]=f;
    doCode("f(10);a=5",n);
    BasicObj* res=exec("f(a)",n);
    std::cout<<res->str()<<std::endl;
    return 0;
}