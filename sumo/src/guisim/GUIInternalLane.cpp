/****************************************************************************/
/// @file    GUIInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 04.09.2003
/// @version $Id$
///
// Lane within junctions, derived from the normal lane
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2D.h>
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
GUIInternalLane::GUIInternalLane(const std::string &id,
                                 SUMOReal maxSpeed, SUMOReal length,
                                 MSEdge * const edge, unsigned int numericalID,
                                 const Position2DVector &shape,
                                 const std::vector<SUMOVehicleClass> &allowed,
                                 const std::vector<SUMOVehicleClass> &disallowed) throw()
        : MSInternalLane(id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed) {}


GUIInternalLane::~GUIInternalLane() throw() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle emission ------
bool
GUIInternalLane::isEmissionSuccess(MSVehicle* aVehicle, SUMOReal speed, SUMOReal pos,
                                   bool recheckNextLanes) throw(ProcessError) {
    myLock.lock();
    bool ret = MSInternalLane::isEmissionSuccess(aVehicle, speed, pos, recheckNextLanes);
    myLock.unlock();
    return ret;
}


// ------ Access to vehicles ------
const MSLane::VehCont &
GUIInternalLane::getVehiclesSecure() const throw() {
    myLock.lock();
    return myVehicles;
}


void
GUIInternalLane::releaseVehicles() const throw() {
    myLock.unlock();
}


bool
GUIInternalLane::moveCritical(SUMOTime t) {
    myLock.lock();
    try {
        bool ret = MSInternalLane::moveCritical(t);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
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
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::push(MSVehicle* veh) {
    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    // check whether the vehicle has ended his route
    myLock.lock();
    try {
        MSLane::push(veh);
        myLock.unlock();
        return false;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle *
GUIInternalLane::removeFirstVehicle() {
    myLock.lock();
    try {
        MSVehicle *ret = MSLane::removeFirstVehicle();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle *
GUIInternalLane::removeVehicle(MSVehicle * remVehicle) {
    myLock.lock();
    try {
        MSVehicle *ret = MSLane::removeVehicle(remVehicle);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
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
    } catch (ProcessError &) {
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
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


GUILaneWrapper *
GUIInternalLane::buildLaneWrapper(GUIGlObjectStorage &idStorage) {
    return new GUILaneWrapper(idStorage, *this, myShape);
}


void
GUIInternalLane::detectCollisions(SUMOTime timestep) {
    myLock.lock();
    try {
        MSLane::detectCollisions(timestep);
        myLock.unlock();
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle*
GUIInternalLane::pop(SUMOTime t) {
    myLock.lock();
    try {
        MSVehicle *ret = MSLane::pop(t);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}



/****************************************************************************/

