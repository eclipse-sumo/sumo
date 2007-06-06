/****************************************************************************/
/// @file    NIVissimExtendedEdgePoint.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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


#include <utils/common/IntVector.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

NIVissimExtendedEdgePoint::NIVissimExtendedEdgePoint(
    int edgeid, IntVector &lanes, SUMOReal position,
    IntVector &assignedVehicles)
        : myEdgeID(edgeid), myLanes(lanes), myPosition(position),
        myAssignedVehicles(assignedVehicles)
{}


NIVissimExtendedEdgePoint::~NIVissimExtendedEdgePoint()
{}


int
NIVissimExtendedEdgePoint::getEdgeID() const
{
    return myEdgeID;
}


SUMOReal
NIVissimExtendedEdgePoint::getPosition() const
{
    return myPosition;
}


Position2D
NIVissimExtendedEdgePoint::getGeomPosition() const
{
    return
        NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


const IntVector &
NIVissimExtendedEdgePoint::getLanes() const
{
    return myLanes;
}



/****************************************************************************/

