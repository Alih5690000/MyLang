#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

//g++ new.cpp -o new && ./new

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
    tmp->refcount++;
    return tmp;
  }
};





struct Node;

std::vector<Node*> parseCode(std::string s,Context& c);

struct Node{
    public:
    virtual BasicObj* eval(Context&)=0;
    virtual void set(Context&,Node*){throw ValueError("Can not asign");}
    virtual std::string str(){return "<Node>";}
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
      std::cout<<"Called FunctiobObject with args"<<std::endl;
      for (auto k:args) std::cout<<k->str()<<std::endl;
      std::cout<<"ArgNames are"<<std::endl;
      for (auto k:argNames) std::cout<<k<<std::endl;
      Context local=*c;
      for (int i=0;i<args.size();i++){
        local.ns[argNames[i]]=args[i];
      }
      if (code.empty())
        return new NullObject();
      for (int i=0;i<code.size();i++){ 
        try{
          code[i]->eval(local);
        }
        catch(const ReturnSig& s){
          std::cout<<"Catched return val is "<<s.val<<std::endl;
          return s.val;
        }
      }
      return new NullObject();
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
    LiteralNode(BasicObj* l):o(l){}
    BasicObj* eval(Context&) override{return o;}
    std::string str() override{
      return "<LiteralNode>";
    }
};

struct IdentifierNode:public Node{
  public:
  std::string name;
  IdentifierNode(std::string n):name(n){}
  BasicObj* eval(Context& c) override{
    if (!c.ns.count(name)) throw ValueError(("Name '"+name+"' not found").c_str());
    return c.ns[name];
  }
  void set(Context& c,Node* e){
    if (c.ns[name])
      c.ns[name]->refcount--;
    BasicObj* res=e->eval(c);
    res->refcount++;
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
    std::cout<<"l is "<<l->str()<<std::endl;
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
    std::cout<<"CallNode calls "<<target->str()<<std::endl;
  }
  BasicObj* eval(Context& c){
    std::vector<BasicObj*> callArgs;
    for (auto i:args){ 
      std::cout<<"Evaling node for func "<<i->str()<<std::endl;
      callArgs.push_back(i->eval(c));
    }
    return target->eval(c)->call(callArgs,&c);
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
    std::cout<<"Evaled FunctionNode name "<<name<<std::endl;
    c.ns[name]=new FunctionObject(argNames,body);
    return new NullObject();
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
    tmp->refcount++;
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
   throw ValueError("Attribute not found");
  }
};

class BoundMethod:public BasicObj{
  public:
  BasicObj* func;
  BasicObj* self;
  BoundMethod(BasicObj* f,BasicObj* s):func(f),self(s){}
  BasicObj* call(std::vector<BasicObj*> a,Context* n) override{
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

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    std::vector<BasicObj*> args;
    InstanceObject(ClassObject* cls, std::vector<BasicObj*> args,Namespace& n) {
        klass = cls;
        this->args = args;
        if (klass->hasattr("__constructor__")) {
          std::cout<<"Calling constructor"<<std::endl;
            BasicObj* ctor = klass->getattr("__constructor__");
            std::vector<BasicObj*> callArgs = { this };
            for (auto* arg : args){ 
              callArgs.push_back(arg);
            }
            Context local;
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
          return attrs[name];
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
      return klass->getattr("__getitem__")->call({this,key},&dull);
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
    return target->eval(c)->getitem(arg->eval(c));
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
    return target->eval(c)->getattr(name);
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
    throw ReturnSig(val->eval(c));
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
    if (var->asbool()){
      std::cout<<"True branch"<<std::endl;
      for (auto i:action){ 
        std::cout<<"Evalin if "<<i->str()<<std::endl;
        i->eval(c);
      }
    }
    else if (!actionElse.empty()){
      std::cout<<"False branch"<<std::endl;
      for (auto i:actionElse){ 
        std::cout<<"i is "<<i->str()<<std::endl;
        i->eval(c);
      }
    }
    return nullptr;
  }
  std::string str() override{
      return "<IfNode>";
    }
};

struct BinaryNode:public Node{
    public:
    std::string op;
    Node *right,*left;
    BinaryNode(Node* r,Node* l,std::string o):right(r),left(l),op(o){}
    BasicObj* eval(Context& a) override{
        BasicObj* r=right->eval(a);
        BasicObj* l=left->eval(a);
        if (op=="+"){
            return l->add(r,false);
        }
        if (op=="-"){
            return l->sub(r,false);
        }
        if (op=="*"){
            return l->mul(r,false);
        }
        if (op=="/"){
            return l->div(r,false);
        }
        return nullptr;
    }
    std::string str() override{
      return "<BinaryNode>";
    }
};

Node* parse(std::string a,Context& c){
  std::cout<<"source "<<a<<std::endl;
    bool sheerName=true;
    bool isInt=true;
    bool hasOp=false;
    bool hasMul=false;
    int depth=0;
    std::string name;
    std::string n;
    for (auto i:a){
        if (isspace(i)) continue;
        n+=i;
    }
    a=n;
    if (a.empty()) return new LiteralNode{new NullObject};
    if (a.back()==';') a.pop_back();
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
    if (a.substr(0,3)=="if("){
      std::cout<<"if branch"<<std::endl;
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
      std::cout<<"Class name "<<name<<std::endl;
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
      std::cout<<"Body is "<<body<<std::endl;
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
      for (auto k:nodes) std::cout<<"Node is "<<k->str()<<std::endl;
      return new FunctionNode(funcName,nodes,argNames);
    }
    for (int ii=0;ii<a.size();ii++){
        char i=a[ii];
        std::cout<<"i is "<<i<<std::endl;
        
        if (i<'0' || i>'9'){ 
          //std::cout<<"Not int due to "<<i<<std::endl;
          isInt=false;
        }
        else{
          //std::cout<<i<<" is number"<<std::endl;
          name+=i;
          continue;
        }
        if ((i=='+' || i=='-') && depth==0){
            std::cout<<"Has op "<<i<<std::endl;
            hasOp=true;
        }
        if ((i=='*' || i=='/') && depth==0){
            std::cout<<"Has mul "<<i<<std::endl;
            hasMul=true;
        }
        if ((i=='(' || i=='[' || i=='=' || i=='.' ) && depth==0){ 
          Node* res=parse(name,c);
          std::cout<<"Base is "<<name<<std::endl;
          sheerName=false;
          for (;ii<a.size();ii++){
              char i=a[ii];
              if (i=='='){
                std::cout<<"Asigning "<<name<<std::endl;
                std::cout<<"Left is "<<res->str()<<std::endl;
                ii++;
                std::string L;
                for (;ii<a.size() && a[ii]!=';';ii++){
                  L+=a[ii];
                }
                Node* r=parse(L,c);
                std::cout<<"Right is "<<L<<std::endl;
                return new AsignNode(res,r);
              }
              if (i=='('){
                std::cout<<"Calling "<<name<<std::endl;
                std::vector<Node*> args;
                std::string acc;
                ii++;
                int depth1=1;
                int depth2=0;
                for (;ii<a.size();ii++){
                  i=a[ii];
                  std::cout<<"Char "<<i<<std::endl;
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
                  std::cout<<"Arg is "<<i->str()<<std::endl;
                }
                res=new CallNode(res,args);
                std::cout<<"Res is "<<res->str()<<std::endl;
              }
              if (i=='.'){
                std::string attrName;
                ii++;
                for (;ii<a.size();ii++){
                  char i=a[ii];
                  if (i=='(' || i=='[' || i=='=' || i=='.' ){ 
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
          std::cout<<"The final res is "<<res->str()<<std::endl;
          return res;
        }
        if (i=='(') depth++;
        if (i==')') depth--;
        name+=i;
    }
    if (hasMul && !hasOp){
      std::cout<<"has mul"<<std::endl;
        int bracketDepth=0;
        bool side=false;
        Node *r=nullptr,*curr=nullptr;
        std::string acc;
        std::string op;
        for (auto i:a){
            std::cout<<"ChAr "<<i<<std::endl;
            if (i=='(') bracketDepth++;
            if (i==')') bracketDepth--;
            if ((i=='*' || i=='/') && bracketDepth==0){ 
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
            if ((i=='+' || i=='-') && bracketDepth==0){ 
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
      std::cout<<"Int is "<<name<<std::endl;
      return new LiteralNode{new IntObj(std::stoi(name))};
    }
    else if (sheerName){ 
      std::cout<<"sheer name "<<name<<std::endl;
      return new IdentifierNode{name};
    }
    
    std::cout<<"None of this"<<std::endl;
    return nullptr;
}

std::vector<Node*> parseCode(std::string s,Context& c){
  int depth=0;
  std::string acc;
  std::vector<Node*> res;
  for (auto i:s){
    acc+=i;
    if (i=='{') depth++;
    else if(i=='}'){ 
      depth--;
      if (depth==0){
        std::cout<<"parseCode is parsing "<<acc<<std::endl;
        if (!acc.empty())
          res.push_back(parse(acc,c));
        acc.clear();
      }
    }
    if (i==';' && depth==0){
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

int main(){
    Context c;
    c.ns["add"]=new FunctionNative([](std::vector<BasicObj*> args,Context* c){
      if (args.size()!=2) throw ValueError("Add requires 2 arguements");
      std::cout<<"Called add"<<std::endl;
      return args[0]->add(args[1],false);
    });
    c.ns["cc"]=new ClassObject((std::vector<Node*>){new AsignNode(new IdentifierNode("x"),new LiteralNode(new IntObj(5)))},nullptr);
    auto a=parseCode("{if (0){b=1;};else{b=0;}}",c);
    std::cout<<"Evaling "<<std::endl;
    for (auto i:a){
       i->eval(c);
    }
    Node* cc=parse("add.x=99;",c);
    cc->eval(c);
    Node* dd=parse("fn(addd)(a,b){return(a+b);}",c);
    dd->eval(c);
    parse("class(klass){a=67;};",c)->eval(c);
    Node* aa=parse("a=klass();",c);
    aa->eval(c);
    Node* b=parse("a.a;",c);
    std::cout<<b->eval(c)->str()<<std::endl;
    return 0;
}
