/****************************************************************************/
/// @file    MSPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage::MSPersonStage(const MSEdge& destination, StageType type)
    : myDestination(destination), myDeparted(-1), myArrived(-1), myType(type) {}


MSPerson::MSPersonStage::~MSPersonStage() {}


const MSEdge&
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
MSPerson::MSPersonStage::isWaitingFor(const std::string& /*line*/) const {
    return false;
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(const std::vector<const MSEdge*> &route,  
    MSBusStop *toBS, 
    SUMOTime walkingTime, SUMOReal speed,
    SUMOReal departPos, SUMOReal arrivalPos)
    : MSPersonStage(*route.back(), WALKING), myWalkingTime(walkingTime), myRoute(route),
    myDepartPos(departPos), myArrivalPos(arrivalPos), myDestinationBusStop(toBS)
{
    /*
    if (speed > 0) {
        SUMOReal time = 0;
        for (MSEdgeVector::const_iterator it = route.begin(); it != route.end(); ++it) {
            time += ((*it)->getLanes())[0]->getLength() / speed;
        }
        myWalkingTime = MAX2(walkingTime, TIME2STEPS(time));
    }
    */
    if(toBS!=0) {
        myArrivalPos = toBS->getEndLanePosition();
    }
    /*
    speed = 5./3.6;
    if (speed > 0) {
        SUMOReal off = myDepartPos;
        SUMOTime time = 0;
        std::vector<const MSEdge*>::const_iterator it = route.begin();
        for (; it != route.end()-1; ++it) {
            myRoute.push_back((MSEdge*) *it);
            SUMOTime dur = TIME2STEPS((((*it)->getLanes())[0]->getLength() - off) / speed);
            time += dur;
            myArrivalTimes.push_back(dur);
            off = 0;
        }
        myRoute.push_back((MSEdge*) *it);
        SUMOTime dur = TIME2STEPS((myArrivalPos - off) / speed);
        time += dur;
        myArrivalTimes.push_back(dur);
        myWalkingTime = MAX2(walkingTime, time);
    }
    */
}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking() {}


const MSEdge *
MSPerson::MSPersonStage_Walking::getEdge(SUMOTime now) const {
    return *myRouteStep;
}


Position 
MSPerson::MSPersonStage_Walking::getPosition(SUMOTime now) const {
    const MSEdge *e = getEdge(now);
    SUMOReal off = STEPS2TIME(now - myLastEntryTime);
    return e->getLanes()[0]->getShape().positionAtLengthPosition(myCurrentBeginPos+myCurrentLength/myCurrentDuration*off);
}

        
void
MSPerson::MSPersonStage_Walking::proceed(MSNet* net,
        MSPerson* person, SUMOTime now,
        const MSEdge& /*previousEdge*/, const Position &pos) {
    myRouteStep = myRoute.begin();
    myLastEntryTime = now;
    ((MSEdge*) *myRouteStep)->addPerson(person);
    myRoute.size()==1
        ? computeWalkingTime(*myRouteStep, myDepartPos, myArrivalPos, myDestinationBusStop, 5.0/3.6)
        : computeWalkingTime(*myRouteStep, myDepartPos, -1, 0, 5.0/3.6);
    net->getBeginOfTimestepEvents().addEvent(new MoveToNextEdge(person, *this), now + TIME2STEPS(myCurrentDuration), MSEventControl::ADAPT_AFTER_EXECUTION);
    //net->getPersonControl().setArrival(MAX2(now, now + myWalkingTime), person);
}


void
MSPerson::MSPersonStage_Walking::computeWalkingTime(const MSEdge * const e, SUMOReal fromPos, SUMOReal toPos, MSBusStop *bs, SUMOReal speed) {
    if(bs!=0) {
        toPos = bs->getEndLanePosition();
    } else if(toPos<0) {
        toPos = e->getLanes()[0]->getLength();
    }
    if(fromPos<0) {
        fromPos = 0;
    }
    myCurrentBeginPos = fromPos;
    myCurrentLength = toPos-fromPos;
    myCurrentDuration = myCurrentLength / speed;
}

void
MSPerson::MSPersonStage_Walking::tripInfoOutput(OutputDevice& os) const {
    (os.openTag("walk") <<
     " arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Walking::beginEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"departure" <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << myRoute.front()->getID() <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Walking::endEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"arrival" <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << myRoute.back()->getID() <<
     "\"").closeTag(true);
}


SUMOTime 
MSPerson::MSPersonStage_Walking::moveToNextEdge(MSPerson *person, SUMOTime currentTime) {
    ((MSEdge*) *myRouteStep)->removePerson(person);
    if(myRouteStep==myRoute.end()-1) {
        if(myDestinationBusStop!=0) {
            myDestinationBusStop->addPerson(person);
        }
        person->proceed(MSNet::getInstance(), currentTime);//net->getPersonControl().setArrival(MAX2(now, now + myWalkingTime), person);
        return 0;
    } else {
        ++myRouteStep;
        myRouteStep==myRoute.end()-1
            ? computeWalkingTime(*myRouteStep, -1, myArrivalPos, myDestinationBusStop, 5.0/3.6)
            : computeWalkingTime(*myRouteStep, -1, -1, 0, 5.0/3.6);
        ((MSEdge*) *myRouteStep)->addPerson(person);
        myLastEntryTime = currentTime;
        return TIME2STEPS(myCurrentDuration);
    }
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Driving::MSPersonStage_Driving(const MSEdge& destination,
        MSBusStop *toBS, const std::vector<std::string> &lines)
    : MSPersonStage(destination, DRIVING), myLines(lines.begin(), lines.end()),
    myVehicle(0), myDestinationBusStop(toBS) {}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


const MSEdge *
MSPerson::MSPersonStage_Driving::getEdge(SUMOTime now) const {
    return myVehicle->getEdge();
}


Position 
MSPerson::MSPersonStage_Driving::getPosition(SUMOTime now) const {
    if(myVehicle!=0) {
        return myVehicle->getEdge()->getLanes()[0]->getShape().positionAtLengthPosition(myVehicle->getPositionOnLane());
    }
    return myWaitingPos;
}

        
void
MSPerson::MSPersonStage_Driving::proceed(MSNet* net,
        MSPerson* person, SUMOTime now,
        const MSEdge& previousEdge, const Position &pos) {
    myWaitingPos = pos;
    myVehicle = net->getVehicleControl().getWaitingVehicle(&previousEdge, myLines);
    if (myVehicle != 0 && myVehicle->getParameter().departProcedure == DEPART_TRIGGERED) {
        myVehicle->addPerson(person);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(&previousEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaitingForPerson();
    } else {
        net->getPersonControl().addWaiting(&previousEdge, person);
    }
}


bool
MSPerson::MSPersonStage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}


void
MSPerson::MSPersonStage_Driving::tripInfoOutput(OutputDevice& os) const {
    (os.openTag("ride") <<
     " depart=\"" << time2string(myDeparted) <<
     "\" arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Driving::beginEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"arrival" <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << getEdge(t)->getID() <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Driving::endEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"arrival" <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << getEdge(t)->getID() <<
     "\"").closeTag(true);
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, const std::string &actType)
    : MSPersonStage(destination, WAITING), myWaitingDuration(duration), myWaitingUntil(until),
    myActType(actType) {}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting() {}


const MSEdge *
MSPerson::MSPersonStage_Waiting::getEdge(SUMOTime now) const {
    return &myDestination;
}


Position 
MSPerson::MSPersonStage_Waiting::getPosition(SUMOTime now) const {
    return myDestination.getLanes()[0]->getShape().positionAtLengthPosition(myDestination.getLanes()[0]->getLength()/2.);
}


void
MSPerson::MSPersonStage_Waiting::proceed(MSNet* net,
        MSPerson* person, SUMOTime now, const MSEdge& /*previousEdge*/, const Position &pos) {
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    net->getPersonControl().setArrival(until, person);
}


void
MSPerson::MSPersonStage_Waiting::tripInfoOutput(OutputDevice& os) const {
    (os.openTag("stop") <<
     " arrival=\"" << time2string(myArrived) <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Waiting::beginEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"actstart " << myActType <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << getEdge(t)->getID() <<
     "\"").closeTag(true);
}


void 
MSPerson::MSPersonStage_Waiting::endEventOutput(const MSPerson &p, SUMOTime t, OutputDevice& os) const {
    (os.openTag("event") <<
     " time=\"" << time2string(t) <<
     "\" type=\"actend " << myActType <<
     "\" agent=\"" << p.getID() <<
     "\" link=\"" << getEdge(t)->getID() <<
     "\"").closeTag(true);
}

/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, MSPersonPlan* plan)
    : myParameter(pars), myPlan(plan) {
    myStep = myPlan->begin();
}


MSPerson::~MSPerson() {
    for (MSPersonPlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        delete *i;
    }
    delete myPlan;
    delete myParameter;
}


const std::string&
MSPerson::getID() const {
    return myParameter->id;
}


void
MSPerson::proceed(MSNet* net, SUMOTime time) {
    const MSEdge& arrivedAt = (*myStep)->getDestination();
    (*myStep)->setArrived(time);
    /*
    if(myWriteEvents) {
        (*myStep)->endEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
    }
    */
    Position pos = (*myStep)->getPosition(time);
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, arrivedAt, pos);
        /*
        if(myWriteEvents) {
            (*myStep)->beginEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
        }
        */
    } else {
        net->getPersonControl().erase(this);
    }
}


SUMOTime
MSPerson::getDesiredDepart() const {
    return myParameter->depart;
}


void
MSPerson::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}


void
MSPerson::tripInfoOutput(OutputDevice& os) const {
    for (MSPersonPlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os);
    }
}


bool
MSPerson::isWaitingFor(const std::string& line) const {
    return (*myStep)->isWaitingFor(line);
}


/****************************************************************************/

