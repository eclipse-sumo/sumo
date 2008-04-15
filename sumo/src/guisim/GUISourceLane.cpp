/****************************************************************************/
/// @file    GUISourceLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
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
#include <iostream> // !!!
#include <utility>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include "GUILaneWrapper.h"
#include "GUISourceLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUISourceLane::GUISourceLane(/*MSNet &net, */std::string id,
        SUMOReal maxSpeed, SUMOReal length,
        MSEdge* edge, size_t numericalID,
        const Position2DVector &shape,
        const std::vector<SUMOVehicleClass> &allowed,
        const std::vector<SUMOVehicleClass> &disallowed)
        : MSSourceLane(/*net, */id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed)
{}


GUISourceLane::~GUISourceLane()
{
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


bool
GUISourceLane::moveNonCritical()
{
    myLock.lock();
    try {
        bool ret = MSSourceLane::moveNonCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUISourceLane::moveCritical()
{
    myLock.lock();
    try {
        bool ret = MSSourceLane::moveCritical();
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUISourceLane::setCritical(std::vector<MSLane*> &into)
{
    myLock.lock();
    try {
        bool ret = MSSourceLane::setCritical(into);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUISourceLane::emit(MSVehicle& newVeh, bool isReinsertion) throw()
{
    myLock.lock();
    bool ret = MSSourceLane::emit(newVeh, isReinsertion);
    myLock.unlock();
    return ret;
}


bool
GUISourceLane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    myLock.lock();
    try {
        bool ret = MSSourceLane::isEmissionSuccess(aVehicle, vstate);
        myLock.unlock();
        return ret;
    } catch (ProcessError &) {
        myLock.unlock();
        throw;
    }
}


bool
GUISourceLane::push(MSVehicle *veh)
{
    MsgHandler::getErrorInstance()->inform("Vehicle '" + veh->getID() + "' was tried to be pushed on source lane '" + getID() + "'.");
    return false;
}


MSVehicle *
GUISourceLane::removeFirstVehicle()
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
GUISourceLane::removeVehicle(MSVehicle * remVehicle)
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
GUISourceLane::releaseVehicles()
{
    myLock.unlock();
}



const MSLane::VehCont &
GUISourceLane::getVehiclesSecure()
{
    myLock.lock();
    return myVehicles;
}


void
GUISourceLane::swapAfterLaneChange()
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
GUISourceLane::integrateNewVehicle()
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
GUISourceLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


void
GUISourceLane::detectCollisions(SUMOTime timestep)
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
GUISourceLane::pop()
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

