/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUITriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Johannes Rummel
/// @date    Mon, 26.04.2004
///
// Builds trigger objects for guisim
/****************************************************************************/
#include <config.h>

#include <string>
#include <fstream>
#include <utils/common/RGBColor.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUINet.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIBusStop.h>
#include <guisim/GUIParkingArea.h>
#include <guisim/GUICalibrator.h>
#include <guisim/GUIChargingStation.h>
#include <guisim/GUIOverheadWire.h>
#include "GUITriggerBuilder.h"



// ===========================================================================
// method definitions
// ===========================================================================
GUITriggerBuilder::GUITriggerBuilder() {}


GUITriggerBuilder::~GUITriggerBuilder() {}


MSLaneSpeedTrigger*
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet& net,
        const std::string& id, const std::vector<MSLane*>& destLanes,
        const std::string& file) {
    GUILaneSpeedTrigger* lst = new GUILaneSpeedTrigger(id, destLanes, file);
    static_cast<GUINet&>(net).registerRenderedObject(lst);
    return lst;
}


MSTriggeredRerouter*
GUITriggerBuilder::buildRerouter(MSNet& net, const std::string& id,
                                 MSEdgeVector& edges, double prob, bool off, bool optional,
                                 SUMOTime timeThreshold, const std::string& vTypes, const Position& pos) {
    GUITriggeredRerouter* rr = new GUITriggeredRerouter(id, edges, prob, off, optional, timeThreshold, vTypes, pos,
            dynamic_cast<GUINet&>(net).getVisualisationSpeedUp());
    return rr;
}


void
GUITriggerBuilder::buildStoppingPlace(MSNet& net, std::string id, std::vector<std::string> lines, MSLane* lane,
                                      double frompos, double topos, const SumoXMLTag element, std::string name,
                                      int personCapacity, double parkingLength, RGBColor& color) {
    myCurrentStop = new GUIBusStop(id, element, lines, *lane, frompos, topos, name, personCapacity, parkingLength, color);
    if (!net.addStoppingPlace(element, myCurrentStop)) {
        delete myCurrentStop;
        myCurrentStop = nullptr;
        throw InvalidArgument("Could not build " + toString(element) + " '" + id + "'; probably declared twice.");
    }
}


void
GUITriggerBuilder::beginParkingArea(MSNet& net, const std::string& id,
                                    const std::vector<std::string>& lines,
                                    const std::vector<std::string>& badges,
                                    MSLane* lane,
                                    double frompos, double topos,
                                    unsigned int capacity,
                                    double width, double length, double angle, const std::string& name,
                                    bool onRoad,
                                    const std::string& departPos,
                                    bool lefthand) {
    assert(myParkingArea == 0);
    GUIParkingArea* stop = new GUIParkingArea(id, lines, badges, *lane, frompos, topos, capacity, width, length, angle, name, onRoad, departPos, lefthand);
    if (!net.addStoppingPlace(SUMO_TAG_PARKING_AREA, stop)) {
        delete stop;
        throw InvalidArgument("Could not build parking area '" + id + "'; probably declared twice.");
    } else {
        myParkingArea = stop;
    }
}


void
GUITriggerBuilder::buildChargingStation(MSNet& net, const std::string& id, MSLane* lane, double frompos, double topos,
                                        const std::string& name, double chargingPower, double efficiency, bool chargeInTransit,
                                        SUMOTime chargeDelay, std::string chargeType, SUMOTime waitingTime, MSParkingArea* parkingArea) {
    GUIChargingStation* chargingStation = (parkingArea == nullptr) ? new GUIChargingStation(id, *lane, frompos, topos, name, chargingPower, efficiency,
                                          chargeInTransit, chargeDelay, chargeType, waitingTime) : new GUIChargingStation(id, parkingArea, name, chargingPower, efficiency,
                                                  chargeInTransit, chargeDelay, chargeType, waitingTime);
    if (!net.addStoppingPlace(SUMO_TAG_CHARGING_STATION, chargingStation)) {
        delete chargingStation;
        throw InvalidArgument("Could not build charging station '" + id + "'; probably declared twice.");
    }
    myCurrentStop = chargingStation;
    static_cast<GUINet&>(net).registerRenderedObject(chargingStation);
}


void
GUITriggerBuilder::buildOverheadWireSegment(MSNet& net, const std::string& id, MSLane* lane, double frompos, double topos,
        bool voltageSource) {
    GUIOverheadWire* overheadWire = new GUIOverheadWire(id, *lane, frompos, topos, voltageSource);
    if (!net.addStoppingPlace(SUMO_TAG_OVERHEAD_WIRE_SEGMENT, overheadWire)) {
        delete overheadWire;
        throw InvalidArgument("Could not build overheadWireSegment '" + id + "'; probably declared twice.");
    }
    static_cast<GUINet&>(net).registerRenderedObject(overheadWire);
}

void
GUITriggerBuilder::buildOverheadWireClamp(MSNet& net, const std::string& id, MSLane* lane_start, MSLane* lane_end) {
    GUIOverheadWireClamp* overheadWireClamp = new GUIOverheadWireClamp(id, *lane_start, *lane_end);
    static_cast<GUINet&>(net).registerRenderedObject(overheadWireClamp);
}


void
GUITriggerBuilder::endParkingArea() {
    if (myParkingArea != nullptr) {
        static_cast<GUINet*>(MSNet::getInstance())->registerRenderedObject(static_cast<GUIParkingArea*>(myParkingArea));
        myParkingArea = nullptr;
    } else {
        throw InvalidArgument("Could not end a parking area that is not opened.");
    }
}


void
GUITriggerBuilder::endStoppingPlace() {
    if (myCurrentStop != nullptr) {
        static_cast<GUINet*>(MSNet::getInstance())->registerRenderedObject(dynamic_cast<GUIGlObject*>(myCurrentStop));
        myCurrentStop = nullptr;
    } else {
        throw InvalidArgument("Could not end a stopping place that is not opened.");
    }
}


/****************************************************************************/
