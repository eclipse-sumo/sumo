#ifndef BinaryInputDevice_h
#define BinaryInputDevice_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <fstream>

class BinaryInputDevice {
public:
    BinaryInputDevice(const std::string &name, bool fliporder=false);
    ~BinaryInputDevice();
    bool good() const;

    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, int &i);
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, unsigned int &i);
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, SUMOReal &f);
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, bool &b);
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, std::string &s);
    friend BinaryInputDevice &operator>>(BinaryInputDevice &os, long &l);


private:
    bool myFlipOrder;
    std::ifstream myStream;

};


#endif
