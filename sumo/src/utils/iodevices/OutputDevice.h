#ifndef OutputDevice_h
#define OutputDevice_h

#include <string>

class OutputDevice {
public:
    OutputDevice() { }
    virtual ~OutputDevice() { }
    virtual bool ok() = 0;
    virtual void close() = 0;

    virtual std::ostream &getOStream() = 0;

};



#endif
