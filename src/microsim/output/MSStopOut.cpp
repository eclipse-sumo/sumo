/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSStopOut.cpp
/// @author  Jakob Erdmann
/// @date    Wed, 21.12.2016
/// @version $Id$
///
// Ouput information about planned vehicle stop
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/vehicle/SUMOVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSParkingArea.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/trigger/MSChargingStation.h>
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
    assert(veh != 0);
    if (myStopped.count(veh) != 0) {
        WRITE_WARNING("Vehicle '" + veh->getID() + "' stops on edge '" + veh->getEdge()->getID()
                      + "', time " + time2string(time)
                      + " without ending the previous stop entered at time " + time2string(myStopped[veh].started));
    }
    StopInfo stopInfo(MSNet::getInstance()->getCurrentTimeStep(), numPersons, numContainers);
    myStopped[veh] = stopInfo;
}

void
MSStopOut::loadedPersons(const SUMOVehicle* veh, int n) {
    // ignore triggered vehicles
    if (veh->hasDeparted()) {
        myStopped[veh].loadedPersons += n;
    }
}

void
MSStopOut::unloadedPersons(const SUMOVehicle* veh, int n) {
    myStopped[veh].unloadedPersons += n;
}

void
MSStopOut::loadedContainers(const SUMOVehicle* veh, int n) {
    myStopped[veh].loadedContainers += n;
}

void
MSStopOut::unloadedContainers(const SUMOVehicle* veh, int n) {
    myStopped[veh].unloadedContainers += n;
}

void
MSStopOut::stopEnded(const SUMOVehicle* veh, const SUMOVehicleParameter::Stop& stop, const std::string& laneOrEdgeID) {
    assert(veh != 0);
    if (myStopped.count(veh) == 0) {
        WRITE_WARNING("Vehicle '" + veh->getID() + "' ends stop on edge '" + veh->getEdge()->getID()
                      + "', time " + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " without entering the stop");
        return;
    }
    double delay = -1;
    if (stop.until >= 0) {
        delay = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep() - stop.until);
    }
    StopInfo& si = myStopped[veh];
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
    myDevice.writeAttr("started", time2string(si.started));
    myDevice.writeAttr("ended", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    myDevice.writeAttr("delay", delay);
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
    myDevice.closeTag();
    myStopped.erase(veh);
}

/****************************************************************************/
