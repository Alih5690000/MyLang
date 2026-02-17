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

class ValueError:public std::exception{
  public:
  const char* what() const noexcept override{
    return "Value is invalid for that operation";
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
    public:
    int a;
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
    public:
    float a;
    FloatObj(float a){
        this->a=a;
    }
    BasicObj* add(BasicObj* b) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(a+i->a);
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
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
      else if(auto i=dynamic_cast<IntObj*>(b)){
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
      else if(auto i=dynamic_cast<IntObj*>(b)){
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
      else if(auto i=dynamic_cast<IntObj*>(b)){
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

BasicObj* exec(std::string code, Namespace& n);

class FunctionObject:public BasicObj{
    public:
    std::vector<std::string> argNames;
    std::string code;
    FunctionObject(std::vector<std::string> argNames,std::string code){
      this->argNames=argNames;
      this->code=code;
    }
    BasicObj* call(std::vector<BasicObj*> args) override{
      if (args.size()!=argNames.size()){
        throw ValueError();
      }
      Namespace n;
      for (int i=0;i<args.size();i++){
        n[argNames[i]]=args[i];
      }
      return exec(code,n);
    }
};

void remBrackets(std::string& code){
  if (code[0]=='(' && code[code.size()-1]==')'){
    code=code.substr(1,code.size()-2);
  }
}

BasicObj* exec(std::string code, Namespace& n){
    try{
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
    std::string name;
    bool sheerName=true;
    for (int i=0;i<code.size();i++) {
       if (!std::isalpha(code[i]) && code[i] != '_'){ 
         sheerName=false;
         if (name.empty()) break;
         if (code[i] =='(') {
           i++;
           std::string inGap;
           for (int j=i;j<code.size();j++){
             if (code[j] ==')') break;
             inGap+=code[j];
           }
           std::vector<BasicObj*> args;
           std::string tmp;
           std::stringstream ss(inGap);
           while (getline(ss,tmp,','))
              args.push_back(exec(tmp,n));
           BasicObj* a=exec(name,n);
           try{
             return a->call(args);
           }
           catch (...){
            std::cout<<"Tried call to "<<name<<" with args "<<inGap<<std::endl;
            throw;
           }
         } 
         else if(code[i]=='='){
            std::string value;
            for (int j=i+1;j<code.size();j++){
              value+=code[j];
            }
            BasicObj* res=exec(value,n);\
            delete n[name];
            n[name]=res;
            return res;
         }
       } 
       name+=code[i];
    } 
    if (sheerName){
      if (n.find(name)!=n.end()){
        return n[name];
      }
      else{
        throw ValueError();
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
    bool hasOp=false;
    int gapDepth=0;
    for (int i=0;i<code.size();i++){
      if (code[i]=='(') gapDepth++;
      if (code[i]==')') gapDepth--;
      if ((code[i]=='+' || code[i]=='-') && gapDepth==0){
        hasOp=true;
        break;
      }
    }
    if (!hasOp){
      std::cout<<"Short "<<code<<std::endl;
      BasicObj* res=nullptr;
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
            remBrackets(acc);
            left=exec(acc,n);
            if (!res) res=left;
            op=i;
            side=true;
            acc="";
            continue;
          }
          else{
            remBrackets(acc);
            right=exec(acc,n);
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
        right=exec(acc,n);
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
  gapDepth=0;
  for (int i=0;i<code.size();i++){
    if (code[i]=='(') gapDepth++;
    if (code[i]==')') gapDepth--;
    if ((code[i]=='+' || code[i]=='-') && gapDepth==0){
      if (!side){
        op=code[i];
        remBrackets(acc);
        res=exec(acc,n);
        side=true;
      }
      else{
        remBrackets(acc);
        right=exec(acc,n);
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
    remBrackets(acc);
    right=exec(acc,n);
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
    catch (const ValueError&){
      throw;
    }
}

void doCode(std::string code, Namespace& n){
    std::string acc;
    std::stringstream ss(code);
    while (std::getline(ss, acc, ';')) {
      exec(acc,n);
    }
}
