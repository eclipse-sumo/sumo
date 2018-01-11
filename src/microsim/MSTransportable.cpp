/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportable.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSEdge.h"
#include "MSLane.h"
#include "MSNet.h"
#include <microsim/pedestrians/MSPerson.h>
#include "MSVehicleControl.h"
#include "MSTransportableControl.h"
#include "MSTransportable.h"

/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
const double MSTransportable::ROADSIDE_OFFSET(3);


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTransportable::Stage - methods
 * ----------------------------------------------------------------------- */
MSTransportable::Stage::Stage(const MSEdge& destination, MSStoppingPlace* toStop, const double arrivalPos, StageType type)
    : myDestination(destination), myDestinationStop(toStop), myArrivalPos(arrivalPos), myDeparted(-1), myArrived(-1), myType(type) {}

MSTransportable::Stage::~Stage() {}

const MSEdge&
MSTransportable::Stage::getDestination() const {
    return myDestination;
}


void
MSTransportable::Stage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}

void
MSTransportable::Stage::setArrived(SUMOTime now) {
    myArrived = now;
}

bool
MSTransportable::Stage::isWaitingFor(const std::string& /*line*/) const {
    return false;
}

Position
MSTransportable::Stage::getEdgePosition(const MSEdge* e, double at, double offset) const {
    return getLanePosition(e->getLanes()[0], at, offset);
}

Position
MSTransportable::Stage::getLanePosition(const MSLane* lane, double at, double offset) const {
    return lane->getShape().positionAtOffset(lane->interpolateLanePosToGeometryPos(at), offset);
}

double
MSTransportable::Stage::getEdgeAngle(const MSEdge* e, double at) const {
    return e->getLanes()[0]->getShape().rotationAtOffset(at);
}


/* -------------------------------------------------------------------------
* MSTransportable::Stage_Waiting - methods
* ----------------------------------------------------------------------- */
MSTransportable::Stage_Waiting::Stage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, double pos, const std::string& actType,
        const bool initial) :
    MSTransportable::Stage(destination, 0, SUMOVehicleParameter::interpretEdgePos(
                               pos, destination.getLength(), SUMO_ATTR_DEPARTPOS, "stopping at " + destination.getID()),
                           initial ? WAITING_FOR_DEPART : WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myActType(actType) {
}


MSTransportable::Stage_Waiting::~Stage_Waiting() {}


const MSEdge*
MSTransportable::Stage_Waiting::getEdge() const {
    return &myDestination;
}


const MSEdge*
MSTransportable::Stage_Waiting::getFromEdge() const {
    return &myDestination;
}


double
MSTransportable::Stage_Waiting::getEdgePos(SUMOTime /* now */) const {
    return myArrivalPos;
}


SUMOTime
MSTransportable::Stage_Waiting::getUntil() const {
    return myWaitingUntil;
}


Position
MSTransportable::Stage_Waiting::getPosition(SUMOTime /* now */) const {
    return getEdgePosition(&myDestination, myArrivalPos,
                           ROADSIDE_OFFSET * (MSNet::getInstance()->lefthand() ? -1 : 1));
}


double
MSTransportable::Stage_Waiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(&myDestination, myArrivalPos) + M_PI / 2 * (MSNet::getInstance()->lefthand() ? -1 : 1);
}


void
MSTransportable::Stage_Waiting::proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous) {
    myDeparted = now;
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    if (dynamic_cast<MSPerson*>(transportable) != 0) {
        previous->getEdge()->addPerson(transportable);
        net->getPersonControl().setWaitEnd(until, transportable);
    } else {
        previous->getEdge()->addContainer(transportable);
        net->getContainerControl().setWaitEnd(until, transportable);
    }
}


void
MSTransportable::Stage_Waiting::tripInfoOutput(OutputDevice& os, MSTransportable*) const {
    if (myType != WAITING_FOR_DEPART) {
        os.openTag("stop");
        os.writeAttr("duration", time2string(myArrived - myDeparted));
        os.writeAttr("arrival", time2string(myArrived));
        os.writeAttr("arrivalPos", toString(myArrivalPos));
        os.writeAttr("actType", toString(myActType));
        os.closeTag();
    }
}


void
MSTransportable::Stage_Waiting::routeOutput(OutputDevice& os) const {
    if (myType != WAITING_FOR_DEPART) {
        os.openTag("stop").writeAttr(SUMO_ATTR_LANE, getDestination().getID());
        if (myWaitingDuration >= 0) {
            os.writeAttr(SUMO_ATTR_DURATION, time2string(myWaitingDuration));
        }
        if (myWaitingUntil >= 0) {
            os.writeAttr(SUMO_ATTR_UNTIL, time2string(myWaitingUntil));
        }
        os.closeTag();
    }
}


void
MSTransportable::Stage_Waiting::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actstart " + myActType)
    .writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSTransportable::Stage_Waiting::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actend " + myActType).writeAttr("agent", p.getID())
    .writeAttr("link", getEdge()->getID()).closeTag();
}


SUMOTime
MSTransportable::Stage_Waiting::getWaitingTime(SUMOTime now) const {
    return now - myDeparted;
}


double
MSTransportable::Stage_Waiting::getSpeed() const {
    return 0;
}


ConstMSEdgeVector
MSTransportable::Stage_Waiting::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(&getDestination());
    return result;
}

void
MSTransportable::Stage_Waiting::abort(MSTransportable* t) {
    MSTransportableControl& tc = (dynamic_cast<MSPerson*>(t) != 0 ?
                                  MSNet::getInstance()->getPersonControl() :
                                  MSNet::getInstance()->getContainerControl());
    tc.abortWaiting(t);
}


/* -------------------------------------------------------------------------
* MSTransportable::Stage_Driving - methods
* ----------------------------------------------------------------------- */
MSTransportable::Stage_Driving::Stage_Driving(const MSEdge& destination,
        MSStoppingPlace* toStop, const double arrivalPos, const std::vector<std::string>& lines) :
    MSTransportable::Stage(destination, toStop, arrivalPos, DRIVING),
    myLines(lines.begin(), lines.end()),
    myVehicle(0),
    myVehicleID("NULL"),
    myVehicleDistance(-1.),
    myStopWaitPos(Position::INVALID) {}


MSTransportable::Stage_Driving::~Stage_Driving() {}

const MSEdge*
MSTransportable::Stage_Driving::getEdge() const {
    if (myVehicle != 0) {
        return &myVehicle->getLane()->getEdge();
    }
    return myWaitingEdge;
}


const MSEdge*
MSTransportable::Stage_Driving::getFromEdge() const {
    return myWaitingEdge;
}


double
MSTransportable::Stage_Driving::getEdgePos(SUMOTime /* now */) const {
    if (isWaiting4Vehicle()) {
        return myWaitingPos;
    }
    // vehicle may already have passed the lane (check whether this is correct)
    return MIN2(myVehicle->getPositionOnLane(), getEdge()->getLength());
}


Position
MSTransportable::Stage_Driving::getPosition(SUMOTime /* now */) const {
    if (isWaiting4Vehicle()) {
        if (myStopWaitPos != Position::INVALID) {
            return myStopWaitPos;
        }
        return getEdgePosition(myWaitingEdge, myWaitingPos,
                               ROADSIDE_OFFSET * (MSNet::getInstance()->lefthand() ? -1 : 1));
    }
    return myVehicle->getPosition();
}


double
MSTransportable::Stage_Driving::getAngle(SUMOTime /* now */) const {
    if (!isWaiting4Vehicle()) {
        MSVehicle* veh = dynamic_cast<MSVehicle*>(myVehicle);
        if (veh != 0) {
            return veh->getAngle();
        } else {
            return 0;
        }
    }
    return getEdgeAngle(myWaitingEdge, myWaitingPos) + M_PI / 2. * (MSNet::getInstance()->lefthand() ? -1 : 1);
}


bool
MSTransportable::Stage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}


bool
MSTransportable::Stage_Driving::isWaiting4Vehicle() const {
    return myVehicle == 0;
}


SUMOTime
MSTransportable::Stage_Driving::getWaitingTime(SUMOTime now) const {
    return isWaiting4Vehicle() ? now - myDeparted : 0;
}


double
MSTransportable::Stage_Driving::getSpeed() const {
    return isWaiting4Vehicle() ? 0 : myVehicle->getSpeed();
}


ConstMSEdgeVector
MSTransportable::Stage_Driving::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(getFromEdge());
    result.push_back(&getDestination());
    return result;
}

void
MSTransportable::Stage_Driving::setArrived(SUMOTime now) {
    MSTransportable::Stage::setArrived(now);
    if (myVehicle != 0) {
        // distance was previously set to driven distance upon embarking
        myVehicleDistance = myVehicle->getRoute().getDistanceBetween(
                                myVehicle->getDepartPos(), myVehicle->getPositionOnLane(),
                                myVehicle->getRoute().begin(),  myVehicle->getCurrentRouteEdge()) - myVehicleDistance;
    } else {
        myVehicleDistance = -1.;
    }
}

void
MSTransportable::Stage_Driving::setVehicle(SUMOVehicle* v) {
    myVehicle = v;
    myVehicleID = v->getID();
    myVehicleLine = v->getParameter().line;
    myVehicleVClass = v->getVClass();
    myVehicleDistance = myVehicle->getRoute().getDistanceBetween(
                            myVehicle->getDepartPos(), myVehicle->getPositionOnLane(),
                            myVehicle->getRoute().begin(),  myVehicle->getCurrentRouteEdge());
}


void
MSTransportable::Stage_Driving::abort(MSTransportable* t) {
    if (myVehicle != 0) {
        // jumping out of a moving vehicle!
        dynamic_cast<MSVehicle*>(myVehicle)->removeTransportable(t);
    }
}


std::string
MSTransportable::Stage_Driving::getWaitingDescription() const {
    return isWaiting4Vehicle() ? ("waiting for " + joinToString(myLines, ",")
                                  + " at " + (myDestinationStop == 0
                                          ? ("edge '" + myWaitingEdge->getID() + "'")
                                          : ("busStop '" + myDestinationStop->getID() + "'"))
                                 ) : "";
}


void
MSTransportable::Stage_Driving::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSTransportable::Stage_Driving::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}



/* -------------------------------------------------------------------------
 * MSTransportable - methods
 * ----------------------------------------------------------------------- */
MSTransportable::MSTransportable(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportablePlan* plan)
    : myParameter(pars), myVType(vtype), myPlan(plan) {
    myStep = myPlan->begin();
}

MSTransportable::~MSTransportable() {
    if (myPlan != 0) {
        for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
            delete *i;
        }
        delete myPlan;
        myPlan = 0;
    }
    delete myParameter;
    if (myVType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myVType);
    }
}

const std::string&
MSTransportable::getID() const {
    return myParameter->id;
}

SUMOTime
MSTransportable::getDesiredDepart() const {
    return myParameter->depart;
}

void
MSTransportable::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}

double
MSTransportable::getEdgePos() const {
    return (*myStep)->getEdgePos(MSNet::getInstance()->getCurrentTimeStep());
}

Position
MSTransportable::getPosition() const {
    return (*myStep)->getPosition(MSNet::getInstance()->getCurrentTimeStep());
}

double
MSTransportable::getAngle() const {
    return (*myStep)->getAngle(MSNet::getInstance()->getCurrentTimeStep());
}

double
MSTransportable::getWaitingSeconds() const {
    return STEPS2TIME((*myStep)->getWaitingTime(MSNet::getInstance()->getCurrentTimeStep()));
}

double
MSTransportable::getSpeed() const {
    return (*myStep)->getSpeed();
}


int
MSTransportable::getNumRemainingStages() const {
    return (int)(myPlan->end() - myStep);
}

int
MSTransportable::getNumStages() const {
    return (int)myPlan->size();
}

void
MSTransportable::appendStage(Stage* stage, int next) {
    // myStep is invalidated upon modifying myPlan
    const int stepIndex = (int)(myStep - myPlan->begin());
    if (next < 0) {
        myPlan->push_back(stage);
    } else {
        if (stepIndex + next > (int)myPlan->size()) {
            throw ProcessError("invalid index '" + toString(next) + "' for inserting new stage into plan of '" + getID() + "'");
        }
        myPlan->insert(myPlan->begin() + stepIndex + next, stage);
    }
    myStep = myPlan->begin() + stepIndex;
}


void
MSTransportable::removeStage(int next) {
    assert(myStep + next < myPlan->end());
    assert(next >= 0);
    if (next > 0) {
        // myStep is invalidated upon modifying myPlan
        int stepIndex = (int)(myStep - myPlan->begin());
        delete *(myStep + next);
        myPlan->erase(myStep + next);
        myStep = myPlan->begin() + stepIndex;
    } else {
        if (myStep + 1 == myPlan->end()) {
            // stay in the simulation until the start of simStep to allow appending new stages (at the correct position)
            appendStage(new Stage_Waiting(*getEdge(), 0, 0, getEdgePos(), "last stage removed", false));
        }
        (*myStep)->abort(this);
        proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
    }
}


void
MSTransportable::setSpeed(double speed) {
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->setSpeed(speed);
    }
}


void
MSTransportable::replaceVehicleType(MSVehicleType* type) {
    if (myVType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myVType);
    }
    myVType = type;
}


MSVehicleType&
MSTransportable::getSingularType() {
    if (myVType->isVehicleSpecific()) {
        return *myVType;
    }
    MSVehicleType* type = myVType->buildSingularType(myVType->getID() + "@" + getID());
    replaceVehicleType(type);
    return *type;
}


PositionVector
MSTransportable::getBoundingBox() const {
    PositionVector centerLine;
    const Position p = getPosition();
    const double angle = getAngle();
    const double length = getVehicleType().getLength();
    const Position back = p + Position(-cos(angle) * length, -sin(angle) * length);
    centerLine.push_back(p);
    centerLine.push_back(back);
    centerLine.move2side(0.5 * getVehicleType().getWidth());
    PositionVector result = centerLine;
    centerLine.move2side(-getVehicleType().getWidth());
    result.append(centerLine.reverse(), POSITION_EPS);
    //std::cout << " transp=" << getID() << " p=" << p << " angle=" << GeomHelper::naviDegree(angle) << " back=" << back << " result=" << result << "\n";
    return result;
}

/****************************************************************************/
