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


void
GUISourceLane::moveNonCritical()
{
    myLock.lock();//Display();
    MSSourceLane::moveNonCritical(/*firstNeighLane, lastNeighLane*/);
    myLock.unlock();//Display();
}


void
GUISourceLane::moveCritical()
{
    myLock.lock();//Display();
    MSSourceLane::moveCritical(/*firstNeighLane, lastNeighLane*/);
    myLock.unlock();//Display();
}


void
GUISourceLane::setCritical()
{
    myLock.lock();//Display();
    MSSourceLane::setCritical();
    myLock.unlock();//Display();
}


bool
GUISourceLane::emit(MSVehicle& newVeh)
{
    myLock.lock();//Display();
    bool ret = MSSourceLane::emit(newVeh);
    myLock.unlock();//Display();
    return ret;
}


bool
GUISourceLane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    myLock.lock();//Display();
    bool ret = MSSourceLane::isEmissionSuccess(aVehicle, vstate);
    myLock.unlock();//Display();
    return ret;
}


bool
GUISourceLane::push(MSVehicle *veh)
{
    MsgHandler::getErrorInstance()->inform("Vehicle '" + veh->getID() + "' was tried to be pushed on source lane '" + getID() + "'.");
    return false;
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
    MSLane::swapAfterLaneChange();
    myLock.unlock();
}


void
GUISourceLane::integrateNewVehicle()
{
    myLock.lock();
    MSLane::integrateNewVehicle();
    myLock.unlock();
}


GUILaneWrapper *
GUISourceLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


SUMOReal
GUISourceLane::getDensity() const
{
    myLock.lock();
    SUMOReal ret = MSLane::getDensity();
    myLock.unlock();
    return ret;
}


SUMOReal
GUISourceLane::getVehLenSum() const
{
    myLock.lock();
    SUMOReal ret = MSLane::getVehLenSum();
    myLock.unlock();
    return ret;
}


void
GUISourceLane::detectCollisions(SUMOTime timestep)
{
    myLock.lock();
    MSLane::detectCollisions(timestep);
    myLock.unlock();
}


MSVehicle*
GUISourceLane::pop()
{
    myLock.lock();
    MSVehicle *ret = MSLane::pop();
    myLock.unlock();
    return ret;
}



/****************************************************************************/

