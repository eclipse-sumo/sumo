/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Builds trigger objects for netedit
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
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorVehicleType.h"
#include "GNEChange_Additional.h"
#include "GNEChange_CalibratorItem.h"
#include "GNEChange_RerouterItem.h"
#include "GNEChange_VariableSpeedSignItem.h"
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
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNEViewNet.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, bool undoAdditionals) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myUndoAdditionals(undoAdditionals) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // Call parse and build depending of tag
    switch (element) {
        case SUMO_TAG_BUS_STOP:
            parseAndBuildBusStop(attrs, tag);
            break;
        case SUMO_TAG_TRAIN_STOP:
            parseAndBuildBusStop(attrs, SUMO_TAG_BUS_STOP);
            break;
        case SUMO_TAG_CONTAINER_STOP:
            parseAndBuildContainerStop(attrs, tag);
            break;
        case SUMO_TAG_CHARGING_STATION:
            parseAndBuildChargingStation(attrs, tag);
            break;
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
            parseAndBuildDetectorE1(attrs, tag);
            break;
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_LANE_AREA_DETECTOR:
            parseAndBuildDetectorE2(attrs, tag);
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
        case SUMO_TAG_ROUTEPROBE:
            parseAndBuildRouteProbe(attrs, tag);
            break;
        case SUMO_TAG_VAPORIZER:
            parseAndBuildVaporizer(attrs, tag);
            break;
        case SUMO_TAG_VSS:
            parseAndBuildVariableSpeedSign(attrs, tag);
            break;
        case SUMO_TAG_STEP:
            parseAndBuildVariableSpeedSignStep(attrs, tag);
            break;
        case SUMO_TAG_CALIBRATOR:
            parseAndBuildCalibrator(attrs, tag);
            break;
        case SUMO_TAG_VTYPE:
            parseAndBuildCalibratorVehicleType(attrs, tag);
            break;
        case SUMO_TAG_ROUTE:
            parseAndBuildCalibratorRoute(attrs, tag);
            break;
        case SUMO_TAG_FLOW:
            parseAndBuildCalibratorFlow(attrs, tag);
            break;
        case SUMO_TAG_REROUTER:
            parseAndBuildRerouter(attrs, tag);
            break;
        case SUMO_TAG_INTERVAL:
            parseAndBuildRerouterInterval(attrs, tag);
            break;
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            parseAndBuildRerouterClosingLaneReroute(attrs, tag);
            break;
        case SUMO_TAG_CLOSING_REROUTE:
            parseAndBuildRerouterClosingReroute(attrs, tag);
            break;
        case SUMO_TAG_DEST_PROB_REROUTE:
            parseAndBuildRerouterDestProbReroute(attrs, tag);
            break;
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            parseAndBuildRerouterRouteProbReroute(attrs, tag);
            break;
        default:
            break;
    }
}


void
GNEAdditionalHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_EDGE, abort);
    double startTime = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_STARTTIME, abort);
    double endTime = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get GNEEdge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        // check that all parameters are valid
        if (edge == NULL) {
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " is not known.");
        } else if (startTime > endTime) {
            WRITE_WARNING("Time interval of " + toString(tag) + " isn't valid. Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
        } else {
            buildVaporizer(myViewNet, myUndoAdditionals, edge, startTime, endTime);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_EDGE, abort);
    double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_BEGIN, abort);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (edge == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else {
            buildRouteProbe(myViewNet, myUndoAdditionals, id, edge, freq, file, begin);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator routes
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, routeID, tag, SUMO_ATTR_EDGES, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, routeID, tag, SUMO_ATTR_COLOR, abort, false);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges (And show warnings if isn't valid)
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::checkGNEEdgesValid(myViewNet->getNet(), edgeIDs, true)) {
            edges = GNEAttributeCarrier::parseGNEEdges(myViewNet->getNet(), edgeIDs);
        }
        // get calibrator parent
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveCalibratorRoute(routeID, false) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + routeID + "'.");
        } else if (calibrator == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_CALIBRATOR) + ".");
        } else if (edges.size() > 0) {
            // create vehicle type and add it to calibrator parent
            buildCalibratorRoute(myViewNet, myUndoAdditionals, calibrator, routeID, edges, color);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorVehicleType(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator vehicle types
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double accel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_ACCEL, abort);
    double decel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_DECEL, abort);
    double sigma = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SIGMA, abort);
    double tau = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_TAU, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_LENGTH, abort);
    double minGap = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MINGAP, abort);
    double maxSpeed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MAXSPEED, abort);
    double speedFactor = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SPEEDFACTOR, abort);
    double speedDev = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SPEEDDEV, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, vehicleTypeID, tag, SUMO_ATTR_COLOR, abort, false);
    SUMOVehicleClass vClass = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleClass>(attrs, vehicleTypeID, tag, SUMO_ATTR_VCLASS, abort);
    std::string emissionClass = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_EMISSIONCLASS, abort);
    SUMOVehicleShape shape = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleShape>(attrs, vehicleTypeID, tag, SUMO_ATTR_GUISHAPE, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_WIDTH, abort);
    std::string filename = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_IMGFILE, abort);
    double impatience = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_IMPATIENCE, abort);
    std::string laneChangeModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_LANE_CHANGE_MODEL, abort);
    std::string carFollowModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_CAR_FOLLOW_MODEL, abort);
    int personCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, vehicleTypeID, tag, SUMO_ATTR_PERSON_CAPACITY, abort);
    int containerCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, vehicleTypeID, tag, SUMO_ATTR_CONTAINER_CAPACITY, abort);
    double boardingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_BOARDING_DURATION, abort);
    double loadingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_LOADING_DURATION, abort);
    std::string latAlignment = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_LATALIGNMENT, abort);
    double minGapLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MINGAP_LAT, abort);
    double maxSpeedLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MAXSPEED_LAT, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get calibrator parent
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveCalibratorVehicleType(vehicleTypeID, false) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + vehicleTypeID + "'.");
        } else if (calibrator == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_CALIBRATOR) + ".");
        } else {
            // build calibrator vehicle type
            buildCalibratorVehicleType(myViewNet, true, calibrator, vehicleTypeID, accel, decel, sigma, tau, length, minGap, maxSpeed, speedFactor, speedDev,
                                       color, vClass, emissionClass, shape, width, filename, impatience, laneChangeModel, carFollowModel, personCapacity,
                                       containerCapacity, boardingDuration, loadingDuration, latAlignment, minGapLat, maxSpeedLat);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of calibrator flows
    std::string flowID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_TYPE, abort);
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_ROUTE, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, flowID, tag, SUMO_ATTR_COLOR, abort, false);
    std::string departLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_DEPARTLANE, abort);
    std::string departPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_DEPARTPOS, abort);
    std::string departSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_DEPARTSPEED, abort);
    std::string arrivalLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_ARRIVALLANE, abort);
    std::string arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_ARRIVALPOS, abort);
    std::string arrivalSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_ARRIVALSPEED, abort);
    std::string line = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_LINE, abort);
    int personNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, flowID, tag, SUMO_ATTR_PERSON_NUMBER, abort);
    int containerNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, flowID, tag, SUMO_ATTR_CONTAINER_NUMBER, abort);
    bool reroute = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, flowID, tag, SUMO_ATTR_REROUTE, abort);
    std::string departPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_DEPARTPOS_LAT, abort);
    std::string arrivalPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, flowID, tag, SUMO_ATTR_ARRIVALPOS_LAT, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, flowID, tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, flowID, tag, SUMO_ATTR_END, abort);
    double vehsPerHour = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, flowID, tag, SUMO_ATTR_VEHSPERHOUR, abort);
    double period = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, flowID, tag, SUMO_ATTR_PERIOD, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, flowID, tag, SUMO_ATTR_PROB, abort);
    int number = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, flowID, tag, SUMO_ATTR_NUMBER, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain route, vehicle type and calibrator parent
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        GNECalibratorRoute* route = myViewNet->getNet()->retrieveCalibratorRoute(routeID, false);
        GNECalibratorVehicleType* vtype = myViewNet->getNet()->retrieveCalibratorVehicleType(vehicleTypeID, false);
        int flowType = getTypeOfFlowDistribution(flowID, vehsPerHour, period, probability);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveCalibratorFlow(flowID, false) != NULL) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW) + " with ID = '" + flowID + "' cannot be created; Another " + toString(SUMO_TAG_FLOW) + " with the same ID was previously declared");
            abort = true;
        } else if (route == NULL) {
            WRITE_WARNING(toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' cannot be created; their " + toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' doesn't exist");
            abort = true;
        } else if (vtype == NULL) {
            WRITE_WARNING(toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' cannot be created; their " + toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' doesn't exist");
            abort = true;
        } else if (calibrator == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_CALIBRATOR) + ".");
        } else {
            buildCalibratorFlow(myViewNet, true, calibrator, flowID, route, vtype, color, departLane, departPos, departSpeed, arrivalLane, arrivalPos, arrivalSpeed,
                                line, personNumber, containerNumber, reroute, departPosLat, arrivalPosLat, begin, end, vehsPerHour, period, probability, number, flowType);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildVariableSpeedSign(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of VSS
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    std::string lanesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANES, abort);
    double posx = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_X, abort);
    double posy = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_Y, abort);
    // Due this additional can have childs, we need to reset myLastInsertedAdditionalParent
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lanes
        std::vector<GNELane*> lanes;
        if (GNEAttributeCarrier::checkGNELanesValid(myViewNet->getNet(), lanesIDs, true)) {
            lanes = GNEAttributeCarrier::parseGNELanes(myViewNet->getNet(), lanesIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if ((lanes.size() > 0) && buildVariableSpeedSign(myViewNet, myUndoAdditionals, id, Position(posx, posy), lanes, file)) {
            // set myLastInsertedAdditionalParent due this additional can have childs
            myLastInsertedAdditionalParent = id;
        }
    }
}

void
GNEAdditionalHandler::parseAndBuildVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // Load step values
    double time = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_TIME, abort);
    double speed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_SPEED, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get Variable Speed Signal
        GNEVariableSpeedSign* variableSpeedSign = dynamic_cast<GNEVariableSpeedSign*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all parameters are valid
        if (variableSpeedSign == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_VSS) + ".");
        } else {
            buildVariableSpeedSignStep(myViewNet, true, variableSpeedSign, time, speed);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_EDGES, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_PROB, abort);
    bool off = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_OFF, abort);
    double timeThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), abort, 0);
    double posx = attrs.getOpt<double>(SUMO_ATTR_X, 0, abort, 0);
    double posy = attrs.getOpt<double>(SUMO_ATTR_Y, 0, abort, 0);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::checkGNEEdgesValid(myViewNet->getNet(), edgesIDs, true)) {
            edges = GNEAttributeCarrier::parseGNEEdges(myViewNet->getNet(), edgesIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if ((edgesIDs.size() > 0) && buildRerouter(myViewNet, myUndoAdditionals, id, Position(posx, posy), edges, probability, file, off, timeThreshold)) {
            // set myLastInsertedAdditionalParent due this additional can have childs
            myLastInsertedAdditionalParent = id;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterInterval(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain rerouter
        GNERerouter* rerouter = dynamic_cast<GNERerouter*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (rerouter == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_REROUTER) + ".");
        } else if (buildRerouterInterval(myViewNet, true, rerouter, begin, end)) {
            // set myLastInsertedAdditionalParent due this additional can have childs
            myLastInsertedAdditionalParent = rerouter->getID() + "_" + toString(begin) + "_" + toString(end);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterClosingLaneReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string laneID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    SVCPermissions allow = GNEAttributeCarrier::parseAttributeFromXML<SVCPermissions>(attrs, "", tag, SUMO_ATTR_ALLOW, abort);
    SVCPermissions disallow = GNEAttributeCarrier::parseAttributeFromXML<SVCPermissions>(attrs, "", tag, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneID, false, true);
        GNERerouterInterval* rerouterInterval = myViewNet->getNet()->getRerouterInterval(myLastInsertedAdditionalParent);
        // check that all elements are valid
        if (lane == NULL) {
            WRITE_WARNING("The lane '" + laneID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_INTERVAL) + ".");
        } else {
            buildClosingLaneReroute(myViewNet, true, rerouterInterval, lane, allow, disallow);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterClosingReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    SVCPermissions allow = GNEAttributeCarrier::parseAttributeFromXML<SVCPermissions>(attrs, "", tag, SUMO_ATTR_ALLOW, abort);
    SVCPermissions disallow = GNEAttributeCarrier::parseAttributeFromXML<SVCPermissions>(attrs, "", tag, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        GNERerouterInterval* rerouterInterval = dynamic_cast<GNERerouterInterval*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (edge == NULL) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_INTERVAL) + ".");
        } else {
            buildClosingReroute(myViewNet, true, rerouterInterval, edge, allow, disallow);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterDestProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        GNERerouterInterval* rerouterInterval = dynamic_cast<GNERerouterInterval*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (edge == NULL) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_INTERVAL) + ".");
        } else {
            builDestProbReroute(myViewNet, true, rerouterInterval, edge, probability);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterRouteProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge
        GNERerouterInterval* rerouterInterval = dynamic_cast<GNERerouterInterval*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all elements are valid
        if (rerouterInterval == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_INTERVAL) + ".");
        } else {
            buildRouteProbReroute(myViewNet, true, rerouterInterval, routeID, probability);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of bus stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort, false);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, tag, SUMO_ATTR_LINES, abort, false);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // Write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else {
            buildBusStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildContainerStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of container stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort, false);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, tag, SUMO_ATTR_LINES, abort, false);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else {
            buildContainerStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double startPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    double endPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort, false);
    double chargingPower = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_CHARGINGPOWER, abort);
    double efficiency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_CHARGEINTRANSIT, abort);
    double chargeDelay = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_CHARGEDELAY, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort, false);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else {
            buildChargingStation(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // due there is two differents calibrators, has to be parsed in a different way
    std::string edgeID, laneId, id;
    SumoXMLTag typeOfCalibrator = tag;
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // change tag depending of XML parmeters
    if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        typeOfCalibrator = SUMO_TAG_CALIBRATOR;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", typeOfCalibrator, SUMO_ATTR_ID, abort);
        edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_EDGE, abort, false);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_OUTPUT, abort, false);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_POSITION, abort);
        double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_FREQUENCY, abort);
        // std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
        // Due this additional can have childs, we need to reset myLastInsertedAdditionalParent
        myLastInsertedAdditionalParent = "";
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer and edge
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
            // check that all elements are valid
            if (myViewNet->getNet()->getAdditional(typeOfCalibrator, id) != NULL) {
                WRITE_WARNING("There is another " + toString(typeOfCalibrator) + " with the same ID='" + id + "'.");
            } else if (edge == NULL) {
                WRITE_WARNING("The  edge '" + edgeID + "' to use within the " + toString(typeOfCalibrator) + " '" + id + "' is not known.");
            } else if (buildCalibrator(myViewNet, myUndoAdditionals, id, edge, position, outfile, freq)) {
                // set myLastInsertedAdditionalParent due this additional can have childs
                myLastInsertedAdditionalParent = id;
            }
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        typeOfCalibrator = SUMO_TAG_LANECALIBRATOR;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", typeOfCalibrator, SUMO_ATTR_ID, abort);
        laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_LANE, abort, false);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_OUTPUT, abort, false);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_POSITION, abort);
        double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_FREQUENCY, abort);
        // std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
        // Due this additional can have childs, we need to reset myLastInsertedAdditionalParent
        myLastInsertedAdditionalParent = "";
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer to lane
            GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
            // check that all elements are valid
            if (myViewNet->getNet()->getAdditional(typeOfCalibrator, id) != NULL) {
                WRITE_WARNING("There is another " + toString(typeOfCalibrator) + " with the same ID='" + id + "'.");
            } else if (lane == NULL) {
                WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(typeOfCalibrator) + " '" + id + "' is not known.");
            } else if (buildCalibrator(myViewNet, myUndoAdditionals, id, lane, position, outfile, freq)) {
                // set myLastInsertedAdditionalParent due this additional can have childs
                myLastInsertedAdditionalParent = id;
            }
        }
    } else {
        WRITE_WARNING("additional " + toString(tag) + " must have either a lane or an edge attribute.");
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E1
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_VTYPES, abort, false);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else {
            buildDetectorE1(myViewNet, myUndoAdditionals, id, lane, position, frequency, file, vehicleTypes, friendlyPos);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E2
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_LENGTH, abort);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double jamDistThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool cont = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_CONT, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // myLastInsertedAdditionalParent must be empty because this additional cannot be child of another additional
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == NULL) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixE2DetectorPositionPosition(position, length, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else {
            buildDetectorE2(myViewNet, myUndoAdditionals, id, lane, position, length, frequency, file, cont, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, friendlyPos);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E3
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort, false);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double posx = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_X, abort);
    double posy = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_Y, abort);
    // Due this additional can have childs, we need to reset myLastInsertedAdditionalParent
    myLastInsertedAdditionalParent = "";
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check that all elements are valid
        if (myViewNet->getNet()->getAdditional(tag, id) != NULL) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (buildDetectorE3(myViewNet, true, id, Position(posx, posy), frequency, file, haltingTimeThreshold, haltingSpeedThreshold)) {
            // set myLastInsertedAdditionalParent due this additional can have childs and was sucesfully created
            myLastInsertedAdditionalParent = id;
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        GNEDetectorE3* E3Parent = dynamic_cast<GNEDetectorE3*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all parameters are valid
        if (lane == NULL) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (E3Parent == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_E3DETECTOR) + ".");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else {
            buildDetectorEntry(myViewNet, myUndoAdditionals, E3Parent, lane, position, friendlyPos);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Exit
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        GNEDetectorE3* E3Parent = dynamic_cast<GNEDetectorE3*>(myViewNet->getNet()->retrieveAdditional(myLastInsertedAdditionalParent, false));
        // check that all parameters are valid
        if (lane == NULL) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (E3Parent == NULL) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_E3DETECTOR) + ".");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else {
            buildDetectorExit(myViewNet, myUndoAdditionals, E3Parent, lane, position, friendlyPos);
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
            std::vector<GNELane*> lanes = GNEAttributeCarrier::parseGNELanes(viewNet->getNet(), values[SUMO_ATTR_LANES]);
            // get rest of parameters
            std::string file = values[SUMO_ATTR_FILE];
            // build VSS
            if (pos.size() == 1) {
                return buildVariableSpeedSign(viewNet, allowUndoRedo, id, pos[0], lanes, file);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // Build calibrator edge
            if (edge) {
                return buildCalibrator(viewNet, allowUndoRedo, id, edge, pos, outfile, freq);
            } else {
                return false;
            }
        }
        case SUMO_TAG_LANECALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // Build calibrator lane
            if (lane) {
                return buildCalibrator(viewNet, allowUndoRedo, id, lane, pos, outfile, freq);
            } else {
                return false;
            }
        }
        case SUMO_TAG_REROUTER: {
            // obtain specify attributes of rerouter
            std::string id = values[SUMO_ATTR_ID];
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            std::vector<GNEEdge*> edges = GNEAttributeCarrier::parseGNEEdges(viewNet->getNet(), values[SUMO_ATTR_EDGES]);
            // Get rest of parameters
            bool off = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_OFF]);
            double prob = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_PROB]);
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            std::string file = values[SUMO_ATTR_FILE];
            // Build rerouter
            if (pos.size() == 1) {
                return buildRerouter(viewNet, allowUndoRedo, id, pos[0], edges, prob, file, off, timeThreshold);
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
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
            viewNet->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(entry);
            lane->addAdditionalChild(entry);
            E3Parent->addAdditionalChild(entry);
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
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
            viewNet->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(exit);
            lane->addAdditionalChild(exit);
            E3Parent->addAdditionalChild(exit);
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& outfile, const double freq) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, lane, pos, freq, outfile);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(calibrator->getGlID(), false);
        } else {
            viewNet->getNet()->insertAdditional(calibrator);
            lane->addAdditionalChild(calibrator);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double pos, const std::string& outfile, const double freq) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CALIBRATOR, id) == NULL) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, edge, pos, freq, outfile);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(calibrator->getGlID(), false);
        } else {
            viewNet->getNet()->insertAdditional(calibrator);
            edge->addAdditionalChild(calibrator);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibratorRoute(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) {
    if (viewNet->getNet()->retrieveCalibratorRoute(routeID, false) == NULL) {
        // create route and add it to calibrator parent
        GNECalibratorRoute* route = new GNECalibratorRoute(calibratorParent, routeID, edges, color);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(route->getTag()));
            viewNet->getUndoList()->add(new GNEChange_CalibratorItem(route, true), true);
            viewNet->getUndoList()->p_end();
            return true;
        } else {
            viewNet->getNet()->insertCalibratorRoute(route);
            calibratorParent->addCalibratorRoute(route);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTE) + " with ID '" + routeID + "' in netedit; probably declared twice.");
    }

}


bool
GNEAdditionalHandler::buildCalibratorVehicleType(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent, std::string vehicleTypeID,
        double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
        double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
        SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
        const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
        double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat) {
    if (viewNet->getNet()->retrieveCalibratorVehicleType(vehicleTypeID, false) == NULL) {
        // create vehicle type and add it to calibrator parent
        GNECalibratorVehicleType* vType = new GNECalibratorVehicleType(calibratorParent, vehicleTypeID, accel, decel, sigma, tau, length, minGap, maxSpeed,
                speedFactor, speedDev, color, vClass, emissionClass, shape, width, filename, impatience,
                laneChangeModel, carFollowModel, personCapacity, containerCapacity, boardingDuration,
                loadingDuration, latAlignment, minGapLat, maxSpeedLat);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(vType->getTag()));
            viewNet->getUndoList()->add(new GNEChange_CalibratorItem(vType, true), true);
            viewNet->getUndoList()->p_end();
            return true;
        } else {
            viewNet->getNet()->insertCalibratorVehicleType(vType);
            calibratorParent->addCalibratorVehicleType(vType);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VTYPE) + " with ID '" + vehicleTypeID + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibratorFlow(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent,
        const std::string& flowID, GNECalibratorRoute* route, GNECalibratorVehicleType* vtype, const RGBColor& color,
        const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
        const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
        const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end, double vehsPerHour, double period,
        double probability, int number, int flowType) {

    if (viewNet->getNet()->retrieveCalibratorFlow(flowID, false) == NULL) {
        // create Flow and add it to calibrator parent
        GNECalibratorFlow* flow = new GNECalibratorFlow(calibratorParent, flowID, vtype, route, color, departLane, departPos, departSpeed,
                arrivalLane, arrivalPos, arrivalSpeed, line, personNumber, containerNumber, reroute,
                departPosLat, arrivalPosLat, begin, end, vehsPerHour, period, probability, number, static_cast<GNECalibratorFlow::TypeOfFlow>(flowType));
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(flow->getTag()));
            viewNet->getUndoList()->add(new GNEChange_CalibratorItem(flow, true), true);
            viewNet->getUndoList()->p_end();
            return true;
        } else {
            viewNet->getNet()->insertCalibratorFlow(flow);
            calibratorParent->addCalibratorFlow(flow);
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_FLOW) + " with ID '" + flowID + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& file, bool off, double timeThreshold) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_REROUTER, id) == NULL) {
        GNERerouter* rerouter = new GNERerouter(id, viewNet, pos, edges, file, prob, off, timeThreshold);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(rerouter);
            // add this rerouter as parent of all edges
            for (auto i : edges) {
                i->addAdditionalParent(rerouter);
            }
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildRerouterInterval(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouter* rerouterParent, double begin, double end) {
    // first create rerouter interval and add it into rerouter parent
    GNERerouterInterval* rerouterInterval = new GNERerouterInterval(rerouterParent, begin, end);
    rerouterParent->addRerouterInterval(rerouterInterval);
    // remove it if there is overlapping with another intervals
    if (rerouterParent->getNumberOfOverlappedIntervals() == 0) {
        // if allowUndoRedo is enabled, remove it and add it again using GNEChange_RerouterItem
        if (allowUndoRedo) {
            rerouterParent->removeRerouterInterval(rerouterInterval);
            viewNet->getUndoList()->p_begin("add " + toString(rerouterInterval->getTag()));
            viewNet->getUndoList()->add(new GNEChange_RerouterItem(rerouterInterval, true), true);
            viewNet->getUndoList()->p_end();
        }
        return true;
    } else {
        // delete created rerouter interval and throw exception
        rerouterParent->removeRerouterInterval(rerouterInterval);
        delete rerouterInterval;
        throw ProcessError("Could not build " + toString(SUMO_TAG_INTERVAL) + " with begin '" + toString(begin) + "' and '" + toString(end) + "' in '" + rerouterParent->getID() + "' due overlapping.");
    }
}


bool
GNEAdditionalHandler::buildClosingLaneReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNELane* closedLane, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) {
    // create closing lane reorute
    GNEClosingLaneReroute* closingLaneReroute = new GNEClosingLaneReroute(rerouterIntervalParent, closedLane, allowedVehicles, disallowedVehicles);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(closingLaneReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_RerouterItem(closingLaneReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addClosingLaneReroute(closingLaneReroute);
    }
    return true;
}


bool
GNEAdditionalHandler::buildClosingReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) {
    // create closing reroute
    GNEClosingReroute* closingReroute = new GNEClosingReroute(rerouterIntervalParent, closedEdge, allowedVehicles, disallowedVehicles);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(closingReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_RerouterItem(closingReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addClosingReroute(closingReroute);
    }
    return true;
}


bool
GNEAdditionalHandler::builDestProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability) {
    // create dest probability reroute
    GNEDestProbReroute* destProbReroute = new GNEDestProbReroute(rerouterIntervalParent, newEdgeDestination, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(destProbReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_RerouterItem(destProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addDestProbReroute(destProbReroute);
    }
    return true;
}


bool
GNEAdditionalHandler::buildRouteProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, const std::string& newRouteId, double probability) {
    // create rout prob rereoute
    GNERouteProbReroute* routeProbReroute = new GNERouteProbReroute(rerouterIntervalParent, newRouteId, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(routeProbReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_RerouterItem(routeProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addRouteProbReroute(routeProbReroute);
    }
    return true;
}


bool
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double freq, const std::string& file, double begin) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, id) == NULL) {
        GNERouteProbe* routeProbe = new GNERouteProbe(id, viewNet, edge, freq, file, begin);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
            viewNet->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(routeProbe->getGlID(), false);
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
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& file) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_VSS, id) == NULL) {
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, file);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
            viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(variableSpeedSign);
            // add this VSS as parent of all edges
            for (auto i : lanes) {
                i->addAdditionalParent(variableSpeedSign);
            }
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSignStep(GNEViewNet* viewNet, bool allowUndoRedo, GNEVariableSpeedSign* VSSParent, double time, double speed) {
    // create Variable Speed Sign
    GNEVariableSpeedSignStep* variableSpeedSignStep = new GNEVariableSpeedSignStep(VSSParent, time, speed);
    // add it depending of allow undoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(variableSpeedSignStep->getTag()));
        viewNet->getUndoList()->add(new GNEChange_VariableSpeedSignItem(variableSpeedSignStep, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        VSSParent->addVariableSpeedSignStep(variableSpeedSignStep);
    }
    return true;
}


bool
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, GNEEdge* edge, double startTime, double end) {
    GNEVaporizer* vaporizer = new GNEVaporizer(viewNet, edge, startTime, end);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
        viewNet->getUndoList()->p_end();
        // center after creation
        viewNet->centerTo(vaporizer->getGlID(), false);
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


int
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

/****************************************************************************/
