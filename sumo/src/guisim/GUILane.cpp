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
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


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


void
GUILane::moveNonCritical()
{
    myLock.lock();//Display();
    MSLane::moveNonCritical();
    myLock.unlock();//Display();
}


void
GUILane::moveCritical()
{
    myLock.lock();//Display();
    MSLane::moveCritical();
    myLock.unlock();//Display();
}


void
GUILane::setCritical()
{
    myLock.lock();//Display();
    MSLane::setCritical();
    myLock.unlock();//Display();
}



bool
GUILane::emit(MSVehicle& newVeh)
{
    myLock.lock();//Display();
    bool ret = MSLane::emit(newVeh);
    myLock.unlock();//Display();
    return ret;
}


bool
GUILane::isEmissionSuccess(MSVehicle* aVehicle, const MSVehicle::State &vstate)
{
    myLock.lock();//Display();
    bool ret = MSLane::isEmissionSuccess(aVehicle, vstate);
    myLock.unlock();//Display();
    return ret;
}


bool
GUILane::push(MSVehicle* veh)
{
    myLock.lock();//Display();
#ifdef ABS_DEBUG
    if (myVehBuffer!=0) {
        DEBUG_OUT << MSNet::globaltime << ":Push Failed on Lane:" << myID << endl;
        DEBUG_OUT << myVehBuffer->getID() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
        DEBUG_OUT << veh->getID() << ", " << veh->pos() << ", " << veh->speed() << endl;
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
        WRITE_WARNING("Vehicle '" + veh->getID() + "' beamed due to a collision on push!\n" + "  Lane: '" + getID() + "', previous vehicle: '" + prev->getID() + "', time: " + toString<SUMOTime>(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        veh->onTripEnd(/* *this*/);
        resetApproacherDistance(); // !!! correct? is it (both lines) really necessary during this simulation part?
        veh->removeApproachingInformationOnKill(/*this*/);
        MSVehicleTransfer::getInstance()->addVeh(veh);
//        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        // maybe the vehicle is being tracked; mark as not within the simulation any longer
        myLock.unlock();//Display();
        return true;
    }
    // check whether the vehicle has ended his route
    if (! veh->destReached(myEdge)) {     // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        veh->_assertPos();
        myLock.unlock();//Display();
//        setApproaching(veh->pos(), veh);
        return false;
    } else {
        veh->onTripEnd(/* *this */);
        resetApproacherDistance();
        veh->removeApproachingInformationOnKill(/*this*/);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        myLock.unlock();//Display();
        return true;
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
    MSLane::swapAfterLaneChange();
    myLock.unlock();
}


void
GUILane::integrateNewVehicle()
{
    myLock.lock();
    MSLane::integrateNewVehicle();
    myLock.unlock();
}


GUILaneWrapper *
GUILane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


SUMOReal
GUILane::getDensity() const
{
    myLock.lock();
    SUMOReal ret = MSLane::getDensity();
    myLock.unlock();
    return ret;
}


SUMOReal
GUILane::getVehLenSum() const
{
    myLock.lock();
    SUMOReal ret = MSLane::getVehLenSum();
    myLock.unlock();
    return ret;
}


void
GUILane::detectCollisions(SUMOTime timestep)
{
    myLock.lock();
    MSLane::detectCollisions(timestep);
    myLock.unlock();
}


MSVehicle*
GUILane::pop()
{
    myLock.lock();
    MSVehicle *ret = MSLane::pop();
    myLock.unlock();
    return ret;
}



/****************************************************************************/

