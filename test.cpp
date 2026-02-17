#include <iostream>
#include "main.cpp"

int main(){
    FunctionObject* f=new FunctionObject({"x"},"x+5");
    Namespace n=Namespace();
    n["f"]=f;
    doCode("out=0;if(5<9){out=1;}",n);
    doCode("print(\"lol\")",n);
    
    return 0;
}