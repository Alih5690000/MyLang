#include "main.hpp"
#include <iostream>
#include <map>

//g++ -shared -o module.so -fPIC moduletest.cpp
extern "C" void Load(Namespace& n,int* t,std::vector<BasicObj*>* a){
    {
        classes=t;
        __objs=a;
        std::cout<<"Loading to ns "<<&n<<std::endl;
        n["sum"]=new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
            BasicObj* res=new IntObj(0);
            for (auto i:args){
                res->refcount--;
                res=res->add(i,false);
            }
            return res;
        });
        std::cout<<"Loaded sum"<<std::endl;
        n["BankAccount"] = new ClassObject("", &n);
        n["BankAccount"]->setattr("m",new IntObj(1488));
        n["BankAccount"]->attrs["__constructor__"] =
            new FunctionNative([](auto args, auto& ns){
                std::cout<<"Creating bank account"<<std::flush<<std::endl;
                args[0]->setattr("money", new IntObj(0));
                return nullptr;
        });
        n["BankAccount"]->setattr("inc",new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
            args[0]->setattr("money",args[0]->getattr("money")->add(args[1],false));
            return nullptr;
        }));
        std::cout<<"Finished loading"<<std::endl;
    }
}
