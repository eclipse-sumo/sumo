/****************************************************************************/
/// @file    ROLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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

#include <utils/common/Named.h>
#include "ROLane.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// method definitions
// ===========================================================================
ROLane::ROLane(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed)
        : Named(id), myLength(length), myMaxSpeed(maxSpeed),
        myAllowedClasses(allowed), myNotAllowedClasses(disallowed)
{}


ROLane::~ROLane()
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

