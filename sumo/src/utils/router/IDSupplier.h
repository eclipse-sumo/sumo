#ifndef IDSupplier_h
#define IDSupplier_h

#include <string>

class IDSupplier {
private:
    long _current;
    std::string _prefix;
public:
    IDSupplier(const std::string &prefix="", long begin=0);
    ~IDSupplier();
    std::string getNext();
};

#endif

