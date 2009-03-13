/****************************************************************************/
/// @file    NIVissimExtendedEdgePoint.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/VectorHelper.h>
#include <netbuild/NBEdge.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimExtendedEdgePoint::NIVissimExtendedEdgePoint(
    int edgeid, const IntVector &lanes, SUMOReal position,
    const IntVector &assignedVehicles) throw()
        : myEdgeID(edgeid), myLanes(lanes), myPosition(position),
        myAssignedVehicles(assignedVehicles) {}


NIVissimExtendedEdgePoint::~NIVissimExtendedEdgePoint() {}


int
NIVissimExtendedEdgePoint::getEdgeID() const {
    return myEdgeID;
}


SUMOReal
NIVissimExtendedEdgePoint::getPosition() const {
    return myPosition;
}


Position2D
NIVissimExtendedEdgePoint::getGeomPosition() const {
    return
        NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


const IntVector &
NIVissimExtendedEdgePoint::getLanes() const {
    return myLanes;
}


void
NIVissimExtendedEdgePoint::recheckLanes(const NBEdge * const edge) throw() {
    // check whether an "all" indicator is there
    bool hadAll = false;
    for (IntVector::const_iterator i=myLanes.begin(); !hadAll&&i!=myLanes.end(); ++i) {
        if ((*i)==-1) {
            hadAll = true;
        }
    }
    // no -> return
    if (!hadAll) {
        return;
    }
    // patch lane indices
    myLanes.clear();
    for (int i=0; i<edge->getNoLanes(); ++i) {
        myLanes.push_back(i);
    }
}


/****************************************************************************/

