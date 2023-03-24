/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStageWaiting.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// An stage for planned waiting (stopping)
/****************************************************************************/
#include <config.h>

#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSStageWaiting.h>

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
    if (myType == MSStageType::WAITING_FOR_DEPART) {
        tc.forceDeparture();
    }
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
        MSNet* net = MSNet::getInstance();
        if (transportable->isPerson()) {
            net->getPersonControl().setWaitEnd(until, transportable);
        } else {
            net->getContainerControl().setWaitEnd(until, transportable);
        }
    }
}

