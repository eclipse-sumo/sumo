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
MSStopOut::stopStarted(const SUMOVehicle* veh, int numPersons, int numContainers, SUMOTime time) {
    assert(veh != nullptr);
    if (myStopped.count(veh) != 0) {
        WRITE_WARNINGF(TL("Vehicle '%' stops on edge '%', time=% without ending the previous stop."),
                       veh->getID(), veh->getEdge()->getID(), time2string(time));
    }
    myStopped.emplace(veh, StopInfo(numPersons, numContainers));
}

void
MSStopOut::loadedPersons(const SUMOVehicle* veh, int n) {
    // ignore triggered vehicles
    if (veh->hasDeparted()) {
        myStopped.find(veh)->second.loadedPersons += n;
    }
}

void
MSStopOut::unloadedPersons(const SUMOVehicle* veh, int n) {
    myStopped.find(veh)->second.unloadedPersons += n;
}

void
MSStopOut::loadedContainers(const SUMOVehicle* veh, int n) {
    // ignore triggered vehicles
    if (veh->hasDeparted()) {
        myStopped.find(veh)->second.loadedContainers += n;
    }
}

void
MSStopOut::unloadedContainers(const SUMOVehicle* veh, int n) {
    myStopped.find(veh)->second.unloadedContainers += n;
}

void
MSStopOut::stopEnded(const SUMOVehicle* veh, const SUMOVehicleParameter::Stop& stop, const std::string& laneOrEdgeID, bool simEnd) {
    assert(veh != nullptr);
    if (myStopped.count(veh) == 0) {
        WRITE_WARNINGF(TL("Vehicle '%' ends stop on edge '%', time=% without entering the stop."),
                       veh->getID(), veh->getEdge()->getID(), time2string(SIMSTEP));
        return;
    }
    const StopInfo& si = myStopped.find(veh)->second;
    double delay = -1;
    double arrivalDelay = -1;
    if (stop.until >= 0 && !simEnd) {
        delay = STEPS2TIME(SIMSTEP - stop.until);
    }
    if (stop.arrival >= 0) {
        arrivalDelay = STEPS2TIME(stop.started - stop.arrival);
    }
    myDevice.openTag("stopinfo");
    myDevice.writeAttr(SUMO_ATTR_ID, veh->getID());
    myDevice.writeAttr(SUMO_ATTR_TYPE, veh->getVehicleType().getID());
    if (MSGlobals::gUseMesoSim) {
        myDevice.writeAttr(SUMO_ATTR_EDGE, laneOrEdgeID);
    } else {
        myDevice.writeAttr(SUMO_ATTR_LANE, laneOrEdgeID);
    }
    myDevice.writeAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane());
    myDevice.writeAttr(SUMO_ATTR_PARKING, stop.parking);
    myDevice.writeAttr("started", time2string(stop.started));
    myDevice.writeAttr("ended", simEnd ? "-1" : time2string(SIMSTEP));
    if (stop.until >= 0) {
        myDevice.writeAttr("delay", delay);
    }
    if (stop.arrival >= 0) {
        myDevice.writeAttr("arrivalDelay", arrivalDelay);
    }
    myDevice.writeAttr("initialPersons", si.initialNumPersons);
    myDevice.writeAttr("loadedPersons", si.loadedPersons);
    myDevice.writeAttr("unloadedPersons", si.unloadedPersons);
    myDevice.writeAttr("initialContainers", si.initialNumContainers);
    myDevice.writeAttr("loadedContainers", si.loadedContainers);
    myDevice.writeAttr("unloadedContainers", si.unloadedContainers);
    if (stop.busstop != "") {
        myDevice.writeAttr(SUMO_ATTR_BUS_STOP, stop.busstop);
    }
    if (stop.containerstop != "") {
        myDevice.writeAttr(SUMO_ATTR_CONTAINER_STOP, stop.containerstop);
    }
    if (stop.parkingarea != "") {
        myDevice.writeAttr(SUMO_ATTR_PARKING_AREA, stop.parkingarea);
    }
    if (stop.chargingStation != "") {
        myDevice.writeAttr(SUMO_ATTR_CHARGING_STATION, stop.chargingStation);
    }
    if (stop.overheadWireSegment != "") {
        myDevice.writeAttr(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, stop.overheadWireSegment);
    }
    if (stop.tripId != "") {
        myDevice.writeAttr(SUMO_ATTR_TRIP_ID, stop.tripId);
    }
    if (stop.line != "") {
        myDevice.writeAttr(SUMO_ATTR_LINE, stop.line);
    }
    if (stop.split != "") {
        myDevice.writeAttr(SUMO_ATTR_SPLIT, stop.split);
    }
    if (MSGlobals::gUseStopEnded) {
        myDevice.writeAttr(SUMO_ATTR_USED_ENDED, stop.ended >= 0);
    }
    myDevice.closeTag();
    myStopped.erase(veh);
}

void
MSStopOut::generateOutputForUnfinished() {
    while (!myStopped.empty()) {
        const auto& item = *myStopped.begin();
        const SUMOVehicle* veh = item.first;
        const SUMOVehicleParameter::Stop* stop = veh->getNextStopParameter();
        assert(stop != nullptr);
        const std::string laneOrEdgeID = MSGlobals::gUseMesoSim ? veh->getEdge()->getID() : Named::getIDSecure(veh->getLane());
        // erases item from myStopped
        stopEnded(veh, *stop, laneOrEdgeID, true);
    }
}

/****************************************************************************/
