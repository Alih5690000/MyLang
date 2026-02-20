#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    std::cout<<"is print in namespace "<<n.count("print")<<std::endl;
    doCode(R"(
        print("Hello, World!");
        if ("a"=="a"){
            print("Equality works!");
        };
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
        fn clearMap(m){
            for (i=0;i<m.size();i++){
                row=m[i];
                for (j=0;j<row.size();j++){
                    row[j]=".";
                };
            };
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
        for (;;){
            in=input();
            print("You entered:",in,"\n");
            print("coords is ",plr.x," ",plr.y);
            clearMap(a);
            a[plr.y][plr.x]="P";
            showMap(a);
            if (in=="w"){
             plr.y-=1;
            };
            if (in=="s"){   
             plr.y+=1;
            };
            if (in=="a"){   
             plr.x-=1;
            };
            if (in=="d"){   
             plr.x+=1;
            };
        };
    )",n);
    return 0;
}