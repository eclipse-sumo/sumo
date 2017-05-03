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
#include "GNECalibrator.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myE3Parent(NULL),
    myCalibratorParent(NULL),
    myVariableSpeedSignParent(NULL),
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
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_CONTAINER_STOP:
                parseAndBuildContainerStop(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseAndBuildChargingStation(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                break;
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                parseAndBuildDetectorE1(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                parseAndBuildDetectorE2(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                parseAndBuildDetectorE3(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_DET_ENTRY:
                parseAndBuildDetectorEntry(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_DET_EXIT:
                parseAndBuildDetectorExit(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                break;
            case SUMO_TAG_VSS:
                parseAndBuildVariableSpeedSign(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                break;
            case SUMO_TAG_REROUTER:
                parseAndBuildRerouter(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_CALIBRATOR:
                parseAndBuildCalibrator(attrs, tag);
                // disable other additional parents
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_VAPORIZER:
                parseAndBuildVaporizer(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_ROUTEPROBE:
                parseAndBuildRouteProbe(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_VTYPE:
                parseCalibratorVehicleType(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_ROUTE:
                parseCalibratorRoute(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
                // disable other additional parents
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_FLOW:
                parseCalibratorFlow(attrs, tag);
                // disable other additional parents
                myE3Parent = NULL;
                myVariableSpeedSignParent = NULL;
                break;
            case SUMO_TAG_STEP:
                parseVariableSpeedSignStep(attrs, tag);
                // disable other additional parents
                myCalibratorParent = NULL;
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
    const std::string edgeId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    double startTime = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_STARTTIME, abort);
    double endTime = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " is not known.");
        } else if (startTime > endTime) {
            // write error if time interval ins't valid
            WRITE_WARNING("Time interval of " + toString(tag) + " isn't valid. Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
        } else {
            // build Vaporizer
            buildVaporizer(myViewNet, edge, startTime, endTime);
        }
    }
}



void
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string edgeId = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    double freq = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_FILE, abort);
    double begin = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_BEGIN, abort);
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
GNEAdditionalHandler::parseCalibratorRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator routes
    std::string routeID = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::vector<std::string> edgeIDs = getParsedAttribute<std::vector<std::string> >(attrs, 0, tag, SUMO_ATTR_EDGES, abort);
    std::string color = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_COLOR, abort);

    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if already exist a route with the same ID
        if (myViewNet->getNet()->routeExists(routeID)) {
            WRITE_WARNING(toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' cannot be created; Another " +
                          toString(SUMO_TAG_ROUTE) + " with the same ID was previously declared");
            abort = true;
        }
        // declare vector with pointers to GNEEdges
        std::vector<GNEEdge*> edges;
        for (std::vector<std::string>::const_iterator i = edgeIDs.begin(); (i != edgeIDs.end()) && (abort == false); i++) {
            GNEEdge* retrievedEdge = myViewNet->getNet()->retrieveEdge((*i), false);
            // stop
            if (retrievedEdge != NULL) {
                edges.push_back(retrievedEdge);
            } else {
                WRITE_WARNING(toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' cannot be created; " +
                              toString(SUMO_TAG_EDGE) + " with id '" + (*i) + "' doesn't exist in net");
            }
        }
        // create vehicle type if calibrator parent is currently defined
        if ((myCalibratorParent != NULL) && (abort == false)) {
            // create vehicle type and add it to calibrator parent
            GNECalibratorRoute route(myCalibratorParent, routeID, edges, color);
            myCalibratorParent->addCalibratorRoute(route);
        }
    }
}


void
GNEAdditionalHandler::parseCalibratorVehicleType(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;

    // parse attribute of calibrator vehicle types
    std::string vehicleTypeID = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    double accel = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_ACCEL, abort);
    double decel = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_DECEL, abort);
    double sigma = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_SIGMA, abort);
    double tau = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_TAU, abort);
    double length = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_LENGTH, abort);
    double minGap = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_MINGAP, abort);
    double maxSpeed = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_MAXSPEED, abort);
    double speedFactor = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_SPEEDFACTOR, abort);
    double speedDev = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_SPEEDDEV, abort);
    std::string color = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_COLOR, abort);
    SUMOVehicleClass vClass = getParsedAttribute<SUMOVehicleClass>(attrs, 0, tag, SUMO_ATTR_VCLASS, abort);
    std::string emissionClass = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_EMISSIONCLASS, abort);
    SUMOVehicleShape shape = getParsedAttribute<SUMOVehicleShape>(attrs, 0, tag, SUMO_ATTR_GUISHAPE, abort);
    double width = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_WIDTH, abort);
    std::string filename = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_IMGFILE, abort);
    double impatience = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_IMPATIENCE, abort);
    std::string laneChangeModel = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_LANE_CHANGE_MODEL, abort);
    std::string carFollowModel = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_CAR_FOLLOW_MODEL, abort);
    int personCapacity = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_PERSON_CAPACITY, abort);
    int containerCapacity = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_CONTAINER_CAPACITY, abort);
    double boardingDuration = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_BOARDING_DURATION, abort);
    double loadingDuration = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_LOADING_DURATION, abort);
    std::string latAlignment = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_LATALIGNMENT, abort);
    double minGapLat = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_MINGAP_LAT, abort);
    double maxSpeedLat = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_MAXSPEED_LAT, abort);

    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if already exist a vehicleType with the same ID
        if (myViewNet->getNet()->vehicleTypeExists(vehicleTypeID)) {
            WRITE_WARNING(toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' cannot be created; Another " + toString(SUMO_TAG_VTYPE) + " with the same ID was previously declared");
        } else if (myCalibratorParent != NULL) {
            // create vehicle type and add it to calibrator parent
            GNECalibratorVehicleType vehicleType(myCalibratorParent, vehicleTypeID, accel, decel, sigma, tau, length, minGap, maxSpeed,
                                                 speedFactor, speedDev, color, vClass, emissionClass, shape, width, filename, impatience,
                                                 laneChangeModel, carFollowModel, personCapacity, containerCapacity, boardingDuration,
                                                 loadingDuration, latAlignment, minGapLat, maxSpeedLat);
            myCalibratorParent->addCalibratorVehicleType(vehicleType);
        }
    }
}


void
GNEAdditionalHandler::parseCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;

    // parse attributes of calibrator flows
    std::string flowID = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string vehicleType = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_TYPE, abort);
    std::string route = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ROUTE, abort);
    std::string color = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_COLOR, abort);
    std::string departLane = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTLANE, abort);
    std::string departPos = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTPOS, abort);
    std::string departSpeed = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTSPEED, abort);
    std::string arrivalLane = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALLANE, abort);
    std::string arrivalPos = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALPOS, abort);
    std::string arrivalSpeed = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALSPEED, abort);
    std::string line = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_LINE, abort);
    int personNumber = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_PERSON_NUMBER, abort);
    int containerNumber = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_CONTAINER_NUMBER, abort);
    bool reroute = getParsedAttribute<bool>(attrs, 0, tag, SUMO_ATTR_REROUTE, abort);
    std::string departPosLat = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTPOS_LAT, abort);
    std::string arrivalPosLat = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALPOS_LAT, abort);
    double begin = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_BEGIN, abort);
    double end = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_END, abort);
    double vehsPerHour = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_VEHSPERHOUR, abort);
    double period = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_PERIOD, abort);
    double probability = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_PROB, abort);
    int number = getParsedAttribute<int>(attrs, 0, tag, SUMO_ATTR_NUMBER, abort);

    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if flowID, route and vehicle type already exists
        if (myViewNet->getNet()->flowExists(flowID) == false) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW) + " with ID = '" + flowID + "' cannot be created; Another " + toString(SUMO_TAG_FLOW) + " with the same ID was previously declared");
            abort = true;
        } else if (myViewNet->getNet()->routeExists(route) == false) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW) + " with ID = '" + flowID + "' cannot be created; their " + toString(SUMO_TAG_ROUTE) + " with ID = '" + route + "' doesn't exist");
            abort = true;
        } else if (myViewNet->getNet()->vehicleTypeExists(vehicleType) == false) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW) + " with ID = '" + flowID + "' cannot be created; their " + toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleType + "' doesn't exist");
            abort = true;
        }
        // check if distributions are correct and calibrator parent is defined
        if ((myCalibratorParent != NULL) && (abort == false)) {
            // obtain type of distribution
            GNECalibratorFlow::TypeOfFlow flowType = getTypeOfFlowDistribution(flowID, vehsPerHour, period, probability);
            if (flowType == GNECalibratorFlow::GNE_CALIBRATORFLOW_INVALID) {
                WRITE_WARNING(toString(SUMO_TAG_FLOW) + " with ID = '" + flowID + "' cannot be created; Type of distribution undefined");
            } else {
                // create Flow and add it to calibrator parent
                GNECalibratorFlow flow(myCalibratorParent, flowID, vehicleType, route, color, departLane, departPos, departSpeed,
                                       arrivalLane, arrivalPos, arrivalSpeed, line, personNumber, containerNumber, reroute,
                                       departPosLat, arrivalPosLat, begin, end, vehsPerHour, period, probability, number);
                myCalibratorParent->addCalibratorFlow(flow);
            }
        }
    }
}


void
GNEAdditionalHandler::parseVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // Load step values
    double time = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_TIME, abort);
    double speed = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_SPEED, abort);
    // Continue if all parameters were sucesfully loaded
    if ((!abort) && (myVariableSpeedSignParent != NULL)) {
        // create step and check that is valid
        GNEVariableSpeedSignStep step(myVariableSpeedSignParent, time, speed);
        // show warning if is duplicated
        if (std::find(myVariableSpeedSignParent->getSteps().begin(), myVariableSpeedSignParent->getSteps().end(), step) != myVariableSpeedSignParent->getSteps().end()) {
            WRITE_WARNING(toString(step.getTag()) + " cannot be inserted into " + toString(myVariableSpeedSignParent->getTag()) + " with id = '" + myVariableSpeedSignParent->getID() +
                          "'; Already exist another " + toString(step.getTag()) + " with the same " + toString(SUMO_ATTR_TIME) + ".");
        } else {
            myVariableSpeedSignParent->addStep(step);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildVariableSpeedSign(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of VSS
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    std::vector<std::string> lanesID = getParsedAttribute<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LANES, abort);
    double posx = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain VSS Values
        // @todo
        std::vector<GNEVariableSpeedSignStep> steps;
        // Obtain pointer to lanes
        std::vector<GNELane*> lanes;
        for (std::vector<std::string>::iterator i = lanesID.begin(); (i < lanesID.end()) && (abort == false); i++) {
            GNELane* lane = myViewNet->getNet()->retrieveLane((*i), false);
            if (lane != NULL) {
                lanes.push_back(lane);
            } else {
                WRITE_WARNING(toString(SUMO_TAG_VSS) + " with ID = '" + id + "' cannot be created; " + toString(SUMO_TAG_LANE) + " '" + (*i) + "' doesn't exist.");
                abort = true;
            }
        }
        // if operation of build variable speed signal was sucesfully, save Id
        if ((abort == false) && buildVariableSpeedSign(myViewNet, id, Position(posx, posy), lanes, file, steps)) {
            myVariableSpeedSignParent = dynamic_cast<GNEVariableSpeedSign*>(myViewNet->getNet()->retrieveAdditional(id));
            myLastTag = myVariableSpeedSignParent->getTag();
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = getParsedAttribute<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::vector<std::string> edgesID = getParsedAttribute<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_EDGES, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    double probability = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_PROB, abort);
    bool off = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_OFF, abort);
    double posx = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
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
    double startPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::vector<std::string> lines = getParsedAttribute<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLength(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
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
    double startPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::vector<std::string> lines = getParsedAttribute<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLength(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
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
    double startPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    double chrgpower = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGINGPOWER, abort);
    double efficiency = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEINTRANSIT, abort);
    double chargeDelay = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEDELAY, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkStopPos(startPos, endPos, lane->getLaneShapeLength(), POSITION_EPS, getFriendlyPosition(attrs, id.c_str()))) {
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
    double position = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    double freq = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    // std::string routeProbe = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
    std::vector<GNECalibratorRoute> calibratorRoutes;
    std::vector<GNECalibratorFlow> calibratorFlows;
    std::vector<GNECalibratorVehicleType> calibratorVehicleTypes;
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (buildCalibrator(myViewNet, id, lane, position, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes)) {
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
    double position = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    double frequency = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    bool splitByType = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_SPLIT_VTYPE, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLength()))) {
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
    double position = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    double frequency = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    double length = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_LENGTH, abort);
    double haltingTimeThreshold = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double jamDistThreshold = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool cont = getParsedAttribute<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CONT, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if ((position < 0) || ((position + length) > (lane->getLaneShapeLength()))) {
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
    double frequency = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = getParsedAttribute<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort);
    double haltingTimeThreshold = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double posx = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = getParsedAttribute<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
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
    double position = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLength()))) {
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
    double position = getParsedAttribute<double>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if ((position < 0) || (position > (lane->getLaneShapeLength()))) {
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
            double startPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_STARTPOS]);
            double endPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
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
            double startPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_STARTPOS]);
            double endPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
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
            double startPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_STARTPOS]);
            double endPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ENDPOS]);
            double chargingPower = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGINGPOWER]);
            double efficiency = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            double chargeDelay = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGEDELAY]);
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
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
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
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            double length = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_LENGTH]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool cont = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CONT]);
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            double speedThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            double jamThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_JAM_DIST_THRESHOLD]);
            // Build detector E2
            if (lane) {
                return buildDetectorE2(viewNet, id, lane, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E3DETECTOR: {
            // obtain specify attributes of detector E3
            bool ok;
            std::string id = values[SUMO_ATTR_ID];
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            double speedThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
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
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
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
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
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
            std::vector<std::string> laneIds = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LANES]);
            // By default, steps are empty
            std::vector<GNEVariableSpeedSignStep> steps;
            // Obtain pointers to lanes
            std::vector<GNELane*> lanes;
            for (int i = 0; i < (int)laneIds.size(); i++) {
                lanes.push_back(viewNet->getNet()->retrieveLane(laneIds.at(i)));
            }
            std::string file = values[SUMO_ATTR_FILE];
            if (pos.size() == 1) {
                return buildVariableSpeedSign(viewNet, id, pos[0], lanes, file, steps);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            // get rest of parameters
            // Currently unused double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            double pos = 0;
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // get Calibrator values
            std::vector<GNECalibratorRoute> calibratorRoutes;
            std::vector<GNECalibratorFlow> calibratorFlows;
            std::vector<GNECalibratorVehicleType> calibratorVehicleTypes;
            // Build calibrator
            if (lane) {
                return buildCalibrator(viewNet, id, lane, pos, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
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
            std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_EDGES]);
            // Get rest of parameters
            bool off = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_OFF]);
            double prob = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_PROB]);
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
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            double begin = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_BEGIN]);
            // Build RouteProbe
            if (edge) {
                return buildRouteProbe(viewNet, id, edge, freq, filename, begin);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VAPORIZER: {
            // obtain specify attributes of vaporizer
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            double startTime = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_STARTTIME]);
            double end = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_END]);
            // Build RouteProbe
            if (edge) {
                return buildVaporizer(viewNet, edge, startTime, end);
            } else {
                return false;
            }
        }
        default:
            return false;
    }
}


bool
GNEAdditionalHandler::buildBusStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double startPos, double endPos, const std::vector<std::string>& lines) {
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
GNEAdditionalHandler::buildContainerStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double startPos, double endPos, const std::vector<std::string>& lines) {
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
GNEAdditionalHandler::buildChargingStation(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double startPos, double endPos, double chargingPower, double efficiency, bool chargeInTransit, double chargeDelay) {
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
GNEAdditionalHandler::buildDetectorE1(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double pos, double freq, const std::string& filename, bool splitByType) {
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
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double pos, double length, double freq, const std::string& filename,
                                      bool cont, const double timeThreshold, double speedThreshold, double jamThreshold) {
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
GNEAdditionalHandler::buildDetectorE3(GNEViewNet* viewNet, const std::string& id, Position pos, double freq, const std::string& filename, const double timeThreshold, double speedThreshold) {
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
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, double pos) {
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
GNEAdditionalHandler::buildDetectorExit(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, double pos) {
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
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, const std::string& id, GNELane* lane, double pos, const std::string& outfile, const double freq,
                                      const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows,
                                      const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
        GNECalibrator* calibrator = new GNECalibrator(id, lane, viewNet, pos, freq, outfile, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
        viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& file, bool off) {
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
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, double freq, const std::string& file, double begin) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, id) == NULL) {
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
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& file, const std::vector<GNEVariableSpeedSignStep>& steps) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VSS, id) == NULL) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, file, steps);
        viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        WRITE_WARNING("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, GNEEdge* edge, double startTime, double end) {
    viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
    GNEVaporizer* vaporizer = new GNEVaporizer(viewNet, edge, startTime, end);
    viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
    viewNet->getUndoList()->p_end();
    return true;
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


double
GNEAdditionalHandler::getPosition(const SUMOSAXAttributes& attrs, GNELane& lane, const std::string& tt, const std::string& tid) {
    bool ok = true;
    double pos = attrs.get<double>(SUMO_ATTR_POSITION, 0, ok, false);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
    if (!ok) {
        WRITE_WARNING("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = lane.getLaneShapeLength() + pos;
    }
    if (pos > lane.getLaneShapeLength()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLength() - (double) 0.1;
        } else {
            WRITE_WARNING("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool
GNEAdditionalHandler::checkStopPos(double& startPos, double& endPos, const double laneLength, const double minLength,  const bool friendlyPos) {
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
            // Check if is the attribute is a file name or special attribute "COLOR"
            if (!ok && ((attribute == SUMO_ATTR_COLOR) || GNEAttributeCarrier::isFilename(tag, attribute))) {
                ok = true;
            }
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
            // Set extra checks for float(double) values
            if (GNEAttributeCarrier::isFloat(tag, attribute)) {
                if (GNEAttributeCarrier::canParse<double>(parsedAttribute)) {
                    // parse to double and check if can be negative
                    double parsedSumoRealAttribute = GNEAttributeCarrier::parse<double>(parsedAttribute);
                    if (GNEAttributeCarrier::isPositive(tag, attribute) && parsedSumoRealAttribute < 0) {
                        errorFormat = "Cannot be negative; ";
                        ok = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to float; ";
                    ok = false;
                }
            }
            // set extra check for time(double) values
            if (GNEAttributeCarrier::isTime(tag, attribute)) {
                if (GNEAttributeCarrier::canParse<double>(parsedAttribute)) {
                    // parse to SUMO Real and check if is negative
                    double parsedSumoRealAttribute = GNEAttributeCarrier::parse<double>(parsedAttribute);
                    if (parsedSumoRealAttribute < 0) {
                        errorFormat = "Time cannot be negative; ";
                        ok = false;
                    }
                } else {
                    errorFormat = "Cannot be parsed to time; ";
                    ok = false;
                }
            }
            // set extra check for filename values
            if (GNEAttributeCarrier::isFilename(tag, attribute) && (GNEAttributeCarrier::isValidFilename(parsedAttribute) == false)) {
                errorFormat = "Filename contains invalid characters; ";
                ok = false;
            }
            // set extra check for Vehicle Classes
            if ((!ok) && (attribute == SUMO_ATTR_VCLASS)) {
                errorFormat = "Is not a part of defined set of Vehicle Classes; ";
            }
            // set extra check for Vehicle Classes
            if ((!ok) && (attribute == SUMO_ATTR_GUISHAPE)) {
                errorFormat = "Is not a part of defined set of Gui Vehicle Shapes; ";
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


GNECalibratorFlow::TypeOfFlow
GNEAdditionalHandler::getTypeOfFlowDistribution(std::string flowID, double vehsPerHour, double period, double probability) {
    if ((vehsPerHour == -1) && (period == -1) && (probability == -1)) {
        WRITE_WARNING("A type of distribution (" + toString(SUMO_ATTR_VEHSPERHOUR) + ", " +  toString(SUMO_ATTR_PERIOD) + " or " +
                      toString(SUMO_ATTR_PROB) + ") must be defined in " + toString(SUMO_TAG_FLOW) +  " '" + flowID + "'");
        return GNECalibratorFlow::GNE_CALIBRATORFLOW_INVALID;
    } else {
        int vehsPerHourDefined = (vehsPerHour != -1) ? 1 : 0;
        int periodDefined = (period != -1) ? 1 : 0;
        int probabilityDefined = (probability != -1) ? 1 : 0;

        if ((vehsPerHourDefined + periodDefined + probabilityDefined) != 1) {
            WRITE_WARNING("Only a type of distribution (" + toString(SUMO_ATTR_VEHSPERHOUR) + ", " +  toString(SUMO_ATTR_PERIOD) + " or " +
                          toString(SUMO_ATTR_PROB) + ") can be defined at the same time in " + toString(SUMO_TAG_FLOW) + " '" + flowID + "'");
            return GNECalibratorFlow::GNE_CALIBRATORFLOW_INVALID;
        } else if (vehsPerHourDefined == 1) {
            return GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR;
        } else if (periodDefined == 1) {
            return GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD;
        } else if (probabilityDefined == 1) {
            return GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY;
        } else {
            return GNECalibratorFlow::GNE_CALIBRATORFLOW_INVALID;
        }
    }
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
    }

    // if last tag was a Calibrator but next tag is a vehicle type, route or flow
    if (!(myLastTag == SUMO_TAG_CALIBRATOR) && ((currentTag == SUMO_TAG_ROUTE) || (currentTag == SUMO_TAG_FLOW) || (currentTag == SUMO_TAG_VTYPE))) {
        if (myCalibratorParent != NULL) {
            // In this case, we're loading a Calibrator with multiple routes/flows/vehicleTypes, then continue
            return true;
        } else {
            // return false to stop procesing current route/flow/vehicleType and go to the next tag (this avoid some useless warnings)
            return false;
        }
    }

    // if last tag wasn't a Variable speed Sign but next tag is a step
    if (!(myLastTag != SUMO_TAG_CALIBRATOR) && (currentTag == SUMO_TAG_STEP)) {
        if (myVariableSpeedSignParent != NULL) {
            // In this case, we're loading a Variable Speed Signal with multiple steps, then continue
            return true;
        } else {
            // return false to stop procesing current step and go to the next tag (this avoid some useless warnings)
            return false;
        }
    }

    // all OK
    return true;
}

/****************************************************************************/
