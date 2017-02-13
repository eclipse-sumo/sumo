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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include "MSPerson.h"
#include <microsim/MSTransportableControl.h>
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
        MSStoppingPlace* toStop,
        SUMOTime walkingTime, SUMOReal speed,
        SUMOReal departPos, SUMOReal arrivalPos, SUMOReal departPosLat) :
    MSTransportable::Stage(*route.back(), toStop, SUMOVehicleParameter::interpretEdgePos(
                               arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS, "person walking to " + route.back()->getID()), MOVING_WITHOUT_VEHICLE), myWalkingTime(walkingTime), myRoute(route),
    myCurrentInternalEdge(0),
    myDepartPos(departPos),
    myDepartPosLat(departPosLat),
    mySpeed(speed),
    myPedestrianState(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                      myDepartPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS, "person walking from " + myRoute.front()->getID());
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


ConstMSEdgeVector
MSPerson::MSPersonStage_Walking::getEdges() const {
    return myRoute;
}

void
MSPerson::MSPersonStage_Walking::proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous) {
    previous->getEdge()->removePerson(person);
    myRouteStep = myRoute.begin();
    if (myWalkingTime == 0) {
        if (!person->proceed(net, now)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        }
        return;
    }
    if (previous->getEdgePos(now) >= 0) {
        myDepartPos = previous->getEdgePos(now);
        if (myWalkingTime > 0) {
            mySpeed = computeAverageSpeed();
        }
    }
    myPedestrianState = MSPModel::getModel()->add(dynamic_cast<MSPerson*>(person), this, now);
    (*myRouteStep)->addPerson(person);
}

void
MSPerson::MSPersonStage_Walking::abort(MSTransportable*) {
    MSPModel::getModel()->remove(myPedestrianState);
    myPedestrianState = 0;
}

void
MSPerson::MSPersonStage_Walking::setSpeed(SUMOReal speed) {
    mySpeed = speed;
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
        if (myDestinationStop != 0) {
            myDestinationStop->addTransportable(person);
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
        MSStoppingPlace* toStop, const SUMOReal arrivalPos, const std::vector<std::string>& lines)
    : MSTransportable::Stage_Driving(destination, toStop, arrivalPos, lines) {}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


void
MSPerson::MSPersonStage_Driving::proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous) {
    if (previous->getDestinationStop() != 0) {
        // the arrival stop may have an access point
        myWaitingEdge = &previous->getDestinationStop()->getLane().getEdge();
        myStopWaitPos = previous->getDestinationStop()->getWaitPosition();
    } else {
        myWaitingEdge = previous->getEdge();
        myStopWaitPos = Position::INVALID;
    }
    myWaitingPos = previous->getEdgePos(now);
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(myWaitingEdge, myLines, myWaitingPos, person->getID());
    if (availableVehicle != 0 && availableVehicle->getParameter().departProcedure == DEPART_TRIGGERED && !availableVehicle->hasDeparted()) {
        myVehicle = availableVehicle;
        myWaitingEdge->removePerson(person);
        myVehicle->addPerson(person);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(myWaitingEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaitingForPerson();
    } else {
        net->getPersonControl().addWaiting(myWaitingEdge, person);
        myWaitingEdge->addPerson(person);
    }
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
    MSTransportable::Stage* prior = *myStep;
    prior->setArrived(time);
    /*
    if(myWriteEvents) {
        (*myStep)->endEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
    }
    */
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, prior);
        /*
        if(myWriteEvents) {
            (*myStep)->beginEventOutput(*this, time, OutputDevice::getDeviceByOption("person-event-output"));
        }
        */
        return true;
    } else {
        prior->getEdge()->removePerson(this);
        return false;
    }
}


const std::string&
MSPerson::getNextEdge() const {
//    if (getCurrentStageType() == MOVING_WITHOUT_VEHICLE) {
//        MSPersonStage_Walking* walkingStage =  dynamic_cast<MSPersonStage_Walking*>(*myStep);
//        assert(walkingStage != 0);
//        const MSEdge* nextEdge = walkingStage->getPedestrianState()->getNextEdge(*walkingStage);
//        if (nextEdge != 0) {
//            return nextEdge->getID();
//        }
//    }
//    return StringUtils::emptyString;
    const MSEdge* nextEdge = getNextEdgePtr();
    if (nextEdge != 0) {
        return nextEdge->getID();
    }
    return StringUtils::emptyString;
}


const MSEdge*
MSPerson::getNextEdgePtr() const {
    if (getCurrentStageType() == MOVING_WITHOUT_VEHICLE) {
        MSPersonStage_Walking* walkingStage =  dynamic_cast<MSPersonStage_Walking*>(*myStep);
        assert(walkingStage != 0);
        return walkingStage->getPedestrianState()->getNextEdge(*walkingStage);

    }
    return 0;
}


void
MSPerson::tripInfoOutput(OutputDevice& os) const {
    os.openTag("personinfo").writeAttr("id", getID()).writeAttr("depart", time2string(getDesiredDepart()));
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os);
    }
    os.closeTag();
}


void
MSPerson::routeOutput(OutputDevice& os) const {
    os.openTag(SUMO_TAG_PERSON).writeAttr(SUMO_ATTR_ID, getID()).writeAttr(SUMO_ATTR_DEPART, time2string(getDesiredDepart()));
    if (myStep == myPlan->end()) {
        os.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
    os.closeTag();
    os.lf();
}

/****************************************************************************/

