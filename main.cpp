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

class SyntaxError:public std::exception{
  public:
  const char* what() const noexcept override{
    return "Invalid syntax";
  }
};

class BasicObj;

static std::vector<BasicObj*> __objs;

class BasicObj{
    public:
    int refcount=1;
    virtual BasicObj* add(BasicObj*,bool){throw NotAvailable();};
    virtual BasicObj* sub(BasicObj*,bool){throw NotAvailable();};
    virtual BasicObj* mul(BasicObj*,bool){throw NotAvailable();};
    virtual BasicObj* div(BasicObj*,bool){throw NotAvailable();};
    virtual std::string str(){throw NotAvailable();};
    virtual bool greater(BasicObj*,bool){throw NotAvailable();};
    virtual bool less(BasicObj*,bool){throw NotAvailable();};
    virtual bool equal(BasicObj*,bool){throw NotAvailable();};
    virtual bool asbool(){throw NotAvailable();};
    virtual void free(){throw NotAvailable();};
    virtual BasicObj* call(std::vector<BasicObj*>){throw NotAvailable();};
    BasicObj(){
      __objs.push_back(this);
    }
    virtual ~BasicObj()=default;
    std::map<std::string,BasicObj*> attrs;
};

typedef std::map<std::string,BasicObj*> Namespace;

class StringObject : public BasicObj {
public:
    std::string value;

    StringObject(const std::string& v) : value(v) {}

    std::string str() override {
        return value;
    }

    BasicObj* add(BasicObj* other, bool) override {
        StringObject* o = dynamic_cast<StringObject*>(other);
        if (!o) throw NotAvailable();
        return new StringObject(value + o->value);
    }

    bool equal(BasicObj* other, bool) override {
        StringObject* o = dynamic_cast<StringObject*>(other);
        if (!o) throw NotAvailable();
        return value == o->value;
    }

    bool asbool() override {
        return !value.empty();
    }
    ~StringObject() override = default;
};

class IntObj:public BasicObj{
    public:
    int a;
    IntObj(int a){
        this->a=a;
    }
    BasicObj* add(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(this->a + i->a);
      }
      else{
        if (!swapped)
          return b->add(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* sub(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(this->a - i->a);
      }
      else{
        if (!swapped)
          return b->sub(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* div(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(this->a / i->a);
      }
      else{
        if (!swapped)
          return b->div(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* mul(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(this->a * i->a);
      }
      else{
        if (!swapped)
          return b->mul(this,true);
        else
          throw ValueError();
      }
    }
    bool greater(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return this->a > i->a;
      }
      else{
        if (!swapped)
          return b->greater(this,true);
        else
          throw ValueError();
      }
    }
    bool less(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return this->a < i->a;
      }
      else{
        if (!swapped)
          return b->less(this,true);
        else
          throw ValueError();
      }
    }
    bool equal(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return this->a == i->a;
      }
      else{
        if (!swapped)
          return b->equal(this,true);
        else
          throw ValueError();
      }
    }
    std::string str() override{
      return std::to_string(a);
    }
    bool asbool() override{
      return a;
    }
    ~IntObj() override = default;
};

class BoolObj:public BasicObj{
  public:
  bool a;
  BoolObj(bool s):a(s){}
  bool asbool() override{
    return a;
  }
  std::string str() override{
    return a?"true":"false";
  }
  ~BoolObj() override = default;
};

class FloatObj:public BasicObj{
    public:
    float a;
    FloatObj(float a){
        this->a=a;
    }
    BasicObj* add(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(this->a + i->a);
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return new FloatObj(this->a + i->a);
      }
      else{
        if (!swapped)
          return b->add(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* sub(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(this->a - i->a);
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return new FloatObj(this->a - i->a);
      }
      else{
        if (!swapped)
          return b->sub(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* div(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(this->a / i->a);
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return new FloatObj(this->a / i->a);
      }
      else{
        if (!swapped)
          return b->div(this,true);
        else
          throw ValueError();
      }
    }
    BasicObj* mul(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return new FloatObj(this->a * i->a);
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return new FloatObj(this->a * i->a);
      }
      else{
        if (!swapped)
          return b->mul(this,true);
        else
          throw ValueError();
      }
    }
    bool greater(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return this->a > i->a;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return this->a > i->a;
      }
      else{
        if (!swapped)
          return b->greater(this,true);
        else
          throw ValueError();
      }
    }
    bool less(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return this->a < i->a;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return this->a < i->a;
      }
      else{
        if (!swapped)
          return b->less(this,true);
        else
          throw ValueError();
      }
    }
    bool equal(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<FloatObj*>(b)){
        return this->a == i->a;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        return this->a == i->a;
      }
      else{
        if (!swapped)
          return b->equal(this,true);
        else
          throw ValueError();
      }
    }
    std::string str() override{
      return std::to_string(a);
    }
    bool asbool() override{
      return a;
    }
    ~FloatObj() override = default;
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
  //std::cout<<"source "<<code<<std::endl;
    try{
    bool quoted=false;
    for (int i=0;i<code.size();i++){
      if (code[i]=='"'){
        quoted=!quoted;
        continue;
      }
      if (!quoted && (code[i]==' ' || code[i]=='\n' || code[i]=='\t')){
        code.erase(code.begin()+i);
        i--;
      }
    }
    if (code[0]=='"' && code.back()=='"'){
      code = code.substr(1, code.size()-2);
      return new StringObject(code);
    }
    if (code.empty()) return nullptr;
    if (!code.empty() && code.back()==';') code.pop_back();
    if (code.empty()) return nullptr;
    if (code.size()>=2 && code[0]=='i' && code[1]=='f'){
      std::cout<<"if"<<std::endl;
      std::string expr;
      int depth=1;
      BasicObj* e=nullptr;
      int exprendpos;
      for (int i=3;i<code.size();i++){
        if (code[i]=='(') depth++;
        if (code[i]==')'){ 
          depth--;
          if (depth==0){
            std::cout<<"expr is "<<expr<<std::endl;
            e=exec(expr,n);
            exprendpos=i;
            break;
          }
        }
        expr+=code[i];
      }
      if (!e || !e->asbool()) return nullptr;
      std::string body;
      int bracedepth=0;
      bool started=false;
      for (int i=exprendpos+1;i<code.size();i++){
        if (!started){
          if (code[i]=='{'){
            started=true;
            continue;
          }
          else continue;
        }
        if (code[i]=='{'){
          bracedepth++;
        }
        else if (code[i]=='}'){
          if (bracedepth==0){
            exec(body,n);
            return nullptr;
          }
          bracedepth--;
          continue;
        }
        body+=code[i];
      }
    }
    else if (code.size()>=3 && code[0]=='f' && code[1]=='o' && code[2]=='r'){
      std::cout<<"for came "<<code<<std::endl;
      std::string expr;
      int depth=1;
      int exprendpos;
      for (int i=4;i<code.size();i++){
        if (code[i]=='(') depth++;
        if (code[i]==')'){ 
          depth--;
          if (depth==0){
            std::cout<<"expr is "<<expr<<std::endl;
            exprendpos=i;
            break;
          }
        }
        expr+=code[i];
      }
      std::string cond;
      std::string counter;
      std::string tmp;
      std::stringstream ss(expr);
      if (getline(ss,tmp,';')){
        exec(tmp,n);
        std::cout<<"Init is "<<tmp<<std::endl;
      }
      else{
        throw SyntaxError();
      }
      if (getline(ss,tmp,';')){
        cond=tmp;
        std::cout<<"Cond is "<<tmp<<std::endl;
      }
      else{
        throw SyntaxError();
      }
      if (getline(ss,tmp,';')){
        counter=tmp;
        std::cout<<"Counter is "<<tmp<<std::endl;
      }
      else{
        throw SyntaxError();
      }
      std::string body;
      int bracedepth=0;
      bool started=false;
      for (int i=exprendpos+1;i<code.size();i++){
        if (!started){
          if (code[i]=='{'){
            started=true;
            continue;
          }
          else continue;
        }
        if (code[i]=='{'){
          bracedepth++;
        }
        else if (code[i]=='}'){
          if (bracedepth==0){
            break;
          }
          bracedepth--;
          continue;
        }
        body+=code[i];
      }
      std::cout<<"Body is "<<body<<std::endl;
      int i=0;
      for(;;){
        BasicObj* condObj = exec(cond, n);
        if (!condObj) break;
        if (!condObj->asbool()) break;
        exec(body,n);
        exec(counter,n);
        i++;
      }
      return nullptr;
    }
    std::string name;
    bool sheerName=true;
    for (int i=0;i<code.size();i++) {
       if (!std::isalpha(code[i]) && code[i] != '_'){ 
         sheerName=false;
         if (name.empty()) break;
         if (code[i]=='{') break;
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
           if (name=="print"){
            for (auto i:args){
              std::cout<<i->str();
            }
            return nullptr;
           }
           BasicObj* a=exec(name,n);
           if (!a) throw ValueError();
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
            BasicObj* res=exec(value,n);
            BasicObj* old=n[name];
            n[name]=res;
            if (old && old!=res) old->refcount--;
            std::cout<<"After asigning "<<name<<" is "<<n[name]->str()<<std::endl;
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
    bool hasMul=false;
    int gapDepth=0;
    int compPos=-1;
    std::string compOp;
    for (int i=0;i<code.size();i++){
      if (code[i]=='(') gapDepth++;
      if (code[i]==')') gapDepth--;
      if (gapDepth==0){
        if (i+1<code.size() && code[i]=='=' && code[i+1]=='='){
          compPos=i; compOp="=="; break;
        }
        if (i+1<code.size() && code[i]=='!' && code[i+1]=='='){
          compPos=i; compOp="!="; break;
        }
        if (i+1<code.size() && code[i]=='<' && code[i+1]=='='){
          compPos=i; compOp="<="; break;
        }
        if (i+1<code.size() && code[i]=='>' && code[i+1]=='='){
          compPos=i; compOp=">="; break;
        }
        if (code[i]=='<' || code[i]=='>'){
          compPos=i; compOp=std::string(1,code[i]); break;
        }
        if (code[i]=='*' || code[i]=='/'){
          hasMul=true;
        }
        if (code[i]=='+' || code[i]=='-'){
          hasOp=true; break;
        }
      }
    }
    if (compPos!=-1){
      std::string left = code.substr(0, compPos);
      std::string right = code.substr(compPos + compOp.size());
      if (left.empty() || right.empty()) throw ValueError();
      remBrackets(left);
      remBrackets(right);
      BasicObj* L = exec(left, n);
      BasicObj* R = exec(right, n);
      if (!L || !R) throw ValueError();
      bool cres=false;
      if (compOp=="==") cres = L->equal(R,false);
      else if (compOp=="!=") cres = !L->equal(R,false);
      else if (compOp==">") cres = L->greater(R,false);
      else if (compOp=="<") cres = L->less(R,false);
      else if (compOp==">=") cres = L->greater(R,false) || L->equal(R,false);
      else if (compOp=="<=") cres = L->less(R,false) || L->equal(R,false);
      return new IntObj(cres?1:0);
    }
    if (!hasOp && hasMul){
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
              res=res->mul(right,false);
            }
            else if(op=='/'){
              res=res->div(right,false);
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
          res=res->mul(right,false);
        }
        else if(op=='/'){
          res=res->div(right,false);
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
          res=res->add(right,false);
        }
        if (op=='-'){
          res=res->sub(right,false);
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
      res=res->add(right,false);
    }
    if (op=='-'){
      res=res->sub(right,false);
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
    int depth = 0;
    int depth2=0;
    for (auto i:code){
      if (i=='{') depth++;
      if (i=='}') depth--;
      if (i=='(') depth2++;
      if (i==')') depth2--;
      if (i==';' && depth==0 && depth2==0){
        exec(acc,n);
        acc.clear();
        continue;
      }
      acc+=i;
    }
    if (!acc.empty()){
      exec(acc,n);
    }
}

void __clean(){
  for (int i=__objs.size()-1;i>=0;i--){
    if (__objs[i] ->refcount<=0){
      __objs.erase(__objs.begin()+i);
      delete __objs[i];
    } 
  }
}
