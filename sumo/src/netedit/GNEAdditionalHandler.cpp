/****************************************************************************/
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Builds trigger objects for netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include "GNECalibratorEdge.h"
#include "GNECalibratorLane.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, bool undoAdditionals) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myUndoAdditionals(undoAdditionals),
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
    const std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    double startTime = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_STARTTIME, abort);
    double endTime = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_END, abort);
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
            buildVaporizer(myViewNet, myUndoAdditionals, edge, startTime, endTime);
        }
    }
}



void
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_EDGE, abort);
    double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_FILE, abort, false);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_BEGIN, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else {
            // build Vaporizer
            buildRouteProbe(myViewNet, myUndoAdditionals, id, edge, freq, file, begin);
        }
    }
}


void
GNEAdditionalHandler::parseCalibratorRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator routes
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::vector<std::string> edgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, 0, tag, SUMO_ATTR_EDGES, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, 0, tag, SUMO_ATTR_COLOR, abort, false);

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
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    double accel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_ACCEL, abort);
    double decel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_DECEL, abort);
    double sigma = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_SIGMA, abort);
    double tau = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_TAU, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_LENGTH, abort);
    double minGap = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_MINGAP, abort);
    double maxSpeed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_MAXSPEED, abort);
    double speedFactor = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_SPEEDFACTOR, abort);
    double speedDev = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_SPEEDDEV, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, 0, tag, SUMO_ATTR_COLOR, abort, false);
    SUMOVehicleClass vClass = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleClass>(attrs, 0, tag, SUMO_ATTR_VCLASS, abort);
    std::string emissionClass = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_EMISSIONCLASS, abort);
    SUMOVehicleShape shape = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleShape>(attrs, 0, tag, SUMO_ATTR_GUISHAPE, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_WIDTH, abort);
    std::string filename = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_IMGFILE, abort);
    double impatience = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_IMPATIENCE, abort);
    std::string laneChangeModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_LANE_CHANGE_MODEL, abort);
    std::string carFollowModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_CAR_FOLLOW_MODEL, abort);
    int personCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, 0, tag, SUMO_ATTR_PERSON_CAPACITY, abort);
    int containerCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, 0, tag, SUMO_ATTR_CONTAINER_CAPACITY, abort);
    double boardingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_BOARDING_DURATION, abort);
    double loadingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_LOADING_DURATION, abort);
    std::string latAlignment = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_LATALIGNMENT, abort);
    double minGapLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_MINGAP_LAT, abort);
    double maxSpeedLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_MAXSPEED_LAT, abort);

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
    std::string flowID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string vehicleType = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_TYPE, abort);
    std::string route = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ROUTE, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, 0, tag, SUMO_ATTR_COLOR, abort, false);
    std::string departLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTLANE, abort);
    std::string departPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTPOS, abort);
    std::string departSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTSPEED, abort);
    std::string arrivalLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALLANE, abort);
    std::string arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALPOS, abort);
    std::string arrivalSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALSPEED, abort);
    std::string line = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_LINE, abort);
    int personNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, 0, tag, SUMO_ATTR_PERSON_NUMBER, abort);
    int containerNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, 0, tag, SUMO_ATTR_CONTAINER_NUMBER, abort);
    bool reroute = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, 0, tag, SUMO_ATTR_REROUTE, abort);
    std::string departPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_DEPARTPOS_LAT, abort);
    std::string arrivalPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ARRIVALPOS_LAT, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_END, abort);
    double vehsPerHour = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_VEHSPERHOUR, abort);
    double period = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_PERIOD, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_PROB, abort);
    int number = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, 0, tag, SUMO_ATTR_NUMBER, abort);

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
    double time = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_TIME, abort);
    double speed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_SPEED, abort);
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
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort, false);
    std::vector<std::string> lanesID = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LANES, abort);
    double posx = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
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
        if ((abort == false) && buildVariableSpeedSign(myViewNet, myUndoAdditionals, id, Position(posx, posy), lanes, file, steps)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::vector<std::string> edgesID = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_EDGES, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort, false);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_PROB, abort);
    bool off = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_OFF, abort);
    double posx = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
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
        if (buildRerouter(myViewNet, myUndoAdditionals, id, Position(posx, posy), edges, probability, file, off)) {
            ;//myAdditionalParent = id;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of bus stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_NAME, abort, false);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // Write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildBusStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildContainerStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of container stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_NAME, abort, false);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id.c_str(), tag, SUMO_ATTR_LINES, abort, false);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildContainerStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_NAME, abort, false);
    double chargingPower = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGINGPOWER, abort);
    double efficiency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEINTRANSIT, abort);
    double chargeDelay = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_CHARGEDELAY, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildChargingStation(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // due there is two differents calibratos, has to be parsed in a different way
    std::string edgeID, laneId, id;
    SumoXMLTag typeOfCalibrator = tag;
    // change tag depending of XML parmeters
    if(attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        typeOfCalibrator = SUMO_TAG_CALIBRATOR_EDGE;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, typeOfCalibrator, SUMO_ATTR_ID, abort);
        edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), typeOfCalibrator, SUMO_ATTR_EDGE, abort, false);
    } else if(attrs.hasAttribute(SUMO_ATTR_LANE)) {
        typeOfCalibrator = SUMO_TAG_CALIBRATOR_EDGE;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, typeOfCalibrator, SUMO_ATTR_ID, abort);
        laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), typeOfCalibrator, SUMO_ATTR_LANE, abort, false);
    } else {
        WRITE_WARNING("additional " + toString(tag) + " must have either a lane or an edge attribute.");

    }
    // if loading first calibrators values was sucesfully, continue)
    if(!abort) {
        // parse rest of attributes of calibrator
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), typeOfCalibrator, SUMO_ATTR_OUTPUT, abort, false);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), typeOfCalibrator, SUMO_ATTR_POSITION, abort);
        double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), typeOfCalibrator, SUMO_ATTR_FREQUENCY, abort);
        // std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
        std::vector<GNECalibratorRoute> calibratorRoutes;
        std::vector<GNECalibratorFlow> calibratorFlows;
        std::vector<GNECalibratorVehicleType> calibratorVehicleTypes;
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer to lane and edge
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
            GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
            // check that all parameters are valid
            if (edgeID != "" && laneId != "") {
                WRITE_WARNING("additional " + toString(typeOfCalibrator) + " with id '" + id + "' can be only placed over either a lane or an edge, not in both.");
            } else if (GNEAttributeCarrier::isValidID(id) == false) {
                WRITE_WARNING("The id '" + id + "' of additional " + toString(typeOfCalibrator) + " contains invalid characters.");
            } else if (myViewNet->getNet()->getAdditional(typeOfCalibrator, id) != NULL) {
                WRITE_WARNING("There is another " + toString(typeOfCalibrator) + " with the same ID='" + id + "'.");
            } else if (edge == NULL && lane == NULL) {
                WRITE_WARNING("The lane '" + laneId + "' or edge '" + edgeID + "' to use within the " + toString(typeOfCalibrator) + " '" + id + "' is not known.");
            } else if (((lane != NULL) && buildCalibratorLane(myViewNet, myUndoAdditionals, id, lane, position, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes)) ||
                       ((edge != NULL) && buildCalibratorEdge(myViewNet, myUndoAdditionals, id, edge, position, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes))) {
                myLastTag = tag;
            }
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E1
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort, false);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_VTYPES, abort, false);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildDetectorE1(myViewNet, myUndoAdditionals, id, lane, position, frequency, file, vehicleTypes, friendlyPos)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E2
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort, false);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_LENGTH, abort);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double jamDistThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool cont = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_CONT, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id.c_str(), tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixE2DetectorPositionPosition(position, length, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildDetectorE2(myViewNet, myUndoAdditionals, id, lane, position, length, frequency, file, cont, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, friendlyPos)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E3
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_ID, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id.c_str(), tag, SUMO_ATTR_FILE, abort, false);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double posx = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_X, abort);
    double posy = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id.c_str(), tag, SUMO_ATTR_Y, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check that all parameters are valid
        if (GNEAttributeCarrier::isValidID(id) == false) {
            WRITE_WARNING("The id '" + id + "' of additional " + toString(tag) + " contains invalid characters.");
            myE3Parent = NULL;
            myLastTag = SUMO_TAG_NOTHING;
        } else if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
            myE3Parent = NULL;
            myLastTag = SUMO_TAG_NOTHING;
        } else if (myViewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
            // Create without possibility of undo/redo
            myE3Parent = new GNEDetectorE3(id, myViewNet, Position(posx, posy), frequency, file, haltingTimeThreshold, haltingSpeedThreshold);
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, 0, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (lane == NULL) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (myE3Parent == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_E3DETECTOR) + ".");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else if (buildDetectorEntry(myViewNet, myUndoAdditionals, myE3Parent, lane, position, friendlyPos)) {
            myLastTag = tag;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Exit
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, 0, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, 0, tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, 0, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false);
        // check that all parameters are valid
        if (lane == NULL) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (myE3Parent == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_E3DETECTOR) + ".");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else if (buildDetectorExit(myViewNet, myUndoAdditionals, myE3Parent, lane, position, friendlyPos)) {
            myLastTag = tag;
        }
    }
}


bool
GNEAdditionalHandler::buildAdditional(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // create additional depending of the tag
    switch (tag) {
        case SUMO_TAG_BUS_STOP: {
            // obtain specify attributes of busStop
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double startPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_STARTPOS]);
            double endPos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ENDPOS]);
            std::string name = values[SUMO_ATTR_NAME];
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            // Build busStop
            if (lane) {
                return buildBusStop(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, lines, friendlyPos);
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
            std::string name = values[SUMO_ATTR_NAME];
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            // Build containerStop
            if (lane) {
                return buildContainerStop(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, lines, friendlyPos);
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
            std::string name = values[SUMO_ATTR_NAME];
            double chargingPower = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGINGPOWER]);
            double efficiency = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            double chargeDelay = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGEDELAY]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            // Build chargingStation
            if (lane) {
                return buildChargingStation(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPos);
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
            std::string vehicleTypes = values[SUMO_ATTR_VTYPES];
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            // Build detector E1
            if (lane) {
                return buildDetectorE1(viewNet, allowUndoRedo, id, lane, pos, freq, filename, vehicleTypes, friendlyPos);
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
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            // Build detector E2
            if (lane) {
                return buildDetectorE2(viewNet, allowUndoRedo, id, lane, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, friendlyPos);
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
                return buildDetectorE3(viewNet, allowUndoRedo, id, pos[0], freq, filename, timeThreshold, speedThreshold);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_ENTRY: {
            // obtain specify attributes of detector Entry
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            GNEDetectorE3* E3 = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->retrieveAdditional(values[GNE_ATTR_PARENT]));
            // Build detector Entry
            if (lane && E3) {
                return buildDetectorEntry(viewNet, allowUndoRedo, E3, lane, pos, friendlyPos);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_EXIT: {
            // obtain specify attributes of Detector Exit
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            GNEDetectorE3* E3 = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->retrieveAdditional(values[GNE_ATTR_PARENT]));
            // Build detector Exit
            if (lane && E3) {
                return buildDetectorExit(viewNet, allowUndoRedo, E3, lane, pos, friendlyPos);
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
                return buildVariableSpeedSign(viewNet, allowUndoRedo, id, pos[0], lanes, file, steps);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR_EDGE: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* lane = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // declare Calibrator values
            std::vector<GNECalibratorRoute> calibratorRoutes;
            std::vector<GNECalibratorFlow> calibratorFlows;
            std::vector<GNECalibratorVehicleType> calibratorVehicleTypes;
            // Build calibrator lane
            if (lane) {
                return buildCalibratorEdge(viewNet, allowUndoRedo, id, lane, pos, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR_LANE: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // declare Calibrator values
            std::vector<GNECalibratorRoute> calibratorRoutes;
            std::vector<GNECalibratorFlow> calibratorFlows;
            std::vector<GNECalibratorVehicleType> calibratorVehicleTypes;
            // Build calibrator lane
            if (lane) {
                return buildCalibratorLane(viewNet, allowUndoRedo, id, lane, pos, outfile, freq, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
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
                return buildRerouter(viewNet, allowUndoRedo, id, pos[0], edges, prob, file, off);
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
                return buildRouteProbe(viewNet, allowUndoRedo, id, edge, freq, filename, begin);
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
                return buildVaporizer(viewNet, allowUndoRedo, edge, startTime, end);
            } else {
                return false;
            }
        }
        default:
            return false;
    }
}


bool
GNEAdditionalHandler::buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_BUS_STOP, id) == NULL) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, name, lines, friendlyPosition);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(busStop);
            lane->addAdditionalChild(busStop);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildContainerStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CONTAINER_STOP, id) == NULL) {
        GNEContainerStop* containerStop = new GNEContainerStop(id, lane, viewNet, startPos, endPos, name, lines, friendlyPosition);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(containerStop);
            lane->addAdditionalChild(containerStop);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildChargingStation(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, double chargingPower, double efficiency, bool chargeInTransit, double chargeDelay, bool friendlyPosition) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CHARGING_STATION, id) == NULL) {
        GNEChargingStation* chargingStation = new GNEChargingStation(id, lane, viewNet, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
            viewNet->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(chargingStation);
            lane->addAdditionalChild(chargingStation);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorE1(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double freq, const std::string& filename, const std::string& vehicleTypes, bool friendlyPos) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E1DETECTOR, id) == NULL) {
        GNEDetectorE1* detectorE1 = new GNEDetectorE1(id, lane, viewNet, pos, freq, filename, vehicleTypes, friendlyPos);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE1);
            lane->addAdditionalChild(detectorE1);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double length, double freq, const std::string& filename,
                                      bool cont, const double timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E2DETECTOR, id) == NULL) {
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lane, viewNet, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, friendlyPos);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE2);
            lane->addAdditionalChild(detectorE2);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorE3(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, double freq, const std::string& filename, const double timeThreshold, double speedThreshold) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
        GNEDetectorE3* detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, timeThreshold, speedThreshold);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE3, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE3);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet* viewNet, bool allowUndoRedo, GNEDetectorE3* E3Parent, GNELane* lane, double pos, bool friendlyPos) {
    // Check if Detector E3 parent and lane is correct
    if (lane == NULL) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == NULL) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorEntry* entry = new GNEDetectorEntry(viewNet, E3Parent, lane, pos, friendlyPos);
        if (allowUndoRedo) {
            // insert E3 parent in net if previoulsy wasn't inserted
            if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
                viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
                viewNet->getUndoList()->add(new GNEChange_Additional(E3Parent, true), true);
                viewNet->getUndoList()->p_end();
            }
            // Create detector Entry if don't exist already in the net
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
            viewNet->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // insert E3 parent in net if previoulsy wasn't inserted
            if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
                viewNet->getNet()->insertAdditional(E3Parent);
            }
            E3Parent->addEntryChild(entry);
            viewNet->getNet()->insertAdditional(entry);
            lane->addAdditionalChild(entry);
            // update geometry for draw lines
            E3Parent->updateGeometry();
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorExit(GNEViewNet* viewNet, bool allowUndoRedo, GNEDetectorE3* E3Parent, GNELane* lane, double pos, bool friendlyPos) {
    // Check if Detector E3 parent and lane is correct
    if (lane == NULL) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == NULL) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorExit* exit = new GNEDetectorExit(viewNet, E3Parent, lane, pos, friendlyPos);
        if (allowUndoRedo) {
            // insert E3 parent in net if previoulsy wasn't inserted
            if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
                viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
                viewNet->getUndoList()->add(new GNEChange_Additional(E3Parent, true), true);
                viewNet->getUndoList()->p_end();
            }
            // Create detector Exit if don't exist already in the net
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
            viewNet->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // insert E3 parent in net if previoulsy wasn't inserted
            if (viewNet->getNet()->getAdditional(E3Parent->getTag(), E3Parent->getID()) == NULL) {
                viewNet->getNet()->insertAdditional(E3Parent);
            }
            E3Parent->addExitChild(exit);
            viewNet->getNet()->insertAdditional(exit);
            lane->addAdditionalChild(exit);
            // update geometry for draw lines
            E3Parent->updateGeometry();
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildCalibratorLane(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& outfile, const double freq,
                                      const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows,
                                      const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        GNECalibratorLane* calibratorLane = new GNECalibratorLane(id, lane, viewNet, pos, freq, outfile, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibratorLane, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(calibratorLane);
            lane->addAdditionalChild(calibratorLane);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibratorEdge(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge *edge, double pos, const std::string& outfile, const double freq,
    const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows,
    const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        GNECalibratorEdge* calibratorEdge = new GNECalibratorEdge(id, edge, viewNet, pos, freq, outfile, calibratorRoutes, calibratorFlows, calibratorVehicleTypes);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibratorEdge, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(calibratorEdge);
            edge->addAdditionalChild(calibratorEdge);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& file, bool off) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        GNERerouter* rerouter = new GNERerouter(id, viewNet, pos, edges, file, prob, off);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(rerouter);
            for (std::vector<GNEEdge*>::const_iterator i = edges.begin(); i != edges.end(); i++) {
                (*i)->addGNERerouter(rerouter);
            }
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double freq, const std::string& file, double begin) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, id) == NULL) {
        GNERouteProbe* routeProbe = new GNERouteProbe(id, viewNet, edge, freq, file, begin);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
            viewNet->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(routeProbe);
            edge->addAdditionalChild(routeProbe);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& file, const std::vector<GNEVariableSpeedSignStep>& steps) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VSS, id) == NULL) {
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, file, steps);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
            viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(variableSpeedSign);
            //// for (std::vector<GNEEdge*>::const_iterator i = edges.begin(); i != edges.end(); i++) {
            ////    (*i)->addGNERerouter(rerouter);
            //// }
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, GNEEdge* edge, double startTime, double end) {
    GNEVaporizer* vaporizer = new GNEVaporizer(viewNet, edge, startTime, end);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        viewNet->getNet()->insertAdditional(vaporizer);
        edge->addAdditionalChild(vaporizer);
    }
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
GNEAdditionalHandler::getPosition(double pos, GNELane& lane, bool friendlyPos , const std::string& additionalID) {
    if (pos < 0) {
        pos = lane.getLaneShapeLength() + pos;
    }
    if (pos > lane.getLaneShapeLength()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLength() - (double) 0.1;
        } else {
            WRITE_WARNING("The position of additional '" + additionalID + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool
GNEAdditionalHandler::fixStoppinPlacePosition(double& startPos, double& endPos, const double laneLength, const double minLength,  const bool friendlyPos) {
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


bool GNEAdditionalHandler::checkAndFixDetectorPositionPosition(double& pos, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || (pos > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength;
        }
    }
    return true;
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


bool GNEAdditionalHandler::fixE2DetectorPositionPosition(double& pos, double& length, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || ((pos + length) > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength;
            length = 0;
        } else if ((pos + length) > laneLength) {
            length = laneLength - pos;
        }
    }
    return true;
}


void
GNEAdditionalHandler::resetLastTag() {
    myLastTag = SUMO_TAG_NOTHING;
    if (myE3Parent != NULL && ((myE3Parent->getNumberOfEntryChilds() + myE3Parent->getNumberOfExitChilds()) == 0)) {
        WRITE_WARNING((toString(myE3Parent->getTag()) + "s without " + toString(SUMO_TAG_DET_ENTRY) + "s or " + toString(SUMO_TAG_DET_EXIT) + " aren't allowed; " +
                       toString(myE3Parent->getTag()) + " with ID = '" + myE3Parent->getID() + "' cannot be created.").c_str());
        // check if has to be removed of the net before removing
        if (myViewNet->getNet()->getAdditional(myE3Parent->getTag(), myE3Parent->getID()) != NULL) {
            myViewNet->getNet()->deleteAdditional(myE3Parent);
        }
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
