/****************************************************************************/
/// @file    GUILane.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Representation of a lane in the micro simulation (gui-version)
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
#include <utils/geom/Position2D.h>
#include <utils/common/MsgHandler.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include "GUILane.h"
#include "GUIVehicle.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GUINet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUILane::GUILane(const std::string &id, SUMOReal maxSpeed, SUMOReal length,
                 MSEdge * const edge, unsigned int numericalID,
                 const Position2DVector &shape,
                 const std::vector<SUMOVehicleClass> &allowed,
                 const std::vector<SUMOVehicleClass> &disallowed) throw()
        : MSLane(id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed) {}


GUILane::~GUILane() throw() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


// ------ Vehicle emission ------
bool
GUILane::isEmissionSuccess(MSVehicle* aVehicle, SUMOReal speed, SUMOReal pos,
                           bool recheckNextLanes) throw(ProcessError) {
    myLock.lock();
    bool ret = MSLane::isEmissionSuccess(aVehicle, speed, pos, recheckNextLanes);
    myLock.unlock();
    return ret;
}


// ------ Access to vehicles ------
const MSLane::VehCont &
GUILane::getVehiclesSecure() const throw() {
    myLock.lock();
    return myVehicles;
}


void
GUILane::releaseVehicles() const throw() {
    myLock.unlock();
}


bool
GUILane::moveCritical(SUMOTime t) {
    myLock.lock();
    try {
        bool ret = MSLane::moveCritical(t);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUILane::setCritical(SUMOTime t, std::vector<MSLane*> &into) {
    myLock.lock();
    try {
        bool ret = MSLane::setCritical(t, into);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUILane::push(MSVehicle* veh) {
    myLock.lock();
    try {
        bool result = MSLane::push(veh);
        myLock.unlock();
        return result;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle *
GUILane::removeVehicle(MSVehicle * remVehicle) {
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
GUILane::swapAfterLaneChange(SUMOTime t) {
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
GUILane::integrateNewVehicle(SUMOTime t) {
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
GUILane::buildLaneWrapper(GUIGlObjectStorage &idStorage) {
    return new GUILaneWrapper(idStorage, *this, myShape);
}


void
GUILane::detectCollisions(SUMOTime timestep) {
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
GUILane::pop(SUMOTime t) {
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

