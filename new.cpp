#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

//g++ new.cpp -o new && ./new

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

class BasicObj;

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
    virtual BasicObj* call(std::vector<BasicObj*>,Namespace&){throw NotAvailable("That is Base class (call)");};
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

struct Context{
    Namespace ns;
};

struct Node{
    public:
    virtual BasicObj* eval(Context&)=0;
    virtual void set(Context&,Node*){throw ValueError("Can not asign");}
    virtual std::string str(){return "<Node>";}
    virtual ~Node()=default;
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
  BasicObj* eval(Context& c){return c.ns[name];}
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
  BasicObj* eval(Context& c){
    l->set(c,r);
    return nullptr;
  }
  std::string str() override{
      return "<AsignNode>";
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
    int depth;
    std::string name;
    std::string n;
    for (auto i:a){
        if (isspace(i)) continue;
        n+=i;
    }
    a=n;
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
    for (int ii=0;ii<a.size();ii++){
      char i=a[ii];
        if (i=='(') depth++;
        if (i==')') depth--;
        if (!std::isalpha(i) && i!='_'){ 
          sheerName=false;
          if (i=='='){
            std::cout<<"Asigning "<<name<<std::endl;
            Node* l=parse(name,c);
            std::cout<<"Left is "<<l->str()<<std::endl;
            ii++;
            std::string L;
            for (int j=ii;j<a.size() && a[j]!=';';j++){
              L+=a[j];
            }
            Node* r=parse(L,c);
            std::cout<<"Right is "<<r->str()<<std::endl;
            return new AsignNode(l,r);
          }
        }
        if (!std::isalnum((unsigned char)i)) isInt=false;
        if ((i=='+' || i=='-') && depth==0){
            hasOp=true;
            break;
        }
        name+=i;
    }
    if (hasOp){
        std::cout<<"has op"<<std::endl;
        int bracketDepth=0;
        bool side=false;
        Node *l,*r;
        std::string acc;
        char op;
        for (auto i:a){
            std::cout<<"char "<<i<<std::endl;
            if (i=='(') bracketDepth++;
            if (i==')') bracketDepth--;
            if ((i=='+' || i=='-') && bracketDepth==0){
                if (!side){
                    l=parse(acc,c);
                    std::cout<<"left "<<acc<<std::endl;
                    acc.clear();
                    op=i;
                    continue;
                }
                else{
                    std::cout<<"right "<<acc<<std::endl;
                    r=parse(acc,c);
                    return new BinaryNode(r,l,std::string(1,op));
                }
                side=!side;
            }
            acc+=i;
        }
        if (!acc.empty()){
            std::cout<<"right "<<acc<<std::endl;
            r=parse(acc,c);
            return new BinaryNode(r,l,std::string(1,op));
        }
    }
    if (sheerName){ 
      std::cout<<"sheer name "<<name<<std::endl;
      return new IdentifierNode{name};
    }
    if (isInt) return new LiteralNode{new IntObj(std::stoi(name))};
    return nullptr;
}

int main(){
    Context c;
    Node* b=parse("a=2;",c);
    b->eval(c);
    Node* a=parse("a;",c);
    std::cout<<a->eval(c)->str()<<std::endl;
    return 0;
}
