#ifndef MSTrigger_h
#define MSTrigger_h

#include <utils/common/Named.h>

class MSTrigger : public Named {
public:
    MSTrigger(const std::string &id) : Named(id) { } 
    ~MSTrigger() { } 
};

#endif
