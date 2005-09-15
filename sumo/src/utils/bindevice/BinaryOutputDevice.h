#ifndef BinaryOutputDevice_h
#define BinaryOutputDevice_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <fstream>

class BinaryOutputDevice {
public:
    BinaryOutputDevice(const std::string &name, bool fliporder=false);
    ~BinaryOutputDevice();

    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const int &i);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const unsigned int &i);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const float &f);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const bool &b);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const std::string &s);

private:
    bool myFlipOrder;
    std::ofstream myStream;

};


#endif
