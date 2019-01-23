/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimExtendedEdgePoint.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/VectorHelper.h>
#include <netbuild/NBEdge.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimExtendedEdgePoint::NIVissimExtendedEdgePoint(
    int edgeid, const std::vector<int>& lanes, double position,
    const std::vector<int>& assignedVehicles)
    : myEdgeID(edgeid), myLanes(lanes), myPosition(position),
      myAssignedVehicles(assignedVehicles) {}


NIVissimExtendedEdgePoint::~NIVissimExtendedEdgePoint() {}


int
NIVissimExtendedEdgePoint::getEdgeID() const {
    return myEdgeID;
}


double
NIVissimExtendedEdgePoint::getPosition() const {
    return myPosition;
}


Position
NIVissimExtendedEdgePoint::getGeomPosition() const {
    return
        NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


const std::vector<int>&
NIVissimExtendedEdgePoint::getLanes() const {
    return myLanes;
}


void
NIVissimExtendedEdgePoint::recheckLanes(const NBEdge* const edge) {
    // check whether an "all" indicator is there
    bool hadAll = false;
    for (std::vector<int>::const_iterator i = myLanes.begin(); !hadAll && i != myLanes.end(); ++i) {
        if ((*i) == -1) {
            hadAll = true;
        }
    }
    // no -> return
    if (!hadAll) {
        return;
    }
    // patch lane indices
    myLanes.clear();
    for (int i = 0; i < (int) edge->getNumLanes(); ++i) {
        myLanes.push_back(i);
    }
}


/****************************************************************************/

