#include <iostream>
#include "main.cpp"

int main(){
    Namespace n=Namespace();
    doCode(R"(
        fn createMap(){
            print("in createMap");
            return([]);
        };
    )", n);
    
    std::cout << "Function created" << std::endl;
    
    doCode("a=createMap();", n);
    std::cout << "Function called success" << std::endl;
    
    return 0;
}
