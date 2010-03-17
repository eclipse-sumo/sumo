/****************************************************************************/
/// @file    MSPerson.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// missing_desc
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
#include <vector>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSPerson.h"
#include "MSPersonControl.h"
#include "MSEdgeControl.h"
#include "MSEmitControl.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage::MSPersonStage(const MSEdge &destination)
        : myDestination(destination) {}


MSPerson::MSPersonStage::~MSPersonStage() {}


const MSEdge &
MSPerson::MSPersonStage::getDestination() const {
    return myDestination;
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(MSEdgeVector route, SUMOTime walkingTime, SUMOReal speed)
        : MSPersonStage(*route.back()), myWalkingTime(walkingTime), myWalkingSpeed(speed) {}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking() {}


SUMOTime
MSPerson::MSPersonStage_Walking::getWalkingTime() {
    return myWalkingTime;
}


void
MSPerson::MSPersonStage_Walking::proceed(MSNet* /*net*/,
        MSPerson* /*person*/, SUMOTime /*now*/,
        MSEdge* /*previousEdge*/) {
//!!!    myWalking.add(now + m_uiWalkingTime, person);
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_PrivateVehicle - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Driving::MSPersonStage_Driving(const MSEdge &destination,
                                                       const std::vector<std::string> &lines)
        : MSPersonStage(destination), myLines(lines) {}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


void
MSPerson::MSPersonStage_Driving::proceed(MSNet* /*net*/,
        MSPerson* /*person*/, SUMOTime /*now*/,
        MSEdge* /*previousEdge*/) {
    /*!!!
    MSVehicle *vehicle = new MSVehicle(m_VehicleId,  (MSNet::Route*) MSNet::routeDict(m_RouteId), now, MSVehicleType::dictionary(m_VehicleType));
    vehicle->addPerson(person, m_pDestination);
    net->myEmitter->addStarting(vehicle);
    */
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge &destination,
                                                       SUMOTime duration, SUMOTime until)
        : MSPersonStage(destination), myWaitingDuration(duration), myWaitingUntil(until) {}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting() {}


void
MSPerson::MSPersonStage_Waiting::proceed(MSNet* /*net*/,
        MSPerson* /*person*/, SUMOTime /*now*/,
        MSEdge* /*previousEdge*/) {
//!!!!    net->myPersons->add(now + m_uiWaitingTime, person);
}


/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, MSPersonPlan *plan)
        : myParameter(pars), myPlan(plan) {
    myStep = myPlan->begin();
}


MSPerson::~MSPerson() {
    delete myPlan;
}


const MSPerson::MSPersonStage &
MSPerson::getCurrentStage() const {
    return **myStep;
}


void
MSPerson::proceed(MSNet* /*net*/, SUMOTime /*time*/) {
    /*!!!
    MSEdge *arrivedAt = m_pStep->getDestination();
    m_pStep++;
    if(m_pStep==m_pRoute->end()) return;
    (*m_pStep).proceed(net, this, time, arrivedAt);
    */
}


bool
MSPerson::endReached() const {
    return myStep == myPlan->end();
}


/****************************************************************************/

