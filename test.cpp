#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
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
        };
        fn showMap(m){
            for (i=0;i<10;i++){
                for (j=0;j<10;j++){
                    print(m[i][j]);
                };
                print("\n");
            };
        };
        a=createMap();
        x=0;y=0;
        for(;;){
            i=input();
            print("infinite loop ",x," ",y);
            showMap(a);
        };
    )",n);
    return 0;
}