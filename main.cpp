#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <exception>
#include <sstream>
#include <cctype>
#include <functional>

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
    virtual BasicObj* getattr(const std::string& s){
      auto it = attrs.find(s);
      if (it==attrs.end()) throw ValueError();
      return it->second;
    };
    virtual BasicObj* getitem(BasicObj* key){throw NotAvailable();};
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

BasicObj* doCode(std::string code, Namespace& n);
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
      std::cout<<"FunctionObject::call invoked with args "<<args.size()<<std::endl;
      if (args.size()!=argNames.size()){
        throw ValueError();
      }
      Namespace n;
      for (int i=0;i<args.size();i++){
        n[argNames[i]]=args[i];
      }
      return doCode(code,n);
    }
};

class ClassObject;

BasicObj* InstanceObj(ClassObject* cls, std::vector<BasicObj*> args);

class ClassObject:public BasicObj{
  public:
  std::map<std::string,BasicObj*> attrs;
  ClassObject(const std::string& a){
      Namespace cls;
      doCode(a,cls);
      attrs=cls;
  }
  BasicObj* call(std::vector<BasicObj*> args){
    return InstanceObj(this,args);
  }
};

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    InstanceObject(ClassObject* cls, std::vector<BasicObj*> args = {}) {
        klass = cls;

        if (klass->attrs.count("__constructor__")) {
            auto* ctor = dynamic_cast<FunctionObject*>(klass->attrs["__constructor__"]);
            std::vector<BasicObj*> callArgs = { this };
            callArgs.insert(callArgs.end(), args.begin(), args.end());
            ctor->call(callArgs);
        }
    }

    BasicObj* add(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__add__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__add__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable();
    }

    BasicObj* sub(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__sub__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__sub__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable();
    }

    BasicObj* mul(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__mul__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__mul__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable();
    }

    BasicObj* div(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__div__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__div__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable();
    }

    bool equal(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__eq__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__eq__"]);
            std::vector<BasicObj*> args = { this, other };
            auto* res = func->call(args);
            return res->asbool();
        }
        throw NotAvailable();
    }

    bool asbool() override {
        if (klass->attrs.count("__bool__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__bool__"]);
            std::vector<BasicObj*> args = { this };
            auto* res = func->call(args);
            return res->asbool();
        }
        return true;
    }

    std::string str() override {
        if (klass->attrs.count("__str__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__str__"]);
            std::vector<BasicObj*> args = { this };
            auto* res = func->call(args);
            return res->str();
        }
        return "<InstanceObject>";
    }
    BasicObj* getattr(const std::string& name) {
        if (attrs.count(name)) {
            return attrs[name];
        }
        else if (klass->attrs.count(name)) {
            return klass->attrs[name];
        }
        throw ValueError();
    }
};

class FunctionNative:public BasicObj{
  public:
  std::function<BasicObj*(std::vector<BasicObj*>)> func;
  FunctionNative(std::function<BasicObj*(std::vector<BasicObj*>)> f):func(f){}
  BasicObj* call(std::vector<BasicObj*> args) override{
    return func(args);
  }
};

class ListObject:public BasicObj{
  public:
  std::vector<BasicObj*> items;
  ListObject(const std::vector<BasicObj*>& items):items(items){
    attrs["append"] = new FunctionNative([this](std::vector<BasicObj*> args) {
      this->items.push_back(args[0]);
      return nullptr;
    });
  }
  BasicObj* getitem(BasicObj* key) override{
    if (auto i=dynamic_cast<IntObj*>(key)){
      if (i->a<0 || i->a>=items.size()) throw ValueError();
      return items[i->a];
    }
    else{
      throw ValueError();
    }
  }

};

BasicObj* InstanceObj(ClassObject* cls, std::vector<BasicObj*> args) {
    return new InstanceObject(cls, args);
}

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
      std::string raw = code.substr(1, code.size()-2);
      std::string unescaped;
      for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
          ++i;
          char c = raw[i];
          switch (c) {
            case 'n': unescaped.push_back('\n'); break;
            case 't': unescaped.push_back('\t'); break;
            case 'r': unescaped.push_back('\r'); break;
            case '\\': unescaped.push_back('\\'); break;
            case '"': unescaped.push_back('"'); break;
            case '\'': unescaped.push_back('\''); break;
            case '0': unescaped.push_back('\0'); break;
            default: unescaped.push_back(c); break;
          }
        } else {
          unescaped.push_back(raw[i]);
        }
      }
      return new StringObject(unescaped);
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
      std::string expr;
      int depth=1;
      int exprendpos;
      for (int i=4;i<code.size();i++){
        if (code[i]=='(') depth++;
        if (code[i]==')'){ 
          depth--;
          if (depth==0){
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
      }
      else{
        throw SyntaxError();
      }
      if (getline(ss,tmp,';')){
        cond=tmp;
      }
      else{
        throw SyntaxError();
      }
      if (getline(ss,tmp,';')){
        counter=tmp;
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
    else if (code.size()>=5 && code.substr(0,5)=="class"){
      std::string name;
      int i=5;
        if (i<code.size() && code[i]=='('){
          i++;
          for (; i<code.size() && code[i]!=')'; ++i){
            name+=code[i];
          }
          if (i>=code.size() || code[i]!=')') throw SyntaxError();
          i++;
        }
        else{
          for (; i<code.size() && (std::isalnum((unsigned char)code[i]) || code[i]=='_'); ++i){
            name+=code[i];
          }
        }
        if (name.empty()) throw SyntaxError();
      std::string body;
      int bracedepth=0;
      bool started=false;
      for (;i<code.size();i++){
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
      ClassObject* cls = new ClassObject(body);
      n[name]=cls;
      return cls;
    }
    else if (code.size()>=2 && code.substr(0,2)=="fn"){
      std::string name;
      int i=2;
      
      if (i>=code.size() || code[i]!='(') throw SyntaxError();
      i++;
      
      for (; i<code.size() && (std::isalnum((unsigned char)code[i]) || code[i]=='_'); ++i){
        name+=code[i];
      }
      if (name.empty()) throw SyntaxError();
      
      if (i>=code.size() || code[i]!='(') throw SyntaxError();
      i++;
      
      std::string argStr;
      for (; i<code.size() && code[i]!=')'; ++i){
        argStr+=code[i];
      }
      if (i>=code.size() || code[i]!=')') throw SyntaxError();
      i++;
      
      if (i>=code.size() || code[i]!=')') throw SyntaxError();
      i++;
      
      std::vector<std::string> argNames;
      std::string tmp;
      std::stringstream ss(argStr);
      while (getline(ss,tmp,',')){
        argNames.push_back(tmp);
      }
      std::string body;
      int bracedepth=0;
      bool started=false;
      for (;i<code.size();i++){
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
      FunctionObject* func = new FunctionObject(argNames, body);
      n[name]=func;
      return func;
    }
    std::string name;
    bool sheerName=true;
    for (int i=0;i<code.size();i++) {
       if (!std::isalpha(code[i]) && code[i] != '_' || code[i]=='"'){ 
         sheerName=false;
         if (name.empty()) break;
         if (code[i]=='{') break;
         if (code[i] =='(') {
           i++;
           std::string inGap;
           bool inQuote=false;
           for (int j=i;j<code.size();j++){
            if (code[j]=='"') inQuote=!inQuote;
             if (code[j] ==')' && !inQuote) break;
             inGap+=code[j];
           }
           std::vector<BasicObj*> args;
           std::string tmp;
           
           inQuote = false;
           for (size_t k = 0; k < inGap.size(); k++) {
             if (inGap[k] == '"') {
               inQuote = !inQuote;
             }
             if (inGap[k] == ',' && !inQuote) {
               args.push_back(exec(tmp, n));
               tmp.clear();
             } else {
               tmp += inGap[k];
             }
           }
           if (!tmp.empty()) {
             args.push_back(exec(tmp, n));
           }
           
           if (name=="print"){
            for (auto i:args){
              if (i) std::cout<<i->str(); else std::cout<<"<null>";
            }
            std::cout<<std::endl;
            return nullptr;
          }
          if (name=="return"){
            if (args.size()!=1) throw SyntaxError();
            return args[0];
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
         else if (code[i]=='['){
          i++;
          std::string inGap;
          bool inQuote=false;
          for (int j=i;j<code.size();j++){
            if (code[j]=='"') inQuote=!inQuote;
             if (code[j] ==']' && !inQuote) break;
             inGap+=code[j];
           }
           BasicObj* a=exec(name,n);
           if (!a) throw ValueError();
           BasicObj* key=exec(inGap,n);
           if (!key) throw ValueError();
           return a->getitem(key);
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
            return res;
         }
         else if(code[i]=='.'){
          std::string attr;
          int j;
          for (j=i+1;j<code.size();j++){
            if (code[j]=='='|| code[j]==';' || code[j]=='(') break;
            attr+=code[j];
          }
          BasicObj* obj=exec(name,n);
          if (!obj) throw ValueError();
          if (j<code.size() && code[j]=='='){
            std::string value;
            for (int k=j+1;k<code.size();k++) value+=code[k];
            BasicObj* res = exec(value,n);
            BasicObj* old = nullptr;
            if (obj->attrs.find(attr)!=obj->attrs.end()) old = obj->attrs[attr];
            obj->attrs[attr]=res;
            if (old && old!=res) old->refcount--;
            return res;
          }
          std::cout<<"Accessing attribute "<<attr<<" of object "<<name<<std::endl;
          BasicObj* attr_obj = obj->getattr(attr);
          
          if (j<code.size() && code[j]=='('){
            int k=j+1;
            std::string inGap;
            bool inQuote=false;
            int parenDepth=1;
            for (;k<code.size();k++){
              if (code[k]=='"') inQuote=!inQuote;
              if (!inQuote && code[k]=='(') parenDepth++;
              if (!inQuote && code[k]==')') {
                parenDepth--;
                if (parenDepth==0) break;
              }
              inGap+=code[k];
            }
            std::vector<BasicObj*> args;
            if (!inGap.empty()){
              std::istringstream iss(inGap);
              std::string tmp;
              bool inQuote=false;
              int depth=0;
              for (int l=0;l<inGap.size();l++){
                if (inGap[l]=='"') inQuote=!inQuote;
                if (!inQuote && inGap[l]=='(') depth++;
                if (!inQuote && inGap[l]==')') depth--;
                if (!inQuote && inGap[l]==',' && depth==0){
                  args.push_back(exec(tmp,n));
                  tmp="";
                  continue;
                }
                tmp+=inGap[l];
              }
              if (!tmp.empty()) {
                args.push_back(exec(tmp, n));
              }
            }
            return attr_obj->call(args);
          }
          return attr_obj;
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

BasicObj* doCode(std::string code, Namespace& n){
    std::string acc;
    int depth = 0;
    int depth2=0;
    BasicObj* ret;
    for (auto i:code){
      if (i=='{') depth++;
      if (i=='}') depth--;
      if (i=='(') depth2++;
      if (i==')') depth2--;
      if (i==';' && depth==0 && depth2==0){
        ret=exec(acc,n);
        acc.clear();
        continue;
      }
      acc+=i;
    }
    if (!acc.empty()){
      ret=exec(acc,n);
    }
    return ret;
}

void __clean(){
  for (int i=(int)__objs.size()-1;i>=0;i--){
    if (__objs[i]->refcount<=0){
      BasicObj* obj = __objs[i];
      delete obj;
      __objs.erase(__objs.begin()+i);
    }
  }
}
