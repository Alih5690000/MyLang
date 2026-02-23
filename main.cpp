#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <exception>
#include <sstream>
#include <cctype>
#include <functional>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif


class BasicObj;

class NotAvailable:public std::exception{
  public:
  std::string mes;
  NotAvailable(const char* mes) {
    this->mes=mes;
  }
  const char* what() const noexcept override{
    return mes.c_str();
  }
};

class ValueError:public std::exception{
  public:
  std::string mes;
  ValueError(const char* mes) {
    this->mes=mes;
  }
  const char* what() const noexcept override{
    return mes.c_str();
  }
};

class SyntaxError:public std::exception{
  public:
  std::string mes;
  SyntaxError(const char* mes) {
    this->mes=mes;
  }
  const char* what() const noexcept override{
    return mes.c_str();
  }
};

class ReturnSig{
  public:
  BasicObj* val;
  ReturnSig(BasicObj* val) {
    this->val=val;
  }
};

class BasicObj;

static std::vector<BasicObj*> __objs;

class BasicObj{
    public:
    int typeID=0;
    int refcount=1;
    virtual BasicObj* add(BasicObj*,bool){throw NotAvailable("That is Base class (add)");};
    virtual BasicObj* sub(BasicObj*,bool){throw NotAvailable("That is Base class (sub)");};
    virtual BasicObj* mul(BasicObj*,bool){throw NotAvailable("That is Base class (mul)");};
    virtual BasicObj* div(BasicObj*,bool){throw NotAvailable("That is Base class (div)");};
    virtual void inc(){throw NotAvailable("That is Base class (inc)");};
    virtual void dec(){throw NotAvailable("That is Base class (dec)");};
    virtual std::string str(){return "Object at "+std::to_string((size_t)this);};
    virtual bool greater(BasicObj*,bool){throw NotAvailable("That is Base class (greater)");};
    virtual bool less(BasicObj*,bool){throw NotAvailable("That is Base class (less)");};
    virtual bool equal(BasicObj*,bool){throw NotAvailable("That is Base class (equal)");};
    virtual bool asbool(){throw NotAvailable("That is Base class (asbool)");};
    virtual void free(){throw NotAvailable("That is Base class (free)");};
    virtual BasicObj* getattr(const std::string& s){
      auto it = attrs.find(s);
      if (it==attrs.end()) throw ValueError("Attribute not found");
      return it->second;
    };
    virtual void setattr(const std::string& name,BasicObj* o){
      attrs[name]=o->clone();
    }
    virtual BasicObj* getitem(BasicObj* key){throw NotAvailable("That is Base class (getitem)");};
    virtual BasicObj* setitem(std::vector<BasicObj*>){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* call(std::vector<BasicObj*>){throw NotAvailable("That is Base class (call)");};
    virtual void setitem(BasicObj* key, BasicObj* value){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* clone(){throw NotAvailable("That is Base class (clone)");};
    BasicObj(){
      __objs.push_back(this);
    }
    virtual ~BasicObj()=default;
    std::map<std::string,BasicObj*> attrs;
};

typedef std::map<std::string,BasicObj*> Namespace;
std::vector<std::function<void()>> __cleanupFuncs={[](){
  for (int i=(int)__objs.size()-1;i>=0;i--){
    if (__objs[i]->refcount<=0){
      BasicObj* obj = __objs[i];
      delete obj;
      __objs.erase(__objs.begin()+i);
    }
  }
}};
Namespace CreateContext();

class StringObject : public BasicObj {
public:
    std::string value;

    StringObject(const std::string& v) : value(v) {
      typeID=1;
    }

    std::string str() override {
        return value;
    }

    BasicObj* add(BasicObj* other, bool) override {
        StringObject* o = dynamic_cast<StringObject*>(other);
        if (!o) throw NotAvailable("Cannot add non-string object to string");
        return new StringObject(value + o->value);
    }

    bool equal(BasicObj* other, bool) override {
        StringObject* o = dynamic_cast<StringObject*>(other);
        if (!o) throw NotAvailable("Cannot compare non-string object to string");
        return value == o->value;
    }

    bool asbool() override {
        return !value.empty();
    }
    BasicObj* clone() override{
        return new StringObject(value);
    }
    ~StringObject() override = default;
};

class IntObj:public BasicObj{
    
    public:
    int a;
    IntObj(int a){
        this->a=a;
        typeID=2;
    }
    BasicObj* add(BasicObj* b,bool swapped) override{
      if (auto i=dynamic_cast<IntObj*>(b)){
        return new IntObj(this->a + i->a);
      }
      else{
        if (!swapped)
          return b->add(this,true);
        else
          throw ValueError("Cannot add non-integer object to integer");
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
          throw ValueError("Cannot subtract non-integer object from integer");
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
          throw ValueError("Cannot divide non-integer object by integer");
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
          throw ValueError("Cannot multiply non-integer object by integer");
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
          throw ValueError("Cannot compare non-integer object to integer");
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
          throw ValueError("Cannot compare non-integer object to integer");
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
          throw ValueError("Cannot compare non-integer object to integer");
      }
    }
    std::string str() override{
      return std::to_string(a);
    }
    bool asbool() override{
      return a;
    }
    void inc() override{
      a++;
    }
    void dec() override{
      a--;
    }
    BasicObj* clone() override{
        return new IntObj(a);
    }
    ~IntObj() override = default;
};

class BoolObj:public BasicObj{
  
    public:
  bool a;
  BoolObj(bool s):a(s){typeID=3;}
  bool asbool() override{
    return a;
  }
  std::string str() override{
    return a?"true":"false";
  }
  BasicObj* clone() override{
    return new BoolObj(a);
  }
  ~BoolObj() override = default;
};

class FloatObj:public BasicObj{
    public:
    float a;
    FloatObj(float a){
        this->a=a;
        typeID=4;
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
          throw ValueError("Cannot add non-float object to float");
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
          throw ValueError("Cannot subtract non-float object from float");
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
          throw ValueError("Cannot divide non-float object by float");
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
          throw ValueError("Cannot multiply non-float object by float");
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
          throw ValueError("Cannot compare non-float object to float");
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
          throw ValueError("Cannot compare non-float object to float");
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
          throw ValueError("Cannot compare non-float object to float");
      }
    }
    std::string str() override{
      return std::to_string(a);
    }
    bool asbool() override{
      return a;
    }
    void inc() override{
      a++;
    }
    void dec() override{
      a--;
    }
    BasicObj* clone() override{
        return new FloatObj(a);
    }
    ~FloatObj() override = default;
};

BasicObj* doCode(std::string code, Namespace& n);
BasicObj* exec(std::string code, Namespace& n);

class MapObject:public BasicObj{
  public:
  std::map<BasicObj*,BasicObj*> items;
  MapObject(){
    typeID=5;
  }
  BasicObj* getitem(BasicObj* key) override{
    std::cout<<"getitem "<<key->str()<<std::endl;
    for (auto [k,val]:items){
      if (k->equal(key,false)){
        return val;
      }
    }
    throw ValueError("Key not found");
  }
  void setitem(BasicObj* key, BasicObj* value) override{
    std::cout<<"setitem "<<key->str()<<" to "<<(value? value->str() : "<null>")<<std::endl;
    items[key]=value;
  }
  std::string str() override{
    std::string result = "{";
    bool first=true;
    for (const auto& pair : items){
      if (!first) result += ", ";
      first=false;
      result += pair.first->str() + ": " + (pair.second? pair.second->str() : "<null>");
    }
    result += "}";
    return result;
  }
  BasicObj* clone() override{
    MapObject* newMap = new MapObject();
    for (const auto& pair : items){
      newMap->items[pair.first] = pair.second;
    }
    return newMap;
  }
};

class FunctionObject:public BasicObj{
    public:
    std::vector<std::string> argNames;
    std::string code;
    Namespace context;
    FunctionObject(std::vector<std::string> argNames,std::string code, Namespace* context){
      typeID=6;
      this->argNames=argNames;
      this->code=code;
      this->context=*context;
    }
    BasicObj* call(std::vector<BasicObj*> args) override{
      if (args.size()!=argNames.size()){
        throw ValueError("Function called with wrong number of arguments");
      }
      Namespace localNamespace = context;
      for (int i=0;i<args.size();i++){
        localNamespace[argNames[i]]=args[i];
      }

      try{
        doCode(code,localNamespace);
        throw SyntaxError("Function must have a return statement (can return null with 'return null')");
      }
      catch (const ReturnSig& sig){
        return sig.val;
      }
      catch(...){
        throw;
      }
    }
    std::string str() override{
      return "<FunctionObject>";
    }
    BasicObj* clone() override{
      return new FunctionObject(argNames, code, &context);
    }
     ~FunctionObject() override = default;
};

class NullObject:public BasicObj{
  public:
  NullObject(){
    typeID=7;
  }
  std::string str() override{
    return "null";
  }
  bool asbool() override{
    return false;
  }
  BasicObj* clone() override{
    return new NullObject();
  }
};

class ClassObject;

BasicObj* InstanceObj(ClassObject* cls, std::vector<BasicObj*> args);

int classes=12;

class ClassObject:public BasicObj{
  public:
  int instanceID=classes++;
  std::map<std::string,BasicObj*> attrs;
  std::string a;
  Namespace* context;
  ClassObject(const std::string& a, Namespace* context){
      typeID=8;
      Namespace cls=*context;
      doCode(a,cls);
      attrs=cls;
      this->a=a;
      this->context=context;
  }
  BasicObj* call(std::vector<BasicObj*> args){
    BasicObj* instance = InstanceObj(this,args);
    instance->typeID=instanceID;
    return instance;
  }
  std::string str() override{
    return "<ClassObject>";
  }
  BasicObj* clone() override{
    return new ClassObject(a,context);
  }
};

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    std::vector<BasicObj*> args;
    InstanceObject(ClassObject* cls, std::vector<BasicObj*> args = {}, Namespace* context = nullptr) {
        klass = cls;
        this->args = args;

        if (klass->attrs.count("__constructor__")) {
            auto* ctor = dynamic_cast<FunctionObject*>(klass->attrs["__constructor__"]);
            if (!ctor) throw ValueError("__constructor__ is not a function");
            std::vector<BasicObj*> callArgs = { this };
            for (auto* arg : args){ 
              callArgs.push_back(arg);
            }
            ctor->call(callArgs);
        }
    }

    BasicObj* add(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__add__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__add__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable("Cannot add non-object to object");
    }

    BasicObj* sub(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__sub__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__sub__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable("Cannot subtract non-object from object");
    }

    BasicObj* mul(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__mul__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__mul__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable("Cannot multiply non-object by object");
    }

    BasicObj* div(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__div__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__div__"]);
            std::vector<BasicObj*> args = { this, other };
            return func->call(args);
        }
        throw NotAvailable("Cannot divide non-object by object");
    }

    bool equal(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__eq__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__eq__"]);
            std::vector<BasicObj*> args = { this, other };
            auto* res = func->call(args);
            return res->asbool();
        }
        throw NotAvailable("Cannot compare non-object to object");
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
        throw ValueError("Attribute not found");
    }
    void setitem(BasicObj* key, BasicObj* value) override{
        std::string attrName = key->str();
        attrs[attrName] = value;
    }
    BasicObj* clone() override{
        std::vector<BasicObj*> newArgs;
        for(auto* a : args) newArgs.push_back(a->clone());
        return new InstanceObject(klass, newArgs);
    }
    ~InstanceObject() override = default;

};

class FunctionNative:public BasicObj{
  public:
  std::function<BasicObj*(std::vector<BasicObj*>)> func;
  FunctionNative(std::function<BasicObj*(std::vector<BasicObj*>)> f):func(f){typeID=10;}
  BasicObj* call(std::vector<BasicObj*> args) override{
    return func(args);
  }
    std::string str() override{
      return "<NativeFunction>";
    }
    BasicObj* clone() override{
      return new FunctionNative(func);
    }
     ~FunctionNative() override = default;
};

class ListObject:public BasicObj{
  public:
  std::vector<BasicObj*> items;
  ListObject(const std::vector<BasicObj*>& items):items(items){
    typeID=11;
    auto appendFunc = new FunctionNative([this](std::vector<BasicObj*> args) {
      this->items.push_back(args[0]->clone());
      return nullptr;
    });
    attrs["append"] = appendFunc;
    attrs["push_back"] = appendFunc;
    attrs["size"]=new FunctionNative([this](std::vector<BasicObj*> args){
      return new IntObj(this->items.size());
    });
  }
  BasicObj* getitem(BasicObj* key) override{
    if (auto i=dynamic_cast<IntObj*>(key)){
      if (i->a<0 || i->a>=items.size()) throw ValueError("Index out of bounds");
      return items[i->a];
    }
    else{
      throw ValueError("Key is not an integer");
    }
  }
  void setitem(BasicObj* key, BasicObj* value) override{
    if (auto i=dynamic_cast<IntObj*>(key)){
      if (i->a<0 || i->a>=items.size()) throw ValueError("Index out of bounds");
      BasicObj* old = items[i->a];
      items[i->a] = value;
      if (old && old!=value) old->refcount--;
    }
    else{
      throw ValueError("Key is not an integer");
    }
  }
  std::string str() override{
    std::string result = "[";
    for (size_t i=0; i<items.size(); i++){
      if (i>0) result += ", ";
      if (items[i]) result += items[i]->str();
      else result += "<null>";
    }
    result += "]";
    return result;
  }
  BasicObj* clone() override{
    return new ListObject(items);
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

int __count=0;

BasicObj* exec(std::string code, Namespace& n){
  //std::cout<<"source "<<code<<std::endl;
  __count++;
  //if (__count>5) return nullptr;
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
    if (code.substr(0,2)=="//"){
      return nullptr;
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

    if (code.size() >= 2 && code[0] == '[' && code.back() == ']') {
      if (code.size() == 2){ 
        return new ListObject({});
      }
      std::vector<BasicObj*> items;
      std::string inner = code.substr(1, code.size() - 2);
      std::string tmp;
      bool inQuote = false;
      int depth = 0;
      for (size_t i = 0; i < inner.size(); ++i) {
        char c = inner[i];
        if (c == '"') {
          inQuote = !inQuote;
        }
        if (!inQuote) {
          if (c == '[') depth++;
          else if (c == ']') depth--;
          if (c == ',' && depth == 0) {
            if (!tmp.empty()) {
              items.push_back(exec(tmp, n));
            }
            tmp.clear();
            continue;
          }
        }
        tmp += c;
      }
      if (!tmp.empty()) {
        items.push_back(exec(tmp, n));
      }
      BasicObj* res=new ListObject(items);
      return res;
    }

    if (code.empty()) return nullptr;
    if (!code.empty() && code.back()==';') code.pop_back();
    if (code.empty()) return nullptr;
    if (code.size()>=2 && code[0]=='i' && code[1]=='f'){
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
        throw SyntaxError("Invalid for loop syntax");
      }
      if (getline(ss,tmp,';')){
        cond=tmp;
      }
      else{
        throw SyntaxError("Invalid for loop syntax");
      }
      if (!getline(ss,tmp,';')){
        counter="";
      }
      else{
        counter=tmp;
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
        }
        body+=code[i];
      }
      int i=0;
      for(;;){
        BasicObj* condObj = nullptr;
        if (!cond.empty()){
          condObj = exec(cond, n);
          if (!condObj) break;
          if (!condObj->asbool()) break;
        }
        doCode(body,n);
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
          if (i>=code.size() || code[i]!=')') throw SyntaxError("Invalid class syntax");
          i++;
        }
        else{
          for (; i<code.size() && (std::isalnum((unsigned char)code[i]) || code[i]=='_'); ++i){
            name+=code[i];
          }
        }
        if (name.empty()) throw SyntaxError("Invalid class name");
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
      ClassObject* cls = new ClassObject(body,&n);
      n[name]=cls;
      return cls;
    }
    else if (code.size()>=2 && code.substr(0,2)=="fn"){
      std::string name;
      int i=2;
      
      for (; i<code.size() && (std::isalnum((unsigned char)code[i]) || code[i]=='_'); ++i){
        name+=code[i];
      }
      if (name.empty()) throw SyntaxError("Invalid function name");
      
      while (i<code.size() && (code[i]==' ' || code[i]=='\t' || code[i]=='\n')) i++;
      
      if (i>=code.size() || code[i]!='(') throw SyntaxError("Invalid function syntax");
      i++;

      std::string argStr;
      for (; i<code.size() && code[i]!=')'; ++i){
        argStr+=code[i];
      }
      if (i>=code.size() || code[i]!=')') throw SyntaxError("Invalid function syntax");
      i++;
      
      std::vector<std::string> argNames;
      std::string tmp;
      if (!argStr.empty()){
        std::stringstream ss(argStr);
        while (getline(ss,tmp,',')){
          argNames.push_back(tmp);
        }
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
        }
        body+=code[i];
      }
      FunctionObject* func = new FunctionObject(argNames, body, &n);
      n[name]=func;
      return func;
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
      if (left.empty() || right.empty()) throw ValueError("Comparison operands are empty");
      remBrackets(left);
      remBrackets(right);
      BasicObj* L = exec(left, n);
      BasicObj* R = exec(right, n);
      if (!L || !R) throw ValueError("Comparison operands are not objects");
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
           int gapDepth=1;
           int squeareDepth=0;
           bool inQuote=false;
           for (int j=i;j<code.size();j++){
            if (code[j]=='"') inQuote=!inQuote;
            if (!inQuote && code[j]=='(') gapDepth++;
            if (!inQuote && code[j]==')') gapDepth--;
            if (!inQuote && code[j]=='[') squeareDepth++;
            if (!inQuote && code[j]==']') squeareDepth--;
            if (code[j] ==')' && !inQuote && gapDepth==0 && squeareDepth==0) break;
             inGap+=code[j];
           }
           std::vector<BasicObj*> args;
           std::string tmp;
           
           gapDepth=0;
           squeareDepth=0;
           inQuote = false;
           for (size_t k = 0; k < inGap.size(); k++) {
             if (inGap[k] == '"') {
               inQuote = !inQuote;
             }
              if (!inQuote && inGap[k] == '(') gapDepth++;
              if (!inQuote && inGap[k] == ')') gapDepth--;
              if (!inQuote && inGap[k] == '[') squeareDepth++;
              if (!inQuote && inGap[k] == ']') squeareDepth--;
             if (inGap[k] == ',' && !inQuote && gapDepth == 0 && squeareDepth == 0) {
               args.push_back(exec(tmp, n));
               tmp.clear();
             } else {
               tmp += inGap[k];
             }
           }
           if (!tmp.empty()) {
             args.push_back(exec(tmp, n));
           }
          if (name=="return"){
            if (args.size()!=1) throw SyntaxError("Return statement must have exactly one argument");
            throw ReturnSig(args[0]);
          }
           BasicObj* a=exec(name,n);
           if (!a) throw ValueError(("Function "+name+" not found").c_str());
           try{
             return a->call(args);
           }
           catch (...){
            throw;
           }
         } 
         else if (code[i]=='['){
          i++;
          std::string inGap;
          bool inQuote=false;
          int bracketDepth=1;
          for (int j=i;j<code.size();j++){
            if (code[j]=='"') inQuote=!inQuote;
            if (!inQuote){
              if (code[j]=='[') bracketDepth++;
              if (code[j]==']') {
                bracketDepth--;
                if (bracketDepth==0) {
                  i=j;
                  break;
                }
              }
            }
             inGap+=code[j];
           }

           BasicObj* a=exec(name,n);
           if (!a) throw ValueError("Indexing non-existent object");
           BasicObj* key=exec(inGap,n);
           if (!key) throw ValueError("Key is not an object");

           if (i+1<code.size() && code[i+1]=='='){
             i++; 
             std::string value;
             for (int j=i+1;j<code.size();j++){
               value+=code[j];
             }
             BasicObj* res=exec(value,n);
             try {
               a->setitem(key, res);
             } catch (...) {
               throw;
             }
             return res;
           }
           
           BasicObj* indexed = a->getitem(key);

           if (i+1<code.size() && (code[i+1]=='[' || code[i+1]=='.' || code[i+1]=='(')){
             std::string remaining;
             for (int j=i+1;j<code.size();j++){
               remaining+=code[j];
             }
             Namespace tempns = n;
             tempns["__indexed_result__"] = indexed;
             return exec("__indexed_result__" + remaining, tempns);
           }
            return indexed;
          }
         else if(code[i]=='='){
            std::string value;
            for (int j=i+1;j<code.size();j++){
              if (code[j]==';') break;
              value+=code[j];
            }
            BasicObj* res=exec(value,n);

            n[name]=res;
            res->refcount++;
            return res;
         }
         else if(i+1<code.size() && code[i]=='+' && code[i+1]=='+'){
            BasicObj* val=n[name];
            val->inc();
            return val;
         }
         else if(i+1<code.size() && code[i]=='-' && code[i+1]=='-'){
            BasicObj* val=n[name];
            val->dec();
            return val;
         }
         if(code[i]=='.'){
          std::string attr;
          int j;
          for (j=i+1;j<code.size();j++){
            if (!std::isalpha(code[j]) && code[j]!='_') break;
            attr+=code[j];
          }
          BasicObj* obj=exec(name,n);
          if (!obj) throw ValueError("Object not found");
          else if (i<code.size() && !std::isalpha(code[i])){
             std::string remaining;
             for (int k=j;k<code.size();k++){
               remaining+=code[k];
             }
             Namespace tempns = n;
             if (remaining[0]=='='){
              std::string val;
              for (int k=1;k<remaining.size();k++){
                if (remaining[k]==';') break;
                val+=remaining[k];
              }
              obj->setattr(attr,exec(val,n));
              return nullptr;
             }
             tempns["__indexed_result__"] = obj->getattr(attr);
             BasicObj* ret=exec("__indexed_result__" + remaining, tempns);
             return ret;
           }
          return obj;
         }
       } 
       name+=code[i];
    } 
    if (sheerName){
      if (n.find(name)!=n.end()){
        return n[name];
      }
      else{
        throw ValueError(("Variable " + name + " not defined").c_str());
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
  for (auto i:__cleanupFuncs){
    i();
  }
}

Namespace CreateContext(){
  Namespace n;
  n["true"]=new BoolObj(true);
  n["false"]=new BoolObj(false);
  n["IntFromString"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("IntFromString expects exactly one argument");
    return new IntObj(std::stoi(args[0]->str()));
  });
  n["print"]=new FunctionNative([](std::vector<BasicObj*> args){
    for (auto i:args){
      if (i) std::cout<<i->str(); else std::cout<<"<null>";
    }
    return nullptr;
  });
  n["input"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()>1) throw ValueError("Input function expects at most one argument");
    std::string prompt;
    if (args.size()==1) prompt=args[0]->str();
    std::cout<<prompt;
    std::cout.flush();

    std::string line;
    std::cin>>line;

    return new StringObject(line);
  });
  n["println"]=new FunctionNative([](std::vector<BasicObj*> args){
    for (auto i:args){
      if (i) std::cout<<i->str(); else std::cout<<"<null>";
    }
    std::cout<<std::endl;
    return nullptr;
  });
  n["null"]=new NullObject();
  n["list"]=new FunctionNative([](std::vector<BasicObj*> args){
    BasicObj* res=new ListObject(args);
    return res;
  });
  n["flushOut"]=new FunctionNative([](std::vector<BasicObj*> args){
    std::cout.flush();
    return nullptr;
  });
  n["currentNamespace"]=new FunctionNative([&n](std::vector<BasicObj*> args){
    ListObject* lst = new ListObject({});
    for (auto& pair : n) {      
      lst->items.push_back(new StringObject(pair.first));
    }
    return lst;
  });
  n["addr"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("addr expects exactly one argument");
    std::stringstream ss;
    ss<<args[0];
    return new StringObject(ss.str());
  });
  n["typeNo"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("typeNo expects exactly one argument");
    return new IntObj(args[0]->typeID);
  });
  n["getRefcount"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("getRefcount expects exactly one argument");
    return new IntObj(args[0]->refcount);
  });
  n["clone"]=new FunctionNative([](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("clone expects exactly one argument");
    return args[0]->clone();
  });
  n["map"]=new FunctionNative([](std::vector<BasicObj*> args){
    return new MapObject();
  });
  n["importdll"]=new FunctionNative([&n](std::vector<BasicObj*> args){
    if (args.size()!=1) throw ValueError("importdll expects exactly one argument");
    std::string dllName=args[0]->str();
      #ifdef _WIN32
          HMODULE hModule = LoadLibraryA(dllName.c_str());
          if (!hModule) {
              throw ValueError("Failed to load DLL: " + dllName);
          }
          FARPROC func = GetProcAddress(hModule, "Load");
          if (!func) {
              FreeLibrary(hModule);
              throw ValueError("Failed to find Load function in DLL: " + dllName);
          }
          FARPROC freeFunc = GetProcAddress(hModule, "Clean");
           if (!freeFunc) {
              FreeLibrary(hModule);
              throw ValueError("Failed to find Clean function in DLL: " + dllName);
          }
          typedef void (*FreeFunc)();
          FreeFunc freeLibrary = (FreeFunc)freeFunc;
          __cleanupFuncs.push_back([hModule, freeLibrary]() {
              freeLibrary();
          });
          typedef Namespace* (*LoadFunc)();
          LoadFunc loadFunc = (LoadFunc)func;
          Namespace* dllNamespace = loadFunc();
          for (const auto& pair : *dllNamespace) {
              n[pair.first] = pair.second;
              __objs.push_back(pair.second);
          }
      #else
          void* handle = dlopen(dllName.c_str(), RTLD_LAZY);
          if (!handle) {
              throw ValueError(("Failed to load shared library: " + dllName + " - " + dlerror()).c_str());
          }
          typedef Namespace* (*LoadFunc)();
          LoadFunc loadFunc = (LoadFunc)dlsym(handle, "Load");
          if (!loadFunc) {
              dlclose(handle);
              throw ValueError(("Failed to find Load function in shared library: " + dllName).c_str());
          }
          Namespace* dllNamespace = loadFunc();
          for (const auto& pair : *dllNamespace) {
              std::cout<<"Importing "<<pair.first<<" from "<<dllName<<std::endl;
              n[pair.first] = pair.second;
          }
      #endif
    return nullptr;
  });
  return n;
}
