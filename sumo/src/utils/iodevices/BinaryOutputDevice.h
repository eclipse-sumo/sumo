/****************************************************************************/
/// @file    BinaryOutputDevice.h
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
#ifndef BinaryOutputDevice_h
#define BinaryOutputDevice_h
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
#include <fstream>

class BinaryOutputDevice
{
public:
    BinaryOutputDevice(const std::string &name, bool fliporder=false);
    ~BinaryOutputDevice();

    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const int &i);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const unsigned int &i);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const SUMOReal &f);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const bool &b);
    friend BinaryOutputDevice &operator<<(BinaryOutputDevice &os, const std::string &s);

private:
    bool myFlipOrder;
    std::ofstream myStream;

};


#endif

/****************************************************************************/

