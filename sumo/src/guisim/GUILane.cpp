/****************************************************************************/
/// @file    GUILane.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A grid of edges for faster drawing
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
// used namespaces
// ===========================================================================
using namespace std;
 

// ===========================================================================
// method definitions
// ===========================================================================
GUILane::GUILane(/*MSNet &net, */std::string id, SUMOReal maxSpeed, SUMOReal length,
                                 MSEdge* edge, size_t numericalID,
                                 const Position2DVector &shape,
                                 const std::vector<SUMOVehicleClass> &allowed,
                                 const std::vector<SUMOVehicleClass> &disallowed)
        : MSLane(/*net, */id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed)
{}


GUILane::~GUILane()
{
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


bool
GUILane::moveNonCritical()
{
    myLock.lock();
    try {
        bool ret = MSLane::moveNonCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUILane::moveCritical()
{
    myLock.lock();
    try {
        bool ret = MSLane::moveCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUILane::setCritical(std::vector<MSLane*> &into)
{
    myLock.lock();
    try {
        bool ret = MSLane::setCritical(into);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}



bool
GUILane::emit(MSVehicle& newVeh, bool isReinsertion) throw()
{
    myLock.lock();
    bool ret = MSLane::emit(newVeh, isReinsertion);
    myLock.unlock();
    return ret;
}


bool
GUILane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    myLock.lock();
    try {
        bool ret = MSLane::isEmissionSuccess(aVehicle, vstate);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUILane::push(MSVehicle* veh)
{
    myLock.lock();
    try {
        // Insert vehicle only if it's destination isn't reached.
        //  and it does not collide with previous
        // check whether the vehicle has ended his route
        if (! veh->destReached(myEdge)) {     // adjusts vehicles routeIterator
            myVehBuffer.push_back(veh);
            veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
            SUMOReal pspeed = veh->getSpeed();
            SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
            veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
            veh->_assertPos();
            myLock.unlock();
            return false;
        } else {
            veh->onTripEnd();
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            myLock.unlock();
            return true;
        }
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


MSVehicle *
GUILane::removeFirstVehicle()
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
GUILane::removeVehicle(MSVehicle * remVehicle)
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
GUILane::releaseVehicles()
{
    myLock.unlock();
}


const MSLane::VehCont &
GUILane::getVehiclesSecure()
{
    myLock.lock();
    return myVehicles;
}


void
GUILane::swapAfterLaneChange()
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
GUILane::integrateNewVehicle()
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
GUILane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


void
GUILane::detectCollisions(SUMOTime timestep)
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
GUILane::pop()
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

