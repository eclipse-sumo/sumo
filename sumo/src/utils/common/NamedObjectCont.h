#ifndef NamedObjectCont_h
#define NamedObjectCont_h

#include <map>
#include <string>

template<class _I>
class NamedObjectCont {
protected:
    typedef std::map<std::string, _I> myCont;
    myCont  _cont;
public:
    NamedObjectCont();
    virtual ~NamedObjectCont();
    virtual bool add(const std::string &id, _I item);
    _I get(const std::string &id) const;
    void clear();
    size_t size() const;
    void erase(const std::string &id);
};

#include "NamedObjectCont.cpp"

#endif
