#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <exception>
#include <cctype>

class NotAvailable:public std::exception{
  public:
  const char* what() const noexcept override{
    return "That function is not available at Object";
  }
};

class BasicObj;

class BasicObj{
    public:
    virtual BasicObj* add(BasicObj*){throw NotAvailable();};
    virtual BasicObj* radd(BasicObj*){throw NotAvailable();};
    virtual BasicObj* sub(BasicObj*){throw NotAvailable();};
    virtual BasicObj* rsub(BasicObj*){throw NotAvailable();};
    virtual BasicObj* mul(BasicObj*){throw NotAvailable();};
    virtual BasicObj* rmul(BasicObj*){throw NotAvailable();};
    virtual BasicObj* div(BasicObj*){throw NotAvailable();};
    virtual BasicObj* rdiv(BasicObj*){throw NotAvailable();};
    virtual std::string str(){throw NotAvailable();};
    virtual void free(){throw NotAvailable();};
    virtual BasicObj* call(std::vector<BasicObj*>){throw NotAvailable();};
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
    std::string str() override{
      return std::to_string(a);
    }
};

class FloatObj:public BasicObj{
    float a;
    public:
    FloatObj(float a){
        this->a=a;
    }
    BasicObj* add(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a+i->a);
      }
      else{
        return b->radd(this);
      }
    }
    BasicObj* sub(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a-i->a);
      }
      else{
        return b->rsub(this);
      }
    }
    BasicObj* div(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a/i->a);
      }
      else{
        return b->rdiv(this);
      }
    }
    BasicObj* mul(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a*i->a);
      }
      else{
        return b->rmul(this);
      }
    }
    std::string str() override{
      return std::to_string(a);
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
    bool isFloat=false;
    for (int i=0;i<code.size();i++){
      if (code[i]=='.'){ 
        isFloat=true;
        continue;
      }
      if (code[i]<'0' || code[i]>'9'){ 
        isInt=false;
        break;
      }
    }
    if (isInt && isFloat){
      return new FloatObj(std::stof(code));
    }
    else if (isInt){
      return new IntObj(std::stoi(code));
    }
  std::string name;
  int splitterPos=0;
  for (int i=0;i<code.size();i++){
    if (!std::isalpha(code[i]) || code[i]!='_'){
      splitterPos=i;
      break;
    }
    name+=code[i];
  }
  std::vector<BasicObj*> args;
  if (code[splitterPos]=='('){
    std::string inGap;
    for (int i=splitterPos+1;;i++){
      if (code[i]==')') break;
      inGap+=code[i];
    }
    std::vector<std::string> tokens;
    /*finish*/
  }
  return new BasicObj();
}
