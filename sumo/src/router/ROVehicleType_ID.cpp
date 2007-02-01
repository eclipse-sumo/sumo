/****************************************************************************/
/// @file    ROVehicleType_ID.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id: $
///
// A type which is only described by its id (other values are invalid)
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
#include <iostream>
#include "ROVehicleType.h"
#include "ROVehicleType_ID.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



// ===========================================================================
// method definitions
// ===========================================================================
ROVehicleType_ID::ROVehicleType_ID(const std::string &id)
        : ROVehicleType(id, RGBColor(-1, -1, -1), 1, SVC_UNKNOWN)
{}


ROVehicleType_ID::~ROVehicleType_ID()
{}


std::ostream &
ROVehicleType_ID::xmlOut(std::ostream &os) const
{
    return os;
}



/****************************************************************************/

