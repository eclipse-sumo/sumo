/****************************************************************************/
/// @file    MSPerson.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include "MSLane.h"
#include "MSPerson.h"
#include "MSPersonControl.h"
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include <utils/iodevices/OutputDevice.h>

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
        : myDestination(destination), myDeparted(-1), myArrived(-1) {}


MSPerson::MSPersonStage::~MSPersonStage() {}


const MSEdge &
MSPerson::MSPersonStage::getDestination() const {
    return myDestination;
}


void
MSPerson::MSPersonStage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}


void
MSPerson::MSPersonStage::setArrived(SUMOTime now) {
    myArrived = now;
}


bool
MSPerson::MSPersonStage::isWaitingFor(const std::string&/*line*/) const {
    return false;
}

/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(MSEdgeVector route, SUMOTime walkingTime, SUMOReal speed)
        : MSPersonStage(*route.back()), myWalkingTime(walkingTime) {
    if (speed > 0) {
        SUMOReal time = 0;
        for (MSEdgeVector::const_iterator it = route.begin(); it != route.end(); ++it) {
            time += ((*it)->getLanes())[0]->getLength() / speed;
        }
        myWalkingTime = MAX2(walkingTime, TIME2STEPS(time));
    }
}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking() {}


void
MSPerson::MSPersonStage_Walking::proceed(MSNet* net,
        MSPerson* person, SUMOTime now,
        const MSEdge & /*previousEdge*/) {
    net->getPersonControl().setArrival(MAX2(now, now + myWalkingTime), person);
}


void
MSPerson::MSPersonStage_Walking::tripInfoOutput(OutputDevice &os) const throw(IOError) {
    (os.openTag("walk") <<
     " arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Driving::MSPersonStage_Driving(const MSEdge &destination,
        const std::vector<std::string> &lines)
        : MSPersonStage(destination), myLines(lines.begin(), lines.end()) {}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


void
MSPerson::MSPersonStage_Driving::proceed(MSNet* net,
        MSPerson* person, SUMOTime /*now*/,
        const MSEdge &previousEdge) {
    SUMOVehicle *v = net->getVehicleControl().getWaitingVehicle(&previousEdge, myLines);
    if (v != 0 && v->getParameter().departProcedure == DEPART_TRIGGERED) {
        v->addPerson(person);
        net->getInsertionControl().add(v);
        net->getVehicleControl().removeWaiting(&previousEdge, v);
        net->getVehicleControl().unregisterOneWaitingForPerson();
    } else {
        net->getPersonControl().addWaiting(&previousEdge, person);
    }
}


bool
MSPerson::MSPersonStage_Driving::isWaitingFor(const std::string &line) const {
    return myLines.count(line) > 0;
}


void
MSPerson::MSPersonStage_Driving::tripInfoOutput(OutputDevice &os) const throw(IOError) {
    (os.openTag("ride") <<
     " depart=\"" << time2string(myDeparted) <<
     "\" arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge &destination,
        SUMOTime duration, SUMOTime until)
        : MSPersonStage(destination), myWaitingDuration(duration), myWaitingUntil(until) {}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting() {}


void
MSPerson::MSPersonStage_Waiting::proceed(MSNet* net,
        MSPerson* person, SUMOTime now,
        const MSEdge & /*previousEdge*/) {
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    net->getPersonControl().setArrival(until, person);
}


void
MSPerson::MSPersonStage_Waiting::tripInfoOutput(OutputDevice &os) const throw(IOError) {
    (os.openTag("stop") <<
     " arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}


/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, MSPersonPlan *plan)
        : myParameter(pars), myPlan(plan) {
    myStep = myPlan->begin();
}


MSPerson::~MSPerson() {
    for (MSPersonPlan::const_iterator i=myPlan->begin(); i!=myPlan->end(); ++i) {
        delete *i;
    }
    delete myPlan;
    delete myParameter;
}


const std::string&
MSPerson::getID() const throw() {
    return myParameter->id;
}


void
MSPerson::proceed(MSNet* net, SUMOTime time) {
    const MSEdge &arrivedAt = (*myStep)->getDestination();
    (*myStep)->setArrived(time);
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, arrivedAt);
    } else {
        net->getPersonControl().erase(this);
    }
}


SUMOTime
MSPerson::getDesiredDepart() const throw() {
    return myParameter->depart;
}


void
MSPerson::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}


const MSEdge &
MSPerson::getDestination() const {
    return (*myStep)->getDestination();
}


void
MSPerson::tripInfoOutput(OutputDevice &os) const throw(IOError) {
    for (MSPersonPlan::const_iterator i=myPlan->begin(); i!=myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os);
    }
}


bool
MSPerson::isWaitingFor(const std::string &line) const {
    return (*myStep)->isWaitingFor(line);
}


/****************************************************************************/

