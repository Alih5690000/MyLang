#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <exception>
#include <sstream>
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
    virtual BasicObj* sub(BasicObj*){throw NotAvailable();};
    virtual BasicObj* mul(BasicObj*){throw NotAvailable();};
    virtual BasicObj* div(BasicObj*){throw NotAvailable();};
    virtual std::string str(){throw NotAvailable();};
    virtual void free(){throw NotAvailable();};
    virtual BasicObj* call(std::vector<BasicObj*>){throw NotAvailable();};
    virtual ~BasicObj(){};
    std::map<std::string,BasicObj*> attrs;
};

typedef std::map<std::string,BasicObj*> Namespace;

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
        return b->add(this);
      }
    }
    BasicObj* sub(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a-i->a);
      }
      else{
        return b->sub(this);
      }
    }
    BasicObj* div(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a/i->a);
      }
      else{
        return b->div(this);
      }
    }
    BasicObj* mul(BasicObj* b) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(a*i->a);
      }
      else{
        return b->mul(this);
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
        return b->add(this);
      }
    }
    BasicObj* sub(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a-i->a);
      }
      else{
        return b->sub(this);
      }
    }
    BasicObj* div(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a/i->a);
      }
      else{
        return b->div(this);
      }
    }
    BasicObj* mul(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a*i->a);
      }
      else{
        return b->mul(this);
      }
    }
    std::string str() override{
      return std::to_string(a);
    }
};

BasicObj* exec(std::string code, Namespace n){
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
    if (code[0]=='('){
      code.erase(code.begin());
      code.erase(code.end()-1);
      return exec(code);
    } 
    std::cout<<"Recieved "<<code<<std::endl;
    std::string name;
    for (int i=0;i<code.size();i++) {
       if (!std::isalpha(code[i]) || code[i] ! ='_') {
         if (code[i] =='(') {
           std::string inGap;
           for (int j=i;j<code.size();j++){
             if (code[j] ==')') break;
             inGap+=code[j] ;
           }
           std::vector<BasicObj*> args;
           std::string tmp;
           std::stringstream ss(code) ;
           while (getline(ss,tmp,',') 
              args.push_back(exec(tmp));
           BasicObj* a=exec(name);
           a->call(args);
         } 
       } 
       name+=i;
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
    bool hasOp=false;
    for (int i=0;i<code.size();i++){
      if (code[i]=='+' || code[i]=='-'){
        hasOp=true;
        break;
      }
    }
    if (!hasOp){
      std::cout<<"Short "<<code<<std::endl;
      BasicObj* res=nullptr;;
      BasicObj *left,*right;
      bool side=false;
      char op;
      std::string acc;
      int gapDepth=0;
      for (auto i:code){
        if (i=='(') gapDepth++;
        if (i==')') gapDepth--;
        if ((i=='*' || i=='/') && gapDepth==0){
          if (!side){
            left=exec(acc);
            if (!res) res=left;
            op=i;
            side=true;
            acc="";
            continue;
          }
          else{
            right=exec(acc);
            BasicObj* old=res;
            if (op=='*'){
              res=res->mul(right);
            }
            else if(op=='/'){
              res=res->div(right);
            }
            delete old;
            side=false;
          }
          continue;
          acc="";
        }
        acc+=i;
      }
      if (!acc.empty()){
        right=exec(acc);
        BasicObj* old=res;
        if (op=='*'){
          res=res->mul(right);
        }
        else if(op=='/'){
          res=res->div(right);
        }
        delete old;
        side=false;
      }
      return res;
    }
  BasicObj* res=nullptr;
  BasicObj* right;
  std::string acc;
  bool side=false;
  char op;
  int gapDepth=0;
  for (int i=0;i<code.size();i++){
    if (code[i]=='(') gapDepth++;
    if (code[i]==')') gapDepth--;
    if ((code[i]=='+' || code[i]=='-') && gapDepth==0){
      if (!side){
        op=code[i];
        res=exec(acc);
        side=true;
      }
      else{
        right=exec(acc);
        BasicObj* old=res;
        if (op=='+'){
          res=res->add(right);
        }
        if (op=='-'){
          res=res->sub(right);
        }
        delete old;
      }
      acc="";
      continue;
    }
    acc+=code[i];
  }
  if (!acc.empty()){
    right=exec(acc);
    BasicObj* old=res;
    if (op=='+'){
      res=res->add(right);
    }
    if (op=='-'){
      res=res->sub(right);
    }
    delete old;
  }
  return res;
}
