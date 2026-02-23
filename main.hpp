#pragma once

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <exception>
#include <cctype>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

class BasicObj;
class ClassObject;
class InstanceObject;

using Namespace = std::map<std::string, BasicObj*>;

extern std::vector<BasicObj*> __objs;

/* ===================== Exceptions ===================== */

class NotAvailable : public std::exception {
public:
    std::string mes;
    NotAvailable(const char* mes);
    const char* what() const noexcept override;
};

class ValueError : public std::exception {
public:
    std::string mes;
    ValueError(const char* mes);
    const char* what() const noexcept override;
};

class SyntaxError : public std::exception {
public:
    std::string mes;
    SyntaxError(const char* mes);
    const char* what() const noexcept override;
};

class ReturnSig {
public:
    BasicObj* val;
    ReturnSig(BasicObj* val);
};

/* ===================== Core Object ===================== */

class BasicObj {
public:
    int typeID;
    int refcount;
    std::map<std::string, BasicObj*> attrs;

    BasicObj();
    virtual ~BasicObj();

    virtual BasicObj* add(BasicObj*, bool);
    virtual BasicObj* sub(BasicObj*, bool);
    virtual BasicObj* mul(BasicObj*, bool);
    virtual BasicObj* div(BasicObj*, bool);

    virtual void inc();
    virtual void dec();

    virtual std::string str();

    virtual bool greater(BasicObj*, bool);
    virtual bool less(BasicObj*, bool);
    virtual bool equal(BasicObj*, bool);
    virtual bool asbool();

    virtual void free();

    virtual BasicObj* getattr(const std::string&);
    virtual void setattr(const std::string&, BasicObj*);

    virtual BasicObj* getitem(BasicObj*);
    virtual void setitem(BasicObj*, BasicObj*);
    virtual BasicObj* call(std::vector<BasicObj*>);
    virtual BasicObj* clone();
};

/* ===================== Primitive Types ===================== */

class StringObject : public BasicObj {
public:
    std::string value;
    StringObject(const std::string& v);

    std::string str() override;
    BasicObj* add(BasicObj*, bool) override;
    bool equal(BasicObj*, bool) override;
    bool asbool() override;
    BasicObj* clone() override;
};

class IntObj : public BasicObj {
public:
    int a;
    IntObj(int a);

    BasicObj* add(BasicObj*, bool) override;
    BasicObj* sub(BasicObj*, bool) override;
    BasicObj* mul(BasicObj*, bool) override;
    BasicObj* div(BasicObj*, bool) override;

    bool greater(BasicObj*, bool) override;
    bool less(BasicObj*, bool) override;
    bool equal(BasicObj*, bool) override;

    std::string str() override;
    bool asbool() override;

    void inc() override;
    void dec() override;

    BasicObj* clone() override;
};

class BoolObj : public BasicObj {
public:
    bool a;
    BoolObj(bool);

    bool asbool() override;
    std::string str() override;
    BasicObj* clone() override;
};

class FloatObj : public BasicObj {
public:
    float a;
    FloatObj(float);

    BasicObj* add(BasicObj*, bool) override;
    BasicObj* sub(BasicObj*, bool) override;
    BasicObj* mul(BasicObj*, bool) override;
    BasicObj* div(BasicObj*, bool) override;

    bool greater(BasicObj*, bool) override;
    bool less(BasicObj*, bool) override;
    bool equal(BasicObj*, bool) override;

    std::string str() override;
    bool asbool() override;

    void inc() override;
    void dec() override;

    BasicObj* clone() override;
};

class NullObject : public BasicObj {
public:
    NullObject();
    std::string str() override;
    bool asbool() override;
    BasicObj* clone() override;
};

/* ===================== Containers ===================== */

class ListObject : public BasicObj {
public:
    std::vector<BasicObj*> items;
    ListObject(const std::vector<BasicObj*>&);

    BasicObj* getitem(BasicObj*) override;
    void setitem(BasicObj*, BasicObj*) override;
    std::string str() override;
    BasicObj* clone() override;
};

class MapObject : public BasicObj {
public:
    std::map<BasicObj*, BasicObj*> items;

    MapObject();
    BasicObj* getitem(BasicObj*) override;
    void setitem(BasicObj*, BasicObj*) override;
    std::string str() override;
    BasicObj* clone() override;
};

/* ===================== Functions ===================== */

class FunctionObject : public BasicObj {
public:
    std::vector<std::string> argNames;
    std::string code;
    Namespace context;

    FunctionObject(std::vector<std::string>, std::string, Namespace*);
    BasicObj* call(std::vector<BasicObj*>) override;
    std::string str() override;
    BasicObj* clone() override;
};

class FunctionNative : public BasicObj {
public:
    std::function<BasicObj*(std::vector<BasicObj*>)> func;

    FunctionNative(std::function<BasicObj*(std::vector<BasicObj*>)>);
    BasicObj* call(std::vector<BasicObj*>) override;
    std::string str() override;
    BasicObj* clone() override;
};

/* ===================== Classes ===================== */

class ClassObject : public BasicObj {
public:
    int instanceID;
    std::map<std::string, BasicObj*> attrs;
    std::string a;
    Namespace* context;

    ClassObject(const std::string&, Namespace*);
    BasicObj* call(std::vector<BasicObj*>) override;
    std::string str() override;
    BasicObj* clone() override;
};

class InstanceObject : public BasicObj {
public:
    ClassObject* klass;
    std::vector<BasicObj*> args;

    InstanceObject(ClassObject*, std::vector<BasicObj*>);

    BasicObj* add(BasicObj*, bool) override;
    BasicObj* sub(BasicObj*, bool) override;
    BasicObj* mul(BasicObj*, bool) override;
    BasicObj* div(BasicObj*, bool) override;

    bool equal(BasicObj*, bool) override;
    bool asbool() override;
    std::string str() override;

    BasicObj* getattr(const std::string&);
    void setitem(BasicObj*, BasicObj*) override;
    BasicObj* clone() override;
};

/* ===================== Runtime API ===================== */

BasicObj* exec(std::string, Namespace&);
BasicObj* doCode(std::string, Namespace&);
Namespace CreateContext();
void __clean();
BasicObj* InstanceObj(ClassObject*, std::vector<BasicObj*>);
