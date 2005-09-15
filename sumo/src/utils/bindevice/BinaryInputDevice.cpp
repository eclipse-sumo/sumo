#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "BinaryInputDevice.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * constants definitions
 * ======================================================================= */
#define BUF_MAX 1000


/* =========================================================================
 * static member definitions
 * ======================================================================= */
char gBinaryInputDeviceBuf[BUF_MAX];


using namespace std;



BinaryInputDevice::BinaryInputDevice(const std::string &name,
                               bool fliporder)
    : myFlipOrder(fliporder),
    myStream(name.c_str(), fstream::in|fstream::binary)
{
}


BinaryInputDevice::~BinaryInputDevice()
{
}


bool
BinaryInputDevice::good() const
{
    return myStream.good();
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, int &i)
{
    os.myStream.read((char*) &i, sizeof(int));
    return os;
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, unsigned int &i)
{
    os.myStream.read((char*) &i, sizeof(unsigned int));
    return os;
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, float &f)
{
    os.myStream.read((char*) &f, sizeof(float));
    return os;
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, bool &b)
{
    b = 0;
    os.myStream.read((char*) &b, sizeof(char));
    return os;
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, std::string &s)
{
    unsigned int size;
    os.myStream >> size;
    if(size<BUF_MAX) {
        os.myStream.read((char*) &gBinaryInputDeviceBuf, sizeof(char)*size);
        gBinaryInputDeviceBuf[size] = 0;
        s = std::string(gBinaryInputDeviceBuf);
        return os;
    }
    throw 1;
}


BinaryInputDevice &
operator>>(BinaryInputDevice &os, long &l)
{
    os.myStream.read((char*) &l, sizeof(long));
    return os;
}



