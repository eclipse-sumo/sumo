/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSStage.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#include <config.h>

#include <utils/vehicle/SUMOVehicleParameter.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/devices/MSTransportableDevice.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPModel.h>


/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
const double MSStage::ROADSIDE_OFFSET(3);


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSStage - methods
 * ----------------------------------------------------------------------- */
MSStage::MSStage(const MSEdge* destination, MSStoppingPlace* toStop, const double arrivalPos, MSStageType type, const std::string& group) :
    myDestination(destination),
    myDestinationStop(toStop),
    myArrivalPos(arrivalPos),
    myDeparted(-1),
    myArrived(-1),
    myType(type),
    myGroup(group),
    myParametersSet(0)
{}

MSStage::~MSStage() {}

const MSEdge*
MSStage::getDestination() const {
    return myDestination;
}


const MSEdge*
MSStage::getEdge() const {
    return myDestination;
}


const MSEdge*
MSStage::getFromEdge() const {
    return myDestination;
}


double
MSStage::getEdgePos(SUMOTime /* now */) const {
    return myArrivalPos;
}

int
MSStage::getDirection() const {
    return MSPModel::UNDEFINED_DIRECTION;
}


SUMOTime
MSStage::getWaitingTime(SUMOTime /* now */) const {
    return 0;
}


double
MSStage::getSpeed() const {
    return 0.;
}


ConstMSEdgeVector
MSStage::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(getDestination());
    return result;
}


void
MSStage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}

SUMOTime
MSStage::getDeparted() const {
    return myDeparted;
}

SUMOTime
MSStage::getArrived() const {
    return myArrived;
}

const std::string
MSStage::setArrived(MSNet* /* net */, MSTransportable* /* transportable */, SUMOTime now, const bool /* vehicleArrived */) {
    myArrived = now;
    return "";
}

bool
MSStage::isWaitingFor(const SUMOVehicle* /*vehicle*/) const {
    return false;
}

Position
MSStage::getEdgePosition(const MSEdge* e, double at, double offset) const {
    return getLanePosition(e->getLanes()[0], at, offset);
}

Position
MSStage::getLanePosition(const MSLane* lane, double at, double offset) const {
    return lane->getShape().positionAtOffset(lane->interpolateLanePosToGeometryPos(at), offset);
}


double
MSStage::getEdgeAngle(const MSEdge* e, double at) const {
    return e->getLanes()[0]->getShape().rotationAtOffset(at);
}


void
MSStage::setDestination(const MSEdge* newDestination, MSStoppingPlace* newDestStop) {
    myDestination = newDestination;
    myDestinationStop = newDestStop;
    if (newDestStop != nullptr) {
        myArrivalPos = (newDestStop->getBeginLanePosition() + newDestStop->getEndLanePosition()) / 2;
    }
}


/* -------------------------------------------------------------------------
* MSStageWaiting - methods
* ----------------------------------------------------------------------- */
MSStageWaiting::MSStageWaiting(const MSEdge* destination, MSStoppingPlace* toStop,
                               SUMOTime duration, SUMOTime until, double pos, const std::string& actType,
                               const bool initial) :
    MSStage(destination, toStop, SUMOVehicleParameter::interpretEdgePos(
                pos, destination->getLength(), SUMO_ATTR_DEPARTPOS, "stopping at " + destination->getID()),
            initial ? MSStageType::WAITING_FOR_DEPART : MSStageType::WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myStopWaitPos(Position::INVALID),
    myActType(actType),
    myStopEndTime(-1) {
}


MSStageWaiting::~MSStageWaiting() {}

MSStage*
MSStageWaiting::clone() const {
    return new MSStageWaiting(myDestination, myDestinationStop, myWaitingDuration, myWaitingUntil, myArrivalPos, myActType, myType == MSStageType::WAITING_FOR_DEPART);
}

SUMOTime
MSStageWaiting::getUntil() const {
    return myWaitingUntil;
}

SUMOTime
MSStageWaiting::getDuration() const {
    return myWaitingDuration;
}


Position
MSStageWaiting::getPosition(SUMOTime /* now */) const {
    if (myStopWaitPos != Position::INVALID) {
        return myStopWaitPos;
    }
    return getEdgePosition(myDestination, myArrivalPos,
                           ROADSIDE_OFFSET * (MSGlobals::gLefthand ? -1 : 1));
}


double
MSStageWaiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(myDestination, myArrivalPos) + M_PI / 2 * (MSGlobals::gLefthand ? -1 : 1);
}


void
MSStageWaiting::proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* previous) {
    myDeparted = now;
    myStopEndTime = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    if (myDestinationStop != nullptr) {
        myDestinationStop->addTransportable(transportable);
        myStopWaitPos = myDestinationStop->getWaitPosition(transportable);
    }

    previous->getEdge()->addTransportable(transportable);
    if (transportable->isPerson()) {
        net->getPersonControl().setWaitEnd(myStopEndTime, transportable);
    } else {
        net->getContainerControl().setWaitEnd(myStopEndTime, transportable);
    }
}


void
MSStageWaiting::tripInfoOutput(OutputDevice& os, const MSTransportable* const) const {
    if (myType != MSStageType::WAITING_FOR_DEPART) {
        os.openTag(SUMO_TAG_STOP);
        os.writeAttr("duration", time2string(myArrived - myDeparted));
        os.writeAttr("arrival", time2string(myArrived));
        os.writeAttr("arrivalPos", toString(myArrivalPos));
        os.writeAttr("actType", myActType == "" ? "waiting" : myActType);
        os.closeTag();
    }
}


void
MSStageWaiting::routeOutput(const bool /* isPerson */, OutputDevice& os, const bool, const MSStage* const /* previous */) const {
    if (myType != MSStageType::WAITING_FOR_DEPART) {
        os.openTag(SUMO_TAG_STOP);
        std::string comment = "";
        if (myDestinationStop != nullptr) {
            os.writeAttr(toString(myDestinationStop->getElement()), myDestinationStop->getID());
            if (myDestinationStop->getMyName() != "") {
                comment =  " <!-- " + StringUtils::escapeXML(myDestinationStop->getMyName(), true) + " -->";
            }
        } else {
            // lane index is arbitrary
            os.writeAttr(SUMO_ATTR_LANE, getDestination()->getID() + "_0");
            os.writeAttr(SUMO_ATTR_ENDPOS, getArrivalPos());
        }
        if (myWaitingDuration >= 0) {
            os.writeAttr(SUMO_ATTR_DURATION, time2string(myWaitingDuration));
        }
        if (myWaitingUntil >= 0) {
            os.writeAttr(SUMO_ATTR_UNTIL, time2string(myWaitingUntil));
        }
        if (OptionsCont::getOptions().getBool("vehroute-output.exit-times")) {
            os.writeAttr(SUMO_ATTR_STARTED, myDeparted >= 0 ? time2string(myDeparted) : "-1");
            os.writeAttr(SUMO_ATTR_ENDED, myArrived >= 0 ? time2string(myArrived) : "-1");
        }
        if (myActType != "") {
            os.writeAttr(SUMO_ATTR_ACTTYPE, myActType);
        }
        os.closeTag(comment);
    }
}


void
MSStageWaiting::abort(MSTransportable* t) {
    MSTransportableControl& tc = (t->isPerson() ?
                                  MSNet::getInstance()->getPersonControl() :
                                  MSNet::getInstance()->getContainerControl());
    tc.abortWaiting(t);
}

std::string
MSStageWaiting::getStageDescription(const bool isPerson) const {
    UNUSED_PARAMETER(isPerson);
    if (myActType != "") {
        return "waiting (" + myActType + ")";
    } else {
        return "waiting";
    }
}

std::string
MSStageWaiting::getStageSummary(const bool /* isPerson */) const {
    std::string timeInfo;
    if (myWaitingUntil >= 0) {
        timeInfo += " until " + time2string(myWaitingUntil);
    }
    if (myWaitingDuration >= 0) {
        timeInfo += " duration " + time2string(myWaitingDuration);
    }
    if (getDestinationStop() != nullptr) {
        std::string nameMsg = "";
        if (getDestinationStop()->getMyName() != "") {
            nameMsg = "(" + getDestinationStop()->getMyName() + ") ";
        }
        return "stopping at stop '" + getDestinationStop()->getID() + "' " + nameMsg + timeInfo + " (" + myActType + ")";
    }
    return "stopping at edge '" + getDestination()->getID() + "' " + timeInfo + " (" + myActType + ")";
}

void
MSStageWaiting::saveState(std::ostringstream& out) {
    out << " " << myDeparted;
}

void
MSStageWaiting::loadState(MSTransportable* transportable, std::istringstream& state) {
    state >> myDeparted;
    const SUMOTime until = MAX3(myDeparted, myDeparted + myWaitingDuration, myWaitingUntil);
    if (myDestinationStop != nullptr) {
        myDestinationStop->addTransportable(transportable);
        myStopWaitPos = myDestinationStop->getWaitPosition(transportable);
    }
    if (myDeparted >= 0) {
        myDestination->addTransportable(transportable);
    }
    MSNet* net = MSNet::getInstance();
    if (transportable->isPerson()) {
        net->getPersonControl().setWaitEnd(until, transportable);
    } else {
        net->getContainerControl().setWaitEnd(until, transportable);
    }
}

/****************************************************************************/
