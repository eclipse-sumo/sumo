/****************************************************************************/
/// @file    MSEdgeWeightsStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    02.11.2009
/// @version $Id$
///
// A storage for edge travel times and efforts
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "MSEdgeWeightsStorage.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSEdgeWeightsStorage::MSEdgeWeightsStorage() {
}


MSEdgeWeightsStorage::~MSEdgeWeightsStorage() {
}


bool
MSEdgeWeightsStorage::retrieveExistingTravelTime(const MSEdge* const e, const SUMOVehicle* const /*v*/,
        SUMOReal t, SUMOReal& value) const {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::const_iterator i = myTravelTimes.find((MSEdge*) e);
    if (i == myTravelTimes.end()) {
        return false;
    }
    const ValueTimeLine<SUMOReal>& tl = (*i).second;
    if (!tl.describesTime(t)) {
        return false;
    }
    value = tl.getValue(t);
    return true;
}


bool
MSEdgeWeightsStorage::retrieveExistingEffort(const MSEdge* const e, const SUMOVehicle* const /*v*/,
        SUMOReal t, SUMOReal& value) const {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::const_iterator i = myEfforts.find((MSEdge*) e);
    if (i == myEfforts.end()) {
        return false;
    }
    const ValueTimeLine<SUMOReal>& tl = (*i).second;
    if (!tl.describesTime(t)) {
        return false;
    }
    value = tl.getValue(t);
    return true;
}


void
MSEdgeWeightsStorage::addTravelTime(const MSEdge* const e,
                                    SUMOReal begin, SUMOReal end,
                                    SUMOReal value) {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::iterator i = myTravelTimes.find((MSEdge*) e);
    if (i == myTravelTimes.end()) {
        myTravelTimes[(MSEdge*)e] = ValueTimeLine<SUMOReal>();
        i = myTravelTimes.find((MSEdge*) e);
    }
    (*i).second.add(begin, end, value);
}


void
MSEdgeWeightsStorage::addEffort(const MSEdge* const e,
                                SUMOReal begin, SUMOReal end,
                                SUMOReal value) {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::iterator i = myEfforts.find((MSEdge*) e);
    if (i == myEfforts.end()) {
        myEfforts[(MSEdge*)e] = ValueTimeLine<SUMOReal>();
        i = myEfforts.find((MSEdge*) e);
    }
    (*i).second.add(begin, end, value);
}


void
MSEdgeWeightsStorage::removeTravelTime(const MSEdge* const e) {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::iterator i = myTravelTimes.find((MSEdge*) e);
    if (i != myTravelTimes.end()) {
        myTravelTimes.erase(i);
    }
}


void
MSEdgeWeightsStorage::removeEffort(const MSEdge* const e) {
    std::map<MSEdge*, ValueTimeLine<SUMOReal> >::iterator i = myEfforts.find((MSEdge*) e);
    if (i != myEfforts.end()) {
        myEfforts.erase(i);
    }
}


bool
MSEdgeWeightsStorage::knowsTravelTime(const MSEdge* const e) const {
    return myTravelTimes.find((MSEdge*) e) != myTravelTimes.end();
}


bool
MSEdgeWeightsStorage::knowsEffort(const MSEdge* const e) const {
    return myEfforts.find((MSEdge*) e) != myEfforts.end();
}



/****************************************************************************/

