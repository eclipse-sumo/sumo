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
// Revision 1.4  2003/09/17 10:14:27  dkrajzew
// handling of unset values patched
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/Named.h>
#include "ROLane.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROLane::ROLane(const std::string &id, double length, double maxSpeed)
    : Named(id), myLength(length), myMaxSpeed(maxSpeed)
{
}


ROLane::~ROLane()
{
}


double
ROLane::getLength() const
{
    return myLength;
}


double
ROLane::getSpeed() const
{
    return myMaxSpeed;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROLane.icc"
//#endif

// Local Variables:
// mode:C++
// End:


