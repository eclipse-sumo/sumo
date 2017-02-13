/****************************************************************************/
/// @file    GNEAdditionalHandler
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// Builds trigger objects for netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include "GNEBusStop.h"
#include "GNEChange_Additional.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNERouteProbe.h"
#include "GNEUndoList.h"
#include "GNEVaporizer.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myE3Parent(NULL),
    rerouterIntervalToInsertValues(NULL),
    myLastTag(SUMO_TAG_NOTHING) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // first chek additional parent tag
    if (checkAdditionalParent(tag)) {
        // Call parse and build depending of tag
        switch (element) {
            case SUMO_TAG_BUS_STOP:
                parseAndBuildBusStop(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_CONTAINER_STOP:
                parseAndBuildContainerStop(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseAndBuildChargingStation(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                parseAndBuildDetectorE1(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                parseAndBuildDetectorE2(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                parseAndBuildDetectorE3(attrs, tag);
                break;
            case SUMO_TAG_DET_ENTRY:
                parseAndBuildDetectorEntry(attrs, tag);
                break;
            case SUMO_TAG_DET_EXIT:
                parseAndBuildDetectorExit(attrs, tag);
                break;
            case SUMO_TAG_VSS:
                parseAndBuildVariableSpeedSign(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_REROUTER:
                parseAndBuildRerouter(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_CALIBRATOR:
                parseAndBuildCalibrator(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_VAPORIZER:
                parseAndBuildVaporizer(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_ROUTEPROBE:
                parseAndBuildRouteProbe(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_FLOW:
                parseCalibratorFlow(attrs, tag);
                myE3Parent = NULL;
                break;
            case SUMO_TAG_STEP:
                parseVariableSpeedSignStep(attrs, tag);
                myE3Parent = NULL;
                break;
            default:
                break;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Vaporizer
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    const std::string edgeId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    SUMOReal startTime = getParsedAttribute<SUMOReal>(attrs, 0, tag, SUMO_ATTR_STARTTIME, abort);
    SUMOReal endTime = getParsedAttribute<SUMOReal>(attrs, 0, tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else {
            // build Vaporizer
            buildVaporizer(myViewNet, id, edge, startTime, endTime);
        }
    }
}



void
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string edgeId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    SUMOReal freq = getParsedAttribute<SUMOReal>(attrs, 0, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_FILE, abort);
    int begin = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_BEGIN, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else {
            // build Vaporizer
            buildRouteProbe(myViewNet, id, edge, freq, file, begin);
        }
    }
}


void
GNEAdditionalHandler::parseCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool ok = true;
    bool abort = false;
    // Load non empty values
    std::string flowId = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok, false);
    if (!ok) {
        WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_ID) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_CALIBRATOR) + " is missing");
        ok = true;
        abort = true;
    }
    std::string type = attrs.get<std::string>(SUMO_ATTR_TYPE, flowId.c_str(), ok, false);
    if (!ok) {
        WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_TYPE) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_CALIBRATOR) + " is missing");
        ok = true;
        abort = true;
    }
    std::string route = attrs.get<std::string>(SUMO_ATTR_ROUTE, flowId.c_str(), ok, false);
    if (!ok) {
        WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_ROUTE) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_CALIBRATOR) + " is missing");
        ok = true;
        abort = true;
    }
    // Declare calibrator flow
    GNECalibrator::GNECalibratorFlow* flow = new GNECalibrator::GNECalibratorFlow(0, type, route);

    // Load rest of parameters
    flow->setColor(attrs.getOpt<std::string>(SUMO_ATTR_COLOR, flowId.c_str(), ok, "", false));
    flow->setDepartLane(attrs.getOpt<std::string>(SUMO_ATTR_DEPARTLANE, flowId.c_str(), ok, "first", false));
    flow->setDepartPos(attrs.getOpt<std::string>(SUMO_ATTR_DEPARTPOS, flowId.c_str(), ok, "base", false));
    flow->setDepartSpeed(attrs.getOpt<std::string>(SUMO_ATTR_DEPARTSPEED, flowId.c_str(), ok, "0", false));
    flow->setArrivalLane(attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALLANE, flowId.c_str(), ok, "current", false));
    flow->setArrivalPos(attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALPOS, flowId.c_str(), ok, "max", false));
    flow->setArrivalSpeed(attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALSPEED, flowId.c_str(), ok, "current", false));
    flow->setLine(attrs.getOpt<std::string>(SUMO_ATTR_LINE, flowId.c_str(), ok, "", false));
    flow->setPersonNumber(attrs.getOpt<int>(SUMO_ATTR_PERSON_NUMBER, flowId.c_str(), ok, 0, false));
    flow->setContainerNumber(attrs.getOpt<int>(SUMO_ATTR_CONTAINER_NUMBER, flowId.c_str(), ok, 0, false));
    flow->setBegin(attrs.getOpt<SUMOReal>(SUMO_ATTR_BEGIN, flowId.c_str(), ok, 0, false));
    flow->setEnd(attrs.getOpt<SUMOReal>(SUMO_ATTR_END, flowId.c_str(), ok, 0, false));
    flow->setVehsPerHour(attrs.getOpt<SUMOReal>(SUMO_ATTR_VEHSPERHOUR, flowId.c_str(), ok, 0, false));
    flow->setPeriod(attrs.getOpt<SUMOReal>(SUMO_ATTR_PERIOD, flowId.c_str(), ok, 0, false));
    flow->setProbability(attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, flowId.c_str(), ok, 0, false));
    flow->setNumber(attrs.getOpt<int>(SUMO_ATTR_NUMBER, flowId.c_str(), ok, 0, false));
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Obtain calibrator
        GNECalibrator* calibratorToInsertFlow = /*dynamic_cast<GNECalibrator*>(myViewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, myAdditionalParent))*/ NULL;
        if (calibratorToInsertFlow == NULL) {
            WRITE_WARNING("A " + toString(SUMO_TAG_CALIBRATOR) + " must be inserter before insertion of the " + toString(tag) + " '" + flowId + "'");
            delete flow;
        } else {
            calibratorToInsertFlow->insertFlow(flow);
        }
        delete flow;
    }
}


void
GNEAdditionalHandler::parseVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool ok = true;
    bool abort = false;
    // Load step values
    SUMOReal time = attrs.get<SUMOReal>(SUMO_ATTR_TIME, 0, ok, false);
    if (!ok) {
        WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_TIME) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_VSS) + " is missing");
        ok = true;
        abort = true;
    }
    SUMOReal speed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, 0, ok, false);
    if (!ok) {
        WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_SPEED) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_VSS) + " is missing");
        ok = true;
        abort = true;
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        GNEVariableSpeedSign* variableSpeedSignToInsertStep = /*dynamic_cast<GNEVariableSpeedSign*>(myViewNet->getNet()->getAdditional(SUMO_TAG_VSS, myAdditionalParent))*/ NULL;
        if (variableSpeedSignToInsertStep == NULL) {
            WRITE_WARNING("A " + toString(SUMO_TAG_VSS) + " must be inserter before insertion of a " + toString(tag));
        } else if (!variableSpeedSignToInsertStep->insertStep(time, speed)) {
            WRITE_WARNING("Parameter '" + toString(SUMO_ATTR_TIME) + "' of " + toString(tag) + "'s " + toString(SUMO_TAG_VSS) + " is duplicated");
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildVariableSpeedSign(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of VSS
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    std::string listOfLanes = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANES, abort);
    SUMOReal posx = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    SUMOReal posy = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Obtain lanes
        std::vector<std::string> lanesID;
        SUMOSAXAttributes::parseStringVector(listOfLanes, lanesID);
        // obtain VSS Values
        // @todo
        std::map<SUMOReal, SUMOReal> VSSValues;
        // Obtain pointer to lanes
        std::vector<GNELane*> lanes;
        for (int i = 0; i < (int)lanesID.size(); i++) {
            GNELane* lane = myViewNet->getNet()->retrieveLane(lanesID.at(i));
            if (lane) {
                lanes.push_back(lane);
            } else {
                WRITE_WARNING(toString(SUMO_TAG_LANE) + " '" + lanesID.at(i) + "' isn't valid");
            }
        }
        // if operation of build variable speed signal was sucesfully, save Id
        if (buildVariableSpeedSign(myViewNet, id, Position(posx, posy), lanes, file, VSSValues)) {
            ;//myAdditionalParent = id;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string edgesAttribute = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_EDGES, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    SUMOReal probability = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_PROB, abort);
    bool off = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_OFF, abort);
    SUMOReal posx = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    SUMOReal posy = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Obtain edges
        std::vector<std::string> edgesID;
        bool ok = false;
        SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_EDGES, id.c_str(), ok, "", false), edgesID);
        // obtain Rerouter values Values

        // Obtain pointer to edges
        std::vector<GNEEdge*> edges;
        for (int i = 0; i < (int)edgesID.size(); i++) {
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgesID.at(i));
            if (edge) {
                edges.push_back(edge);
            } else {
                throw ProcessError(); /**************** ARREGLAR **********/
            }
        }
        // if operation of build variable speed signal was sucesfully, save Id
        if (buildRerouter(myViewNet, id, Position(posx, posy), edges, probability, file, off)) {
            ;//myAdditionalParent = id;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of bus stop
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    SUMOReal startPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    SUMOReal endPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::string linesAttribute = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Parse lines
        std::vector<std::string> lines;
        SUMOSAXAttributes::parseStringVector(linesAttribute, lines);
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
            // Write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildBusStop(myViewNet, id, lane, startPos, endPos, lines)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildContainerStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of container stop
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    SUMOReal startPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    SUMOReal endPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::string linesAttribute = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Parse lines
        std::vector<std::string> lines;
        SUMOSAXAttributes::parseStringVector(linesAttribute, lines);
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildContainerStop(myViewNet, id, lane, startPos, endPos, lines)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    SUMOReal startPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    SUMOReal endPos = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    SUMOReal chrgpower = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGINGPOWER, abort);
    SUMOReal efficiency = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEINTRANSIT, abort);
    SUMOReal chargeDelay = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEDELAY, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLenght(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildChargingStation(myViewNet, id, lane, startPos, endPos, chrgpower, efficiency, chargeInTransit, chargeDelay)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of calibrator
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    std::string outfile = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_OUTPUT, abort);
    SUMOReal position = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    SUMOReal freq = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    // std::string routeProbe = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to edge
        GNEEdge* edge = &(myViewNet->getNet()->retrieveLane(laneId, false)->getParentEdge());
        if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (buildCalibrator(myViewNet, id, edge, position, outfile, freq, std::vector<GNECalibrator::GNECalibratorFlow*>())) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E1
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    SUMOReal position = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    SUMOReal frequency = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    bool splitByType = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_SPLIT_VTYPE, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLenght()))) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildDetectorE1(myViewNet, id, lane, position, frequency, file, splitByType)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E2
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    SUMOReal position = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    SUMOReal frequency = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    SUMOReal length = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_LENGTH, abort);
    SUMOReal haltingTimeThreshold = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    SUMOReal haltingSpeedThreshold = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    SUMOReal jamDistThreshold = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool cont = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CONT, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if ((position < 0) || ((position + length) > (lane->getLaneShapeLenght()))) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildDetectorE2(myViewNet, id, lane, position, length, frequency, file, cont, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E3
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    SUMOReal frequency = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    SUMOReal haltingTimeThreshold = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    SUMOReal haltingSpeedThreshold = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    SUMOReal posx = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    SUMOReal posy = getParsedAttribute<SUMOReal>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // Create without possibility of undo/redo
        if (myViewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
            myE3Parent = new GNEDetectorE3(id, myViewNet, Position(posx, posy), frequency, file, haltingTimeThreshold, haltingSpeedThreshold);
            myLastTag = myE3Parent->getTag();
        } else {
            WRITE_WARNING("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
            myE3Parent = NULL;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_LANE, abort);
    SUMOReal position = getParsedAttribute<SUMOReal>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLenght()))) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else if (myE3Parent != NULL && buildDetectorEntry(myViewNet, myE3Parent, lane, position)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Exit
    std::string laneId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_LANE, abort);
    SUMOReal position = getParsedAttribute<SUMOReal>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLenght()))) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else if (myE3Parent != NULL && buildDetectorExit(myViewNet, myE3Parent, lane, position)) {
            myLastTag = tag;
        }
    }
}


bool
GNEAdditionalHandler::buildAdditional(GNEViewNet* viewNet, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // create additional depending of the tag
    switch (tag) {
        case SUMO_TAG_BUS_STOP: {
            // obtain specify attributes of busStop
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LINES], lines);
            // Build busStop
            if (lane) {
                return buildBusStop(viewNet, id, lane, startPos, endPos, lines);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CONTAINER_STOP: {
            // obtain specify attributes of containerStop
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LINES], lines);
            // Build containerStop
            if (lane) {
                return buildContainerStop(viewNet, id, lane, startPos, endPos, lines);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CHARGING_STATION: {
            // obtain specify attributes of chargingStation
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_CHARGINGPOWER]);
            SUMOReal efficiency = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            SUMOReal chargeDelay = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_CHARGEDELAY]);
            // Build chargingStation
            if (lane) {
                return buildChargingStation(viewNet, id, lane, startPos, endPos, chargingPower, efficiency, chargeInTransit, chargeDelay);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E1DETECTOR: {
            // obtain specify attributes of detector E1
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            SUMOReal freq = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool splitByType = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_SPLIT_VTYPE]);
            // Build detector E1
            if (lane) {
                return buildDetectorE1(viewNet, id, lane, pos, freq, filename, splitByType);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E2DETECTOR: {
            // obtain specify attributes of detector E2
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            SUMOReal freq = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_FREQUENCY]);
            SUMOReal lenght = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_LENGTH]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool cont = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CONT]);
            SUMOReal timeThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            SUMOReal speedThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            SUMOReal jamThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_JAM_DIST_THRESHOLD]);
            // Build detector E2
            if (lane) {
                return buildDetectorE2(viewNet, id, lane, pos, lenght, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E3DETECTOR: {
            // obtain specify attributes of detector E3
            bool ok;
            std::string id = values[SUMO_ATTR_ID];
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            SUMOReal freq = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            SUMOReal timeThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            SUMOReal speedThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            // Build detector E3
            if (pos.size() == 1) {
                return buildDetectorE3(viewNet, id, pos[0], freq, filename, timeThreshold, speedThreshold);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_ENTRY: {
            // obtain specify attributes of detector Entry
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            GNEDetectorE3* E3 = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->retrieveAdditional(values[GNE_ATTR_PARENT]));
            // Build detector Entry
            if (lane && E3) {
                return buildDetectorEntry(viewNet, E3, lane, pos);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_EXIT: {
            // obtain specify attributes of Detector Exit
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            GNEDetectorE3* E3 = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->retrieveAdditional(values[GNE_ATTR_PARENT]));
            // Build detector Exit
            if (lane && E3) {
                return buildDetectorExit(viewNet, E3, lane, pos);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VSS: {
            // obtain specify attributes of variable speed signal
            std::string id = values[SUMO_ATTR_ID];
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            // Parse lane Ids
            std::vector<std::string> laneIds;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_LANES], laneIds);
            // By default, VSSValues are empty
            std::map<SUMOReal, SUMOReal> VSSValues;
            // Obtain pointers to lanes
            std::vector<GNELane*> lanes;
            for (int i = 0; i < (int)laneIds.size(); i++) {
                lanes.push_back(viewNet->getNet()->retrieveLane(laneIds.at(i)));
            }
            std::string file = values[SUMO_ATTR_FILE];
            if (pos.size() == 1) {
                return buildVariableSpeedSign(viewNet, id, pos[0], lanes, file, VSSValues);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = &(viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false)->getParentEdge());
            // get rest of parameters
            // Currently unused SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            SUMOReal pos = 0;
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            SUMOReal freq = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_FREQUENCY]);
            // get flow values
            std::vector<GNECalibrator::GNECalibratorFlow*> flowValues;
            // Build calibrator
            if (edge) {
                return buildCalibrator(viewNet, id, edge, pos, outfile, freq, flowValues);
            } else {
                return false;
            }
        }
        case SUMO_TAG_REROUTER: {
            // obtain specify attributes of rerouter
            std::string id = values[SUMO_ATTR_ID];
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            // Parse edges Ids
            std::vector<std::string> edgeIds;
            SUMOSAXAttributes::parseStringVector(values[SUMO_ATTR_EDGES], edgeIds);
            // Get rest of parameters
            bool off = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_OFF]);
            SUMOReal prob = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_PROB]);
            std::string file = values[SUMO_ATTR_FILE];
            // Obtain pointers to edges
            std::vector<GNEEdge*> edges;
            for (int i = 0; i < (int)edgeIds.size(); i++) {
                edges.push_back(viewNet->getNet()->retrieveEdge(edgeIds.at(i)));
            }
            // Build rerouter
            if (pos.size() == 1) {
                return buildRerouter(viewNet, id, pos[0], edges, prob, file, off);
            } else {
                return false;
            }
        }
        case SUMO_TAG_ROUTEPROBE: {
            // obtain specify attributes of RouteProbe
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            SUMOReal freq = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            SUMOReal begin = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_BEGIN]);
            // Build RouteProbe
            if (edge) {
                return buildRouteProbe(viewNet, id, edge, freq, filename, begin);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VAPORIZER: {
            // obtain specify attributes of vaporizer
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            SUMOReal startTime = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTTIME]);
            SUMOReal end = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_END]);
            // Build RouteProbe
            if (edge) {
                return buildVaporizer(viewNet, id, edge, startTime, end);
            } else {
                return false;
            }
        }
        default:
            return false;
    }
}


bool
GNEAdditionalHandler::buildBusStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_BUS_STOP, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, lines);
        viewNet->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildContainerStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CONTAINER_STOP, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
        GNEContainerStop* containerStop = new GNEContainerStop(id, lane, viewNet, startPos, endPos, lines);
        viewNet->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildChargingStation(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CHARGING_STATION, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
        GNEChargingStation* chargingStation = new GNEChargingStation(id, lane, viewNet, startPos, endPos, chargingPower, efficiency, chargeInTransit, chargeDelay);
        viewNet->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE1(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal pos, SUMOReal freq, const std::string& filename, bool splitByType) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E1DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
        GNEDetectorE1* detectorE1 = new GNEDetectorE1(id, lane, viewNet, pos, freq, filename, splitByType);
        viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename,
                                      bool cont, const SUMOReal timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E2DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lane, viewNet, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold);
        viewNet->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorE3(GNEViewNet* viewNet, const std::string& id, Position pos, SUMOReal freq, const std::string& filename, const SUMOReal timeThreshold, SUMOReal speedThreshold) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
        GNEDetectorE3* detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, timeThreshold, speedThreshold);
        viewNet->getUndoList()->add(new GNEChange_Additional(detectorE3, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, SUMOReal pos) {
    // Check if Detector E3 parent and lane is correct
    if (lane == NULL) {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
        return false;
    } else if (E3Parent == NULL) {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
        return false;
    } else {
        // insert E3 parent in net if previoulsy wasn't inserted
        if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(E3Parent, true), true);
            viewNet->getUndoList()->p_end();
        }
        // Create detector Entry if don't exist already in the net
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
        GNEDetectorEntry* entry = new GNEDetectorEntry(viewNet, E3Parent, lane, pos);
        viewNet->getUndoList()->add(new GNEChange_Additional(entry, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorExit(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, SUMOReal pos) {
    // Check if Detector E3 parent and lane is correct
    if (lane == NULL) {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
        return false;
    } else if (E3Parent == NULL) {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
        return false;
    } else {
        // insert E3 parent in net if previoulsy wasn't inserted
        if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(E3Parent, true), true);
            viewNet->getUndoList()->p_end();
        }
        // Create detector Exit if don't exist already in the net
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
        GNEDetectorExit* exit = new GNEDetectorExit(viewNet, E3Parent, lane, pos);
        viewNet->getUndoList()->add(new GNEChange_Additional(exit, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal pos, const std::string& outfile, const SUMOReal freq, const std::vector<GNECalibrator::GNECalibratorFlow*>& flowValues) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
        GNECalibrator* calibrator = new GNECalibrator(id, edge, viewNet, pos, freq, outfile, flowValues);
        viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, SUMOReal prob, const std::string& file, bool off) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
        GNERerouter* rerouter = new GNERerouter(id, viewNet, pos, edges, file, prob, off);
        viewNet->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal freq, const std::string& file, SUMOReal begin) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
        GNERouteProbe* routeProbe = new GNERouteProbe(id, viewNet, edge, freq, file, begin);
        viewNet->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& file, const std::map<SUMOReal, SUMOReal>& VSSValues) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VSS, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, file, VSSValues);
        viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal startTime, SUMOReal end) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VAPORIZER, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        GNEVaporizer* vaporizer = new GNEVaporizer(id, viewNet, edge, startTime, end);
        viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_VAPORIZER) + " with ID '" + id + "' in netedit; probably declared twice.");
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
        WRITE_WARNING("No filename given.");
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
    SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok, false);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
    if (!ok) {
        WRITE_WARNING("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = lane.getLaneShapeLenght() + pos;
    }
    if (pos > lane.getLaneShapeLenght()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLenght() - (SUMOReal) 0.1;
        } else {
            WRITE_WARNING("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane.getID() + "' length.");
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


template <typename T> T
GNEAdditionalHandler::getParsedAttribute(const SUMOSAXAttributes& attrs, const char* objectid, SumoXMLTag tag, SumoXMLAttr attribute, bool& abort, bool report) {
    bool ok = true;
    std::string parsedAttribute = "0";
    // only show one warning for every error/warning loading additional
    if (!abort) {
        // set additionalOfWarningMessage
        std::string additionalOfWarningMessage;
        if (objectid) {
            additionalOfWarningMessage = toString(tag) + " with ID '" + toString(objectid) + "'";
        } else {
            additionalOfWarningMessage = toString(tag);
        }
        // first check that attribute exists
        if (attrs.hasAttribute(attribute)) {
            // Parse attribute as string
            parsedAttribute = attrs.get<std::string>(attribute, objectid, ok, false);
            // check that parsed attribute can be converted to type T
            if (ok && !GNEAttributeCarrier::canParse<T>(parsedAttribute)) {
                ok = false;
            }
            std::string errorFormat;
            // Set extra checks for int values
            if (GNEAttributeCarrier::isInt(tag, attribute)) {
                if (GNEAttributeCarrier::canParse<int>(parsedAttribute)) {
                    // parse to int and check if can be negative
                    int parsedIntAttribute = GNEAttributeCarrier::parse<int>(parsedAttribute);
                    if (GNEAttributeCarrier::isPositive(tag, attribute) && parsedIntAttribute < 0) {
                        errorFormat = "Cannot be negative; ";
                        ok = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to int; ";
                    ok = false;
                }
            }
            // Set extra checks for float(SUMOReal) values
            if (GNEAttributeCarrier::isFloat(tag, attribute)) {
                if (GNEAttributeCarrier::canParse<SUMOReal>(parsedAttribute)) {
                    // parse to SUMOReal and check if can be negative
                    SUMOReal parsedSumoRealAttribute = GNEAttributeCarrier::parse<SUMOReal>(parsedAttribute);
                    if (GNEAttributeCarrier::isPositive(tag, attribute) && parsedSumoRealAttribute < 0) {
                        errorFormat = "Cannot be negative; ";
                        ok = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to float; ";
                    ok = false;
                }
            }
            // set extra check for time(SUMOReal) values
            if (GNEAttributeCarrier::isTime(tag, attribute)) {
                if (GNEAttributeCarrier::canParse<SUMOReal>(parsedAttribute)) {
                    // parse to SUMO Real and check if is negative
                    SUMOReal parsedSumoRealAttribute = GNEAttributeCarrier::parse<SUMOReal>(parsedAttribute);
                    if (parsedSumoRealAttribute < 0) {
                        errorFormat = "Time cannot be negative; ";
                        ok = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to time; ";
                    ok = false;
                }
            }
            // If attribute has an invalid format
            if (!ok) {
                // if attribute has a default value, take it as string. In other case, abort.
                if (GNEAttributeCarrier::hasDefaultValue(tag, attribute)) {
                    parsedAttribute = toString(GNEAttributeCarrier::getDefaultValue<T>(tag, attribute));
                    // report warning of default value
                    if (report) {
                        WRITE_WARNING("Format of optional " + GNEAttributeCarrier::getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                      additionalOfWarningMessage + " is invalid; " + errorFormat + "Default value '" + toString(parsedAttribute) + "' will be used.");
                    }
                } else {
                    WRITE_WARNING("Format of essential " + GNEAttributeCarrier::getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                  additionalOfWarningMessage +  " is invalid; " + errorFormat + "Additional cannot be created");
                    // set default value of parsedAttribute (to avoid exceptions during conversions)
                    parsedAttribute = "0";
                    abort = true;
                }
            }
        } else {
            // if attribute has a default value, take it. In other case, abort.
            if (GNEAttributeCarrier::hasDefaultValue(tag, attribute)) {
                parsedAttribute = toString(GNEAttributeCarrier::getDefaultValue<T>(tag, attribute));
                // report warning of default value
                if (report) {
                    WRITE_WARNING("Optional " + GNEAttributeCarrier::getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                                  additionalOfWarningMessage + " is missing; Default value '" + toString(parsedAttribute) + "' will be used.");
                }
            } else {
                WRITE_WARNING("Essential " + GNEAttributeCarrier::getAttributeType(tag, attribute) + " attribute '" + toString(attribute) + "' of " +
                              additionalOfWarningMessage +  " is missing; Additional cannot be created");
                abort = true;
            }
        }
    }
    // return parsed attribute
    return GNEAttributeCarrier::parse<T>(parsedAttribute);
}


bool
GNEAdditionalHandler::getFriendlyPosition(const SUMOSAXAttributes& attrs, const char* objectid) {
    bool ok = true;
    return attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, objectid, ok, false);
}


void
GNEAdditionalHandler::resetLastTag() {
    myLastTag = SUMO_TAG_NOTHING;
    if (myE3Parent != NULL && ((myE3Parent->getNumberOfEntryChilds() + myE3Parent->getNumberOfExitChilds()) == 0)) {
        WRITE_WARNING((toString(myE3Parent->getTag()) + "s without " + toString(SUMO_TAG_DET_ENTRY) + "s or " + toString(SUMO_TAG_DET_EXIT) + " aren't allowed; " +
                       toString(myE3Parent->getTag()) + " with ID = '" + myE3Parent->getID() + "' cannot be created.").c_str());
        delete myE3Parent;
        myE3Parent = NULL;
    }
}


bool
GNEAdditionalHandler::checkAdditionalParent(SumoXMLTag currentTag) {
    // If last tag was an E3 but next tag isn't an Entry or Exit
    if (((myLastTag == SUMO_TAG_E3DETECTOR) || (myLastTag == SUMO_TAG_ENTRY_EXIT_DETECTOR)) &&
            !((currentTag == SUMO_TAG_DET_ENTRY) || (currentTag == SUMO_TAG_DET_EXIT))) {
        // Remove created E3 to avoid load empty detectors
        if (myE3Parent != NULL) {
            // show E3 empty warning and  delete empty E3
            WRITE_WARNING((toString(myE3Parent->getTag()) + "s without " + toString(SUMO_TAG_DET_ENTRY) + "s or " + toString(SUMO_TAG_DET_EXIT) + " aren't allowed; " +
                           toString(myE3Parent->getTag()) + " with ID = '" + myE3Parent->getID() + "' cannot be created.").c_str());
            delete myE3Parent;
            myE3Parent = NULL;
        }
        // continue with the processing of additional
        return true;
    }

    // if last tag wasn't an E3 but next tag is an entry or exit
    if (!((myLastTag == SUMO_TAG_E3DETECTOR) || (myLastTag == SUMO_TAG_ENTRY_EXIT_DETECTOR)) &&
            ((currentTag == SUMO_TAG_DET_ENTRY) || (currentTag == SUMO_TAG_DET_EXIT))) {
        if (myE3Parent != NULL) {
            // In this case, we're loading a E3 with multiple entry exits, then continue
            return true;
        } else {
            // return false to stop procesing current entry or exit and go to the next tag (this avoid some useless warnings)
            return false;
        }
    } else {
        // all OK
        return true;
    }
}

/****************************************************************************/
