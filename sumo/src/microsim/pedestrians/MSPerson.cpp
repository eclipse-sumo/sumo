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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include "MSPerson.h"
#include <microsim/MSPersonControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicle.h>
#include "MSPModel.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(const ConstMSEdgeVector& route,
        MSStoppingPlace* toBS,
        SUMOTime walkingTime, SUMOReal speed,
        SUMOReal departPos, SUMOReal arrivalPos) :
    MSTransportable::Stage(*route.back(), MOVING_WITHOUT_VEHICLE), myWalkingTime(walkingTime), myRoute(route),
    myCurrentInternalEdge(0),
    myDepartPos(departPos), myArrivalPos(arrivalPos), myDestinationBusStop(toBS),
    mySpeed(speed),
    myPedestrianState(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                      myDepartPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS, "person walking from " + myRoute.front()->getID());
    myArrivalPos = SUMOVehicleParameter::interpretEdgePos(
                       myArrivalPos, myRoute.back()->getLength(), SUMO_ATTR_ARRIVALPOS, "person walking to " + myRoute.back()->getID());
    if (walkingTime > 0) {
        mySpeed = computeAverageSpeed();
    }
}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking() {
}


const MSEdge*
MSPerson::MSPersonStage_Walking::getEdge() const {
    if (myCurrentInternalEdge != 0) {
        return myCurrentInternalEdge;
    } else {
        return *myRouteStep;
    }
}


const MSEdge*
MSPerson::MSPersonStage_Walking::getFromEdge() const {
    return myRoute.front();
}


SUMOReal
MSPerson::MSPersonStage_Walking::getEdgePos(SUMOTime now) const {
    return myPedestrianState->getEdgePos(*this, now);
}


Position
MSPerson::MSPersonStage_Walking::getPosition(SUMOTime now) const {
    return myPedestrianState->getPosition(*this, now);
}


SUMOReal
MSPerson::MSPersonStage_Walking::getAngle(SUMOTime now) const {
    return myPedestrianState->getAngle(*this, now);
}


SUMOTime
MSPerson::MSPersonStage_Walking::getWaitingTime(SUMOTime now) const {
    return myPedestrianState->getWaitingTime(*this, now);
}


SUMOReal
MSPerson::MSPersonStage_Walking::getSpeed() const {
    return myPedestrianState->getSpeed(*this);
}


void
MSPerson::MSPersonStage_Walking::proceed(MSNet* net, MSTransportable* person, SUMOTime now,
        MSEdge* previousEdge, const SUMOReal at) {
    previousEdge->removePerson(person);
    myRouteStep = myRoute.begin();
    if (myWalkingTime == 0) {
        if (!person->proceed(net, now)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        };
        return;
    }
    MSNet::getInstance()->getPersonControl().setWalking(person);
    if (at >= 0) {
        myDepartPos = at;
        if (myWalkingTime > 0) {
            mySpeed = computeAverageSpeed();
        }
    }
    myPedestrianState = MSPModel::getModel()->add(dynamic_cast<MSPerson*>(person), this, now);
    (*myRouteStep)->addPerson(person);
}


SUMOReal
MSPerson::MSPersonStage_Walking::computeAverageSpeed() const {
    SUMOReal length = 0;
    for (ConstMSEdgeVector::const_iterator i = myRoute.begin(); i != myRoute.end(); ++i) {
        length += (*i)->getLength();
    }
    length -= myDepartPos;
    length -= myRoute.back()->getLength() - myArrivalPos;
    return length / STEPS2TIME(myWalkingTime + 1); // avoid systematic rounding errors
}


void
MSPerson::MSPersonStage_Walking::tripInfoOutput(OutputDevice& os) const {
    os.openTag("walk").writeAttr("arrival", time2string(myArrived)).closeTag();
}


void
MSPerson::MSPersonStage_Walking::routeOutput(OutputDevice& os) const {
    os.openTag("walk").writeAttr(SUMO_ATTR_EDGES, myRoute);
    if (myWalkingTime > 0) {
        os.writeAttr(SUMO_ATTR_DURATION, time2string(myWalkingTime));
    } else if (mySpeed > 0) {
        os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    }
    os.closeTag();
}


void
MSPerson::MSPersonStage_Walking::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "departure")
    .writeAttr("agent", p.getID()).writeAttr("link", myRoute.front()->getID()).closeTag();
}


void
MSPerson::MSPersonStage_Walking::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival")
    .writeAttr("agent", p.getID()).writeAttr("link", myRoute.back()->getID()).closeTag();
}


bool
MSPerson::MSPersonStage_Walking::moveToNextEdge(MSPerson* person, SUMOTime currentTime, MSEdge* nextInternal) {
    ((MSEdge*)getEdge())->removePerson(person);
    //std::cout << SIMTIME << " moveToNextEdge person=" << person->getID() << "\n";
    if (myRouteStep == myRoute.end() - 1) {
        MSNet::getInstance()->getPersonControl().unsetWalking(person);
        if (myDestinationBusStop != 0) {
            myDestinationBusStop->addTransportable(person);
        }
        if (!person->proceed(MSNet::getInstance(), currentTime)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        }
        //std::cout << " end walk. myRouteStep=" << (*myRouteStep)->getID() << "\n";
        return true;
    } else {
        if (nextInternal == 0) {
            ++myRouteStep;
            myCurrentInternalEdge = 0;
        } else {
            myCurrentInternalEdge = nextInternal;
        }
        ((MSEdge*) getEdge())->addPerson(person);
        return false;
    }
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Driving::MSPersonStage_Driving(const MSEdge& destination,
        MSStoppingPlace* toBS, const std::vector<std::string>& lines)
    : MSTransportable::Stage(destination, DRIVING), myLines(lines.begin(), lines.end()),
      myVehicle(0), myDestinationBusStop(toBS) {}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


const MSEdge*
MSPerson::MSPersonStage_Driving::getEdge() const {
    if (myVehicle != 0) {
        return myVehicle->getEdge();
    }
    return myWaitingEdge;
}


const MSEdge*
MSPerson::MSPersonStage_Driving::getFromEdge() const {
    return myWaitingEdge;
}


SUMOReal
MSPerson::MSPersonStage_Driving::getEdgePos(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        // vehicle may already have passed the lane (check whether this is correct)
        return MIN2(myVehicle->getPositionOnLane(), getEdge()->getLength());
    }
    return myWaitingPos;
}


Position
MSPerson::MSPersonStage_Driving::getPosition(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        /// @bug this fails while vehicle is driving across a junction
        return myVehicle->getEdge()->getLanes()[0]->getShape().positionAtOffset(myVehicle->getPositionOnLane());
    }
    return getEdgePosition(myWaitingEdge, myWaitingPos, MSPModel::SIDEWALK_OFFSET);
}


SUMOReal
MSPerson::MSPersonStage_Driving::getAngle(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        MSVehicle* veh = dynamic_cast<MSVehicle*>(myVehicle);
        if (veh != 0) {
            return veh->getAngle();
        } else {
            return 0;
        }
    }
    return getEdgeAngle(myWaitingEdge, myWaitingPos) + 90;
}



void
MSPerson::MSPersonStage_Driving::proceed(MSNet* net, MSTransportable* person, SUMOTime now,
        MSEdge* previousEdge, const SUMOReal at) {
    myWaitingEdge = previousEdge;
    myWaitingPos = at;
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(previousEdge, myLines, myWaitingPos, person->getID());
    if (availableVehicle != 0 && availableVehicle->getParameter().departProcedure == DEPART_TRIGGERED) {
        myVehicle = availableVehicle;
        previousEdge->removePerson(person);
        myVehicle->addPerson(person);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(previousEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaitingForPerson();
    } else {
        net->getPersonControl().addWaiting(previousEdge, person);
        previousEdge->addPerson(person);
    }
}


bool
MSPerson::MSPersonStage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}


bool
MSPerson::MSPersonStage_Driving::isWaiting4Vehicle() const {
    return myVehicle == 0;
}


SUMOTime
MSPerson::MSPersonStage_Driving::getWaitingTime(SUMOTime now) const {
    return isWaiting4Vehicle() ? now - myWaitingSince : 0;
}


SUMOReal
MSPerson::MSPersonStage_Driving::getSpeed() const {
    return myVehicle == 0 ? 0 : myVehicle->getSpeed();
}


std::string
MSPerson::MSPersonStage_Driving::getStageDescription() const {
    return isWaiting4Vehicle() ? "waiting for " + joinToString(myLines, ",") : "driving";
}


void
MSPerson::MSPersonStage_Driving::tripInfoOutput(OutputDevice& os) const {
    os.openTag("ride").writeAttr("depart", time2string(myDeparted)).writeAttr("arrival", time2string(myArrived)).closeTag();
}


void
MSPerson::MSPersonStage_Driving::routeOutput(OutputDevice& os) const {
    os.openTag("ride").writeAttr(SUMO_ATTR_FROM, getFromEdge()->getID()).writeAttr(SUMO_ATTR_TO, getDestination().getID());
    os.writeAttr(SUMO_ATTR_LINES, myLines).closeTag();
}


void
MSPerson::MSPersonStage_Driving::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSPerson::MSPersonStage_Driving::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType) :
    MSTransportable::Stage(destination, WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myActType(actType),
    myStartPos(pos) {
    myStartPos = SUMOVehicleParameter::interpretEdgePos(
                     myStartPos, myDestination.getLength(), SUMO_ATTR_DEPARTPOS, "person stopping at " + myDestination.getID());
}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting() {}


const MSEdge*
MSPerson::MSPersonStage_Waiting::getEdge() const {
    return &myDestination;
}


const MSEdge*
MSPerson::MSPersonStage_Waiting::getFromEdge() const {
    return &myDestination;
}


SUMOReal
MSPerson::MSPersonStage_Waiting::getEdgePos(SUMOTime /* now */) const {
    return myStartPos;
}


SUMOTime
MSPerson::MSPersonStage_Waiting::getUntil() const {
    return myWaitingUntil;
}


Position
MSPerson::MSPersonStage_Waiting::getPosition(SUMOTime /* now */) const {
    return getEdgePosition(&myDestination, myStartPos, MSPModel::SIDEWALK_OFFSET);
}


SUMOReal
MSPerson::MSPersonStage_Waiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(&myDestination, myStartPos) - 90;
}


void
MSPerson::MSPersonStage_Waiting::proceed(MSNet* net, MSTransportable* person, SUMOTime now,
        MSEdge* previousEdge, const SUMOReal /* at */) {
    previousEdge->addPerson(person);
    myWaitingStart = now;
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    net->getPersonControl().setWaitEnd(until, person);
}


void
MSPerson::MSPersonStage_Waiting::tripInfoOutput(OutputDevice& os) const {
    os.openTag("stop").writeAttr("arrival", time2string(myArrived)).closeTag();
}


void
MSPerson::MSPersonStage_Waiting::routeOutput(OutputDevice& os) const {
    os.openTag("stop").writeAttr(SUMO_ATTR_LANE, getDestination().getID());
    if (myWaitingDuration >= 0) {
        os.writeAttr(SUMO_ATTR_DURATION, time2string(myWaitingDuration));
    }
    if (myWaitingUntil >= 0) {
        os.writeAttr(SUMO_ATTR_UNTIL, time2string(myWaitingUntil));
    }
    os.closeTag();
}


void
MSPerson::MSPersonStage_Waiting::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actstart " + myActType)
    .writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSPerson::MSPersonStage_Waiting::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actend " + myActType).writeAttr("agent", p.getID())
    .writeAttr("link", getEdge()->getID()).closeTag();
}


SUMOTime
MSPerson::MSPersonStage_Waiting::getWaitingTime(SUMOTime now) const {
    return now - myWaitingStart;
}


SUMOReal
MSPerson::MSPersonStage_Waiting::getSpeed() const {
    return 0;
}




/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan)
    : MSTransportable(pars, vtype, plan) {
}


MSPerson::~MSPerson() {
}


bool
MSPerson::proceed(MSNet* net, SUMOTime time) {
    MSEdge* arrivedAt = (MSEdge*)(*myStep)->getEdge();
    SUMOReal atPos = (*myStep)->getEdgePos(time);
    //MSPersonPlan::iterator prior = myStep;
    (*myStep)->setArrived(time);
    /*
    if(myWriteEvents) {
        (*myStep)->endEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
    }
    */
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, arrivedAt, atPos);
        /*
        if(myWriteEvents) {
            (*myStep)->beginEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
        }
        */
        return true;
    } else {
        arrivedAt->removePerson(this);
        return false;
    }
}


void
MSPerson::routeOutput(OutputDevice& os) const {
    MSTransportable::MSTransportablePlan::const_iterator i = myPlan->begin();
    if ((*i)->getStageType() == WAITING && getDesiredDepart() == static_cast<MSPersonStage_Waiting*>(*i)->getUntil()) {
        ++i;
    }
    for (; i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
}

const std::string&
MSPerson::getNextEdge() const {
    if (getCurrentStageType() == MOVING_WITHOUT_VEHICLE) {
        MSPersonStage_Walking* walkingStage =  dynamic_cast<MSPersonStage_Walking*>(*myStep);
        assert(walkingStage != 0);
        const MSEdge* nextEdge = walkingStage->getPedestrianState()->getNextEdge(*walkingStage);
        if (nextEdge != 0) {
            return nextEdge->getID();
        }
    }
    return StringUtils::emptyString;
}
/****************************************************************************/

