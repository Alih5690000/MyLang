#include <iostream>
#include "main.cpp"

//g++ arraytest.cpp -o arraytest && ./arraytest

int main(){
    Namespace n=CreateContext();
    doCode(R"(
        a=list();
        a.push_back([1,2,3]);
        a.push_back([4,5,6]);
        for (i=0;i<a.size();i++){
            print("Here");
            for (j=0;j<a[i].size();j++){
                print(a[i][j]);
            };
            print("\n");
        };
        if ((2+2)==4){
            print("Math works!\n");
        };
        print(a);
    )",n);
}