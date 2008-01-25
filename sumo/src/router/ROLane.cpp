/****************************************************************************/
/// @file    ROLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single lane the router may use
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

#include <utils/common/Named.h>
#include "ROLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROLane::ROLane(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed) throw()
        : Named(id), myLength(length), myMaxSpeed(maxSpeed),
        myAllowedClasses(allowed), myNotAllowedClasses(disallowed)
{}


ROLane::~ROLane() throw()
{}


SUMOReal
ROLane::getLength() const
{
    return myLength;
}


SUMOReal
ROLane::getSpeed() const
{
    return myMaxSpeed;
}

ROEdge *
ROLane::getEdge()
{
    return myEdge;
}


const std::vector<SUMOVehicleClass> &
ROLane::getAllowedClasses() const
{
    return myAllowedClasses;
}

const std::vector<SUMOVehicleClass> &
ROLane::getNotAllowedClasses() const
{
    return myNotAllowedClasses;
}



/****************************************************************************/

