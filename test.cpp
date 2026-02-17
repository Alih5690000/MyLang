#include <iostream>
#include "main.cpp"

int main(){
    FunctionObject* f=new FunctionObject({"x"},"x+5");
    Namespace n=Namespace();
    n["f"]=f;
    doCode(R"(
        for (i=0;i<9;i=i+1){
            print(i);
        }
    )",n);
    __clean();
    std::cout<<"Im ok"<<std::endl;
    doCode(R"(
        print("I is ",i);
    )",n);
    return 0;
}