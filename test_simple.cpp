#include "new.cpp"
int main(){
    Context c=CreateContext();
    auto nums = parseCode("x = [1, 2, 3];", c);
    std::cout << "Simple list created" << std::endl;
    return 0;
}
