#pragma once

#include <string>
#include <map>
#include <vector>
#include <exception>
#include <functional>

class BasicObj;
class FunctionObject;
class FunctionNative;
class ClassObject;
class InstanceObject;
class BoundMethod;

extern std::vector<BasicObj*> __objs;
extern int classes;

using Namespace = std::map<std::string, BasicObj*>;

// ================= EXCEPTIONS =================

class NotAvailable : public std::exception {
public:
    NotAvailable(const char* msg);
    const char* what() const noexcept override;
};

class ValueError : public std::exception {
public:
    ValueError(const char* msg);
    const char* what() const noexcept override;
};

// ================= BASE OBJECT =================

class BasicObj {
public:
    int refcount;
    int typeID;
    std::map<std::string, BasicObj*> attrs;

    BasicObj();
    virtual ~BasicObj();

    virtual BasicObj* call(std::vector<BasicObj*> args, Namespace& n);
    virtual BasicObj* clone();

    virtual BasicObj* getattr(const std::string& name);
    virtual void setattr(const std::string& name, BasicObj* value);

    virtual BasicObj* add(BasicObj* other, bool reverse = false);
    virtual BasicObj* sub(BasicObj* other, bool reverse = false);
    virtual BasicObj* mul(BasicObj* other, bool reverse = false);
    virtual BasicObj* div(BasicObj* other, bool reverse = false);

    virtual BasicObj* getitem(BasicObj* key);
    virtual void setitem(BasicObj* key, BasicObj* value);

    virtual std::string str();
};

// ================= FUNCTION OBJECT =================

class FunctionObject : public BasicObj {
public:
    std::vector<std::string> argNames;
    std::string body;
    Namespace* closure;

    FunctionObject(const std::vector<std::string>& args,
                   const std::string& body,
                   Namespace* closure);

    BasicObj* call(std::vector<BasicObj*> args, Namespace& n) override;
    BasicObj* clone() override;
};

// ================= NATIVE FUNCTION =================

class FunctionNative : public BasicObj {
public:
    using NativeFn = std::function<BasicObj*(std::vector<BasicObj*>, Namespace&)>;

    NativeFn func;

    FunctionNative(NativeFn f);

    BasicObj* call(std::vector<BasicObj*> args, Namespace& n) override;
    BasicObj* clone() override;
};

// ================= CLASS OBJECT =================

class ClassObject : public BasicObj {
public:
    std::string body;
    Namespace* context;

    ClassObject(const std::string& body, Namespace* ctx);

    BasicObj* call(std::vector<BasicObj*> args, Namespace& n) override;
    BasicObj* clone() override;
};

// ================= INSTANCE OBJECT =================

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;

    InstanceObject(ClassObject* cls,
                   std::vector<BasicObj*> args,
                   Namespace& n);

    BasicObj* getattr(const std::string& name) override;
    void setattr(const std::string& name, BasicObj* value) override;

    BasicObj* clone() override;
};

// ================= BOUND METHOD =================

class BoundMethod : public BasicObj {
public:
    BasicObj* func;
    BasicObj* self;

    BoundMethod(BasicObj* func, BasicObj* self);
    ~BoundMethod();

    BasicObj* call(std::vector<BasicObj*> args, Namespace& n) override;
};

// ================= INT OBJECT =================

class IntObj : public BasicObj {
public:
    int value;

    IntObj(int v);

    BasicObj* add(BasicObj* other, bool reverse) override;
    BasicObj* sub(BasicObj* other, bool reverse ) override;
    BasicObj* mul(BasicObj* other, bool reverse ) override;
    BasicObj* div(BasicObj* other, bool reverse ) override;

    std::string str() override;
    BasicObj* clone() override;
};

// ================= FLOAT OBJECT =================

class FloatObj : public BasicObj {
public:
    float value;

    FloatObj(float v);

    BasicObj* add(BasicObj* other, bool reverse ) override;
    BasicObj* sub(BasicObj* other, bool reverse ) override;
    BasicObj* mul(BasicObj* other, bool reverse ) override;
    BasicObj* div(BasicObj* other, bool reverse ) override;

    std::string str() override;
    BasicObj* clone() override;
};

// ================= STRING OBJECT =================

class StrObj : public BasicObj {
public:
    std::string value;

    StrObj(const std::string& v);

    BasicObj* add(BasicObj* other, bool reverse) override;
    BasicObj* getitem(BasicObj* key) override;

    std::string str() override;
    BasicObj* clone() override;
};

// ================= BOOL OBJECT =================

class BoolObj : public BasicObj {
public:
    bool value;

    BoolObj(bool v);

    std::string str() override;
    BasicObj* clone() override;
};

// ================= NONE OBJECT =================

class NoneObj : public BasicObj {
public:
    NoneObj();

    std::string str() override;
    BasicObj* clone() override;
};
