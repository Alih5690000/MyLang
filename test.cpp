#include <iostream>
#include "main.cpp"

int main(){
    BasicObj* a=exec("3*4+3*3");
    std::cout<<a->str()<<std::endl;
    return 0;
}