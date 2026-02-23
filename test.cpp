#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=CreateContext();
    std::cout<<"is print in namespace "<<n.count("print")<<std::endl;
    doCode(R"(
        print("Hello, World!");
        arr=[1,2,3];
        println(typeNo(arr.size()));
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
            fn a(self){
                println("a");
                return (null);
            };
            fn move(self,dx,dy){
                self.x=self.x+dx;
                self.y=self.y+dy;
                return(null);
            };
        };
        plr=Player(5,5);
        plr.a(plr);
        print("after creation");
        a=createMap();
        for (;;){
            in=input();
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
            clearMap(a);
            a[plr.y][plr.x]="P";
            showMap(a);
        };
    )",n);
    return 0;
}