#include <iostream>
#include "main.cpp"

int main(){
    BasicObj* a=exec("314");
    std::cout<<a->str()<<std::endl;
    return 0;
}