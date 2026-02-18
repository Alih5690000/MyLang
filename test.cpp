#include <iostream>
#include "main.cpp"

int main(){
    FunctionObject* f=new FunctionObject({"x"},"x+5");
    Namespace n=Namespace();
    n["f"]=f;
    doCode(R"(
        for (i=0;i<9;i=i+1){
            print(i);
        };
    )",n);
    __clean();
    std::cout<<"Im ok"<<std::endl;
    doCode(R"(
        print(return(88));
        class(A){
            a=123;
        };
        i=A();
        print("i is ready\n");
        print("i.a is \n",i.a);
        fn(addone(x)){
            print("lol");
            return (x+1);
        };
        print("addone(5) is ",addone(5));
    )",n);
    n["list"]=new ListObject({});
    doCode(R"(
        list.append(123);
        list.append(456);
        print(list[0]);
        print(list[1]);
    )",n);
    return 0;
}