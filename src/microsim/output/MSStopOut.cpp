/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSStopOut.cpp
/// @author  Jakob Erdmann
/// @date    Wed, 21.12.2016
///
// Ouput information about planned vehicle stop
/****************************************************************************/
#include <config.h>

#include <utils/vehicle/SUMOVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSStop.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSParkingArea.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSVehicleType.h>
#include <microsim/trigger/MSChargingStation.h>
#include <microsim/trigger/MSOverheadWire.h>
#include "MSStopOut.h"


// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
MSStopOut* MSStopOut::myInstance = nullptr;

void
MSStopOut::init() {
    if (OptionsCont::getOptions().isSet("stop-output")) {
        myInstance = new MSStopOut(OutputDevice::getDeviceByOption("stop-output"));
    }
}

void
MSStopOut::cleanup() {
    delete myInstance;
    myInstance = nullptr;
}

// ===========================================================================
// method definitions
// ===========================================================================
MSStopOut::MSStopOut(OutputDevice& dev) :
    myDevice(dev) {
}

MSStopOut::~MSStopOut() {}


void
MSStopOut::stopBlocked(const SUMOVehicle* veh, SUMOTime time) {
    assert(veh != nullptr);
    if (myStopped.count(veh) == 0) {
        myStopped.emplace(veh, StopInfo(-time, -1, -1));
    }
}


void
MSStopOut::stopNotStarted(const SUMOVehicle* veh) {
    assert(veh != nullptr);
    myStopped.erase(veh);
}


void
MSStopOut::stopStarted(const SUMOVehicle* veh, int numPersons, int numContainers, SUMOTime time) {
    assert(veh != nullptr);
    if (myStopped.count(veh) == 0) {
        myStopped.emplace(veh, StopInfo(0, numPersons, numContainers));
    } else {
        MSStopOut::StopInfo& info = myStopped.find(veh)->second;
        info.blockTime += time;
        info.initialNumPersons = numPersons;
        info.initialNumContainers = numContainers;
    }
}


void
MSStopOut::loadedPersons(const SUMOVehicle* veh, int n) {
    // ignore triggered vehicles
    if (veh->hasDeparted()) {
        if (myStopped.count(veh) == 0) {
            WRITE_WARNINGF(TL("Vehicle '%' loads persons on edge '%', time=% without starting the stop."),
                           veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
        } else {
            myStopped.find(veh)->second.loadedPersons += n;
        }
    }
}


void
MSStopOut::unloadedPersons(const SUMOVehicle* veh, int n) {
    if (myStopped.count(veh) == 0) {
        WRITE_WARNINGF(TL("Vehicle '%' unloads persons on edge '%', time=% without starting the stop."),
                       veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
    } else {
        myStopped.find(veh)->second.unloadedPersons += n;
    }
}


void
MSStopOut::loadedContainers(const SUMOVehicle* veh, int n) {
    // ignore triggered vehicles
    if (veh->hasDeparted()) {
        if (myStopped.count(veh) == 0) {
            WRITE_WARNINGF(TL("Vehicle '%' loads container on edge '%', time=% without starting the stop."),
                           veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
        } else {
            myStopped.find(veh)->second.loadedContainers += n;
        }
    }
}


void
MSStopOut::unloadedContainers(const SUMOVehicle* veh, int n) {
    if (myStopped.count(veh) == 0) {
        WRITE_WARNINGF(TL("Vehicle '%' unloads container on edge '%', time=% without starting the stop."),
                       veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
    } else {
        myStopped.find(veh)->second.unloadedContainers += n;
    }
}


void
MSStopOut::stopEnded(const SUMOVehicle* veh, const MSStop& stop, bool simEnd) {
    assert(veh != nullptr);
    if (myStopped.count(veh) == 0) {
        WRITE_WARNINGF(TL("Vehicle '%' ends stop on edge '%', time=% without entering the stop."),
                       veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
        return;
    }
    const SUMOVehicleParameter::Stop& pars = stop.pars;
    const StopInfo& si = myStopped.find(veh)->second;
    double delay = -1;
    double arrivalDelay = -1;
    if (pars.until >= 0 && !simEnd) {
        delay = STEPS2TIME(SIMSTEP - pars.until);
    }
    if (pars.arrival >= 0) {
        arrivalDelay = STEPS2TIME(pars.started - pars.arrival);
    }
    myDevice.openTag("stopinfo");
    myDevice.writeAttr(SUMO_ATTR_ID, veh->getID());
    myDevice.writeAttr(SUMO_ATTR_TYPE, veh->getVehicleType().getID());
    if (MSGlobals::gUseMesoSim) {
        myDevice.writeAttr(SUMO_ATTR_EDGE, veh->getEdge()->getID());
    } else {
        myDevice.writeAttr(SUMO_ATTR_LANE, stop.lane->getID());
    }
    myDevice.writeAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane());
    myDevice.writeAttr(SUMO_ATTR_PARKING, pars.parking);
    myDevice.writeAttr(SUMO_ATTR_STARTED, time2string(pars.started));
    myDevice.writeAttr(SUMO_ATTR_ENDED, simEnd ? "-1" : time2string(SIMSTEP));
    if (pars.until >= 0) {
        myDevice.writeAttr("delay", delay);
    }
    if (pars.arrival >= 0) {
        myDevice.writeAttr(SUMO_ATTR_ARRIVALDELAY, arrivalDelay);
    }
    if (pars.busstop != "") {
        myDevice.writeAttr(SUMO_ATTR_BUS_STOP, pars.busstop);
    }
    if (pars.containerstop != "") {
        myDevice.writeAttr(SUMO_ATTR_CONTAINER_STOP, pars.containerstop);
    }
    if (pars.parkingarea != "") {
        myDevice.writeAttr(SUMO_ATTR_PARKING_AREA, pars.parkingarea);
    }
    if (pars.chargingStation != "") {
        myDevice.writeAttr(SUMO_ATTR_CHARGING_STATION, pars.chargingStation);
    }
    if (pars.overheadWireSegment != "") {
        myDevice.writeAttr(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, pars.overheadWireSegment);
    }
    if (pars.tripId != "") {
        myDevice.writeAttr(SUMO_ATTR_TRIP_ID, pars.tripId);
    }
    if (pars.line != "") {
        myDevice.writeAttr(SUMO_ATTR_LINE, pars.line);
    }
    if (pars.split != "") {
        myDevice.writeAttr(SUMO_ATTR_SPLIT, pars.split);
    }
    if (MSGlobals::gUseStopEnded) {
        myDevice.writeAttr(SUMO_ATTR_USED_ENDED, pars.ended >= 0);
    }
    myDevice.writeAttr("initialPersons", si.initialNumPersons);
    myDevice.writeAttr("loadedPersons", si.loadedPersons);
    myDevice.writeAttr("unloadedPersons", si.unloadedPersons);
    myDevice.writeAttr("initialContainers", si.initialNumContainers);
    myDevice.writeAttr("loadedContainers", si.loadedContainers);
    myDevice.writeAttr("unloadedContainers", si.unloadedContainers);
    myDevice.writeAttr("blockedDuration", time2string(si.blockTime));

    if (stop.pars.speed > 0) {
        if (stop.waypointWithStop) {
            myDevice.writeAttr(SUMO_ATTR_STATE, "waypointStopped");
        } else {
            myDevice.writeAttr(SUMO_ATTR_STATE, "waypoint");
        }
    } else if (stop.skipOnDemand) {
        myDevice.writeAttr(SUMO_ATTR_STATE, "skippedOnDemand");
    }

    myDevice.closeTag();
    myStopped.erase(veh);
}


void
MSStopOut::generateOutputForUnfinished() {
    while (!myStopped.empty()) {
        const auto& item = *myStopped.begin();
        const SUMOVehicle* veh = item.first;
        assert(veh->isStopped());
        const MSStop& stop = veh->getNextStop();
        // erases item from myStopped
        stopEnded(veh, stop, true);
    }
}


/****************************************************************************/
