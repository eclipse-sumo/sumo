/****************************************************************************/
/// @file    BinaryOutputDevice.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "BinaryOutputDevice.h"

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
char gBinaryOutputDeviceBuf[BUF_MAX];

// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;



BinaryOutputDevice::BinaryOutputDevice(const std::string &name,
                                       bool fliporder)
        : myFlipOrder(fliporder),
        myStream(name.c_str(), fstream::out|fstream::binary)
{}


BinaryOutputDevice::~BinaryOutputDevice()
{}



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
operator<<(BinaryOutputDevice &os, const SUMOReal &f)
{
    os.myStream.write((char*) &f, sizeof(SUMOReal));
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
    if (s.length()<BUF_MAX) {
        os << (unsigned int) s.length();
        os.myStream.write((char*) s.c_str(), sizeof(char)*s.length());
        return os;
    }
    throw 1;
}



/****************************************************************************/

