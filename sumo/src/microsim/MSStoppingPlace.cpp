/****************************************************************************/
/// @file    MSStoppingPlace.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cassert>
#include <map>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <microsim/MSVehicleType.h>
#include "MSLane.h"
#include "MSTransportable.h"
#include "MSStoppingPlace.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSStoppingPlace::MSStoppingPlace(const std::string& id,
                                 const std::vector<std::string>& lines,
                                 MSLane& lane,
                                 SUMOReal begPos, SUMOReal endPos)
    : Named(id), myLines(lines), myLane(lane),
      myBegPos(begPos), myEndPos(endPos), myLastFreePos(endPos), myWaitingPos(endPos) {
    computeLastFreePos();
}


MSStoppingPlace::~MSStoppingPlace() {}


const MSLane&
MSStoppingPlace::getLane() const {
    return myLane;
}


SUMOReal
MSStoppingPlace::getBeginLanePosition() const {
    return myBegPos;
}


SUMOReal
MSStoppingPlace::getEndLanePosition() const {
    return myEndPos;
}


void
MSStoppingPlace::enter(SUMOVehicle* what, SUMOReal beg, SUMOReal end) {
    myEndPositions[what] = std::pair<SUMOReal, SUMOReal>(beg, end);
    computeLastFreePos();
}


SUMOReal
MSStoppingPlace::getLastFreePos(const SUMOVehicle& forVehicle) const {
    if (myLastFreePos != myEndPos) {
        return myLastFreePos - forVehicle.getVehicleType().getMinGap();
    }
    return myLastFreePos;
}


Position
MSStoppingPlace::getWaitPosition() const {
    return myLane.getShape().positionAtOffset(myLane.interpolateLanePosToGeometryPos(myWaitingPos), .5);
}


SUMOReal
MSStoppingPlace::getStoppingPosition(const SUMOVehicle* veh) const {
    std::map<const SUMOVehicle*, std::pair<SUMOReal, SUMOReal> >::const_iterator i = myEndPositions.find(veh);
    if (i != myEndPositions.end()) {
        return i->second.second;
    } else {
        return getLastFreePos(*veh);
    }
}

void
MSStoppingPlace::addTransportable(MSTransportable* p) {
    myWaitingTransportables.push_back(p);
    myWaitingPos -= p->getVehicleType().getLength();
}


void
MSStoppingPlace::removeTransportable(MSTransportable* p) {
    std::vector<MSTransportable*>::iterator i = std::find(myWaitingTransportables.begin(), myWaitingTransportables.end(), p);
    if (i != myWaitingTransportables.end()) {
        if (i == myWaitingTransportables.end() - 1) {
            myWaitingPos -= p->getVehicleType().getLength();
        }
        if (i == myWaitingTransportables.begin()) {
            myWaitingPos = getEndLanePosition();
        }
        myWaitingTransportables.erase(i);
    }
}


void
MSStoppingPlace::leaveFrom(SUMOVehicle* what) {
    assert(myEndPositions.find(what) != myEndPositions.end());
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSStoppingPlace::computeLastFreePos() {
    myLastFreePos = myEndPos;
    std::map<const SUMOVehicle*, std::pair<SUMOReal, SUMOReal> >::iterator i;
    for (i = myEndPositions.begin(); i != myEndPositions.end(); i++) {
        if (myLastFreePos > (*i).second.second) {
            myLastFreePos = (*i).second.second;
        }
    }
}


bool
MSStoppingPlace::hasAccess(const MSEdge* edge) const {
    if (edge == &myLane.getEdge()) {
        return true;
    }
    for (std::multimap<MSLane*, SUMOReal>::const_iterator i = myAccessPos.begin(); i != myAccessPos.end(); ++i) {
        if (edge == &i->first->getEdge()) {
            return true;
        }
    }
    return false;
}

/****************************************************************************/

