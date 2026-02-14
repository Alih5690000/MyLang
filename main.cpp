#include <vector>
#include <map>
class BasicObj;

class BasicObj{
    public:
    virtual BasicObj* add(BasicObj*,BasicObj*){};
    virtual BasicObj* sub(BasicObj*,BasicObj*){};
    virtual BasicObj* mul(BasicObj*,BasicObj*){};
    virtual BasicObj* div(BasicObj*,BasicObj*){};
    virtual void free(BasicObj*){};
    virtual BasicObj* call(std::vector<BasicObj*>){};
    std::map<BasicObj*> attrs;
};

class IntObj:public BasicObj{
    int a;
    BasicObj* add(BasicObj* a,BasicObj* b) override{
      
    }
};
