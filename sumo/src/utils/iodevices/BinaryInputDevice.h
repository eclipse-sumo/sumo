/****************************************************************************/
/// @file    BinaryInputDevice.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id: BinaryInputDevice.h 4389 2007-08-28 10:21:00Z behrisch $
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
#ifndef BinaryInputDevice_h
#define BinaryInputDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>

class BinaryInputDevice
{
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

/****************************************************************************/

