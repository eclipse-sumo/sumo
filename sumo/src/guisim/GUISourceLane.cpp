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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
    if (_lock.locked()) {
        _lock.unlock();
    }
}


void
GUISourceLane::moveNonCritical()
{
    _lock.lock();//Display();
    MSSourceLane::moveNonCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUISourceLane::moveCritical()
{
    _lock.lock();//Display();
    MSSourceLane::moveCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUISourceLane::setCritical()
{
    _lock.lock();//Display();
    MSSourceLane::setCritical();
    _lock.unlock();//Display();
}


bool
GUISourceLane::emit(MSVehicle& newVeh)
{
    _lock.lock();//Display();
    bool ret = MSSourceLane::emit(newVeh);
    _lock.unlock();//Display();
    return ret;
}


bool
GUISourceLane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    _lock.lock();//Display();
    bool ret = MSSourceLane::isEmissionSuccess(aVehicle, vstate);
    _lock.unlock();//Display();
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
    _lock.unlock();
}



const MSLane::VehCont &
GUISourceLane::getVehiclesSecure()
{
    _lock.lock();
    return myVehicles;
}


void
GUISourceLane::swapAfterLaneChange()
{
    _lock.lock();
    MSLane::swapAfterLaneChange();
    _lock.unlock();
}


void
GUISourceLane::integrateNewVehicle()
{
    _lock.lock();
    MSLane::integrateNewVehicle();
    _lock.unlock();
}


GUILaneWrapper *
GUISourceLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


SUMOReal
GUISourceLane::getDensity() const
{
    _lock.lock();
    SUMOReal ret = MSLane::getDensity();
    _lock.unlock();
    return ret;
}


SUMOReal
GUISourceLane::getVehLenSum() const
{
    _lock.lock();
    SUMOReal ret = MSLane::getVehLenSum();
    _lock.unlock();
    return ret;
}


void
GUISourceLane::detectCollisions(SUMOTime timestep)
{
    _lock.lock();
    MSLane::detectCollisions(timestep);
    _lock.unlock();
}


MSVehicle*
GUISourceLane::pop()
{
    _lock.lock();
    MSVehicle *ret = MSLane::pop();
    _lock.unlock();
    return ret;
}



/****************************************************************************/

