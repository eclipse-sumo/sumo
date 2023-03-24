/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStop.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
///
// A lane area vehicles can halt at
/****************************************************************************/
#include <config.h>

#include <mesosim/MESegment.h>
#include "MSLane.h"
#include "MSNet.h"
#include "MSParkingArea.h"
#include "MSStoppingPlace.h"
#include "MSStop.h"


// ===========================================================================
// method definitions
// ===========================================================================
double
MSStop::getEndPos(const SUMOVehicle& veh) const {
    const double brakePos = veh.getEdge() == getEdge() ? veh.getPositionOnLane() + veh.getBrakeGap() : 0;
    if ((pars.parametersSet & STOP_END_SET) != 0) {
        return pars.endPos;
    } else if (busstop != nullptr) {
        return busstop->getLastFreePos(veh, brakePos);
    } else if (containerstop != nullptr) {
        return containerstop->getLastFreePos(veh, brakePos);
    } else if (parkingarea != nullptr) {
        return parkingarea->getLastFreePos(veh, brakePos);
    } else if (chargingStation != nullptr) {
        return chargingStation->getLastFreePos(veh);
    } else if (overheadWireSegment != nullptr) {
        return overheadWireSegment->getLastFreePos(veh);
    }
    return pars.endPos;
}

const MSEdge*
MSStop::getEdge() const {
    if (lane != nullptr) {
        return &lane->getEdge();
    } else if (segment != nullptr) {
        return &segment->getEdge();
    }
    return nullptr;
}

double
MSStop::getReachedThreshold() const {
    return isOpposite ? lane->getOppositePos(pars.endPos) - (pars.endPos - pars.startPos) : pars.startPos;
}

std::string
MSStop::getDescription() const {
    std::string result;
    if (parkingarea != nullptr) {
        result = "parkingArea:" + parkingarea->getID();
    } else if (containerstop != nullptr) {
        result = "containerStop:" + containerstop->getID();
    } else if (busstop != nullptr) {
        result = "busStop:" + busstop->getID();
    } else if (chargingStation != nullptr) {
        result = "chargingStation:" + chargingStation->getID();
    } else if (overheadWireSegment != nullptr) {
        result = "overheadWireSegment:" + overheadWireSegment->getID();
    } else {
        result = "lane:" + lane->getID() + " pos:" + toString(pars.endPos);
    }
    if (pars.actType != "") {
        result += " actType:" + pars.actType;
    }
    return result;
}


void
MSStop::write(OutputDevice& dev) const {
    SUMOVehicleParameter::Stop tmp = pars;
    tmp.duration = duration;
    if (busstop == nullptr
            && containerstop == nullptr
            && parkingarea == nullptr
            && chargingStation == nullptr) {
        tmp.parametersSet |= STOP_START_SET | STOP_END_SET;
    }
    tmp.write(dev, false);
    // if the stop has already started but hasn't ended yet we are writing it in
    // the context of saveState (but we do not want to write the attribute twice
    if (pars.started >= 0 && (pars.parametersSet & STOP_STARTED_SET) == 0) {
        dev.writeAttr(SUMO_ATTR_STARTED, time2string(pars.started));
    }
    dev.closeTag();
}

void
MSStop::initPars(const SUMOVehicleParameter::Stop& stopPar) {
    busstop = MSNet::getInstance()->getStoppingPlace(stopPar.busstop, SUMO_TAG_BUS_STOP);
    containerstop = MSNet::getInstance()->getStoppingPlace(stopPar.containerstop, SUMO_TAG_CONTAINER_STOP);
    parkingarea = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(stopPar.parkingarea, SUMO_TAG_PARKING_AREA));
    chargingStation = MSNet::getInstance()->getStoppingPlace(stopPar.chargingStation, SUMO_TAG_CHARGING_STATION);
    overheadWireSegment = MSNet::getInstance()->getStoppingPlace(stopPar.overheadWireSegment, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
    duration = stopPar.duration;
    triggered = stopPar.triggered;
    containerTriggered = stopPar.containerTriggered;
    joinTriggered = stopPar.joinTriggered || stopPar.join != "";
    numExpectedPerson = (int)stopPar.awaitedPersons.size();
    numExpectedContainer = (int)stopPar.awaitedContainers.size();
}


int
MSStop::getStateFlagsOld() const {
    return ((reached ? 1 : 0) + 2 * pars.getFlags());
}


SUMOTime
MSStop::getMinDuration(SUMOTime time) const {
    if (MSGlobals::gUseStopEnded && pars.ended >= 0) {
        return pars.ended - time;
    }
    if (pars.until >= 0) {
        if (duration == -1) {
            return pars.until - time;
        } else {
            return MAX2(duration, pars.until - time);
        }
    } else {
        return duration;
    }
}


SUMOTime
MSStop::getUntil() const {
    return MSGlobals::gUseStopEnded && pars.ended >= 0 ? pars.ended : pars.until;
}


double
MSStop::getSpeed() const {
    return skipOnDemand ? std::numeric_limits<double>::max() : pars.speed;
}


bool
MSStop::isInRange(const double pos, const double tolerance) const {
    return pars.startPos - tolerance <= pos && pars.endPos + tolerance >= pos;
}

/****************************************************************************/
