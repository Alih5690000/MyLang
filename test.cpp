#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    std::cout<<"is print in namespace "<<n.count("print")<<std::endl;
    doCode(R"(
        print("Hello, World!");
        fn createMap(){
            a=list();
            for (i=0;i<10;i++){
                row=list();
                for (j=0;j<10;j++){
                    row.push_back(".");
                };
                a.push_back(row);
            };
            return(a);
        };
        fn showMap(m){
            print("Frist row: ", m[0]);
        };
        a=createMap();
        print("Created map a: ", a, " address is ", addr(a));
        print(currentNamespace());
        print("refcount of a: ", getRefcount(a));
        showMap(a);
    )",n);
    return 0;
}