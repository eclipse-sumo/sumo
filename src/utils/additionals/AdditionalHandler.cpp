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
/// @file    AdditionalHandler.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2015
///
// The XML-Handler for network loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>

#include "AdditionalHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

AdditionalHandler::AdditionalHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


AdditionalHandler::~AdditionalHandler() {}


bool
AdditionalHandler::parse() {
    // run parser and save result
    const bool parserResult = XMLSubSys::runParser(*this, getFileName());
    // now parse over SumoBaseObjects
    if (myCommonXMLStructure.getSumoBaseObjectRoot()) {
        parseSumoBaseObject(myCommonXMLStructure.getSumoBaseObjectRoot());
    }

    return parserResult;
}


void
AdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag> (element);
    // check tag
    try {
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_BUS_STOP:
            case SUMO_TAG_TRAIN_STOP:
                parseBusStopAttributes(attrs);
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
            case SUMO_TAG_E2DETECTOR_MULTILANE:
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
            case SUMO_TAG_FLOW_CALIBRATOR:
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
}


void
AdditionalHandler::myEndElement(int element) {
    // get object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getLastInsertedSumoBaseObject();
    // check object
    if (obj) {
        // obtain tag
        const SumoXMLTag tag = static_cast<SumoXMLTag> (element);
        // check tag
        switch (tag) {
            // Stopping Places
            case SUMO_TAG_BUS_STOP:
            case SUMO_TAG_TRAIN_STOP:
            case SUMO_TAG_ACCESS:
            case SUMO_TAG_CONTAINER_STOP:
            case SUMO_TAG_CHARGING_STATION:
            case SUMO_TAG_PARKING_AREA:
            // detectors
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_E2DETECTOR_MULTILANE:
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
                // parse object and all their childrens
                parseSumoBaseObject(obj);
                // just close node
                myCommonXMLStructure.closeTag();
                // delete object
                delete obj;
                break;
            default:
                // just close node
                myCommonXMLStructure.closeTag();
                break;
        }
    }
}


void
AdditionalHandler::parseBusStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // now obtain attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk, false);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, false);
    const std::string startPos = attrs.get<std::string>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, false);
    const std::string endPos = attrs.get<std::string>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, false);
    const std::string name = attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, false);
    const std::vector<std::string> lines = attrs.get<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, false);
    const double personCapacity = attrs.get<double>(SUMO_ATTR_PERSON_CAPACITY, id.c_str(), parsedOk, false);
    const double parkingLength = attrs.get<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), parsedOk, false);
    const bool friendlyPos = attrs.get<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // first open tag
        myCommonXMLStructure.openTag(SUMO_TAG_BUS_STOP);
        // add all attributes
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PARKING_LENGTH, parkingLength);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseAccessAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseContainerStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // now obtain attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk, false);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, false);
    const std::string startPos = attrs.get<std::string>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, false);
    const std::string endPos = attrs.get<std::string>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, false);
    const std::string name = attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, false);
    const std::vector<std::string> lines = attrs.get<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, false);
    const bool friendlyPos = attrs.get<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // first open tag
        myCommonXMLStructure.openTag(SUMO_TAG_CONTAINER_STOP);
        // add all attributes
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseChargingStationAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // now obtain attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk, false);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, false);
    const std::string startPos = attrs.get<std::string>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, false);
    const std::string endPos = attrs.get<std::string>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, false);
    const std::string name = attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, false);
    const std::vector<std::string> lines = attrs.get<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, false);
    const double chargingPower = attrs.get<double>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), parsedOk, false);
    const double efficiency = attrs.get<double>(SUMO_ATTR_EFFICIENCY, id.c_str(), parsedOk, false);
    const bool chargeInTransit = attrs.get<bool>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), parsedOk, false);
    const SUMOTime chargeDelay = attrs.get<SUMOTime>(SUMO_ATTR_CHARGEDELAY, id.c_str(), parsedOk, false);
    const bool friendlyPos = attrs.get<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // first open tag
        myCommonXMLStructure.openTag(SUMO_TAG_BUS_STOP);
        // add all attributes
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_CHARGINGPOWER, chargingPower);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_EFFICIENCY, efficiency);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_CHARGEINTRANSIT, chargeInTransit);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addSUMOTimeAttribute(SUMO_ATTR_CHARGEDELAY, chargeDelay);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseParkingAreaAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseParkingSpaceAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseE1Attributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // now obtain attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk, false);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, false);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, false);
    const SUMOTime frequency = attrs.get<SUMOTime>(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk, false);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk, false);
    const std::string vehicleTypes = attrs.get<std::string>(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, false);
    const std::string name = attrs.get<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, false);
    const bool friendlyPos = attrs.get<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // first open tag
        myCommonXMLStructure.openTag(SUMO_TAG_E1DETECTOR);
        // add all attributes
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addSUMOTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getLastInsertedSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseE2Attributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseE3Attributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseEntryAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseExitAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseE1InstantAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseTAZAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseTAZSourceAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseTAZSinkAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseVariableSpeedSignAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseVariableSpeedSignStepAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseCalibratorAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseCalibratorFlowAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseRerouterAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseRerouterIntervalAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseClosingLaneRerouteAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseClosingRerouteAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseDestProbRerouteAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseParkingAreaRerouteAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseRouteProbRerouteAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseRouteProbeAttributes(const SUMOSAXAttributes& attrs) {

}


void
AdditionalHandler::parseVaporizerAttributes(const SUMOSAXAttributes& attrs) {

}


void 
AdditionalHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // continue if key was sucesfully loaded
    if (parsedOk && myCommonXMLStructure.getLastInsertedSumoBaseObject()) {
        // get tag str
        const std::string tagStr = toString(myCommonXMLStructure.getLastInsertedSumoBaseObject()->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            WRITE_WARNING("Error parsing key from " + tagStr + " generic parameter. Key cannot be empty");
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            WRITE_WARNING("Error parsing key from " + tagStr + " generic parameter. Key contains invalid characters");
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + tagStr);
            // insert parameter
            myCommonXMLStructure.getLastInsertedSumoBaseObject()->addParameter(key, value);
        }
    }
}


void 
AdditionalHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            buildBusStop(attrs);
            break;
        case SUMO_TAG_ACCESS:
            buildAccess(attrs);
            break;
        case SUMO_TAG_CONTAINER_STOP:
            buildContainerStop(attrs);
            break;
        case SUMO_TAG_CHARGING_STATION:
            buildChargingStation(attrs);
            break;
        case SUMO_TAG_PARKING_AREA:
            buildParkingArea(attrs);
            break;
        case SUMO_TAG_PARKING_SPACE:
            buildParkingSpace(attrs);
            break;
        // Detectors
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
            // build E1
            buildE1Detector(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getStringAttribute(SUMO_ATTR_LANE),
                obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                obj->getSUMOTimeAttribute(SUMO_ATTR_FREQUENCY),
                obj->getStringAttribute(SUMO_ATTR_FILE),
                obj->getStringAttribute(SUMO_ATTR_VTYPES),
                obj->getStringAttribute(SUMO_ATTR_NAME),
                obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                obj->getParameters());
            break;
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_LANE_AREA_DETECTOR:
            buildSingleLaneDetectorE2(attrs);
            break;
        case SUMO_TAG_E2DETECTOR_MULTILANE:
            buildMultiLaneDetectorE2(attrs);
            break;
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            buildDetectorE3(attrs);
            break;
        case SUMO_TAG_DET_ENTRY:
            buildDetectorEntry(attrs);
            break;
        case SUMO_TAG_DET_EXIT:
            buildDetectorExit(attrs);
            break;
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
            buildDetectorE1Instant(attrs);
            break;
        // TAZs
        case SUMO_TAG_TAZ:
            buildTAZ(attrs);
            break;
        case SUMO_TAG_TAZSOURCE:
            buildTAZSource(attrs);
            break;
        case SUMO_TAG_TAZSINK:
            buildTAZSink(attrs);
            break;
        // Variable Speed Sign
        case SUMO_TAG_VSS:
            buildVariableSpeedSign(attrs);
            break;
        case SUMO_TAG_STEP:
            buildVariableSpeedSignStep(attrs);
            break;
        // Calibrator
        case SUMO_TAG_CALIBRATOR:
            buildEdgeCalibrator(attrs);
            break;
        case SUMO_TAG_LANECALIBRATOR:
            buildLaneCalibrator(attrs);
            break;
        case SUMO_TAG_FLOW_CALIBRATOR:
            buildCalibratorFlow(attrs);
            break;
        // Rerouter
        case SUMO_TAG_REROUTER:
            buildRerouter(attrs);
            break;
        case SUMO_TAG_INTERVAL:
            buildRerouterInterval(attrs);
            break;
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            buildClosingLaneReroute(attrs);
            break;
        case SUMO_TAG_CLOSING_REROUTE:
            buildClosingReroute(attrs);
            break;
        case SUMO_TAG_DEST_PROB_REROUTE:
            builDestProbReroute(attrs);
            break;
        case SUMO_TAG_PARKING_ZONE_REROUTE:
            builParkingAreaReroute(attrs);
            break;
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            buildRouteProbReroute(attrs);
            break;
        // Route probe
        case SUMO_TAG_ROUTEPROBE:
            buildRouteProbe(attrs);
            break;
        // Vaporizer (deprecated)
        case SUMO_TAG_VAPORIZER:
            buildVaporizer(attrs);
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

/****************************************************************************/
