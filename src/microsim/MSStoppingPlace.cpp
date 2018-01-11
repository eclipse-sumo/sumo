/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSStoppingPlace.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
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


// ===========================================================================
// method definitions
// ===========================================================================
MSStoppingPlace::MSStoppingPlace(const std::string& id,
                                 const std::vector<std::string>& lines,
                                 MSLane& lane,
                                 double begPos, double endPos, const std::string name)
    : Named(id), myLines(lines), myLane(lane),
      myBegPos(begPos), myEndPos(endPos), myLastFreePos(endPos), myWaitingPos(endPos), myName(name) {
    computeLastFreePos();
}


MSStoppingPlace::~MSStoppingPlace() {}


const MSLane&
MSStoppingPlace::getLane() const {
    return myLane;
}


double
MSStoppingPlace::getBeginLanePosition() const {
    return myBegPos;
}


double
MSStoppingPlace::getEndLanePosition() const {
    return myEndPos;
}


void
MSStoppingPlace::enter(SUMOVehicle* what, double beg, double end) {
    myEndPositions[what] = std::pair<double, double>(beg, end);
    computeLastFreePos();
}


double
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


double
MSStoppingPlace::getStoppingPosition(const SUMOVehicle* veh) const {
    std::map<const SUMOVehicle*, std::pair<double, double> >::const_iterator i = myEndPositions.find(veh);
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
    myWaitingPos = MAX2(myBegPos, myWaitingPos);
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
    std::map<const SUMOVehicle*, std::pair<double, double> >::iterator i;
    for (i = myEndPositions.begin(); i != myEndPositions.end(); i++) {
        if (myLastFreePos > (*i).second.second) {
            myLastFreePos = (*i).second.second;
        }
    }
}


double
MSStoppingPlace::getAccessPos(const MSEdge* edge) const {
    if (edge == &myLane.getEdge()) {
        return (myBegPos + myEndPos) / 2.;
    }
    for (std::multimap<MSLane*, double>::const_iterator i = myAccessPos.begin(); i != myAccessPos.end(); ++i) {
        if (edge == &i->first->getEdge()) {
            return i->second;
        }
    }
    return -1.;
}


bool
MSStoppingPlace::hasAccess(const MSEdge* edge) const {
    if (edge == &myLane.getEdge()) {
        return true;
    }
    for (std::multimap<MSLane*, double>::const_iterator i = myAccessPos.begin(); i != myAccessPos.end(); ++i) {
        if (edge == &i->first->getEdge()) {
            return true;
        }
    }
    return false;
}


const std::string&
MSStoppingPlace::getMyName() const {
    return myName;
}


/****************************************************************************/
