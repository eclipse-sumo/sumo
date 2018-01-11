/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include "MSPModel.h"

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(const std::string& personID,
        const ConstMSEdgeVector& route,
        MSStoppingPlace* toStop,
        SUMOTime walkingTime, double speed,
        double departPos, double arrivalPos, double departPosLat) :
    MSTransportable::Stage(*route.back(), toStop,
                           SUMOVehicleParameter::interpretEdgePos(arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS,
                                   "person '" + personID + "' walking to " + route.back()->getID()),
                           MOVING_WITHOUT_VEHICLE),
    myWalkingTime(walkingTime),
    myRoute(route),
    myCurrentInternalEdge(0),
    myDepartPos(departPos),
    myDepartPosLat(departPosLat),
    mySpeed(speed),
    myPedestrianState(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(departPos, route.front()->getLength(), SUMO_ATTR_DEPARTPOS,
                  "person '" + personID + "' walking from " + route.front()->getID());
    if (walkingTime > 0) {
        mySpeed = computeAverageSpeed();
    }
}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking() {
    delete myPedestrianState;
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


double
MSPerson::MSPersonStage_Walking::getEdgePos(SUMOTime now) const {
    return myPedestrianState == 0 ? -1 : myPedestrianState->getEdgePos(*this, now);
}


Position
MSPerson::MSPersonStage_Walking::getPosition(SUMOTime now) const {
    return myPedestrianState->getPosition(*this, now);
}


double
MSPerson::MSPersonStage_Walking::getAngle(SUMOTime now) const {
    return myPedestrianState->getAngle(*this, now);
}


SUMOTime
MSPerson::MSPersonStage_Walking::getWaitingTime(SUMOTime now) const {
    return myPedestrianState->getWaitingTime(*this, now);
}


double
MSPerson::MSPersonStage_Walking::getSpeed() const {
    return myPedestrianState->getSpeed(*this);
}


ConstMSEdgeVector
MSPerson::MSPersonStage_Walking::getEdges() const {
    return myRoute;
}

void
MSPerson::MSPersonStage_Walking::proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous) {
    myDeparted = now;
    myRouteStep = myRoute.begin();
    if (myWalkingTime == 0) {
        if (!person->proceed(net, now)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        }
        return;
    }
    if (previous->getEdgePos(now) >= 0 && previous->getEdge() == *myRouteStep) {
        myDepartPos = previous->getEdgePos(now);
        if (myWalkingTime > 0) {
            mySpeed = computeAverageSpeed();
        }
    }
    myPedestrianState = MSPModel::getModel()->add(dynamic_cast<MSPerson*>(person), this, now);
    if (myPedestrianState == 0) {
        MSNet::getInstance()->getPersonControl().erase(person);
        return;
    }
    (*myRouteStep)->addPerson(person);
}

void
MSPerson::MSPersonStage_Walking::abort(MSTransportable*) {
    MSPModel::getModel()->remove(myPedestrianState);
}

void
MSPerson::MSPersonStage_Walking::setSpeed(double speed) {
    mySpeed = speed;
}


double
MSPerson::MSPersonStage_Walking::computeAverageSpeed() const {
    return walkDistance() / STEPS2TIME(myWalkingTime + 1); // avoid systematic rounding errors
}


double
MSPerson::MSPersonStage_Walking::walkDistance() const {
    double length = 0;
    for (const MSEdge* edge : myRoute) {
        length += edge->getLength();
    }
    if (myRoute.size() > 1 && MSPModel::getModel()->usingInternalLanes()) {
        // use lower bound for distance to pass the intersection
        for (ConstMSEdgeVector::const_iterator i = myRoute.begin(); i != myRoute.end() - 1; ++i) {
            const MSEdge* fromEdge = *i;
            const MSEdge* toEdge = *(i + 1);
            const MSLane* from = getSidewalk<MSEdge, MSLane>(fromEdge);
            const MSLane* to = getSidewalk<MSEdge, MSLane>(toEdge);
            Position fromPos;
            Position toPos;
            if (from != 0 && to != 0) {
                if (fromEdge->getToJunction() == toEdge->getFromJunction()) {
                    fromPos = from->getShape().back();
                    toPos = to->getShape().front();
                } else if (fromEdge->getToJunction() == toEdge->getToJunction()) {
                    fromPos = from->getShape().back();
                    toPos = to->getShape().back();
                } else if (fromEdge->getFromJunction() == toEdge->getFromJunction()) {
                    fromPos = from->getShape().front();
                    toPos = to->getShape().front();
                } else if (fromEdge->getFromJunction() == toEdge->getToJunction()) {
                    fromPos = from->getShape().front();
                    toPos = to->getShape().back();
                }
                length += fromPos.distanceTo2D(toPos);
            }
        }
    }
    // determine walking direction for depart and arrival
    bool departFwd = true;
    bool arriveFwd = true;
    if (myRoute.front() == myRoute.back()) {
        if (myDepartPos > myArrivalPos) {
            departFwd = false;
            arriveFwd = false;
        }
    } else {
        // disconnected defaults to forward
        if ((myRoute.front()->getFromJunction() == myRoute[1]->getToJunction())
                || (myRoute.front()->getFromJunction() == myRoute[1]->getFromJunction())) {
            departFwd = false;
        }
        if ((myRoute.back()->getToJunction() == myRoute[myRoute.size() - 2]->getFromJunction())
                || (myRoute.back()->getToJunction() == myRoute[myRoute.size() - 2]->getToJunction())) {
            arriveFwd = false;
        }
    }
    length -= (departFwd ? myDepartPos : myRoute.front()->getLength() - myDepartPos);
    length -= (arriveFwd ? myRoute.back()->getLength() - myArrivalPos : myArrivalPos);
    return MAX2(POSITION_EPS, length);
}


void
MSPerson::MSPersonStage_Walking::tripInfoOutput(OutputDevice& os, MSTransportable* person) const {
    const double distance = walkDistance();
    const double maxSpeed = getMaxSpeed(dynamic_cast<MSPerson*>(person));
    const SUMOTime duration = myArrived - myDeparted;
    const SUMOTime timeLoss = duration - TIME2STEPS(distance / maxSpeed);
    MSDevice_Tripinfo::addPedestrianData(distance, duration, timeLoss);
    os.openTag("walk");
    os.writeAttr("depart", time2string(myDeparted));
    os.writeAttr("departPos", myDepartPos);
    os.writeAttr("arrival", time2string(myArrived));
    os.writeAttr("arrivalPos", myArrivalPos);
    os.writeAttr("duration", time2string(duration));
    os.writeAttr("routeLength", distance);
    os.writeAttr("timeLoss", time2string(timeLoss));
    os.writeAttr("maxSpeed", maxSpeed);
    os.closeTag();
}


void
MSPerson::MSPersonStage_Walking::routeOutput(OutputDevice& os) const {
    os.openTag("walk").writeAttr(SUMO_ATTR_EDGES, myRoute);
    if (myDestinationStop != 0) {
        os.writeAttr(SUMO_ATTR_BUS_STOP, myDestinationStop->getID());
    }
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


double
MSPerson::MSPersonStage_Walking::getMaxSpeed(const MSPerson* person) const {
    return mySpeed > 0 ? mySpeed : person->getVehicleType().getMaxSpeed() * person->getSpeedFactor();
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Driving::MSPersonStage_Driving(const MSEdge& destination,
        MSStoppingPlace* toStop, const double arrivalPos, const std::vector<std::string>& lines) :
    MSTransportable::Stage_Driving(destination, toStop,
                                   SUMOVehicleParameter::interpretEdgePos(
                                       arrivalPos, destination.getLength(), SUMO_ATTR_ARRIVALPOS, "person riding to " + destination.getID()),
                                   lines) {
}


MSPerson::MSPersonStage_Driving::~MSPersonStage_Driving() {}


void
MSPerson::MSPersonStage_Driving::proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous) {
    if (previous->getDestinationStop() != 0) {
        // the arrival stop may have an access point
        myWaitingEdge = &previous->getDestinationStop()->getLane().getEdge();
        myStopWaitPos = previous->getDestinationStop()->getWaitPosition();
        myWaitingPos = previous->getDestinationStop()->getWaitingPositionOnLane();
    } else {
        myWaitingEdge = previous->getEdge();
        myStopWaitPos = Position::INVALID;
        myWaitingPos = previous->getEdgePos(now);
    }
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(myWaitingEdge, myLines, myWaitingPos, person->getID());
    if (availableVehicle != 0 && availableVehicle->getParameter().departProcedure == DEPART_TRIGGERED && !availableVehicle->hasDeparted()) {
        setVehicle(availableVehicle);
        myVehicle->addPerson(person);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(myWaitingEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaiting(true);
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
MSPerson::MSPersonStage_Driving::tripInfoOutput(OutputDevice& os, MSTransportable*) const {
    const SUMOTime waitingTime = myDeparted - myWaitingSince;
    const SUMOTime duration = myArrived - myDeparted;
    MSDevice_Tripinfo::addRideData(myVehicleDistance, duration, myVehicleVClass, myVehicleLine, waitingTime);
    os.openTag("ride");
    os.writeAttr("waitingTime", time2string(waitingTime));
    os.writeAttr("vehicle", myVehicleID);
    os.writeAttr("depart", time2string(myDeparted));
    os.writeAttr("arrival", time2string(myArrived));
    os.writeAttr("arrivalPos", toString(myArrivalPos));
    os.writeAttr("duration", time2string(duration));
    os.writeAttr("routeLength", myVehicleDistance);
    os.closeTag();
}


void
MSPerson::MSPersonStage_Driving::routeOutput(OutputDevice& os) const {
    os.openTag("ride").writeAttr(SUMO_ATTR_FROM, getFromEdge()->getID()).writeAttr(SUMO_ATTR_TO, getDestination().getID());
    if (myDestinationStop != 0) {
        os.writeAttr(SUMO_ATTR_BUS_STOP, myDestinationStop->getID());
    }
    os.writeAttr(SUMO_ATTR_LINES, myLines).closeTag();
}



/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan, const double speedFactor) :
    MSTransportable(pars, vtype, plan),
    myInfluencer(0), myChosenSpeedFactor(speedFactor) {
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
    //if (getID() == "ego") {
    //    std::cout << time2string(time) << " person=" << getID() << " proceed priorStep=" << myStep - myPlan->begin() << " planSize=" << myPlan->size() << "\n";
    //}
    // must be done before increasing myStep to avoid invalid state for rendering
    prior->getEdge()->removePerson(this);
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
MSPerson::tripInfoOutput(OutputDevice& os, MSTransportable* transportable) const {
    os.openTag("personinfo").writeAttr("id", getID()).writeAttr("depart", time2string(getDesiredDepart()));
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os, transportable);
    }
    os.closeTag();
}


void
MSPerson::routeOutput(OutputDevice& os) const {
    const std::string typeID = getVehicleType().getID() != DEFAULT_PEDTYPE_ID ? getVehicleType().getID() : "";
    myParameter->write(os, OptionsCont::getOptions(), SUMO_TAG_PERSON, typeID);
    if (myStep == myPlan->end()) {
        os.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
    os.closeTag();
    os.lf();
}


void
MSPerson::reroute(ConstMSEdgeVector& newEdges) {
    double departPos = getEdgePos();
    double arrivalPos = getArrivalPos();
    double speed = getVehicleType().getMaxSpeed();
    //std::cout << " from=" << from->getID() << " to=" << to->getID() << " newEdges=" << toString(newEdges) << "\n";
    MSPerson::MSPersonStage_Walking* newStage = new MSPerson::MSPersonStage_Walking(getID(), newEdges, 0, -1, speed, departPos, arrivalPos, 0);
    if (getNumRemainingStages() == 1) {
        // Do not remove the last stage (a waiting stage would be added otherwise)
        appendStage(newStage);
        //std::cout << "case a: remaining=" << p->getNumRemainingStages() << "\n";
        removeStage(0);
    } else {
        removeStage(0);
        appendStage(newStage);
        //std::cout << "case b: remaining=" << p->getNumRemainingStages() << "\n";
    }
}

MSPerson::Influencer&
MSPerson::getInfluencer() {
    if (myInfluencer == 0) {
        myInfluencer = new Influencer();
    }
    return *myInfluencer;
}


const MSPerson::Influencer*
MSPerson::getInfluencer() const {
    return myInfluencer;
}



/* -------------------------------------------------------------------------
 * methods of MSPerson::Influencer
 * ----------------------------------------------------------------------- */
MSPerson::Influencer::Influencer() {}


MSPerson::Influencer::~Influencer() {}


void
MSPerson::Influencer::setRemoteControlled(Position xyPos, MSLane* l, double pos, double posLat, double angle, int edgeOffset, const ConstMSEdgeVector& route, SUMOTime t) {
    myRemoteXYPos = xyPos;
    myRemoteLane = l;
    myRemotePos = pos;
    myRemotePosLat = posLat;
    myRemoteAngle = angle;
    myRemoteEdgeOffset = edgeOffset;
    myRemoteRoute = route;
    myLastRemoteAccess = t;
}


bool
MSPerson::Influencer::isRemoteControlled() const {
    return myLastRemoteAccess == MSNet::getInstance()->getCurrentTimeStep();
}


bool
MSPerson::Influencer::isRemoteAffected(SUMOTime t) const {
    return myLastRemoteAccess >= t - TIME2STEPS(10);
}


void
MSPerson::Influencer::postProcessRemoteControl(MSPerson* p) {
    switch (p->getStageType(0)) {
        case MOVING_WITHOUT_VEHICLE: {
            MSPersonStage_Walking* s = dynamic_cast<MSPerson::MSPersonStage_Walking*>(p->getCurrentStage());
            assert(s != 0);
            s->getPedestrianState()->moveToXY(p, myRemoteXYPos, myRemoteLane, myRemotePos, myRemotePosLat, myRemoteAngle, myRemoteEdgeOffset, myRemoteRoute,
                                              MSNet::getInstance()->getCurrentTimeStep());
        }
        break;
        default:
            break;
    }
}


/****************************************************************************/
