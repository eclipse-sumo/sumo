/****************************************************************************/
/// @file    GUILane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Representation of a lane in the micro simulation (gui-version)
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
#include <utils/foxtools/MFXMutex.h>
#include <utils/geom/Position.h>
#include <utils/common/MsgHandler.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include "GUILane.h"
#include "GUIVehicle.h"
#include "GUINet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUILane::GUILane(const std::string& id, SUMOReal maxSpeed, SUMOReal length,
                 MSEdge* const edge, unsigned int numericalID,
                 const PositionVector& shape, SUMOReal width,
                 SVCPermissions permissions) :
    MSLane(id, maxSpeed, length, edge, numericalID, shape, width, permissions) {}


GUILane::~GUILane() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle insertion ------
void
GUILane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed,
                            const MSLane::VehCont::iterator& at,
                            MSMoveReminder::Notification notification) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::incorporateVehicle(veh, pos, speed, at, notification);
}


// ------ Access to vehicles ------
const MSLane::VehCont&
GUILane::getVehiclesSecure() const {
    myLock.lock();
    return myVehicles;
}


void
GUILane::releaseVehicles() const {
    myLock.unlock();
}


bool
GUILane::moveCritical(SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::moveCritical(t);
}


bool
GUILane::setCritical(SUMOTime t, std::vector<MSLane*>& into) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::setCritical(t, into);
}


MSVehicle*
GUILane::removeVehicle(MSVehicle* remVehicle) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::removeVehicle(remVehicle);
}


void
GUILane::swapAfterLaneChange(SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::swapAfterLaneChange(t);
}


bool
GUILane::integrateNewVehicle(SUMOTime t) {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSLane::integrateNewVehicle(t);
}


GUILaneWrapper*
GUILane::buildLaneWrapper(unsigned int index) {
    return new GUILaneWrapper(*this, myShape, index);
}


void
GUILane::detectCollisions(SUMOTime timestep) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSLane::detectCollisions(timestep);
}



/****************************************************************************/

