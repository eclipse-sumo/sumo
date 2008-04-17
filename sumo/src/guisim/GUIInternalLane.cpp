/****************************************************************************/
/// @file    GUIInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 04.09.2003
/// @version $Id$
///
// Lane within junctions, derived from the normal lane
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIInternalLane::GUIInternalLane(/*MSNet &net, */std::string id,
        SUMOReal maxSpeed, SUMOReal length,
        MSEdge* edge, size_t numericalID,
        const Position2DVector &shape,
        const std::vector<SUMOVehicleClass> &allowed,
        const std::vector<SUMOVehicleClass> &disallowed)
        : MSInternalLane(id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed)
{}


GUIInternalLane::~GUIInternalLane()
{
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


bool
GUIInternalLane::moveNonCritical()
{
    myLock.lock();
    try {
        bool ret = MSInternalLane::moveNonCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::moveCritical()
{
    myLock.lock();
    try {
        bool ret = MSInternalLane::moveCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::setCritical(std::vector<MSLane*> &into)
{
    myLock.lock();
    try {
        bool ret = MSInternalLane::setCritical(into);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}





bool
GUIInternalLane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    myLock.lock();
    try {
        bool ret = MSInternalLane::isEmissionSuccess(aVehicle, vstate);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::push(MSVehicle* veh)
{
    myLock.lock();
    try {
        // check whether the vehicle has ended his route
        veh->destReached(myEdge);
        myVehBuffer.push_back(veh);
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        veh->_assertPos();
        myLock.unlock();
        return false;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle *
GUIInternalLane::removeFirstVehicle()
{
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
GUIInternalLane::removeVehicle(MSVehicle * remVehicle)
{
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
GUIInternalLane::releaseVehicles()
{
    myLock.unlock();
}



const MSLane::VehCont &
GUIInternalLane::getVehiclesSecure()
{
    myLock.lock();
    return myVehicles;
}


void
GUIInternalLane::swapAfterLaneChange()
{
    myLock.lock();
    try {
        MSLane::swapAfterLaneChange();
        myLock.unlock();
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUIInternalLane::integrateNewVehicle()
{
    myLock.lock();
    try {
        bool ret = MSLane::integrateNewVehicle();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


GUILaneWrapper *
GUIInternalLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


void
GUIInternalLane::detectCollisions(SUMOTime timestep)
{
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
GUIInternalLane::pop()
{
    myLock.lock();
    try {
        MSVehicle *ret = MSLane::pop();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}



/****************************************************************************/

