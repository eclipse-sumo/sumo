/****************************************************************************/
/// @file    BinaryInputDevice.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id: BinaryInputDevice.cpp 4389 2007-08-28 10:21:00Z behrisch $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "BinaryInputDevice.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// constants definitions
// ===========================================================================
#define BUF_MAX 1000


// ===========================================================================
// static member definitions
// ===========================================================================
char gBinaryInputDeviceBuf[BUF_MAX];

// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;



BinaryInputDevice::BinaryInputDevice(const std::string &name,
                                     bool fliporder)
        : myFlipOrder(fliporder),
        myStream(name.c_str(), fstream::in|fstream::binary)
{}


BinaryInputDevice::~BinaryInputDevice()
{}


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
operator>>(BinaryInputDevice &os, SUMOReal &f)
{
    os.myStream.read((char*) &f, sizeof(SUMOReal));
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
    os >> size;
    if (size<BUF_MAX) {
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



/****************************************************************************/

