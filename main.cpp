#include <vector>
#include <string>
#include <map>
class BasicObj;

class BasicObj{
    public:
    virtual BasicObj* add(BasicObj*){};
    virtual BasicObj* radd(BasicObj*){};
    virtual BasicObj* sub(BasicObj*){};
    virtual BasicObj* rsub(BasicObj*){};
    virtual BasicObj* mul(BasicObj*){};
    virtual BasicObj* rmul(BasicObj*){};
    virtual BasicObj* div(BasicObj*){};
    virtual BasicObj* rdiv(BasicObj*){};
    virtual void free(){};
    virtual BasicObj* call(std::vector<BasicObj*>){};
    virtual ~BasicObj(){};
    std::map<std::string,BasicObj*> attrs;
};

class IntObj:public BasicObj{
    int a;
    public:
    IntObj(int a){
        this->a=a;
    }
    BasicObj* add(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a+i->a);
      }
      else{
        return b->radd(this);
      }
    }
    BasicObj* sub(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a-i->a);
      }
      else{
        return b->rsub(this);
      }
    }
    BasicObj* div(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a/i->a);
      }
      else{
        return b->rdiv(this);
      }
    }
    BasicObj* mul(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a*i->a);
      }
      else{
        return b->rmul(this);
      }
    }
};

BasicObj* exec(std::string code){
    bool quoted=false;
    for (int i=0;i<code.size();i++){
      if (code[i]=='"'){
        quoted=!quoted;
        continue;
      }
      if (!quoted && code[i]==' '){
        code.erase(code.begin()+i);
      }
    }
    bool isInt=true;
    for (int i=0;i<code.size();i++){
      if (code[i]<'0' || code[i]>'9'){ 
        isInt=false;
        break;
      }
    }
    if (isInt){
      return new IntObj(std::stoi(code));
    }
}
