#ifndef OutputDevice_COUT_h
#define OutputDevice_COUT_h

#include "OutputDevice.h"

class OutputDevice_COUT : public OutputDevice {
public:
    OutputDevice_COUT();
    ~OutputDevice_COUT();
    bool ok();
    void close();

    std::ostream &getOStream();

};

#endif
