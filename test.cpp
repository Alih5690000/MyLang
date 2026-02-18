#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=Namespace();
    doCode("print(input());",n);
    doCode(R"(
        fn createMap(){
            a=[];
            for (i=0;i<10;i++){
                print(i);
                for (j=0;j<10;j++){
                    a.push_back(".");
                };
            };
            return(a);
        }
        a=createMap();
        x=0;y=0;
        for(;;){
            print("infinite loop ",x," ",y);
            i=input();
        };
    )",n);
    return 0;
}