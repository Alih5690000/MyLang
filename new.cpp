#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

//g++ -g -fsanitize=address new.cpp -o new && ./new

int classes;

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

class ReturnSig{
  public:
  BasicObj* val;
  ReturnSig(BasicObj* val) {
    this->val=val;
  }
  const char* what(){
    return "return is not allowed here";
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

typedef std::map<std::string,BasicObj*> Namespace;

struct Context{
    Namespace ns;
};

class Node;

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
    virtual bool hasattr(const std::string& s){return attrs.count(s);};
    virtual BasicObj* getattr(const std::string& s){
      auto it = attrs.find(s);
      if (it==attrs.end()) throw ValueError(("Attribute "+s+" not found").c_str());
      BasicObj* res = it->second;
      res->refcount++;
      return res;
    };
    virtual void setattr(const std::string& s, BasicObj* value){
      if (attrs.count(s)) {
          attrs[s]->refcount--;
      }

      BasicObj* cloned = value->clone();
      cloned->refcount++;
      attrs[s] = cloned;
    }
    virtual BasicObj* getitem(BasicObj* key){throw NotAvailable("That is Base class (getitem)");};
    virtual BasicObj* setitem(std::vector<BasicObj*>){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* call(std::vector<BasicObj*>,Context*){throw NotAvailable("That is Base class (call)");};
    virtual void setitem(BasicObj* key, BasicObj* value){throw NotAvailable("That is Base class (setitem)");};
    virtual BasicObj* clone(){throw NotAvailable("That is Base class (clone)");};
    virtual ~BasicObj()=default;
    std::map<std::string,BasicObj*> attrs;
};

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
      return tmp;
    }
    ~IntObj() override = default;
};
class NullObject:public BasicObj{
  public:
  NullObject(){
    typeID=7;
  }
  std::string str() override{
    return "<null>";
  }
  bool asbool() override{
    return false;
  }
  BasicObj* clone() override{
    BasicObj* tmp = new NullObject();
    return tmp;
  }
};

struct Node;

std::vector<Node*> parseCode(std::string s,Context& c);

struct Node{
    public:
    virtual BasicObj* eval(Context&)=0;
    virtual void set(Context&,Node*){throw ValueError(("Can not asign:"+str()).c_str());}
    virtual std::string str(){return "<Node>" ;}
    virtual ~Node()=default;
};

class FunctionObject:public BasicObj{
    public:
    std::vector<std::string> argNames;
    std::vector<Node*> code;
    FunctionObject(std::vector<std::string> argNames,std::vector<Node*> code){
      typeID=6;
      this->argNames=argNames;
      this->code=code;
    }
    BasicObj* call(std::vector<BasicObj*> args,Context* c) override{
      if (args.size()!=argNames.size()){
        throw ValueError(("Function called with wrong number of arguments. "+
          std::to_string(args.size())+" instead of "+std::to_string(argNames.size())).c_str());
      }
      // std::cout<<"Called FunctionObject with args"<<std::endl;
      // for (auto k:args) std::cout<<k->str()<<std::endl;
      // std::cout<<"ArgNames are"<<std::endl;
      // for (auto k:argNames) std::cout<<k<<std::endl;
      Context local=*c;
      for (int i=0;i<args.size();i++){
        local.ns[argNames[i]]=args[i]->clone();
        local.ns[argNames[i]]->refcount++;
      }
      if (code.empty()){
        BasicObj* r=new NullObject();
        r->refcount++;
        return r;
      }
      for (int i=0;i<code.size();i++){ 
        try{
          code[i]->eval(local);
        }
        catch(const ReturnSig& s){
          // std::cout<<"Catched return val is "<<s.val<<std::endl;
          for (int i=0;i<args.size();i++){
            local.ns[argNames[i]]->refcount--;
          }
          return s.val;
        }
        BasicObj* r=new NullObject();
        r->refcount++;
        return r;
      }
    }
    std::string str() override{
      return "<FunctionObject>";
    }
     ~FunctionObject() override = default;
};

class FunctionNative:public BasicObj{
  public:
  Context* saved;
  std::function<BasicObj*(std::vector<BasicObj*>,Context*)> func;
  FunctionNative(std::function<BasicObj*(std::vector<BasicObj*>,Context*)> f):func(f){typeID=10;}
  BasicObj* call(std::vector<BasicObj*> args,Context* c) override{
    BasicObj* res = func(args,c);
    return res;
  }
    std::string str() override{
      return "<NativeFunction>";
    }
    BasicObj* clone() override{
      BasicObj* tmp = new FunctionNative(func);
      return tmp;
    }
     ~FunctionNative() override = default;
};

struct LiteralNode:public Node{
    BasicObj* o;
    LiteralNode(BasicObj* l):o(l){l->refcount++;}
    BasicObj* eval(Context&) override{
      o->refcount++;
      return o;
    }
    std::string str() override{
      return "<LiteralNode>";
    }
};

class ListObject:public BasicObj{
  public:
  std::vector<BasicObj*> items;
  ListObject(const std::vector<BasicObj*>& items){
    for (auto i:items){
      BasicObj* curr=i->clone();
      curr->refcount++;
      this->items.push_back(curr);
    }
    typeID=12;
    auto appendFunc = new FunctionNative([this](std::vector<BasicObj*> args,Context*) {
      BasicObj* a=args[0]->clone();
      a->refcount++;
      this->items.push_back(a);
      BasicObj* r = new NullObject();
      r->refcount++;
      return r;
    });
    appendFunc->refcount++;
    attrs["append"] = appendFunc;
    attrs["push_back"] = appendFunc;
    BasicObj* sizeFunc = new FunctionNative([this](std::vector<BasicObj*> args,Context* c){
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
      BasicObj* h=items[i->a];
      h->refcount++;
      return h;
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
    return tmp;
  }
};

struct IdentifierNode:public Node{
  public:
  std::string name;
  IdentifierNode(std::string n):name(n){}
  BasicObj* eval(Context& c) override{
    if (!c.ns.count(name)) throw ValueError(("Name '"+name+"' not found").c_str());
    BasicObj* res = c.ns[name];
    res->refcount++;
    return res;
  }
  void set(Context& c,Node* e){
    if (c.ns[name])
      c.ns[name]->refcount--;
    BasicObj* res=e->eval(c);
    c.ns[name]=res;
  }
  std::string str() override{
      return "<IdentifierNode>";
    }
};

struct AsignNode:public Node{
  Node *l,*r;
  AsignNode(Node* a,Node* b):l(a),r(b){}
  BasicObj* eval(Context& c) override{
    // std::cout<<"l is "<<l->str()<<std::endl;
    l->set(c,r);
    return nullptr;
  }
  std::string str() override{
      return "<AsignNode>";
    }
};

struct CallNode:public Node{
  Node* target;
  std::vector<Node*> args;
  CallNode(Node* a,std::vector<Node*> b):target(a),args(b){
    // std::cout<<"CallNode calls "<<target->str()<<std::endl;
  }
  BasicObj* eval(Context& c){
    std::vector<BasicObj*> callArgs;
    for (auto i:args){ 
      // std::cout<<"Evaling node for func "<<i->str()<<std::endl;
      BasicObj* curr=i->eval(c);
      callArgs.push_back(curr);
    }
    BasicObj* func = target->eval(c);
    BasicObj* res = func->call(callArgs,&c);
    func->refcount--;
    for (auto i:callArgs){ 
      i->refcount--;
    }
    return res;
  }
  std::string str() override{
      return "<CallNode>";
    }
};

struct FunctionNode:public Node{
  std::string name;
  std::vector<Node*> body;
  std::vector<std::string> argNames;
  FunctionNode(std::string n,std::vector<Node*> b,std::vector<std::string> a):name(n),body(b),argNames(a){}
  BasicObj* eval(Context& c) override{
    // std::cout<<"Evaled FunctionNode name "<<name<<std::endl;
    c.ns[name]=new FunctionObject(argNames,body);
    c.ns[name]->refcount++;
    BasicObj* ret=new NullObject();
    ret->refcount++;
    return ret;
  }
};

class ClassObject;

BasicObj* InstanceObj(ClassObject*,std::vector<BasicObj*>,Context&);

class ClassObject:public BasicObj{
  public:
  ClassObject* parent=nullptr;
  int instanceID=classes++;
  std::vector<Node*> a;
  ClassObject(std::vector<Node*> a,ClassObject* parent){
      this->parent=parent;
      typeID=8;
      Context cls;
      if (parent){
        for (auto [k,v]:parent->attrs){
          attrs[k]=v;
        }
      }
      for (auto i:a) i->eval(cls);
      attrs=cls.ns;
      this->a=a;
  }
  BasicObj* call(std::vector<BasicObj*> args,Context* c) override{
    refcount++;
    BasicObj* instance = InstanceObj(this,args,*c);
    instance->typeID=instanceID;
    refcount--;
    return instance;
  }
  std::string str() override{
    return "<ClassObject>";
  }
  BasicObj* clone() override{
    BasicObj* tmp = new ClassObject(a,parent);
    return tmp;
  }
  bool hasattr(const std::string& name) override{
    ClassObject* cls = this;
    while (cls) {
        if (cls->attrs.count(name)) {
            return true;
        }
        cls = dynamic_cast<ClassObject*>(cls->parent);
    }
    return false;
  }
  BasicObj* getattr(const std::string& str) override{
    ClassObject* cls=this;
    while (cls){
      if (cls->attrs.count(str)){
        BasicObj* r=cls->attrs[str];
        r->refcount++;
        return r;
      }
      cls=cls->parent;
   }
   throw ValueError(("Attribute "+str+"not found").c_str());
  }
};

class BoundMethod:public BasicObj{
  public:
  BasicObj* func;
  BasicObj* self;
  BoundMethod(BasicObj* f,BasicObj* s):func(f),self(s){}
  BasicObj* call(std::vector<BasicObj*> a,Context* n) override{
    // std::cout<<"Called BoundMethode with "<<a.size()<<" args"<<std::endl;
    std::vector<BasicObj*> callArgs={self};
    for (auto i:a){
      callArgs.push_back(i);
    }
    BasicObj* res = func->call(callArgs,n);
    return res;
  }
  std::string str() override{
    return "<BoundMethod>";
  }
};

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    std::vector<BasicObj*> args;
    InstanceObject(ClassObject* cls, std::vector<BasicObj*> args,Namespace& n) {
        klass = cls;
        this->args = args;
        if (klass->hasattr("__constructor__")) {
          // std::cout<<"Calling constructor"<<std::endl;
            BasicObj* ctor = klass->getattr("__constructor__");
            std::vector<BasicObj*> callArgs = { this };
            for (auto* arg : args){ 
              callArgs.push_back(arg);
            }
            Context local;
            local.ns=n;
            local.ns["self"] = this;

            ctor->call(callArgs, &local);
        }
    }

    BasicObj* add(BasicObj* other, bool swapped) override {
        if (klass->hasattr("__add__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__add__"));
            std::vector<BasicObj*> args = { this, other };
            Context n;
            return func->call(args,&n);
        }
        throw NotAvailable("Cannot add non-object to object");
    }

    BasicObj* sub(BasicObj* other, bool swapped) override {
        if (klass->hasattr("__sub__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__sub__"));
            std::vector<BasicObj*> args = { this, other };
            Context n;
            return func->call(args,&n);
        }
        throw NotAvailable("Cannot subtract non-object from object");
    }

    BasicObj* mul(BasicObj* other, bool swapped) override {
        if (klass->hasattr("__mul__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__mul__"));
            std::vector<BasicObj*> args = { this, other };
            Context n;
            return func->call(args,&n);
        }
        throw NotAvailable("Cannot multiply non-object by object");
    }

    BasicObj* div(BasicObj* other, bool swapped) override {
        if (klass->hasattr("__div__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__div__"));
            std::vector<BasicObj*> args = { this, other };
            Context n;
            return func->call(args,&n);
        }
        throw NotAvailable("Cannot divide non-object by object");
    }

    bool equal(BasicObj* other, bool swapped) override {
        if (klass->hasattr("__eq__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__eq__"));
            std::vector<BasicObj*> args = { this, other };
            Context n;
            auto* res = func->call(args,&n);
            return res->asbool();
        }
        throw NotAvailable("Cannot compare non-object to object");
    }

    bool asbool() override {
        if (klass->hasattr("__bool__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__bool__"));
            std::vector<BasicObj*> args = { this };
            Context n;
            auto* res = func->call(args,&n);
            return res->asbool();
        }
        return true;
    }

    std::string str() override {
        if (klass->hasattr("__str__")) {
            auto* func = dynamic_cast<FunctionObject*>(klass->getattr("__str__"));
            std::vector<BasicObj*> args = { this };
            Context n;
            auto* res = func->call(args,&n);
            return res->str();
        }
        return "<InstanceObject>";
    }
    BasicObj* getattr(const std::string& name) override {
      if (attrs.count(name)) {
          BasicObj* res = attrs[name];
          res->refcount++;
          return res;
      }

          BasicObj* val = klass->getattr(name);

          if (dynamic_cast<FunctionObject*>(val) ||
              dynamic_cast<FunctionNative*>(val)) {
                  BasicObj* tmp = new BoundMethod(val, this);
                  tmp->refcount++;
                  return tmp;
                }

          return val;


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
      Context dull;
      klass->getattr("__setitem__")->call({this,key,val},&dull);
    }
    BasicObj* getitem(BasicObj* key)override{
      Context dull;
      BasicObj* a=klass->getattr("__getitem__")->call({this,key},&dull);
      return a;
    }
    ~InstanceObject() override = default;

};

BasicObj* InstanceObj(ClassObject* cls,std::vector<BasicObj*> args,Context& c){
  return new InstanceObject(cls,args,c.ns);
}

struct ClassNode:public Node{
  std::string name;
  std::vector<Node*> body;
  Node* p;
  ClassNode(std::string n,std::vector<Node*> b,Node* a):name(n),body(b),p(a){}
  BasicObj* eval(Context& c)override{
    ClassObject* par=nullptr;
    if (p){
      BasicObj* original=p->eval(c);
      if (original){
        par=dynamic_cast<ClassObject*>(original);
        if (!par) throw ValueError("Trying to inhertit from non-class object");
      }
    }
    c.ns[name]=new ClassObject(body,par);
    return nullptr;
  }
};

struct IndexNode:public Node{
  Node* target;
  Node* arg;
  IndexNode(Node* a,Node* b):target(a),arg(b){}
  BasicObj* eval(Context& c){
    BasicObj* t = target->eval(c);
    BasicObj* k = arg->eval(c);

    BasicObj* res = t->getitem(k);

    t->refcount--;
    k->refcount--;

    return res;
  }
  void set(Context& c,Node* val) override{
    target->eval(c)->setitem(arg->eval(c),val->eval(c));
  }
  std::string str() override{
      return "<IndexNode>";
    }
};

struct AttributeNode:public Node{
  Node* target;
  std::string name;
  AttributeNode(Node* a,std::string b):target(a),name(b){}
  BasicObj* eval(Context& c){
    BasicObj* t = target->eval(c);
    BasicObj* res = t->getattr(name);
    t->refcount--;
    return res;
  }
  void set(Context& c,Node* val) override{
    target->eval(c)->setattr(name,val->eval(c));
  }
  std::string str() override{
      return "<AttributeNode>";
    }
};

struct ReturnNode:public Node{
  Node* val;
  ReturnNode(Node* r):val(r){}
  BasicObj* eval(Context& c) override{
    BasicObj* r = val->eval(c);
    throw ReturnSig(r);
  }
  std::string str() override{
    return "<ReturnNode>";
  }
};

struct IfNode:public Node{
  Node* cond;
  std::vector<Node*> action,actionElse;
  IfNode(Node* a,std::vector<Node*> c,std::vector<Node*> b):cond(a),action(b),actionElse(c){}
  BasicObj* eval(Context& c) override{
    BasicObj* var=cond->eval(c);
    if (!var) throw ValueError("Condition is null");

    bool cc=var->asbool();
    var->refcount--;

    if (cc){
      // std::cout<<"True branch"<<std::endl;
      for (auto i:action){ 
        // std::cout<<"Evalin if "<<i->str()<<std::endl;
        i->eval(c);
      }
    }
    else if (!actionElse.empty()){
      // std::cout<<"False branch"<<std::endl;
      for (auto i:actionElse){ 
        // std::cout<<"i is "<<i->str()<<std::endl;
        i->eval(c);
      }
    }
    return nullptr;
  }
  std::string str() override{
      return "<IfNode>";
    }
};

struct ForNode:public Node{
  Node* init;
  Node* cond;
  Node* step;
  std::vector<Node*> body;
  ForNode(Node* c,Node* s,std::vector<Node*> b,Node* i)
    :cond(c),step(s),body(b),init(i){}
  BasicObj* eval(Context& c) override{
    int i=0;
    if (init) {
      BasicObj* tmp=init->eval(c);
      if (tmp) tmp->refcount--;
    }
    BasicObj* co = nullptr;
    if (cond) {
      co = cond->eval(c);
    }
    if (!co) {
      co = new IntObj(1);
      co->refcount++;
    }
    while (co->asbool()){
      // std::cout<<"Loop NO "<<i<<std::endl;
      i++;
      for (auto node : body) {
        BasicObj* tmp=node->eval(c);
        if (tmp) tmp->refcount--;
      }
      if (step) {
        BasicObj* tmp=step->eval(c);
        if (tmp) tmp->refcount--;
      }
      if (cond) {
        co->refcount--;
        co = cond->eval(c);
      }
    }
    co->refcount--;
    return nullptr;
  }
};

struct BinaryNode:public Node{
    public:
    std::string op;
    Node *right,*left;
    BinaryNode(Node* r,Node* l,std::string o):right(r),left(l),op(o){}
    BasicObj* eval(Context& a) override{
        BasicObj* res=nullptr;
        BasicObj* r=right->eval(a);
        BasicObj* l=left->eval(a);
        if (op=="+"){
            res=l->add(r,false);
        }
        if (op=="-"){
            res=l->sub(r,false);
        }
        if (op=="*"){
            res=l->mul(r,false);
        }
        if (op=="/"){
            res=l->div(r,false);
        }
        if (op=="<") res=new IntObj(l->less(r,false));
        if (op==">") res=new IntObj(l->greater(r,false));
        if (op=="==") res=new IntObj(l->equal(r,false));
        res->refcount++;
        return res;
    }
    std::string str() override{
      return "<BinaryNode>";
    }
};

Node* parse(std::string a,Context& c){
  // std::cout<<"source "<<a<<std::endl;
    bool sheerName=true;
    bool isInt=true;
    bool hasOp=false;
    bool hasMul=false;
    int depth=0;
    std::string name;
    std::string n;
    bool quoted=false;
    for (auto i:a){
        if (i=='"') quoted=!quoted;
        if (isspace(i) && !quoted) continue;
        n+=i;
    }
    a=n;
    if (a.back()==';') a.pop_back();
    if (a.substr(0,2)=="//") return new LiteralNode{new NullObject};
    if (a.empty()) return new LiteralNode{new NullObject};
    if (a[0]=='"'){
      bool quoted=false;
      std::string content;
      for (int i=0;i<a.size();i++){
          if (a[i]=='"') {
            quoted=!quoted;
            if (!quoted && i==a.size()-1){
              return new LiteralNode{new StringObject(content)};
            }
            continue;
          }
          if (a[i]=='\\' && quoted){
            if (i+1<a.size()){
              if (a[i+1]=='n') content+='\n';
              else if (a[i+1]=='t') content+='\t';
              else content+=a[i+1];
              i++;
            }
          }
          else
            content+=a[i];
      }
    }
    if (a[0]=='('){
        int depth=0;
        for (int i=0;i<a.size();i++){
            if (a[i]=='(') depth++;
            if (a[i]==')'){ 
                depth--;
                if (i==a.size()-1 && depth==0){
                    return parse(a.substr(1,a.size()-2),c);
                }
            }
        }
    }
    if (a[0]=='{'){
        int depth=0;
        for (int i=0;i<a.size();i++){
            if (a[i]=='{') depth++;
            if (a[i]=='}'){ 
                depth--;
                if (i==a.size()-1 && depth==0){
                    return parse(a.substr(1,a.size()-2),c);
                }
            }
        }
    }
    if (a[0]=='"'){
      bool quoted=false;
      for (int i=0;i<a.size();i++){
          if (a[i]=='"') {
            quoted=!quoted;
            if (!quoted && i==a.size()-1){
              return new LiteralNode{new StringObject(a.substr(1,a.size()-2))};
            }
          }
      }
    }
    if (a[0]=='['){
      bool quoted=false;
      int depth=0;
      for (int i=0;i<a.size();i++){
          if (a[i]=='"') quoted=!quoted;
          if (!quoted){
            if (a[i]=='[') depth++;
            if (a[i]==']'){ 
              depth--;
              if (i==a.size()-1 && depth==0){
                std::vector<Node*> items;
                std::string acc;
                int innerDepth=0;
                bool innerQuoted=false;
                for (int j=1;j<a.size()-1;j++){
                  if (a[j]=='"') innerQuoted=!innerQuoted;
                  if (!innerQuoted){
                    if (a[j]=='[') innerDepth++;
                    if (a[j]==']') innerDepth--;
                    if (a[j]==',' && innerDepth==0){
                      items.push_back(parse(acc,c));
                      acc.clear();
                      continue;
                    }
                  }
                  acc+=a[j];
                }
                if (!acc.empty()){
                  items.push_back(parse(acc,c));
                }
                std::vector<BasicObj*> itemObjs;
                for (auto i:items){
                  itemObjs.push_back(i->eval(c));
                }
                return new LiteralNode{new ListObject(itemObjs)};
              }
            }
          }
      }
    }
    if (a.substr(0,3)=="if("){
      // std::cout<<"if branch"<<std::endl;
      Node* cond;
      std::string acc;
      int depth=1;
      int j=3;
      for (;j<a.size();j++){
        if (a[j]=='(') depth++;
        else if (a[j]==')'){ 
          depth--;
          if (depth==0){
            cond=parse(acc,c);
            //std::cout<<"cond is "<<acc<<std::endl;
            break;
          }
        }
        acc+=a[j];
      }
      j++;
      if (a[j]!='{') throw SyntaxError("Invalid if syntax");
      j++;
      acc.clear();
      std::vector<Node*> body,elseBody;
      depth=1;
      for (;j<a.size();j++){
        if (a[j]=='{') depth++;
        else if (a[j]=='}'){ 
          depth--;
          if (depth==0){
            body=parseCode(acc,c);
            //std::cout<<"Body is "<<acc<<std::endl;
            break;
          }
        }
        acc+=a[j];
      }
      acc.clear();
      j++;
      if (j+5<a.size() && a.substr(j+1,4)=="else"){
        //std::cout<<"Else branch"<<std::endl;
        j+=4;
        if (a[j+1]!='{') throw ValueError("Invalid else syntax");
        j++;
        depth=0;
        for (;j<a.size();j++){
          acc+=a[j];
          //std::cout<<"char is "<<a[j]<<std::endl;
          if (a[j]=='{') depth++;
          else if (a[j]=='}'){ 
            depth--;
            if (depth==0){
              elseBody=parseCode(acc,c);
              break;
            } 
          }
        }
      }
      else{
        elseBody={};
      }
      return new IfNode(cond,elseBody,body);
    }
    else if(a.substr(0,4)=="for("){
      // std::cout<<"is for "<<std::endl;
      int j=4;
      int depth=1;
      std::string acc;
      std::vector<Node*> exprs;
      for (;j<a.size();j++){
        if (a[j]=='(') depth++;
        else if (a[j]==')'){ 
          depth--;
          if (depth==0) break;
        }
        if (a[j]==';' && depth==1){
          exprs.push_back(parse(acc,c));
          // std::cout<<"ACC "<<acc<<std::endl;
          acc.clear();
          continue;
        }
        acc+=a[j];
      }
      if (!acc.empty()){
        exprs.push_back(parse(acc,c));
        // std::cout<<"ACC "<<acc<<std::endl;
        acc.clear();
      }
      j+=2;
      depth=1;
      acc.clear();
      for (;j<a.size();j++){
        if (a[j]=='{') depth++;
        else if (a[j]=='}'){ 
          depth--;
          if (depth==0) break;
        }
        acc+=a[j];
      }
      // std::cout<<"BODY "<<acc<<std::endl;
      return new ForNode(exprs[1],exprs[2],parseCode(acc,c),exprs[0]);
    }
    else if (a.substr(0,7)=="return("){
      int j=7;
      std::string val;
      depth=1;
      for (;j<a.size();j++){
        if (a[j]=='(') depth++;
        else if (a[j]==')') {
          depth--;
          if (depth==0) break;
        }
        val+=a[j];
      }
      return new ReturnNode(parse(val,c));
    }
    else if (a.substr(0,6)=="class("){
      std::cout<<"Parsing class"<<std::endl;
      int j=6;
      std::string name;
      for (;j<a.size();j++){
        if (a[j]==')') break;
        name+=a[j];
      }
      // std::cout<<"Class name "<<name<<std::endl;
      j++;
      if (a[j]!='{') throw SyntaxError("'{' for class body");
      j++;
      std::string body;
      int depth=1;
      for (;j<a.size();j++){
        if (a[j]=='{') depth++;
        if (a[j]=='}'){
           depth--;
           if (depth==0) break;
        }
        body+=a[j];
      }
      // std::cout<<"Body is "<<body<<std::endl;
      auto Body=parseCode(body,c);
      return new ClassNode(name,Body,nullptr);
    }
    else if (a.substr(0,3)=="fn("){
      int j=3;
      std::string funcName;
      for (;j<a.size();j++){
        if (a[j]==')') break;
        funcName+=a[j];
      }
      std::cout<<"name is "<<funcName<<std::endl;
      if (j==a.size()-1){
        throw SyntaxError("No closing ')'");
      }
      j++;
      if (a[j]!='(') throw SyntaxError("'(' expected for arguments list");
      j++;
      std::vector<std::string> argNames;
      std::string acc;
      for (;j<a.size();j++){
        if (a[j]==','){
          argNames.push_back(acc);
          acc.clear();
          continue;
        }
        if (a[j]==')') break;
        acc+=a[j];
      }
      if (!acc.empty()){
        argNames.push_back(acc);
        acc.clear();
      }
      std::string body;
      j++;
      if (a[j]!='{') throw SyntaxError("Expected '{' for function body");
      j++;
      int depth=1;
      for (;j<a.size();j++){
        if (a[j]=='{') depth++;
        if (a[j]=='}'){
           depth--;
           if (depth==0) break;
        }
        body+=a[j];
      }
      auto nodes=parseCode(body,c);
      for (auto k:nodes) /* std::cout<<"Node is "<<k->str()<<std::endl; */ (void)k;
      return new FunctionNode(funcName,nodes,argNames);
    }
    for (int ii=0;ii<a.size();ii++){
        char i=a[ii];
        // std::cout<<"i is "<<i<<std::endl;
        
        if (i<'0' || i>'9'){ 
          //std::cout<<"Not int due to "<<i<<std::endl;
          isInt=false;
        }
        else{
          //std::cout<<i<<" is number"<<std::endl;
          name+=i;
          continue;
        }
        if ((i=='+' || i=='-' || i=='<' || i=='>') && depth==0){
            // std::cout<<"Has op "<<i<<std::endl;
            hasOp=true;
        }
        if ((i=='*' || i=='/') && depth==0){
            // std::cout<<"Has mul "<<i<<std::endl;
            hasMul=true;
        }
        if ((i=='(' || i=='[' || i=='=' || i=='.' ) && depth==0){ 
          Node* res=parse(name,c);
          // std::cout<<"Base is "<<name<<std::endl;
          sheerName=false;
          for (;ii<a.size();ii++){
              char i=a[ii];
              if ((i=='+'||i=='-'||i=='*'||i=='/'||i=='<'||i=='>') && depth==0) {
                  std::string op(1,i);
                  std::string rightStr = a.substr(ii+1);
                  Node* rightNode = parse(rightStr,c);
                  return new BinaryNode(rightNode,res,op);
              }
              if (i=='='){
                // std::cout<<"Asigning "<<name<<std::endl;
                // std::cout<<"Left is "<<res->str()<<std::endl;
                ii++;
                std::string L;
                for (;ii<a.size() && a[ii]!=';';ii++){
                  L+=a[ii];
                }
                Node* r=parse(L,c);
                // std::cout<<"Right is "<<L<<std::endl;
                return new AsignNode(res,r);
              }
              if (i=='('){
                // std::cout<<"Calling "<<name<<std::endl;
                std::vector<Node*> args;
                std::string acc;
                ii++;
                int depth1=1;
                int depth2=0;
                for (;ii<a.size();ii++){
                  i=a[ii];
                  // std::cout<<"Char "<<i<<std::endl;
                  if (i=='(') depth1++;
                  if (i==')'){ 
                    depth1--;
                    if (depth1==0) break;
                  }
                  if (i=='[') depth2++;
                  if (i==']') depth2--;
                  if (i==',' && depth1==1 && depth2==0){
                    if(!acc.empty()){
                      args.push_back(parse(acc,c));
                      acc.clear();
                    }
                    continue;
                  }
                  acc+=i;
                }
                if(!acc.empty())
                  args.push_back(parse(acc,c));
                for (auto i:args){
                  // std::cout<<"Arg is "<<i->str()<<std::endl;
                }
                res=new CallNode(res,args);
                // std::cout<<"Res is "<<res->str()<<std::endl;
              }
              if (i=='.'){
                std::string attrName;
                ii++;
                for (;ii<a.size();ii++){
                  char i=a[ii];
                  if (!std::isalpha(i) && i!='_'){ 
                    ii--;
                    break;
                  }
                  attrName+=i;
                }
                res=new AttributeNode(res,attrName);
              }
              if (i=='['){
                Node* key;
                std::string acc;
                ii++;
                int depth1=0;
                int depth2=1;
                for (;ii<a.size();ii++){
                  i=a[ii];
                  if (i=='(') depth1++;
                  if (i==')'){ 
                    depth1--;
                  }
                  if (i=='[') depth2++;
                  if (i==']'){ 
                    depth2--;
                    if (depth2==0 && depth1==0){
                      res=new IndexNode(res,parse(acc,c));
                      break;
                    }
                  }
                  acc+=i;
                }
              }
          }
          // std::cout<<"The final res is "<<res->str()<<std::endl;
          return res;
        }
        if (i=='(') depth++;
        if (i==')') depth--;
        name+=i;
    }
    if (hasMul && !hasOp){
      // std::cout<<"has mul"<<std::endl;
        int bracketDepth=0;
        bool side=false;
        Node *r=nullptr,*curr=nullptr;
        std::string acc;
        std::string op;
        for (auto i:a){
            // std::cout<<"ChAr "<<i<<std::endl;
            if (i=='(') bracketDepth++;
            if (i==')') bracketDepth--;
            if ((i=='*' || i=='/') && bracketDepth==0){ 
                if (!curr){
                  curr=parse(acc,c);
                  // std::cout<<"curr's first is "<<acc<<std::endl;
                }
                else{
                  // std::cout<<"right is "<<acc<<std::endl;
                  r=parse(acc,c);
                  curr=new BinaryNode(r,curr,std::string(1,i));
                }
                op=i;
                acc.clear();
                continue;
            }
            acc+=i;
        }
        if (!acc.empty()){
            if (!curr){
              curr=parse(acc,c);
              // std::cout<<"curr's first is "<<acc<<std::endl;
            }
            else{
              // std::cout<<"right is "<<acc<<std::endl;
              r=parse(acc,c);
              curr=new BinaryNode(r,curr,op);
            }
            acc.clear();
        }
      return curr;
    }
    if (hasOp){
        std::cout<<"has op"<<std::endl;
        int bracketDepth=0;
        bool side=false;
        Node *r=nullptr,*curr=nullptr;
        std::string acc;
        std::string op;
        for (auto i:a){
            std::cout<<"ChAr "<<i<<std::endl;
            if (i=='(') bracketDepth++;
            if (i==')') bracketDepth--;
            if ((i=='+' || i=='-' || i=='<' || i=='>') && bracketDepth==0){ 
                if (!curr){
                  curr=parse(acc,c);
                  std::cout<<"curr's first is "<<acc<<std::endl;
                }
                else{
                  std::cout<<"right is "<<acc<<std::endl;
                  r=parse(acc,c);
                  curr=new BinaryNode(r,curr,std::string(1,i));
                }
                op=i;
                acc.clear();
                continue;
            }
            acc+=i;
        }
        if (!acc.empty()){
            if (!curr){
              curr=parse(acc,c);
              std::cout<<"curr's first is "<<acc<<std::endl;
            }
            else{
              std::cout<<"right is "<<acc<<std::endl;
              r=parse(acc,c);
              curr=new BinaryNode(r,curr,op);
            }
            acc.clear();
        }
      return curr;
    }
    if (isInt){ 
      // std::cout<<"Int is "<<name<<std::endl;
      return new LiteralNode{new IntObj(std::stoi(name))};
    }
    else if (sheerName){ 
      // std::cout<<"sheer name "<<name<<std::endl;
      return new IdentifierNode{name};
    }
    
    // std::cout<<"None of this"<<std::endl;
    return nullptr;
}

std::vector<Node*> parseCode(std::string s,Context& c){
  int depth=0;
  int depth2=0;
  std::string acc;
  std::vector<Node*> res;
  for (auto i:s){
    acc+=i;
    if (i=='{') depth++;
    else if(i=='}'){ 
      depth--;
      if (depth==0){
        // std::cout<<"parseCode is parsing "<<acc<<std::endl;
        if (!acc.empty())
          res.push_back(parse(acc,c));
        acc.clear();
      }
    }
    if (i=='(') depth2++;
    else if(i==')') depth2--;
    if (i==';' && depth==0 && depth2==0){
      if (!acc.empty())
        res.push_back(parse(acc,c));
      acc.clear();
    }
  }
  if (!acc.empty()){
    res.push_back(parse(acc,c));
    acc.clear();
  }
  return res;
}

Context CreateContext(){
  Context c;
  c.ns["print"]=new FunctionNative([](std::vector<BasicObj*> args,Context* c){
    for (auto i:args){
      std::cout<<i->str();
    }
    BasicObj* a=new NullObject();
    a->refcount++;
    return a;
  });
  c.ns["print"]->refcount++;
  c.ns["input"]=new FunctionNative([](std::vector<BasicObj*> args,Context* c){
    std::string s;
    std::cin>>s;
    BasicObj* a=new StringObject(s);
    a->refcount++;
    return a;
  });
  c.ns["input"]->refcount++;
  return c;
}

int main(){
    Context c=CreateContext();
    auto a=parseCode(R"(
      class(Player){
        fn(__constructor__)(self){
          self.x=0;
          self.y=y;
        };
        fn(move)(self,dx,dy){
          self.x=self.x+dx;
          self.y=self.y+dy;
        };
      };
      print("Declared class");
      fn(createMap)(){
        print("Creating map");
        res=[];
        for (i=0;i<10;i=i+1){
          row=[];
          for (j=0;j<10;j=j+1){
            row.push_back(".");
          };
          res.push_back(row);
        };
        return(res);
      };
      fn(showMap)(m){
        for (i=0;i<10;i=i+1){
          for (j=0;j<10;j=j+1){
            print(m[i][j]," ");
          };
        print("\n");
        };
      };
      class(Player){
        fn(__constructor__)(self,x,y){
          self.x=x;
          self.y=y;
        };
        fn(render)(self,m){
          m[self.y][self.x]="$";
        }
      }
      print("Decalred class and func");
      m=createMap();
      p=Player(0,0);
      showMap(m);
      for (i=0;i<1;i=i-1){
        a=input();
        p.render(m);
        showMap(m);
      };
    )",c);
    for (auto i:a){
      i->eval(c);
    }
    return 0;
}
