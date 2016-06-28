/****************************************************************************/
/// @file    GNEAdditionalHandler
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// Builds trigger objects for netEdit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <utils/geom/GeomConvHelper.h>

#include "GNEAdditionalHandler.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEJunction.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEChange_Additional.h"
#include "GNEBusStop.h"
#include "GNEChargingStation.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNERerouter.h"
#include "GNEVariableSpeedSignal.h"
#include "GNERouteProbe.h"
#include "GNECalibrator.h"
#include "GNEContainerStop.h"
#include "GNEVaporizer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet) :
    SUMOSAXHandler(file),
    myViewNet(viewNet) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    try {
        switch (element) {
            case SUMO_TAG_BUS_STOP:
                parseAndBuildBusStop(attrs);
                break;
            case SUMO_TAG_CONTAINER_STOP:
                parseAndBuildContainerStop(attrs);
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseAndBuildChargingStation(attrs);
                break;
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                parseAndBuildDetectorE1(attrs);
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                parseAndBuildDetectorE2(attrs);
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                parseAndBuildDetectorE3(attrs);
                break;
            case SUMO_TAG_DET_ENTRY:
                parseAndBuildDetectorEntry(attrs);
                break;
            case SUMO_TAG_DET_EXIT:
                parseAndBuildDetectorExit(attrs);
                break;
            case SUMO_TAG_VSS:
                parseAndBuildVariableSpeedSignal(attrs);
                break;
            case SUMO_TAG_REROUTER:
                parseAndBuildRerouter(attrs);
                break;
            case SUMO_TAG_VAPORIZER:
                parseAndBuildVaporizer(attrs);
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
GNEAdditionalHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // edge Lane ID
    std::string edgeId = attrs.get<std::string>(SUMO_ATTR_EDGE, id.c_str(), ok);
    // get attributes
    std::string edgeID = attrs.getOpt<std::string>(SUMO_ATTR_EDGE, id.c_str(), ok, 0);
    SUMOReal startTime = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTTIME, id.c_str(), ok, 0);
    SUMOReal endTime = attrs.getOpt<SUMOReal>(SUMO_ATTR_END, id.c_str(), ok, 0);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get edge
    GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
    if(edge == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The edge " + edgeId + " to use within the " + toString(SUMO_TAG_VAPORIZER) + " '" + id + "' is not known.");
    } else {
        // build Vaporizer
        buildVaporizer(myViewNet, id, edge, startTime, endTime, false);
    }
}



void 
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // edge Lane ID
    std::string edgeId = attrs.get<std::string>(SUMO_ATTR_EDGE, id.c_str(), ok);
    // get attributes
    int freq = attrs.getOpt<int>(SUMO_ATTR_FREQUENCY, id.c_str(), ok, 0);
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), ok, 0);
    int begin = attrs.getOpt<int>(SUMO_ATTR_BEGIN, id.c_str(), ok, 0);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get edge
    GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
    if(edge == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The edge " + edgeId + " to use within the " + toString(SUMO_TAG_ROUTEPROBE) + " '" + id + "' is not known.");
    } else {
        // build Vaporizer
        buildRouteProbe(myViewNet, id, edge, freq, file, begin, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildVariableSpeedSignal(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    const SUMOReal posx = attrs.getOpt<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok, 0);
    const SUMOReal posy = attrs.getOpt<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok, 0);
    const std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), ok, "", false);
    std::vector<std::string> lanesID;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LANES, id.c_str(), ok, "", false), lanesID);
    // Check if parsing of parameter was correct
    if (!ok) {
        throw ProcessError();
    }
    // Obtain pointer to lanes
    std::vector<GNELane*> lanes;
    for(int i = 0; i < (int)lanesID.size(); i++) {
        GNELane *lane = myViewNet->getNet()->retrieveLane(lanesID.at(i));
        if(lane) {
            lanes.push_back(lane);
        } else {
            throw ProcessError();
        }
    }
    try {
        // if operation of build variable speed signal was sucesfully, save Id
        if(buildVariableSpeedSignal(myViewNet, id, Position(posx,posy), lanes, file, false)) {
            myAdditionalSetParent = id;
        }

    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs) {
    std::cout << "Function buildRerouter of class GNEAdditionalHandler not implemented yet";
    /*
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_EDGES, id.c_str(), ok);
    if (!ok) {
        WRITE_ERROR("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
    }
    MSEdgeVector edges;
    std::vector<std::string> edgeIDs;
    SUMOSAXAttributes::parseStringVector(objectid, edgeIDs);
    for (std::vector<std::string>::iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
        MSEdge* edge = MSEdge::dictionary(*i);
        if (edge == 0) {
            WRITE_ERROR("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
        }
        edges.push_back(edge);
    }
    if (edges.size() == 0) {
        WRITE_ERROR("No edges found for MSTriggeredRerouter '" + id + "'.");
    }
    SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok, 1);
    bool off = attrs.getOpt<bool>(SUMO_ATTR_OFF, id.c_str(), ok, false);
    if (!ok) {
        WRITE_ERROR("Could not parse MSTriggeredRerouter '" + id + "'.");
    }
    MSTriggeredRerouter* trigger = buildRerouter(myViewNet->getNet(), id, edges, prob, file, off);
    // read in the trigger description
    if (file == "") {
        trigger->registerParent(SUMO_TAG_REROUTER, myHandler);
    } else if (!XMLSubSys::runParser(*trigger, file)) {
        throw ProcessError();
    }
*/
}


void
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    // get positions
    SUMOReal startPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal endPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, 10);
    // get lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_BUS_STOP) + " '" + id + "' is not known.");
    } else if(!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, friendlyPos)) {
        // Write error if position isn't valid
        WRITE_ERROR("Invalid position for " + toString(SUMO_TAG_BUS_STOP) + " '" + id + "'.");
    } else {
        // build busStop
        buildBusStop(myViewNet, id, lane, startPos, endPos, lines, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildContainerStop(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    // get positions
    SUMOReal startPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal endPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, 10);
    // get lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_CONTAINER_STOP) + " '" + id + "' is not known.");
    // Check position and build container stop
    } else if(!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, friendlyPos)) {
        WRITE_ERROR("Invalid position for " + toString(SUMO_TAG_CONTAINER_STOP) + " '" + id + "'.");
    } else {
        buildContainerStop(myViewNet, id, lane, startPos, endPos, lines, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    // get positions
    SUMOReal startPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal endPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, 10);
    // get rest of parameters
    SUMOReal chrgpower = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), ok, 0);
    SUMOReal efficiency = attrs.getOpt<SUMOReal>(SUMO_ATTR_EFFICIENCY, id.c_str(), ok, 0);
    bool chargeInTransit = attrs.getOpt<bool>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), ok, 0);
    SUMOReal chargeDelay = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGEDELAY, id.c_str(), ok, 0);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_CHARGING_STATION) + " '" + id + "' is not known.");
    } else if(!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, friendlyPos)) {
        WRITE_ERROR("Invalid position for " + toString(SUMO_TAG_CHARGING_STATION) + " '" + id + "'.");
    } else {
        buildChargingStation(myViewNet, id, lane, startPos, endPos, chrgpower, efficiency, chargeInTransit, chargeDelay, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs) {
    std::cout << "Function buildCalibrator of class GNEAdditionalHandler not implemented yet";
    /*
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    MSLane* lane = getLane(attrs, "calibrator", id);
    const SUMOReal pos = getPosition(attrs, lane, "calibrator", id);
    const SUMOTime freq = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, DELTA_T); // !!! no error handling
    std::string file = getFileName(attrs, base, true);
    std::string outfile = attrs.getOpt<std::string>(SUMO_ATTR_OUTPUT, id.c_str(), ok, "");
    std::string routeProbe = attrs.getOpt<std::string>(SUMO_ATTR_ROUTEPROBE, id.c_str(), ok, "");
    MSRouteProbe* probe = 0;
    if (routeProbe != "") {
        probe = dynamic_cast<MSRouteProbe*>(net.getDetectorControl().getTypedDetectors(SUMO_TAG_ROUTEPROBE).get(routeProbe));
    }
    if (MSGlobals::gUseMesoSim) {
#ifdef HAVE_INTERNAL
        METriggeredCalibrator* trigger = buildMECalibrator(myViewNet->getNet(), id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
#endif
    } else {
        MSCalibrator* trigger = buildCalibrator(myViewNet->getNet(), id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    }*/
}


void
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    // get rest of attributes
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
    const bool splitByType = attrs.getOpt<bool>(SUMO_ATTR_SPLIT_VTYPE, id.c_str(), ok, false, false);
    const std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), ok, "", false);
    // Check if parsing of parameters was correct
    if (!ok) 
        throw ProcessError();
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_E1DETECTOR) + " '" + id + "' is not known.");
    } else {
        // build detector E1
        buildDetectorE1(myViewNet, id, lane, position, frequency, file, splitByType, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f / 3.6f);
    const SUMOReal jamDistThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), ok, 10.0f);
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
    const SUMOReal length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, id.c_str(), ok);
    const bool cont = attrs.getOpt<bool>(SUMO_ATTR_CONT, id.c_str(), ok, false);
    const std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), ok, "", false);
    // Check if parsing of parameters was correct
    if (!ok) {
        throw ProcessError();
    }
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_E2DETECTOR) + " '" + id + "' is not known.");
    } else {
        // build detector E2
        buildDetectorE2(myViewNet, id, lane, position, length, frequency, file, cont, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOReal posx = attrs.getOpt<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok, 0, false);
    const SUMOReal posy = attrs.getOpt<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok, 0, false);
    const std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), ok, "", false);
    // Check if parsing of parameters was correct
    if (!ok) {
        throw ProcessError();
    }
    // if operation of build detector E3 was sucesfully, save Id
    if(buildDetectorE3(myViewNet, id, Position(posx,posy), frequency, file, false)) {
        myAdditionalSetParent = id;
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, 0, ok);
    // Check if parsing of parameters was correct
    if (!ok) {
        throw ProcessError();
    }
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_E2DETECTOR) + " is not known.");
    } else {
        // get the ID. Note: This Id is interne, and cannot be defined by user
        int indexEntry = 0;
        while(myViewNet->getNet()->getAdditional(SUMO_TAG_DET_ENTRY, toString(SUMO_TAG_DET_ENTRY) + "_" + toString(indexEntry) + "_" + myAdditionalSetParent) != NULL) {
            indexEntry++;
        }
        // build detector entry
        buildDetectorEntry(myViewNet, toString(SUMO_TAG_DET_ENTRY) + "_" + toString(indexEntry) + "_" + myAdditionalSetParent, lane, position, myAdditionalSetParent, false);
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
    // get Lane ID
    std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, 0, ok);
    // Check if parsing of parameters was correct
    if (!ok) {
        throw ProcessError();
    }
    // get pointer to lane
    GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
    if(lane == NULL) {
        // Write error if lane isn't valid
        WRITE_ERROR("The lane " + laneId + " to use within the " + toString(SUMO_TAG_E2DETECTOR) + " is not known.");
    } else {
        // get the ID. Note: This Id is interne, and cannot be defined by user
        int indexExit = 0;
        while(myViewNet->getNet()->getAdditional(SUMO_TAG_DET_EXIT, toString(SUMO_TAG_DET_EXIT) + "_" + toString(indexExit) + "_" + myAdditionalSetParent) != NULL) {
            indexExit++;
        }
        // build detector Exit
        buildDetectorExit(myViewNet, toString(SUMO_TAG_DET_EXIT) + "_" + toString(indexExit) + "_" + myAdditionalSetParent, lane, position, myAdditionalSetParent, false);
    }

}


bool
GNEAdditionalHandler::buildAdditional(GNEViewNet *viewNet, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // Extract common attributes
    std::string id = values[SUMO_ATTR_ID];
    bool blocked = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
    // create additional depending of the tag
    switch(tag) {
        case SUMO_TAG_BUS_STOP: {
            // get own attributes of busStop
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LINES], lines);
            // Build busStop
            if(lane) {
                return buildBusStop(viewNet, id, lane, startPos, endPos, lines, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CONTAINER_STOP: {
            // get own attributes of containerStop
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LINES], lines);
            // Build containerStop
            if(lane) {
                return buildContainerStop(viewNet, id, lane, startPos, endPos, lines, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CHARGING_STATION: {
            // get own attributes of chargingStation
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_CHARGINGPOWER]);
            SUMOReal efficiency = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            int chargeDelay = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_CHARGEDELAY]);
            // Build chargingStation
            if(lane) {
                return buildChargingStation(viewNet, id, lane, startPos, endPos, chargingPower, efficiency, chargeInTransit, chargeDelay, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E1DETECTOR: {
            // get own attributes of detector E1
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool splitByType = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_SPLIT_VTYPE]);
            // Build detector E1
            if(lane) {
                return buildDetectorE1(viewNet, id, lane, pos, freq, filename, splitByType, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E2DETECTOR: {
            // get own attributes of detector E2
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            SUMOReal lenght = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_LENGTH]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool cont = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CONT]);
            int timeThreshold = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            SUMOReal speedThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            SUMOReal jamThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_JAM_DIST_THRESHOLD]);
            // Build detector E2
            if(lane) {
                return buildDetectorE2(viewNet, id, lane, pos, lenght, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E3DETECTOR: {
            // get own attributes of detector E3
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            // Build detector E3
            if(pos.size() == 1) {
                return buildDetectorE3(viewNet, id, pos[0], freq, filename, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_ENTRY: {
            // get own attributes of detector Entry
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            std::string detectorE3ParentID = values[GNE_ATTR_PARENT];
            // Build detector Entry
            if(lane && !detectorE3ParentID.empty()) {
                return buildDetectorEntry(viewNet, id, lane, pos, detectorE3ParentID, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_EXIT: {
            // get own attributes of Detector Exit
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            std::string detectorE3ParentID = values[GNE_ATTR_PARENT];
            // Build detector Exit
            if(lane && !detectorE3ParentID.empty()) {
                return buildDetectorExit(viewNet, id, lane, pos, detectorE3ParentID, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VSS: {
            // get own attributes of variable speed signal
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            // Parse lane Ids
            std::vector<std::string> laneIds;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LANES], laneIds);
            // Obtain pointers to lanes
            std::vector<GNELane*> lanes;
            for(int i = 0; i < (int)laneIds.size(); i++) {
                lanes.push_back(viewNet->getNet()->retrieveLane(laneIds.at(i)));
            }
            std::string file = values[SUMO_ATTR_FILE];
            if(pos.size() == 1) {
                return buildVariableSpeedSignal(viewNet, id, pos[0], lanes, file, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR: {
            // get own attributes of calibrator
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            // Currently unused SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            SUMOReal pos = 0;
            std::string file = values[SUMO_ATTR_FILE];
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            SUMOTime freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            // Build calibrator
            if(lane) {
                return buildCalibrator(viewNet, id, &lane->getParentEdge(), pos, file, outfile, freq, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_REROUTER: {
            // get own attributes of rerouter
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            // Parse edges Ids
            std::vector<std::string> edgeIds;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_EDGES], edgeIds);
            // Obtain pointers to edges
            std::vector<GNEEdge*> edges;
            for(int i = 0; i < (int)edgeIds.size(); i++) {
                edges.push_back(viewNet->getNet()->retrieveEdge(edgeIds.at(i)));
            }
            SUMOReal prob = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_PROB]);
            std::string file = values[SUMO_ATTR_FILE];
            bool off = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_OFF]);
            // Build rerouter
            if(pos.size() == 1) {
                return buildRerouter(viewNet, id, pos[0], edges, prob, file, off, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_ROUTEPROBE: {
            // get own attributes of RouteProbe
            GNELane *lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            int begin = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_BEGIN]);
            // Build RouteProbe
            if(lane) {
                return buildRouteProbe(viewNet, id, &(lane->getParentEdge()), freq, filename, begin, blocked);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VAPORIZER: {
            // get own attributes of vaporizer
            GNEEdge *edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            int startTime = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_STARTTIME]);
            int end = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_END]);
            // Build RouteProbe
            if(edge) {
                return buildVaporizer(viewNet, id, edge, startTime, end, blocked);
            } else {
                return false;
            }
        }
        default:
            return false;
    }
}


bool
GNEAdditionalHandler::buildBusStop(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_BUS_STOP, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, lines, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), busStop, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_BUS_STOP) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildContainerStop(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CONTAINER_STOP, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
        GNEContainerStop* containerStop = new GNEContainerStop(id, lane, viewNet, startPos, endPos, lines, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), containerStop, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildChargingStation(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal startPos, SUMOReal endPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CHARGING_STATION, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
        GNEChargingStation* chargingStation = new GNEChargingStation(id, lane, viewNet, startPos, endPos, chargingPower, efficiency, chargeInTransit, chargeDelay, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), chargingStation, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE1(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, int freq, const std::string &filename, bool splitByType, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E1DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
        GNEDetectorE1 *detectorE1 = new GNEDetectorE1(id, lane, viewNet, pos, freq, filename, splitByType, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE1, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_E1DETECTOR) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, const std::string& id, GNELane *lane, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename,  bool cont, int timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E2DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
        GNEDetectorE2 *detectorE2 = new GNEDetectorE2(id, lane, viewNet, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE2, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_E2DETECTOR) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE3(GNEViewNet *viewNet, const std::string& id, Position pos, int freq, const std::string &filename, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
        GNEDetectorE3 *detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE3, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_E3DETECTOR) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, std::string idDetectorE3Parent, bool blocked) {
    // get DetectorE3 parent
    GNEDetectorE3 *detectorE3Parent = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, idDetectorE3Parent));
    // Check if DetectorE3 parent is correct
    if(detectorE3Parent == NULL) {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " '" + id + "' in netEdit; '" + toString(SUMO_TAG_E3DETECTOR) + " '" + idDetectorE3Parent + "' don't valid.");
        return false;
    } else if (viewNet->getNet()->getAdditional(SUMO_TAG_DET_ENTRY, id) == NULL) {
        // Create detector Entry if don't exist already in the net
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
        GNEDetectorEntry *entry = new GNEDetectorEntry(id, viewNet, lane, pos, detectorE3Parent, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), entry, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_DET_ENTRY) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorExit(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, std::string idDetectorE3Parent, bool blocked) {
    // get DetectorE3 parent
    GNEDetectorE3 *detectorE3Parent = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, idDetectorE3Parent));
    // Check if DetectorE3 parent is correct
    if(detectorE3Parent == NULL) {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_DET_EXIT) + " '" + id + "' in netEdit; '" + idDetectorE3Parent + "' don't valid.");
        return false;
    } else if (viewNet->getNet()->getAdditional(SUMO_TAG_DET_EXIT, id) == NULL) {
        // Create detector Exit if don't exist already in the net
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
        GNEDetectorExit *exit = new GNEDetectorExit(id, viewNet, lane, pos, detectorE3Parent, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), exit, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_DET_EXIT) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet *viewNet, const std::string& id, GNEEdge *edge, SUMOReal pos, const std::string& file, const std::string& outfile, const SUMOTime freq, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
        GNECalibrator *calibrator = new GNECalibrator(id, edge, viewNet, pos, freq, outfile, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), calibrator, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_CALIBRATOR) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet *viewNet, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, SUMOReal prob, const std::string& file, bool off, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
        GNERerouter *rerouter = new GNERerouter(id, viewNet, pos, edges, file, prob, off, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), rerouter, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_REROUTER) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildRouteProbe(GNEViewNet *viewNet, const std::string& id, GNEEdge *edge, int freq, const std::string& file, int begin, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
        GNERouteProbe *routeProbe = new GNERouteProbe(id, viewNet, edge, freq, file, begin, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), routeProbe, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSignal(GNEViewNet *viewNet,const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& file, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VSS, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
        GNEVariableSpeedSignal *variableSpeedSignal = new GNEVariableSpeedSignal(id, viewNet, pos, lanes, file, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), variableSpeedSignal, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_VSS) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildVaporizer(GNEViewNet *viewNet, const std::string& id, GNEEdge *edge, SUMOTime startTime, SUMOTime end, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VAPORIZER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        GNEVaporizer *vaporizer = new GNEVaporizer(id, viewNet, edge, startTime, end, blocked);
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), vaporizer, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_ERROR("Could not build " + toString(SUMO_TAG_VAPORIZER) + "'" + id + "' in netEdit; probably declared twice.");
        return false;
    }
}


std::string
GNEAdditionalHandler::getFileName(const SUMOSAXAttributes& attrs, const std::string& base, const bool allowEmpty) {
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, 0, ok, "");
    if (file == "") {
        if (allowEmpty) {
            return file;
        }
        WRITE_ERROR("No filename given.");
    }
    // check whether absolute or relative filenames are given
    if (!FileHelpers::isAbsolute(file)) {
        return FileHelpers::getConfigurationRelative(base, file);
    }
    return file;
}


SUMOReal
GNEAdditionalHandler::getPosition(const SUMOSAXAttributes& attrs, GNELane& lane, const std::string& tt, const std::string& tid) {
    bool ok = true;
    SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
    if (!ok) {
        WRITE_ERROR("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = lane.getLaneShapeLenght() + pos;
    }
    if (pos > lane.getLaneShapeLenght()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLenght() - (SUMOReal) 0.1;
        } else {
            WRITE_ERROR("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool
GNEAdditionalHandler::checkStopPos(SUMOReal& startPos, SUMOReal& endPos, const SUMOReal laneLength, const SUMOReal minLength,  const bool friendlyPos) {
    if (minLength > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < minLength || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if (startPos < 0 || startPos > endPos - minLength) {
        if (!friendlyPos) {
            return false;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > endPos - minLength) {
            startPos = endPos - minLength;
        }
    }
    return true;
}


/****************************************************************************/
