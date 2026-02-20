#include "main.cpp"
#include <map>

extern "C" void Clean(){
    for (int i=0;i<__objs.size();i++){
        if (__objs[i]->refcount<=0){
            delete __objs[i];
            __objs.erase(__objs.begin()+i);
            i--;
        }
    }
}

extern "C" Namespace* Load(){
    Namespace* n=new Namespace({{"a",new IntObj(5)}});
    {
        (*n)["sum"]=new FunctionNative([](std::vector<BasicObj*> args){
            BasicObj* res=new IntObj(0);
            for (auto i:args){
                res->refcount--;
                res=res->add(i,false);
            }
            return res;
        });
    }
    return n;
}
