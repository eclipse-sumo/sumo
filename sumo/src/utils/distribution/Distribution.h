#ifndef Distribution_h
#define Distribution_h

#include <utils/common/Named.h>

class Distribution : public Named {
public:
    Distribution(const std::string &id) : Named(id) { }
    virtual ~Distribution() { }
};

#endif
