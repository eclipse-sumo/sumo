#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "BinaryOutputDevice.h"

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
char gBinaryOutputDeviceBuf[BUF_MAX];


using namespace std;



BinaryOutputDevice::BinaryOutputDevice(const std::string &name,
                               bool fliporder)
    : myFlipOrder(fliporder),
    myStream(name.c_str(), fstream::out|fstream::binary)
{
}


BinaryOutputDevice::~BinaryOutputDevice()
{
}



BinaryOutputDevice &
operator<<(BinaryOutputDevice &os, const int &i)
{
    os.myStream.write((char*) &i, sizeof(int));
    return os;
}


BinaryOutputDevice &
operator<<(BinaryOutputDevice &os, const unsigned int &i)
{
    os.myStream.write((char*) &i, sizeof(unsigned int));
    return os;
}


BinaryOutputDevice &
operator<<(BinaryOutputDevice &os, const float &f)
{
    os.myStream.write((char*) &f, sizeof(float));
    return os;
}


BinaryOutputDevice &
operator<<(BinaryOutputDevice &os, const bool &b)
{
    os.myStream.write((char*) &b, sizeof(char));
    return os;
}


BinaryOutputDevice &
operator<<(BinaryOutputDevice &os, const std::string &s)
{
    if(s.length()<BUF_MAX) {
        os << (unsigned int) s.length();
        os.myStream.write((char*) s.c_str(), sizeof(char)*s.length());
        return os;
    }
    throw 1;
}

