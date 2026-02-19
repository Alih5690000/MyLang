#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    std::cout<<"is print in namespace "<<n.count("print")<<std::endl;
    doCode(R"(
        print("Hello, World!");
        fn createMap(){
            print("Creating map");
            a=list();
            for (i=0;i<10;i++){
                row=list();
                for (j=0;j<10;j++){
                    row.push_back(".");
                }  
                a.push_back(row);
            };
            return(a);
        };
        fn showMap(m){
            for (i=0;i<m.size();i++){
                row=m[i];
                for (j=0;j<row.size();j++){
                    print(row[j]);
                }
                print("\n");
            };
            return(null);
        };
        a=createMap();
        showMap(a);
    )",n);
    return 0;
}