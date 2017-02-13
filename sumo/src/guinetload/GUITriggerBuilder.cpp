/****************************************************************************/
/// @file    GUITriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Builds trigger objects for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include <fstream>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUINet.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIBusStop.h>
#include <guisim/GUIContainerStop.h>
#include <guisim/GUIParkingArea.h>
#include <guisim/GUICalibrator.h>
#include <guisim/GUIChargingStation.h>
#include "GUITriggerBuilder.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(lst);
    return lst;
}


MSTriggeredRerouter*
GUITriggerBuilder::buildRerouter(MSNet& net, const std::string& id,
                                 MSEdgeVector& edges,
                                 SUMOReal prob, const std::string& file, bool off) {
    GUITriggeredRerouter* rr = new GUITriggeredRerouter(id, edges, prob, file, off,
            dynamic_cast<GUINet&>(net).getVisualisationSpeedUp());
    return rr;
}


void
GUITriggerBuilder::buildStoppingPlace(MSNet& net, const std::string& id, const std::vector<std::string>& lines,
                                      MSLane* lane, SUMOReal frompos, SUMOReal topos, const SumoXMLTag element) {
    bool success = false;
    GUIGlObject* o = 0;
    if (element == SUMO_TAG_CONTAINER_STOP) {
        GUIContainerStop* stop = new GUIContainerStop(id, lines, *lane, frompos, topos);
        success = net.addContainerStop(stop);
        o = stop;
        myCurrentStop = stop;
    } else {
        GUIBusStop* stop = new GUIBusStop(id, lines, *lane, frompos, topos);
        success = net.addBusStop(stop);
        o = stop;
        myCurrentStop = stop;
    }
    if (!success) {
        delete o;
        throw InvalidArgument("Could not build " + toString(element) + " stop '" + id + "'; probably declared twice.");
    }
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(o);
}


void
GUITriggerBuilder::beginParkingArea(MSNet& net, const std::string& id,
                                    const std::vector<std::string>& lines,
                                    MSLane* lane,
                                    SUMOReal frompos, SUMOReal topos,
                                    unsigned int capacity,
                                    SUMOReal width, SUMOReal length, SUMOReal angle) {
    assert(myParkingArea == 0);

    GUIParkingArea* stop = new GUIParkingArea(id, lines, *lane, frompos, topos, capacity, width, length, angle);
    if (!net.addParkingArea(stop)) {
        delete stop;
        throw InvalidArgument("Could not build parking area '" + id + "'; probably declared twice.");
    } else {
        myParkingArea = stop;
    }
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(stop);
}

void
GUITriggerBuilder::buildChargingStation(MSNet& net, const std::string& id, MSLane* lane, SUMOReal frompos, SUMOReal topos,
                                        SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, int chargeDelay) {
    GUIChargingStation* chargingStation = new GUIChargingStation(id, *lane, frompos, topos, chargingPower, efficiency, chargeInTransit, chargeDelay);

    if (!net.addChargingStation(chargingStation)) {
        delete chargingStation;
        throw InvalidArgument("Could not build charging station '" + id + "'; probably declared twice.");
    }

    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(chargingStation);
}

MSCalibrator*
GUITriggerBuilder::buildCalibrator(MSNet& net, const std::string& id,
                                   MSEdge* edge, SUMOReal pos,
                                   const std::string& file,
                                   const std::string& outfile,
                                   const SUMOTime freq,
                                   const MSRouteProbe* probe) {
    GUICalibrator* cali = new GUICalibrator(id, edge, pos, file, outfile, freq, probe);
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(cali);
    return cali;
}


/****************************************************************************/

