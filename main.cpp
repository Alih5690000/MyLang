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

std::map<std::string, int> typeNames = {
    {"BasicObj",      0},
    {"StringObject",   1},
    {"IntObj",          2},
    {"BoolObj",          3},
    {"FloatObj",         4},
    {"MapObject",         5},
    {"FunctionObject",     6},
    {"NullObject",           7},
    {"ClassObject",            8},
    {"FunctionNative",           10},
    {"ListObject",                 11}
};

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

std::vector<BasicObj*> __objs;
typedef std::map<std::string,BasicObj*> Namespace;

class BasicObj{
    public:
    int typeID=0;
    int refcount=0;
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
      if (it==attrs.end()) throw ValueError(("Attribute "+s+" not found").c_str());
      BasicObj* res = it->second;
      res->refcount++;
      return res;
    };
    virtual void setattr(const std::string& name,BasicObj* o){
      if (attrs.count(name)) {
        attrs[name]->refcount--;
      }
      BasicObj* cloned = o->clone();
      cloned->refcount++;
      attrs[name]=cloned;
    }
    virtual BasicObj* getitem(BasicObj* key){throw NotAvailable("That is Base class (getitem)");};
    virtual BasicObj* setitem(std::vector<BasicObj*>){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* call(std::vector<BasicObj*>,Namespace&){throw NotAvailable("That is Base class (call)");};
    virtual void setitem(BasicObj* key, BasicObj* value){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* clone(){throw NotAvailable("That is Base class (clone)");};
    BasicObj(){
      __objs.push_back(this);
    }
    virtual ~BasicObj()=default;
    std::map<std::string,BasicObj*> attrs;
};

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
      BasicObj* tmp = new StringObject(value + o->value);
      tmp->refcount++;
      return tmp;
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
      BasicObj* tmp = new StringObject(value);
      tmp->refcount++;
      return tmp;
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
        BasicObj* tmp = new IntObj(this->a + i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new IntObj(this->a - i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new IntObj(this->a / i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new IntObj(this->a * i->a);
        tmp->refcount++;
        return tmp;
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
      BasicObj* tmp = new IntObj(a);
      tmp->refcount++;
      return tmp;
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
    BasicObj* tmp = new BoolObj(a);
    tmp->refcount++;
    return tmp;
  }
  ~BoolObj() override = default;
};

class ReferenceObject : public BasicObj {
public:
    BasicObj** o;

    ReferenceObject(BasicObj** target) : o(target) {
        typeID = 12;
    }

    // ===== доступ к целевому объекту =====
    BasicObj* target() {
        if (!o || !*o) throw ValueError("Reference is null");
        return *o;
    }

    // ===== делегация арифметики =====
    BasicObj* add(BasicObj* other, bool swapped) override {
        return target()->add(other, swapped);
    }

    BasicObj* sub(BasicObj* other, bool swapped) override {
        return target()->sub(other, swapped);
    }

    BasicObj* mul(BasicObj* other, bool swapped) override {
        return target()->mul(other, swapped);
    }

    BasicObj* div(BasicObj* other, bool swapped) override {
        return target()->div(other, swapped);
    }

    // ===== инк/дек =====
    void inc() override {
        target()->inc();
    }

    void dec() override {
        target()->dec();
    }

    // ===== сравнения =====
    bool greater(BasicObj* other, bool swapped) override {
        return target()->greater(other, swapped);
    }

    bool less(BasicObj* other, bool swapped) override {
        return target()->less(other, swapped);
    }

    bool equal(BasicObj* other, bool swapped) override {
        return target()->equal(other, swapped);
    }

    // ===== логика =====
    bool asbool() override {
        return target()->asbool();
    }

    // ===== строка =====
    std::string str() override {
        return target()->str();
    }

    // ===== clone =====
    BasicObj* clone() override {
      BasicObj* tmp = new ReferenceObject(o);
      tmp->refcount++;
      return tmp;
    }

    // ===== getattr / setattr =====
    BasicObj* getattr(const std::string& s) override {
        return target()->getattr(s);
    }

    void setattr(const std::string& name, BasicObj* obj) override {
        target()->setattr(name, obj);
    }

    // ===== getitem / setitem =====
    BasicObj* getitem(BasicObj* key) override {
        return target()->getitem(key);
    }

    void setitem(BasicObj* key, BasicObj* value) override {
        target()->setitem(key, value);
    }

    // ===== call (если target callable) =====
    BasicObj* call(std::vector<BasicObj*> args, Namespace& n) override {
      refcount++;
      BasicObj* res = target()->call(args, n);
      refcount--;
      return res;
    }
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
        BasicObj* tmp = new FloatObj(this->a + i->a);
        tmp->refcount++;
        return tmp;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        BasicObj* tmp = new FloatObj(this->a + i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new FloatObj(this->a - i->a);
        tmp->refcount++;
        return tmp;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        BasicObj* tmp = new FloatObj(this->a - i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new FloatObj(this->a / i->a);
        tmp->refcount++;
        return tmp;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        BasicObj* tmp = new FloatObj(this->a / i->a);
        tmp->refcount++;
        return tmp;
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
        BasicObj* tmp = new FloatObj(this->a * i->a);
        tmp->refcount++;
        return tmp;
      }
      else if(auto i=dynamic_cast<IntObj*>(b)){
        BasicObj* tmp = new FloatObj(this->a * i->a);
        tmp->refcount++;
        return tmp;
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
      BasicObj* tmp = new FloatObj(a);
      tmp->refcount++;
      return tmp;
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
        val->refcount++;
        return val;
      }
    }
    throw ValueError("Key not found");
  }
  void setitem(BasicObj* key, BasicObj* value) override{
    std::cout<<"setitem "<<key->str()<<" to "<<(value? value->str() : "<null>")<<std::endl;
    if (items.count(key)) {
      items[key]->refcount--;
    }
    value->refcount++;
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
    newMap->refcount++;
    return newMap;
  }
};

class FunctionObject:public BasicObj{
    public:
    std::vector<std::string> argNames;
    std::string code;
    FunctionObject(std::vector<std::string> argNames,std::string code){
      typeID=6;
      this->argNames=argNames;
      this->code=code;
    }
    BasicObj* call(std::vector<BasicObj*> args,Namespace& n) override{
      refcount++;
      if (args.size()!=argNames.size()){
        refcount--;
        throw ValueError(("Function called with wrong number of arguments. "+
          std::to_string(args.size())+" instead of "+std::to_string(argNames.size())).c_str());
      }
      Namespace localNamespace = n;
      for (int i=0;i<args.size();i++){
        localNamespace[argNames[i]]=args[i];
      }

      try{
        doCode(code,localNamespace);
        refcount--;
        throw SyntaxError("Function must have a return statement (can return null with 'return null')");
      }
      catch (const ReturnSig& sig){
        refcount--;
        return sig.val;
      }
      catch(...){
        refcount--;
        throw;
      }
    }
    std::string str() override{
      return "<FunctionObject>";
    }
    BasicObj* clone() override{
      BasicObj* tmp = new FunctionObject(argNames, code);
      tmp->refcount++;
      return tmp;
    }
     ~FunctionObject() override = default;
};

class BoundMethod:public BasicObj{
  public:
  BasicObj* func;
  BasicObj* self;
  BoundMethod(BasicObj* f,BasicObj* s):func(f),self(s){}
  BasicObj* call(std::vector<BasicObj*> a,Namespace& n) override{
    refcount++;
    std::cout<<"Called BoundMethode with "<<a.size()<<" args"<<std::endl;
    std::vector<BasicObj*> callArgs={self};
    for (auto i:a){
      callArgs.push_back(i);
    }
    BasicObj* res = func->call(callArgs,n);
    refcount--;
    return res;
  }
  std::string str() override{
    return "<BoundMethod>";
  }
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
    BasicObj* tmp = new NullObject();
    tmp->refcount++;
    return tmp;
  }
};

class ClassObject;

BasicObj* InstanceObj(ClassObject* cls, std::vector<BasicObj*> args,Namespace&);

#ifndef OWNER
int classes=13;
#endif

class ClassObject:public BasicObj{
  public:
  int instanceID=classes++;
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
  BasicObj* call(std::vector<BasicObj*> args,Namespace& n) override{
    refcount++;
    BasicObj* instance = InstanceObj(this,args,n);
    instance->typeID=instanceID;
    refcount--;
    return instance;
  }
  std::string str() override{
    return "<ClassObject>";
  }
  BasicObj* clone() override{
    BasicObj* tmp = new ClassObject(a,context);
    tmp->refcount++;
    return tmp;
  }
};


class FunctionNative:public BasicObj{
  public:
  std::function<BasicObj*(std::vector<BasicObj*>,Namespace&)> func;
  FunctionNative(std::function<BasicObj*(std::vector<BasicObj*>,Namespace&)> f):func(f){typeID=10;}
  BasicObj* call(std::vector<BasicObj*> args,Namespace& n) override{
    refcount++;
    BasicObj* res = func(args,n);
    refcount--;
    return res;
  }
    std::string str() override{
      return "<NativeFunction>";
    }
    BasicObj* clone() override{
      BasicObj* tmp = new FunctionNative(func);
      tmp->refcount++;
      return tmp;
    }
     ~FunctionNative() override = default;
};


class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    std::vector<BasicObj*> args;
    InstanceObject(ClassObject* cls, std::vector<BasicObj*> args,Namespace& n) {
        klass = cls;
        this->args = args;
        if (klass->attrs.count("__constructor__")) {
          std::cout<<"Calling constructor"<<std::endl;
            BasicObj* ctor = klass->attrs["__constructor__"];
            std::vector<BasicObj*> callArgs = { this };
            for (auto* arg : args){ 
              callArgs.push_back(arg);
            }
            Namespace local = *klass->context;
            local["self"] = this;

            ctor->call(callArgs, local);
        }
    }

    BasicObj* add(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__add__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__add__"]);
            std::vector<BasicObj*> args = { this, other };
            Namespace n;
            return func->call(args,n);
        }
        throw NotAvailable("Cannot add non-object to object");
    }

    BasicObj* sub(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__sub__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__sub__"]);
            std::vector<BasicObj*> args = { this, other };
            Namespace n;
            return func->call(args,n);
        }
        throw NotAvailable("Cannot subtract non-object from object");
    }

    BasicObj* mul(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__mul__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__mul__"]);
            std::vector<BasicObj*> args = { this, other };
            Namespace n;
            return func->call(args,n);
        }
        throw NotAvailable("Cannot multiply non-object by object");
    }

    BasicObj* div(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__div__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__div__"]);
            std::vector<BasicObj*> args = { this, other };
            Namespace n;
            return func->call(args,n);
        }
        throw NotAvailable("Cannot divide non-object by object");
    }

    bool equal(BasicObj* other, bool swapped) override {
        if (klass->attrs.count("__eq__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__eq__"]);
            std::vector<BasicObj*> args = { this, other };
            Namespace n;
            auto* res = func->call(args,n);
            return res->asbool();
        }
        throw NotAvailable("Cannot compare non-object to object");
    }

    bool asbool() override {
        if (klass->attrs.count("__bool__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__bool__"]);
            std::vector<BasicObj*> args = { this };
            Namespace n;
            auto* res = func->call(args,n);
            return res->asbool();
        }
        return true;
    }

    std::string str() override {
        if (klass->attrs.count("__str__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->attrs["__str__"]);
            std::vector<BasicObj*> args = { this };
            Namespace n;
            auto* res = func->call(args,n);
            return res->str();
        }
        return "<InstanceObject>";
    }
    BasicObj* getattr(const std::string& name) override {
      if (attrs.count(name)) {
          return attrs[name];
      }

      if (klass->attrs.count(name)) {
          BasicObj* val = klass->attrs[name];

          if (val->typeID == typeNames.at("FunctionObject") ||
              val->typeID == typeNames.at("FunctionNative")) {
                {
                  BasicObj* tmp = new BoundMethod(val, this);
                  tmp->refcount++;
                  return tmp;
                }
          }

          return val;
      }

      throw ValueError(("Attribute "+name+" not found").c_str());
    }
    void setattr(const std::string& s, BasicObj* value) override{
        std::string attrName = s;
        if (attrs.count(attrName)) {
          attrs[attrName]->refcount--;
        }
        value->refcount++;
        attrs[attrName] = value;
    }
    BasicObj* clone() override{
      std::vector<BasicObj*> newArgs;
      for(auto* a : args) newArgs.push_back(a->clone());
      BasicObj* tmp = new InstanceObject(klass, newArgs,attrs);
      tmp->refcount++;
      return tmp;
    }
    void setitem(BasicObj* key,BasicObj* val)override{
      Namespace dull;
      klass->getattr("__getitem__")->call({this,key,val},dull);
    }
    BasicObj* getitem(BasicObj* key)override{
      Namespace dull;
      return klass->getattr("__setitem__")->call({this,key},dull);
    }
    ~InstanceObject() override = default;

};

class ListObject:public BasicObj{
  public:
  std::vector<BasicObj*> items;
  ListObject(const std::vector<BasicObj*>& items):items(items){
    typeID=11;
    auto appendFunc = new FunctionNative([this](std::vector<BasicObj*> args,Namespace) {
      this->items.push_back(args[0]->clone());
      return nullptr;
    });
    appendFunc->refcount++;
    attrs["append"] = appendFunc;
    attrs["push_back"] = appendFunc;
    BasicObj* sizeFunc = new FunctionNative([this](std::vector<BasicObj*> args,Namespace&){
      BasicObj* tmp = new IntObj(this->items.size());
      tmp->refcount++;
      return tmp;
    });
    sizeFunc->refcount++;
    attrs["size"] = sizeFunc;
  }
  BasicObj* getitem(BasicObj* key) override{
    if (auto i=dynamic_cast<IntObj*>(key)){
      if (i->a<0 || i->a>=items.size()) throw ValueError(
        ("Index out of bounds (read) accesing to "+std::to_string(i->a)).c_str()
      );
      return items[i->a];
    }
    else{
      throw ValueError("Key is not an integer");
    }
  }
  void setitem(BasicObj* key, BasicObj* value) override{
    if (auto i=dynamic_cast<IntObj*>(key)){
      if (i->a<0 || i->a>=items.size()) throw ValueError(
        ("Index out of bounds (write) accesing to "+std::to_string(i->a)).c_str()
      );;
      BasicObj* old = items[i->a];
      if (old && old!=value) old->refcount--;
      value->refcount++;
      items[i->a] = value;
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
    BasicObj* tmp = new ListObject(items);
    tmp->refcount++;
    return tmp;
  }
};

BasicObj* InstanceObj(ClassObject* cls, std::vector<BasicObj*> args,Namespace& n) {
  BasicObj* tmp = new InstanceObject(cls, args, n);
  tmp->refcount++;
  return tmp;
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
      {
        BasicObj* tmp = new StringObject(unescaped);
        tmp->refcount++;
        return tmp;
      }
    }

    if (code.size() >= 2 && code[0] == '[' && code.back() == ']') {
      if (code.size() == 2){ 
        {
          BasicObj* tmp = new ListObject({});
          tmp->refcount++;
          return tmp;
        }
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
      FunctionObject* func = new FunctionObject(argNames, body);
      n[name]=func;
      return func;
    }
    bool hasOp=false;
    bool hasMul=false;
    int gapDepth=0;
    int compPos=-1;
    int opPos=0;
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
          hasOp=true; opPos=i; break;
        }
      }
    }
    if (hasOp){
      std::string left = code.substr(0, opPos);
      std::string right = code.substr(opPos);
      if (right[1]=='=' && (right[0]=='+' || right[0]=='-' || right[0]=='*' || right[0]=='/')){
        exec(
          left+
          '='+
          left+
          right[0]+
          right.substr(2,right.size()),n
        );
        return nullptr;
      }
    }
    if (compPos!=-1){
      bool inPlace=false;
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
      {
        BasicObj* tmp = new IntObj(cres?1:0);
        tmp->refcount++;
        return tmp;
      }
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
             return a->call(args,n);
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

            if (n.count(name))
              n[name]->refcount--;
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
    int startIdx=0;
    if (!code.empty() && (code[0]=='+' || code[0]=='-')) startIdx=1;
    for (int i=startIdx;i<code.size();i++){
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
      {
        BasicObj* tmp = new FloatObj(std::stof(code));
        tmp->refcount++;
        return tmp;
      }
    }
    else if (isInt){
      {
        BasicObj* tmp = new IntObj(std::stoi(code));
        tmp->refcount++;
        return tmp;
      }
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
  {
    BasicObj* t = new BoolObj(true);
    t->refcount++;
    n["true"] = t;
  }
  {
    BasicObj* f = new BoolObj(false);
    f->refcount++;
    n["false"] = f;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()!=1) throw ValueError("IntFromString expects exactly one argument");
      BasicObj* tmp = new IntObj(std::stoi(args[0]->str()));
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["IntFromString"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      for (auto i:args){
        if (i) std::cout<<i->str(); else std::cout<<"<null>";
      }
      return nullptr;
    });
    fn->refcount++;
    n["print"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()>1) throw ValueError("Input function expects at most one argument");
      std::string prompt;
      if (args.size()==1) prompt=args[0]->str();
      std::cout<<prompt;
      std::cout.flush();
      std::string line;
      std::cin>>line;
      BasicObj* tmp = new StringObject(line);
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["input"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      for (auto i:args){
        if (i) std::cout<<i->str(); else std::cout<<"<null>";
      }
      std::cout<<std::endl;
      return nullptr;
    });
    fn->refcount++;
    n["println"] = fn;
  }
  {
    BasicObj* nullObj = new NullObject();
    nullObj->refcount++;
    n["null"] = nullObj;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      BasicObj* res=new ListObject(args);
      res->refcount++;
      return res;
    });
    fn->refcount++;
    n["list"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      std::cout.flush();
      return nullptr;
    });
    fn->refcount++;
    n["flushOut"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([&n](std::vector<BasicObj*> args,Namespace&){
      ListObject* lst = new ListObject({});
      lst->refcount++;
      for (auto& pair : n) {
        BasicObj* s = new StringObject(pair.first);
        s->refcount++;
        lst->items.push_back(s);
      }
      return lst;
    });
    fn->refcount++;
    n["currentNamespace"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()!=1) throw ValueError("addr expects exactly one argument");
      std::stringstream ss;
      ss<<args[0];
      BasicObj* tmp = new StringObject(ss.str());
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["addr"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()!=1) throw ValueError("typeNo expects exactly one argument");
      BasicObj* tmp = new IntObj(args[0]->typeID);
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["typeNo"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()!=1) throw ValueError("getRefcount expects exactly one argument");
      BasicObj* tmp = new IntObj(args[0]->refcount);
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["getRefcount"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      if (args.size()!=1) throw ValueError("clone expects exactly one argument");
      return args[0]->clone();
    });
    fn->refcount++;
    n["clone"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace& n) {
      std::cout<<"Calling in ns"<<std::endl;
      for (auto [key,val]:n){
        std::cout<<key<<":"<<val->str()<<std::endl;
      }
      std::string name = args[0]->str();
      auto it = n.find(name);
      if (it != n.end()) {
          BasicObj* tmp = new ReferenceObject(&it->second);
          tmp->refcount++;
          return tmp;
      }
      throw ValueError(("Name not "+name+ " found").c_str());
    });
    fn->refcount++;
    n["ref"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      BasicObj* tmp = new MapObject();
      tmp->refcount++;
      return tmp;
    });
    fn->refcount++;
    n["map"] = fn;
  }
  {
    BasicObj* fn = new FunctionNative([](std::vector<BasicObj*> args,Namespace&){
      std::cout<<"Called view dict"<<std::endl;
      for (auto [key,val]:args[0]->attrs){
        std::cout<<key<<":"<<val->str()<<std::endl;
      }
      return nullptr;
    });
    fn->refcount++;
    n["viewDict"] = fn;
  }
  n["importdll"]=new FunctionNative([](std::vector<BasicObj*> args,Namespace& n){
    std::cout<<"Importing dll to ns "<<&n<<std::endl;
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
          typedef void (*LoadFunc)(Namespace&,int*,std::vector<BasicObj*>*);
          LoadFunc loadFunc = (LoadFunc)dlsym(handle, "Load");
          if (!loadFunc) {
              dlclose(handle);
              throw ValueError(("Failed to find Load function in shared library: " + dllName).c_str());
          }
          loadFunc(n,&classes,&__objs);
      #endif
    return nullptr;
  });
  return n;
}

#ifndef OWNER
#define OWNER
#endif

