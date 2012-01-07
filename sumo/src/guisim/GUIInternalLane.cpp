/****************************************************************************/
/// @file    GUIInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 04.09.2003
/// @version $Id$
///
// Lane within junctions, derived from the normal lane
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include "GUILaneWrapper.h"
#include "GUIInternalLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIInternalLane::GUIInternalLane(const std::string& id,
                                 SUMOReal maxSpeed, SUMOReal length,
                                 MSEdge* const edge, unsigned int numericalID,
                                 const PositionVector& shape, SUMOReal width,
                                 const SUMOVehicleClasses& allowed,
                                 const SUMOVehicleClasses& disallowed)
    : MSInternalLane(id, maxSpeed, length, edge, numericalID, shape, width, allowed, disallowed) {}


GUIInternalLane::~GUIInternalLane() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle insertion ------
void
GUIInternalLane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed,
                                    const MSLane::VehCont::iterator& at,
                                    MSMoveReminder::Notification notification) {
    myLock.lock();
    try {
        MSInternalLane::incorporateVehicle(veh, pos, speed, at, notification);
        myLock.unlock();
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


// ------ Access to vehicles ------
const MSLane::VehCont&
GUIInternalLane::getVehiclesSecure() const {
    myLock.lock();
    return myVehicles;
}


void
GUIInternalLane::releaseVehicles() const {
    myLock.unlock();
}


bool
GUIInternalLane::moveCritical(SUMOTime t) {
    myLock.lock();
    try {
        bool ret = MSInternalLane::moveCritical(t);
        myLock.unlock();
        return ret;
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::setCritical(SUMOTime t, std::vector<MSLane*> &into) {
    myLock.lock();
    try {
        bool ret = MSInternalLane::setCritical(t, into);
        myLock.unlock();
        return ret;
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


MSVehicle*
GUIInternalLane::removeVehicle(MSVehicle* remVehicle) {
    myLock.lock();
    try {
        MSVehicle* ret = MSLane::removeVehicle(remVehicle);
        myLock.unlock();
        return ret;
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


void
GUIInternalLane::swapAfterLaneChange(SUMOTime t) {
    myLock.lock();
    try {
        MSLane::swapAfterLaneChange(t);
        myLock.unlock();
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::integrateNewVehicle(SUMOTime t) {
    myLock.lock();
    try {
        bool ret = MSLane::integrateNewVehicle(t);
        myLock.unlock();
        return ret;
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}


GUILaneWrapper*
GUIInternalLane::buildLaneWrapper(unsigned int index) {
    return new GUILaneWrapper(*this, myShape, index);
}


void
GUIInternalLane::detectCollisions(SUMOTime timestep) {
    myLock.lock();
    try {
        MSLane::detectCollisions(timestep);
        myLock.unlock();
    } catch (ProcessError&) {
        myLock.unlock();
        throw;
    }
}



/****************************************************************************/

