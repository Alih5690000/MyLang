#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    std::cout<<"is print in namespace "<<n.count("print")<<std::endl;
    doCode(R"(
        print("Hello, World!");
        fn createMap(){
            a=list();
            for (i=0;i<10;i=i+1){
                row=list();
                for (j=0;j<10;j=j+1){
                    row.push_back(".");
                }
                print("row ", i, ": ", row);
                a.push_back(row);
            }
            return(a);
        };
        fn showMap(m){
            print(m);
            return (null);
        };
        a=createMap();
        showMap(a);
    )",n);
    return 0;
}