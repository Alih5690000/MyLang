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
                };
                a.push_back(row);
            };
            return(a);
        };
        fn showMap(m){
            for (i=0;i<m.size();i++){
                row=m[i];
                for (j=0;j<row.size();j++){
                    print(row[j]," ");
                };
                print("\n");
            };
            println();
            return(null);
        };
        class Player{
            fn __constructor__(self,x,y){
                self.x=x;
                self.y=y;
                return(null);
            };
        };
        plr=Player(5,5);
        print("after creation");
        a=createMap();
        a[plr.x][plr.y]="P";
        showMap(a);
    )",n);
    return 0;
}