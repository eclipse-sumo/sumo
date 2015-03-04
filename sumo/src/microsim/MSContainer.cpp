/****************************************************************************/
/// @file    MSContainer.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Thu, 12 Jun 2014
/// @version $$
///
// The class for modelling container-movements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSNet.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSContainer.h"
#include "microsim/trigger/MSContainerStop.h"
#include "MSContainerControl.h"
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include "MSCModel_NonInteracting.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

/* -------------------------------------------------------------------------
 * static member definitions
 * ----------------------------------------------------------------------- */

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage::MSContainerStage(const MSEdge& destination, StageType type)
    : myDestination(destination), myDeparted(-1), myArrived(-1), myType(type) {}

MSContainer::MSContainerStage::~MSContainerStage() {}

const MSEdge&
MSContainer::MSContainerStage::getDestination() const {
    return myDestination;
}

void
MSContainer::MSContainerStage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}

void
MSContainer::MSContainerStage::setArrived(SUMOTime now) {
    myArrived = now;
}

bool
MSContainer::MSContainerStage::isWaitingFor(const std::string& /*line*/) const {
    return false;
}

Position
MSContainer::MSContainerStage::getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const {
    return getLanePosition(e->getLanes()[0], at, offset);
}

Position
MSContainer::MSContainerStage::getLanePosition(const MSLane* lane, SUMOReal at, SUMOReal offset) const {
    return lane->getShape().positionAtOffset(lane->interpolateLanePosToGeometryPos(at), offset);
}

SUMOReal
MSContainer::MSContainerStage::getEdgeAngle(const MSEdge* e, SUMOReal at) const {
    PositionVector shp = e->getLanes()[0]->getShape();
    return -shp.rotationDegreeAtOffset(at);
}



/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Driving::MSContainerStage_Driving(const MSEdge& destination,
        MSContainerStop* toCS, const std::vector<std::string>& lines)
    : MSContainerStage(destination, DRIVING), myLines(lines.begin(), lines.end()),
      myVehicle(0), myDestinationContainerStop(toCS) {}


MSContainer::MSContainerStage_Driving::~MSContainerStage_Driving() {}

void
MSContainer::MSContainerStage_Driving::proceed(MSNet* net, MSContainer* container, SUMOTime now,
        MSEdge* previousEdge, const SUMOReal at) {
    myWaitingEdge = previousEdge;
    myWaitingPos = at;
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(previousEdge, myLines, myWaitingPos, container->getID());
	if (availableVehicle != 0 && availableVehicle->getParameter().departProcedure == DEPART_CONTAINER_TRIGGERED) {
        myVehicle = availableVehicle;
        previousEdge->removeContainer(container);
        myVehicle->addContainer(container);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(previousEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaitingForContainer();
    } else {
        net->getContainerControl().addWaiting(previousEdge, container);
        previousEdge->addContainer(container);
    }
}

const MSEdge*
MSContainer::MSContainerStage_Driving::getEdge() const {
    if (myVehicle != 0) {
        return myVehicle->getEdge();
    }
    return myWaitingEdge;
}


const MSEdge*
MSContainer::MSContainerStage_Driving::getFromEdge() const {
    return myWaitingEdge;
}


SUMOReal
MSContainer::MSContainerStage_Driving::getEdgePos(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        // vehicle may already have passed the lane (check whether this is correct)
        return MIN2(myVehicle->getPositionOnLane(), getEdge()->getLength());
    }
    return myWaitingPos;
}

Position
MSContainer::MSContainerStage_Driving::getPosition(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        /// @bug this fails while vehicle is driving across a junction
        return myVehicle->getEdge()->getLanes()[0]->getShape().positionAtOffset(myVehicle->getPositionOnLane());
    }
	//TODO: make class MSCModel
    return getEdgePosition(myWaitingEdge, myWaitingPos, 0.0);
}

SUMOReal
MSContainer::MSContainerStage_Driving::getAngle(SUMOTime /* now */) const {
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

bool
MSContainer::MSContainerStage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}

bool
MSContainer::MSContainerStage_Driving::isWaiting4Vehicle() const {
    return myVehicle == 0;
}

SUMOTime
MSContainer::MSContainerStage_Driving::getWaitingTime(SUMOTime now) const {
    return isWaiting4Vehicle() ? now - myWaitingSince : 0;
}

SUMOReal
MSContainer::MSContainerStage_Driving::getSpeed() const {
    return myVehicle == 0 ? 0 : myVehicle->getSpeed();
}

std::string
MSContainer::MSContainerStage_Driving::getStageDescription() const {
    return isWaiting4Vehicle() ? "waiting for " + joinToString(myLines, ",") : "transport";
}

MSContainerStop*
MSContainer::MSContainerStage_Driving::getDestinationContainerStop() const {
    return myDestinationContainerStop;
}

MSContainerStop*
MSContainer::MSContainerStage_Driving::getDepartContainerStop() const {
    return myDepartContainerStop;
}

void
MSContainer::MSContainerStage_Driving::tripInfoOutput(OutputDevice& os) const {
    os.openTag("transport").writeAttr("depart", time2string(myDeparted)).writeAttr("arrival", time2string(myArrived)).closeTag();
}

void
MSContainer::MSContainerStage_Driving::routeOutput(OutputDevice& os) const {
    os.openTag("transport").writeAttr(SUMO_ATTR_FROM, getFromEdge()->getID()).writeAttr(SUMO_ATTR_TO, getDestination().getID());
    os.writeAttr(SUMO_ATTR_LINES, myLines).closeTag();
}

void
MSContainer::MSContainerStage_Driving::beginEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}

void
MSContainer::MSContainerStage_Driving::endEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}



/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Waiting::MSContainerStage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType) :
    MSContainerStage(destination, WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myActType(actType),
    myStartPos(pos) {
    myStartPos = SUMOVehicleParameter::interpretEdgePos(
                     myStartPos, myDestination.getLength(), SUMO_ATTR_DEPARTPOS, "container stopping at " + myDestination.getID());
}
    
MSContainer::MSContainerStage_Waiting::~MSContainerStage_Waiting() {}

const MSEdge*
MSContainer::MSContainerStage_Waiting::getEdge() const {
    return &myDestination;
}

const MSEdge*
MSContainer::MSContainerStage_Waiting::getFromEdge() const {
    return &myDestination;
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getEdgePos(SUMOTime /* now */) const {
    return myStartPos;
}

SUMOTime
MSContainer::MSContainerStage_Waiting::getUntil() const {
    return myWaitingUntil;
}

Position
MSContainer::MSContainerStage_Waiting::getPosition(SUMOTime /* now */) const {
    return getEdgePosition(&myDestination, myStartPos, 0.0);
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(&myDestination, myStartPos) - 90;
}

SUMOTime
MSContainer::MSContainerStage_Waiting::getWaitingTime(SUMOTime now) const {
    return now - myWaitingStart;
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getSpeed() const {
    return 0;
}

MSContainerStop*
MSContainer::MSContainerStage_Waiting::getDestinationContainerStop() const{
    return myCurrentContainerStop;
}

MSContainerStop*
MSContainer::MSContainerStage_Waiting::getDepartContainerStop() const{
    return myCurrentContainerStop;
}

void
MSContainer::MSContainerStage_Waiting::proceed(MSNet* net, MSContainer* container, SUMOTime now,
        MSEdge* previousEdge, const SUMOReal /* at */) {
    previousEdge->addContainer(container);
    myWaitingStart = now;
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    net->getContainerControl().setWaitEnd(until, container);
}

void
MSContainer::MSContainerStage_Waiting::tripInfoOutput(OutputDevice& os) const {
    os.openTag("stop").writeAttr("arrival", time2string(myArrived)).closeTag();
}

void
MSContainer::MSContainerStage_Waiting::routeOutput(OutputDevice& os) const {
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
MSContainer::MSContainerStage_Waiting::beginEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actstart " + myActType)
    .writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}

void
MSContainer::MSContainerStage_Waiting::endEventOutput(const MSContainer& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actend " + myActType).writeAttr("agent", container.getID())
    .writeAttr("link", getEdge()->getID()).closeTag();
}

/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Tranship - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Tranship::MSContainerStage_Tranship(const std::vector<const MSEdge*>& route,
        MSContainerStop* toCS,
        SUMOReal speed,
        SUMOReal departPos, SUMOReal arrivalPos) :
    MSContainerStage(*route.back(), TRANSHIP), myRoute(route),
    myCurrentInternalEdge(0),
    myDepartPos(departPos), myArrivalPos(arrivalPos), myDestinationContainerStop(toCS),
    mySpeed(speed),
    myContainerState(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                      myDepartPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS, "container getting transhipped from " + myRoute.front()->getID());
    myArrivalPos = SUMOVehicleParameter::interpretEdgePos(
                       myArrivalPos, myRoute.back()->getLength(), SUMO_ATTR_ARRIVALPOS, "container getting transhipped to " + myRoute.back()->getID());
}

MSContainer::MSContainerStage_Tranship::~MSContainerStage_Tranship() {
}

void
MSContainer::MSContainerStage_Tranship::proceed(MSNet* net, MSContainer* container, SUMOTime now, MSEdge* previousEdge, const SUMOReal at) {
    previousEdge->removeContainer(container);
    myRouteStep = myRoute.end() - 1;   //define that the container is already on its destination edge
    MSNet::getInstance()->getContainerControl().setTranship(container);
    if (at >= 0) {
        myDepartPos = at;
    }
    myContainerState = MSCModel_NonInteracting::getModel()->add(container, this, now);
    ((MSEdge*) *myRouteStep)->addContainer(container);
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getEdge() const {
    if (myCurrentInternalEdge != 0) {
        return myCurrentInternalEdge;
    } else {
        return *myRouteStep;
    }
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getFromEdge() const {
    return myRoute.front();
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getToEdge() const {
    return myRoute.back();
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getEdgePos(SUMOTime now) const {
    return myContainerState->getEdgePos(*this, now);
}

Position
MSContainer::MSContainerStage_Tranship::getPosition(SUMOTime now) const {
    return myContainerState->getPosition(*this, now);
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getAngle(SUMOTime now) const {
    return myContainerState->getAngle(*this, now);
}

SUMOTime
MSContainer::MSContainerStage_Tranship::getWaitingTime(SUMOTime now) const {
    return 0;
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getSpeed() const {
    return myContainerState->getSpeed(*this);
}

MSContainerStop*
MSContainer::MSContainerStage_Tranship::getDestinationContainerStop() const {
    return myDestinationContainerStop;
}

MSContainerStop*
MSContainer::MSContainerStage_Tranship::getDepartContainerStop() const {
    return myDepartContainerStop;
}

void
MSContainer::MSContainerStage_Tranship::tripInfoOutput(OutputDevice& os) const {
    os.openTag("tranship").writeAttr("arrival", time2string(myArrived)).closeTag();
}


void
MSContainer::MSContainerStage_Tranship::routeOutput(OutputDevice& os) const {
    os.openTag("tranship").writeAttr(SUMO_ATTR_EDGES, myRoute);
    os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    os.closeTag();
}


void
MSContainer::MSContainerStage_Tranship::beginEventOutput(const MSContainer& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "departure")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.front()->getID()).closeTag();
}


void
MSContainer::MSContainerStage_Tranship::endEventOutput(const MSContainer& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.back()->getID()).closeTag();
}

bool
MSContainer::MSContainerStage_Tranship::moveToNextEdge(MSContainer* container, SUMOTime currentTime, MSEdge* nextInternal) {
    ((MSEdge*)getEdge())->removeContainer(container);
    if (myRouteStep == myRoute.end() - 1) {
        MSNet::getInstance()->getContainerControl().unsetTranship(container);
        if (myDestinationContainerStop != 0) {
            myDestinationContainerStop->addContainer(container);    //jakob
        }
        if (!container->proceed(MSNet::getInstance(), currentTime)) {
            MSNet::getInstance()->getContainerControl().erase(container);
        }
        return true;
    } else {
        if (nextInternal == 0) {
            ++myRouteStep;
            myCurrentInternalEdge = 0;
        } else {
            myCurrentInternalEdge = nextInternal;
        }
        ((MSEdge*) getEdge())->addContainer(container);
        return false;
    }
}

/* -------------------------------------------------------------------------
 * MSContainer - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSContainerPlan* plan)
    : myParameter(pars), myVType(vtype), myPlan(plan) {
    myStep = myPlan->begin();
    lastDestination = &(myPlan->back())->getDestination();
}

MSContainer::~MSContainer() {
    for (MSContainerPlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        delete *i;
    }
    delete myPlan;
    delete myParameter;
}

const std::string&
MSContainer::getID() const {
    return myParameter->id;
}

bool
MSContainer::proceed(MSNet* net, SUMOTime time) {
    MSEdge* arrivedAt = (MSEdge*)(*myStep)->getEdge();
    SUMOReal atPos = (*myStep)->getEdgePos(time);
    (*myStep)->setArrived(time);
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, arrivedAt, atPos);
        return true;
    } else {
        arrivedAt->removeContainer(this);
        return false;
    }
}

SUMOTime
MSContainer::getDesiredDepart() const {
    return myParameter->depart;
}

void
MSContainer::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}

SUMOReal 
MSContainer::getEdgePos() const {
    return (*myStep)->getEdgePos(MSNet::getInstance()->getCurrentTimeStep());
}

Position 
MSContainer::getPosition() const {
    return (*myStep)->getPosition(MSNet::getInstance()->getCurrentTimeStep());
}

SUMOReal
MSContainer::getAngle() const {
    return (*myStep)->getAngle(MSNet::getInstance()->getCurrentTimeStep());
}

SUMOReal 
MSContainer::getWaitingSeconds() const {
    return STEPS2TIME((*myStep)->getWaitingTime(MSNet::getInstance()->getCurrentTimeStep()));
}

SUMOReal 
MSContainer::getSpeed() const {
    return (*myStep)->getSpeed();
}

MSContainerStop*
MSContainer::getDestinationContainerStop() const {
    return (*myStep)->getDestinationContainerStop();
}

MSContainerStop*
MSContainer::getDepartContainerStop() const {
    return (*myStep)->getDepartContainerStop();
}

void
MSContainer::tripInfoOutput(OutputDevice& os) const {
    for (MSContainerPlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os);
    }
}

void
MSContainer::routeOutput(OutputDevice& os) const {
    MSContainerPlan::const_iterator i = myPlan->begin();
    if ((*i)->getStageType() == WAITING && getDesiredDepart() == static_cast<MSContainerStage_Waiting*>(*i)->getUntil()) {
        ++i;
    }
    for (; i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
}


/****************************************************************************/
