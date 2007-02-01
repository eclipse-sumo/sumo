/****************************************************************************/
/// @file    GUIInternalLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 04.09.2003
/// @version $Id: $
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT std::cout


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
    if (_lock.locked()) {
        _lock.unlock();
    }
}


void
GUIInternalLane::moveNonCritical()
{
    _lock.lock();//Display();
    MSInternalLane::moveNonCritical();
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveCritical()
{
    _lock.lock();//Display();
    MSInternalLane::moveCritical();
    _lock.unlock();//Display();
}


void
GUIInternalLane::setCritical()
{
    _lock.lock();//Display();
    MSInternalLane::setCritical();
    _lock.unlock();//Display();
}





bool
GUIInternalLane::emit(MSVehicle& newVeh)
{
    _lock.lock();//Display();
    bool ret = MSInternalLane::emit(newVeh);
    _lock.unlock();//Display();
    return ret;
}


bool
GUIInternalLane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    _lock.lock();//Display();
    bool ret = MSInternalLane::isEmissionSuccess(aVehicle, vstate);
    _lock.unlock();//Display();
    return ret;
}


bool
GUIInternalLane::push(MSVehicle* veh)
{
    _lock.lock();//Display();
#ifdef ABS_DEBUG
    if (myVehBuffer!=0) {
        DEBUG_OUT << MSNet::globaltime << ":Push Failed on Lane:" << myID << std::endl;
        DEBUG_OUT << myVehBuffer->getID() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << std::endl;
        DEBUG_OUT << veh->getID() << ", " << veh->pos() << ", " << veh->speed() << std::endl;
    }
#endif
    MSVehicle *last = myVehicles.size()!=0
                      ? myVehicles.front()
                      : 0;

    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    if (myVehBuffer != 0 || (last!=0 && last->getPositionOnLane() < veh->getPositionOnLane())) {
        MSVehicle *prev = myVehBuffer!=0
                          ? myVehBuffer : last;
        WRITE_WARNING("Vehicle '" + veh->getID() + "' beamed due to a collision on push!\n" + "  Lane: '" + myID + "', previous vehicle: '" + prev->getID() + "', time: " + toString<SUMOTime>(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        veh->onTripEnd(/* *this*/);
        veh->removeApproachingInformationOnKill(/*this*/);
        MSVehicleTransfer::getInstance()->addVeh(veh);
//        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        // maybe the vehicle is being tracked; mark as not within the simulation any longer
        _lock.unlock();//Display();
        return true;
    }
    // check whether the vehicle has ended his route
    veh->destReached(myEdge);
    myVehBuffer = veh;
    veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
    SUMOReal pspeed = veh->getSpeed();
    SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
    veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
    veh->_assertPos();
    _lock.unlock();//Display();
//    setApproaching(veh->pos(), veh);
    return false;
}


void
GUIInternalLane::releaseVehicles()
{
    _lock.unlock();
}



const MSLane::VehCont &
GUIInternalLane::getVehiclesSecure()
{
    _lock.lock();
    return myVehicles;
}


void
GUIInternalLane::swapAfterLaneChange()
{
    _lock.lock();
    MSLane::swapAfterLaneChange();
    _lock.unlock();
}


void
GUIInternalLane::integrateNewVehicle()
{
    _lock.lock();
    MSLane::integrateNewVehicle();
    _lock.unlock();
}


GUILaneWrapper *
GUIInternalLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


SUMOReal
GUIInternalLane::getDensity() const
{
    _lock.lock();
    SUMOReal ret = MSLane::getDensity();
    _lock.unlock();
    return ret;
}


SUMOReal
GUIInternalLane::getVehLenSum() const
{
    _lock.lock();
    SUMOReal ret = MSLane::getVehLenSum();
    _lock.unlock();
    return ret;
}


void
GUIInternalLane::detectCollisions(SUMOTime timestep)
{
    _lock.lock();
    MSLane::detectCollisions(timestep);
    _lock.unlock();
}


MSVehicle*
GUIInternalLane::pop()
{
    _lock.lock();
    MSVehicle *ret = MSLane::pop();
    _lock.unlock();
    return ret;
}



/****************************************************************************/

