/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    RouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The XML-Handler for route elements loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RGBColor.h>
#include <utils/shapes/Shape.h>

#include "RouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

RouteHandler::RouteHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


RouteHandler::~RouteHandler() {}


bool
RouteHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void 
RouteHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        // route
        case SUMO_TAG_ROUTE:
            buildRoute(obj,
                obj->getStringListAttribute(SUMO_ATTR_EDGES),
                obj->getColorAttribute(SUMO_ATTR_COLOR),
                obj->getIntAttribute(SUMO_ATTR_REPEAT),
                obj->getTimeAttribute(SUMO_ATTR_CYCLETIME),
                obj->getParameters());
            break;
        // vehicles
        case SUMO_TAG_TRIP:
            buildTrip(obj, 
                obj->getVehicleParameter(),
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringListAttribute(SUMO_ATTR_VIA),
                obj->getParameters());
            break;
        case SUMO_TAG_VEHICLE:
            if (obj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                buildVehicleOverRoute(obj,
                    obj->getVehicleParameter(),
                    obj->getParameters());
            } else {
                buildVehicleEmbeddedRoute(obj,
                    obj->getVehicleParameter(),
                    obj->getStringListAttribute(SUMO_ATTR_EDGES),
                    obj->getParameters());
            }
            break;
        // flows
        case SUMO_TAG_FLOW:
            if (obj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                buildFlowOverRoute(obj,
                    obj->getVehicleParameter(),
                    obj->getParameters());
            } else if (obj->hasStringAttribute(SUMO_ATTR_EDGES)) {
                buildFlowEmbeddedRoute(obj,
                    obj->getVehicleParameter(),
                    obj->getStringListAttribute(SUMO_ATTR_EDGES),
                    obj->getParameters());
            } else {
                buildFlow(obj,
                    obj->getVehicleParameter(),
                    obj->getStringAttribute(SUMO_ATTR_FROM),
                    obj->getStringAttribute(SUMO_ATTR_TO),
                    obj->getStringListAttribute(SUMO_ATTR_VIA),
                    obj->getParameters());
            }
            break;
        // stop
        case SUMO_TAG_STOP:
            buildStop(obj,
                obj->getStopParameter());
            break;
        // persons
        case SUMO_TAG_PERSON:
            buildPerson(obj,
                obj->getVehicleParameter(),
                obj->getParameters());
            break;
        case SUMO_TAG_PERSONFLOW:
            buildPersonFlow(obj,
                obj->getVehicleParameter(),
                obj->getParameters());
            break;
        // person plans
        case SUMO_TAG_PERSONTRIP:
            buildPersonTrip(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringAttribute(SUMO_ATTR_BUS_STOP),
                obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                obj->getStringListAttribute(SUMO_ATTR_MODES));
            break;
        case SUMO_TAG_RIDE:
            buildRide(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringAttribute(SUMO_ATTR_BUS_STOP),
                obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                obj->getStringListAttribute(SUMO_ATTR_LINES));
            break;
        case SUMO_TAG_WALK:
            buildWalk(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringAttribute(SUMO_ATTR_BUS_STOP),
                obj->getStringListAttribute(SUMO_ATTR_EDGES),
                obj->getStringAttribute(SUMO_ATTR_ROUTE),
                obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS));
            break;
        // container
        case SUMO_TAG_CONTAINER:
            buildContainer(obj,
                obj->getVehicleParameter(),
                obj->getParameters());
            break;
        case SUMO_TAG_CONTAINERFLOW:
            buildContainerFlow(obj,
                obj->getVehicleParameter(),
                obj->getParameters());
            break;
        // container plans
        case SUMO_TAG_TRANSPORT:
            buildTransport(obj, 
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringAttribute(SUMO_ATTR_BUS_STOP),
                obj->getStringListAttribute(SUMO_ATTR_LINES),
                obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS));
            break;
        case SUMO_TAG_TRANSHIP:
            buildTranship(obj,
                obj->getStringAttribute(SUMO_ATTR_FROM),
                obj->getStringAttribute(SUMO_ATTR_TO),
                obj->getStringAttribute(SUMO_ATTR_BUS_STOP),
                obj->getStringListAttribute(SUMO_ATTR_EDGES),
                obj->getDoubleAttribute(SUMO_ATTR_SPEED),
                obj->getDoubleAttribute(SUMO_ATTR_DEPARTPOS),
                obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS));
            break;
        default:
            break;
    }
    // now iterate over childrens
    for (const auto &child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
}


void 
RouteHandler::buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::vector<std::string> &edges, 
    const RGBColor &color, const int repeat, const SUMOTime cycleTime, const std::map<std::string, std::string> &parameters) {
    //
}


void 
RouteHandler::buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
RouteHandler::buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
    const std::vector<std::string>& edges, const std::map<std::string, std::string> &parameters) {
    //
}


void 
RouteHandler::buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
    const std::vector<std::string>& edges, const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
    const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via, const std::map<std::string, std::string> &parameters) {
    //
}


void 
RouteHandler::buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
    const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via, const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) {
    //
}


void 
RouteHandler::buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
    const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes) {
    //
}


void
RouteHandler::buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
    const std::string &toBusStop, const std::vector<std::string>& edges, const std::string &route, double arrivalPos) {
    //
}


void
RouteHandler::buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge, 
    const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& lines) {
    //
}


void
RouteHandler::buildStopPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edge, const std::string &busStop, 
    const SUMOVehicleParameter::Stop& stopParameters) {
    //
}


void
RouteHandler::buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void
RouteHandler::buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters,
    const std::map<std::string, std::string> &parameters) {
    //
}


void 
RouteHandler::buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
    const std::string &toBusStop, const std::vector<std::string>& lines, const double arrivalPos) {
    //
}


void
RouteHandler::buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
    const std::string &toBusStop, const std::vector<std::string>& edges, const double speed, const double departPosition, const double arrivalPosition) {
    //
}


void 
RouteHandler::buildStopContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edge, const std::string &containerStop, 
    const SUMOVehicleParameter::Stop& stopParameters) {
    //
}


void
RouteHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
/*
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject 
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_BUS_STOP:
                parseBusStopAttributes(attrs);
                break;
            case SUMO_TAG_TRAIN_STOP:
                parseTrainStopAttributes(attrs);
                break;
            case SUMO_TAG_ACCESS:
                parseAccessAttributes(attrs);
                break;
            case SUMO_TAG_CONTAINER_STOP:
                parseContainerStopAttributes(attrs);
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseChargingStationAttributes(attrs);
                break;
            case SUMO_TAG_PARKING_AREA:
                parseParkingAreaAttributes(attrs);
                break;
            case SUMO_TAG_PARKING_SPACE:
                parseParkingSpaceAttributes(attrs);
                break;
            // Detectors
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                parseE1Attributes(attrs);
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                parseE2Attributes(attrs);
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                parseE3Attributes(attrs);
                break;
            case SUMO_TAG_DET_ENTRY:
                parseEntryAttributes(attrs);
                break;
            case SUMO_TAG_DET_EXIT:
                parseExitAttributes(attrs);
                break;
            case SUMO_TAG_INSTANT_INDUCTION_LOOP:
                parseE1InstantAttributes(attrs);
                break;
            // TAZs
            case SUMO_TAG_TAZ:
                parseTAZAttributes(attrs);
                break;
            case SUMO_TAG_TAZSOURCE:
                parseTAZSourceAttributes(attrs);
                break;
            case SUMO_TAG_TAZSINK:
                parseTAZSinkAttributes(attrs);
                break;
            // Variable Speed Sign
            case SUMO_TAG_VSS:
                parseVariableSpeedSignAttributes(attrs);
                break;
            case SUMO_TAG_STEP:
                parseVariableSpeedSignStepAttributes(attrs);
                break;
            // Calibrator
            case SUMO_TAG_CALIBRATOR:
            case SUMO_TAG_LANECALIBRATOR:
                parseCalibratorAttributes(attrs);
                break;
            // flow (calibrator)
            case SUMO_TAG_FLOW:
                parseCalibratorFlowAttributes(attrs);
                break;
            // Rerouter
            case SUMO_TAG_REROUTER:
                parseRerouterAttributes(attrs);
                break;
            case SUMO_TAG_INTERVAL:
                parseRerouterIntervalAttributes(attrs);
                break;
            case SUMO_TAG_CLOSING_LANE_REROUTE:
                parseClosingLaneRerouteAttributes(attrs);
                break;
            case SUMO_TAG_CLOSING_REROUTE:
                parseClosingRerouteAttributes(attrs);
                break;
            case SUMO_TAG_DEST_PROB_REROUTE:
                parseDestProbRerouteAttributes(attrs);
                break;
            case SUMO_TAG_PARKING_ZONE_REROUTE:
                parseParkingAreaRerouteAttributes(attrs);
                break;
            case SUMO_TAG_ROUTE_PROB_REROUTE:
                parseRouteProbRerouteAttributes(attrs);
                break;
            // Route probe
            case SUMO_TAG_ROUTEPROBE:
                parseRouteProbeAttributes(attrs);
                break;
            // Vaporizer (deprecated)
            case SUMO_TAG_VAPORIZER:
                parseVaporizerAttributes(attrs);
                break;
            // Poly
            case SUMO_TAG_POLY:
                parsePolyAttributes(attrs);
                break;
            case SUMO_TAG_POI:
                parsePOIAttributes(attrs);
                break;
            // parameters
            case SUMO_TAG_PARAM:
                parseParameters(attrs);
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
*/
}


void
RouteHandler::myEndElement(int element) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject 
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (tag) {
        // Stopping Places
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
        case SUMO_TAG_CONTAINER_STOP:
        case SUMO_TAG_CHARGING_STATION:
        case SUMO_TAG_PARKING_AREA:
        // detectors
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_LANE_AREA_DETECTOR:
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
        // TAZs
        case SUMO_TAG_TAZ:
        // Variable Speed Sign
        case SUMO_TAG_VSS:
        // Calibrator
        case SUMO_TAG_CALIBRATOR:
        case SUMO_TAG_LANECALIBRATOR:
        // Rerouter
        case SUMO_TAG_REROUTER:
        // Route probe
        case SUMO_TAG_ROUTEPROBE:
        // Vaporizer (deprecated)
        case SUMO_TAG_VAPORIZER:
        // Shapes
        case SUMO_TAG_POLY:
        case SUMO_TAG_POI:
            // parse object and all their childrens
            parseSumoBaseObject(obj);
            // delete object (and all of their childrens)
            delete obj;
            break;
        default:
            break;
    }
}


void 
RouteHandler::parseRoute(const SUMOSAXAttributes& attrs) {
    //
}


void 
RouteHandler::parseVehicleOverRoute(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseFlowOverRoute(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseVehicleEmbeddedRoute(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseFlowEmbeddedRoute(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseTrip(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseFlow(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseStop(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parsePerson(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parsePersonFlow(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parsePersonTrip(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseWalk(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseRide(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler:: parseStopPerson(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseContainer(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseContainerFlow(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseTransport(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseTranship(const SUMOSAXAttributes& attrs) {
    //
}


void
RouteHandler::parseStopContainer(const SUMOSAXAttributes& attrs) {
    //
}


void 
RouteHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // get SumoBaseObject parent
    CommonXMLStructure::SumoBaseObject* SumoBaseObjectParent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    // check parent
    if (SumoBaseObjectParent == nullptr) {
        WRITE_ERROR("Parameters must be defined within an object");
    }
    // check tag
    if (SumoBaseObjectParent->getTag() == SUMO_TAG_NOTHING) {
        WRITE_ERROR("Parameters cannot be defined in either the route element file's root nor another parameter");
    }
    // continue if key was sucesfully loaded
    if (parsedOk) {
        // get tag str
        const std::string parentTagStr = toString(SumoBaseObjectParent->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key cannot be empty");
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key contains invalid characters");
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + parentTagStr);
            // insert parameter in SumoBaseObjectParent
            SumoBaseObjectParent->addParameter(key, value);
        }
    }
}


void
RouteHandler::checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) const {
    // check that parent SUMOBaseObject's tag is the parentTag
    if ((myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject() && 
        (myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject()->getTag() == parentTag)) == false) {
        WRITE_ERROR(toString(currentTag) + " must be defined within the definition of a " + toString(parentTag));
        ok = false;
    }
}

/****************************************************************************/
