#ifndef Named_h
#define Named_h

#include <string>

class Named {
protected:
    std::string _id;
public:
    Named(const std::string &id) : _id(id) { }
    virtual ~Named() { }
    std::string getID() const { return _id; }
};

#endif
