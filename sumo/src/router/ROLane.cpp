//---------------------------------------------------------------------------//
//                        ROLane.cpp -
//  A single lane the router may use
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.10  2006/01/26 08:42:50  dkrajzew
// made lanes and edges being aware to vehicle classes
//
// Revision 1.9  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2004/11/23 10:25:51  dkrajzew
// debugging
//
// Revision 1.5  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.4  2003/09/17 10:14:27  dkrajzew
// handling of unset values patched
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/Named.h>
#include "ROLane.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROLane::ROLane(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
			   const std::vector<SUMOVehicleClass> &allowed,
			   const std::vector<SUMOVehicleClass> &disallowed)
    : Named(id), myLength(length), myMaxSpeed(maxSpeed),
	myAllowedClasses(allowed), myDisAllowedClasses(disallowed)
{
}


ROLane::~ROLane()
{
}


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
ROLane::getDisallowedClasses() const
{
	return myDisAllowedClasses;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


