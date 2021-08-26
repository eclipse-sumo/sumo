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
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The XML-Handler for additionals loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RGBColor.h>
#include <utils/shapes/Shape.h>

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
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void
AdditionalHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_BUS_STOP:
            buildBusStop(obj,
                         obj->getStringAttribute(SUMO_ATTR_ID),
                         obj->getStringAttribute(SUMO_ATTR_LANE),
                         obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                         obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                         obj->getStringAttribute(SUMO_ATTR_NAME),
                         obj->getStringListAttribute(SUMO_ATTR_LINES),
                         obj->getIntAttribute(SUMO_ATTR_PERSON_CAPACITY),
                         obj->getDoubleAttribute(SUMO_ATTR_PARKING_LENGTH),
                         obj->getColorAttribute(SUMO_ATTR_COLOR),
                         obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                         obj->getParameters());
            break;
        case SUMO_TAG_TRAIN_STOP:
            buildTrainStop(obj,
                           obj->getStringAttribute(SUMO_ATTR_ID),
                           obj->getStringAttribute(SUMO_ATTR_LANE),
                           obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                           obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                           obj->getStringAttribute(SUMO_ATTR_NAME),
                           obj->getStringListAttribute(SUMO_ATTR_LINES),
                           obj->getIntAttribute(SUMO_ATTR_PERSON_CAPACITY),
                           obj->getDoubleAttribute(SUMO_ATTR_PARKING_LENGTH),
                           obj->getColorAttribute(SUMO_ATTR_COLOR),
                           obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                           obj->getParameters());
            break;
        case SUMO_TAG_ACCESS:
            buildAccess(obj,
                        obj->getStringAttribute(SUMO_ATTR_LANE),
                        obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                        obj->getDoubleAttribute(SUMO_ATTR_LENGTH),
                        obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                        obj->getParameters());
            break;
        case SUMO_TAG_CONTAINER_STOP:
            buildContainerStop(obj,
                               obj->getStringAttribute(SUMO_ATTR_ID),
                               obj->getStringAttribute(SUMO_ATTR_LANE),
                               obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                               obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                               obj->getStringAttribute(SUMO_ATTR_NAME),
                               obj->getStringListAttribute(SUMO_ATTR_LINES),
                               obj->getIntAttribute(SUMO_ATTR_CONTAINER_CAPACITY),
                               obj->getDoubleAttribute(SUMO_ATTR_PARKING_LENGTH),
                               obj->getColorAttribute(SUMO_ATTR_COLOR),
                               obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                               obj->getParameters());
            break;
        case SUMO_TAG_CHARGING_STATION:
            buildChargingStation(obj,
                                 obj->getStringAttribute(SUMO_ATTR_ID),
                                 obj->getStringAttribute(SUMO_ATTR_LANE),
                                 obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                                 obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                                 obj->getStringAttribute(SUMO_ATTR_NAME),
                                 obj->getDoubleAttribute(SUMO_ATTR_CHARGINGPOWER),
                                 obj->getDoubleAttribute(SUMO_ATTR_EFFICIENCY),
                                 obj->getBoolAttribute(SUMO_ATTR_CHARGEINTRANSIT),
                                 obj->getTimeAttribute(SUMO_ATTR_CHARGEDELAY),
                                 obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                                 obj->getParameters());
            break;
        case SUMO_TAG_PARKING_AREA:
            buildParkingArea(obj,
                             obj->getStringAttribute(SUMO_ATTR_ID),
                             obj->getStringAttribute(SUMO_ATTR_LANE),
                             obj->getDoubleAttribute(SUMO_ATTR_STARTPOS),
                             obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                             obj->getStringAttribute(SUMO_ATTR_DEPARTPOS),
                             obj->getStringAttribute(SUMO_ATTR_NAME),
                             obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                             obj->getIntAttribute(SUMO_ATTR_ROADSIDE_CAPACITY),
                             obj->getBoolAttribute(SUMO_ATTR_ONROAD),
                             obj->getDoubleAttribute(SUMO_ATTR_WIDTH),
                             obj->getDoubleAttribute(SUMO_ATTR_LENGTH),
                             obj->getDoubleAttribute(SUMO_ATTR_ANGLE),
                             obj->getParameters());
            break;
        case SUMO_TAG_PARKING_SPACE:
            buildParkingSpace(obj,
                              obj->getDoubleAttribute(SUMO_ATTR_X),
                              obj->getDoubleAttribute(SUMO_ATTR_Y),
                              obj->getDoubleAttribute(SUMO_ATTR_Z),
                              obj->getStringAttribute(SUMO_ATTR_NAME),
                              obj->getStringAttribute(SUMO_ATTR_WIDTH),
                              obj->getStringAttribute(SUMO_ATTR_LENGTH),
                              obj->getStringAttribute(SUMO_ATTR_ANGLE),
                              obj->getDoubleAttribute(SUMO_ATTR_SLOPE),
                              obj->getParameters());
            break;
        // Detectors
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
            // build E1
            buildE1Detector(obj,
                            obj->getStringAttribute(SUMO_ATTR_ID),
                            obj->getStringAttribute(SUMO_ATTR_LANE),
                            obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                            obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                            obj->getStringAttribute(SUMO_ATTR_FILE),
                            obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                            obj->getStringAttribute(SUMO_ATTR_NAME),
                            obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                            obj->getParameters());
            break;
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_LANE_AREA_DETECTOR:
            if (obj->hasStringAttribute(SUMO_ATTR_LANE)) {
                buildSingleLaneDetectorE2(obj,
                                          obj->getStringAttribute(SUMO_ATTR_ID),
                                          obj->getStringAttribute(SUMO_ATTR_LANE),
                                          obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                                          obj->getDoubleAttribute(SUMO_ATTR_LENGTH),
                                          obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                                          obj->getStringAttribute(SUMO_ATTR_TLID),
                                          obj->getStringAttribute(SUMO_ATTR_FILE),
                                          obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                          obj->getStringAttribute(SUMO_ATTR_NAME),
                                          obj->getTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD),
                                          obj->getDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD),
                                          obj->getDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD),
                                          obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                                          obj->getParameters());
            } else {
                buildMultiLaneDetectorE2(obj,
                                         obj->getStringAttribute(SUMO_ATTR_ID),
                                         obj->getStringListAttribute(SUMO_ATTR_LANES),
                                         obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                                         obj->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                                         obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                                         obj->getStringAttribute(SUMO_ATTR_TLID),
                                         obj->getStringAttribute(SUMO_ATTR_FILE),
                                         obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                         obj->getStringAttribute(SUMO_ATTR_NAME),
                                         obj->getTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD),
                                         obj->getDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD),
                                         obj->getDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD),
                                         obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                                         obj->getParameters());
            }
            break;
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            buildDetectorE3(obj,
                            obj->getStringAttribute(SUMO_ATTR_ID),
                            obj->getPositionAttribute(SUMO_ATTR_POSITION),
                            obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                            obj->getStringAttribute(SUMO_ATTR_FILE),
                            obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                            obj->getStringAttribute(SUMO_ATTR_NAME),
                            obj->getTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD),
                            obj->getDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD),
                            obj->getParameters());
            break;
        case SUMO_TAG_DET_ENTRY:
            buildDetectorEntry(obj,
                               obj->getStringAttribute(SUMO_ATTR_LANE),
                               obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                               obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                               obj->getParameters());
            break;
        case SUMO_TAG_DET_EXIT:
            buildDetectorExit(obj,
                              obj->getStringAttribute(SUMO_ATTR_LANE),
                              obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                              obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                              obj->getParameters());
            break;
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
            buildDetectorE1Instant(obj,
                                   obj->getStringAttribute(SUMO_ATTR_ID),
                                   obj->getStringAttribute(SUMO_ATTR_LANE),
                                   obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                                   obj->getStringAttribute(SUMO_ATTR_FILE),
                                   obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                   obj->getStringAttribute(SUMO_ATTR_NAME),
                                   obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                                   obj->getParameters());
            break;
        // TAZs
        case SUMO_TAG_TAZ:
            buildTAZ(obj,
                     obj->getStringAttribute(SUMO_ATTR_ID),
                     obj->getPositionVectorAttribute(SUMO_ATTR_SHAPE),
                     obj->getColorAttribute(SUMO_ATTR_COLOR),
                     obj->getStringListAttribute(SUMO_ATTR_EDGES),
                     obj->getStringAttribute(SUMO_ATTR_NAME),
                     obj->getParameters());
            break;
        case SUMO_TAG_TAZSOURCE:
            buildTAZSource(obj,
                           obj->getStringAttribute(SUMO_ATTR_ID),
                           obj->getDoubleAttribute(SUMO_ATTR_WEIGHT));
            break;
        case SUMO_TAG_TAZSINK:
            buildTAZSink(obj,
                         obj->getStringAttribute(SUMO_ATTR_ID),
                         obj->getDoubleAttribute(SUMO_ATTR_WEIGHT));
            break;
        // Variable Speed Sign
        case SUMO_TAG_VSS:
            buildVariableSpeedSign(obj,
                                   obj->getStringAttribute(SUMO_ATTR_ID),
                                   obj->getPositionAttribute(SUMO_ATTR_POSITION),
                                   obj->getStringListAttribute(SUMO_ATTR_LANES),
                                   obj->getStringAttribute(SUMO_ATTR_NAME),
                                   obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                   obj->getParameters());
            break;
        case SUMO_TAG_STEP:
            buildVariableSpeedSignStep(obj,
                                       obj->getTimeAttribute(SUMO_ATTR_TIME),
                                       obj->getStringAttribute(SUMO_ATTR_SPEED));
            break;
        // Calibrator
        case SUMO_TAG_CALIBRATOR:
            buildEdgeCalibrator(obj,
                                obj->getStringAttribute(SUMO_ATTR_ID),
                                obj->getStringAttribute(SUMO_ATTR_EDGE),
                                obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                                obj->getStringAttribute(SUMO_ATTR_NAME),
                                obj->getStringAttribute(SUMO_ATTR_OUTPUT),
                                obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                                obj->getStringAttribute(SUMO_ATTR_ROUTEPROBE),
                                obj->getDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD),
                                obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                obj->getParameters());
            break;
        case SUMO_TAG_LANECALIBRATOR:
            buildLaneCalibrator(obj,
                                obj->getStringAttribute(SUMO_ATTR_ID),
                                obj->getStringAttribute(SUMO_ATTR_LANE),
                                obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                                obj->getStringAttribute(SUMO_ATTR_NAME),
                                obj->getStringAttribute(SUMO_ATTR_OUTPUT),
                                obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                                obj->getStringAttribute(SUMO_ATTR_ROUTEPROBE),
                                obj->getDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD),
                                obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                obj->getParameters());
            break;
        case SUMO_TAG_FLOW:
            buildCalibratorFlow(obj,
                                obj->getStringAttribute(SUMO_ATTR_TYPE),
                                obj->getStringAttribute(SUMO_ATTR_ROUTE),
                                obj->getStringAttribute(SUMO_ATTR_VEHSPERHOUR),
                                obj->getStringAttribute(SUMO_ATTR_SPEED),
                                obj->getColorAttribute(SUMO_ATTR_COLOR),
                                obj->getStringAttribute(SUMO_ATTR_DEPARTLANE),
                                obj->getStringAttribute(SUMO_ATTR_DEPARTPOS),
                                obj->getStringAttribute(SUMO_ATTR_DEPARTSPEED),
                                obj->getStringAttribute(SUMO_ATTR_ARRIVALLANE),
                                obj->getStringAttribute(SUMO_ATTR_ARRIVALPOS),
                                obj->getStringAttribute(SUMO_ATTR_ARRIVALSPEED),
                                obj->getStringAttribute(SUMO_ATTR_LINE),
                                obj->getIntAttribute(SUMO_ATTR_NUMBER),
                                obj->getIntAttribute(SUMO_ATTR_CONTAINER_NUMBER),
                                obj->getBoolAttribute(SUMO_ATTR_REROUTE),
                                obj->getStringAttribute(SUMO_ATTR_DEPARTPOS_LAT),
                                obj->getStringAttribute(SUMO_ATTR_ARRIVALPOS_LAT),
                                obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                                obj->getTimeAttribute(SUMO_ATTR_END),
                                obj->getParameters());
            break;
        // Rerouter
        case SUMO_TAG_REROUTER:
            buildRerouter(obj,
                          obj->getStringAttribute(SUMO_ATTR_ID),
                          obj->getPositionAttribute(SUMO_ATTR_POSITION),
                          obj->getStringListAttribute(SUMO_ATTR_EDGES),
                          obj->getDoubleAttribute(SUMO_ATTR_PROB),
                          obj->getStringAttribute(SUMO_ATTR_NAME),
                          obj->getStringAttribute(SUMO_ATTR_FILE),
                          obj->getBoolAttribute(SUMO_ATTR_OFF),
                          obj->getTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD),
                          obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                          obj->getParameters());
            break;
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            buildClosingLaneReroute(obj,
                                    obj->getStringAttribute(SUMO_ATTR_ID),
                                    parseVehicleClasses(obj->getStringAttribute(SUMO_ATTR_ALLOW), obj->getStringAttribute(SUMO_ATTR_DISALLOW)));
            break;
        case SUMO_TAG_CLOSING_REROUTE:
            buildClosingReroute(obj,
                                obj->getStringAttribute(SUMO_ATTR_ID),
                                parseVehicleClasses(obj->getStringAttribute(SUMO_ATTR_ALLOW), obj->getStringAttribute(SUMO_ATTR_DISALLOW)));
            break;
        case SUMO_TAG_DEST_PROB_REROUTE:
            buildDestProbReroute(obj,
                                 obj->getStringAttribute(SUMO_ATTR_ID),
                                 obj->getDoubleAttribute(SUMO_ATTR_PROB));
            break;
        case SUMO_TAG_PARKING_ZONE_REROUTE:
            buildParkingAreaReroute(obj,
                                    obj->getStringAttribute(SUMO_ATTR_ID),
                                    obj->getDoubleAttribute(SUMO_ATTR_PROB),
                                    obj->getBoolAttribute(SUMO_ATTR_VISIBLE));
            break;
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            buildRouteProbReroute(obj,
                                  obj->getStringAttribute(SUMO_ATTR_ID),
                                  obj->getDoubleAttribute(SUMO_ATTR_PROB));
            break;
        case SUMO_TAG_INTERVAL:
            // check if is VSS or a REROUTER interval
            if (obj->getParentSumoBaseObject()->getTag() == SUMO_TAG_REROUTER) {
                buildRerouterInterval(obj,
                                      obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                                      obj->getTimeAttribute(SUMO_ATTR_END));
            } else {
                buildVariableSpeedSignStep(obj,
                                           obj->getTimeAttribute(SUMO_ATTR_TIME),
                                           obj->getStringAttribute(SUMO_ATTR_SPEED));
            }
            break;
        // Route probe
        case SUMO_TAG_ROUTEPROBE:
            buildRouteProbe(obj,
                            obj->getStringAttribute(SUMO_ATTR_ID),
                            obj->getStringAttribute(SUMO_ATTR_EDGE),
                            obj->getTimeAttribute(SUMO_ATTR_FREQUENCY),
                            obj->getStringAttribute(SUMO_ATTR_NAME),
                            obj->getStringAttribute(SUMO_ATTR_FILE),
                            obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                            obj->getParameters());
            break;
        // Vaporizer (deprecated)
        case SUMO_TAG_VAPORIZER:
            buildVaporizer(obj,
                           obj->getStringAttribute(SUMO_ATTR_ID),
                           obj->getTimeAttribute(SUMO_ATTR_BEGIN),
                           obj->getTimeAttribute(SUMO_ATTR_END),
                           obj->getStringAttribute(SUMO_ATTR_NAME),
                           obj->getParameters());
            break;
        // Polygon
        case SUMO_TAG_POLY:
            buildPolygon(obj,
                         obj->getStringAttribute(SUMO_ATTR_ID),
                         obj->getStringAttribute(SUMO_ATTR_TYPE),
                         obj->getColorAttribute(SUMO_ATTR_COLOR),
                         obj->getDoubleAttribute(SUMO_ATTR_LAYER),
                         obj->getDoubleAttribute(SUMO_ATTR_ANGLE),
                         obj->getStringAttribute(SUMO_ATTR_IMGFILE),
                         obj->getBoolAttribute(SUMO_ATTR_RELATIVEPATH),
                         obj->getPositionVectorAttribute(SUMO_ATTR_SHAPE),
                         obj->getBoolAttribute(SUMO_ATTR_GEO),
                         obj->getBoolAttribute(SUMO_ATTR_FILL),
                         obj->getDoubleAttribute(SUMO_ATTR_LINEWIDTH),
                         obj->getStringAttribute(SUMO_ATTR_NAME),
                         obj->getParameters());
            break;
        // POI
        case SUMO_TAG_POI:
            // check if we want to create a POI, POILane or POIGEO
            if (obj->hasDoubleAttribute(SUMO_ATTR_X)) {
                // build PO
                buildPOI(obj,
                         obj->getStringAttribute(SUMO_ATTR_ID),
                         obj->getStringAttribute(SUMO_ATTR_TYPE),
                         obj->getColorAttribute(SUMO_ATTR_COLOR),
                         obj->getDoubleAttribute(SUMO_ATTR_X),
                         obj->getDoubleAttribute(SUMO_ATTR_Y),
                         obj->getDoubleAttribute(SUMO_ATTR_LAYER),
                         obj->getDoubleAttribute(SUMO_ATTR_ANGLE),
                         obj->getStringAttribute(SUMO_ATTR_IMGFILE),
                         obj->getBoolAttribute(SUMO_ATTR_RELATIVEPATH),
                         obj->getDoubleAttribute(SUMO_ATTR_WIDTH),
                         obj->getDoubleAttribute(SUMO_ATTR_HEIGHT),
                         obj->getStringAttribute(SUMO_ATTR_NAME),
                         obj->getParameters());
            } else if (obj->hasStringAttribute(SUMO_ATTR_LANE)) {
                // build POI over Lane
                buildPOILane(obj,
                             obj->getStringAttribute(SUMO_ATTR_ID),
                             obj->getStringAttribute(SUMO_ATTR_TYPE),
                             obj->getColorAttribute(SUMO_ATTR_COLOR),
                             obj->getStringAttribute(SUMO_ATTR_LANE),
                             obj->getDoubleAttribute(SUMO_ATTR_POSITION),
                             obj->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                             obj->getDoubleAttribute(SUMO_ATTR_POSITION_LAT),
                             obj->getDoubleAttribute(SUMO_ATTR_LAYER),
                             obj->getDoubleAttribute(SUMO_ATTR_ANGLE),
                             obj->getStringAttribute(SUMO_ATTR_IMGFILE),
                             obj->getBoolAttribute(SUMO_ATTR_RELATIVEPATH),
                             obj->getDoubleAttribute(SUMO_ATTR_WIDTH),
                             obj->getDoubleAttribute(SUMO_ATTR_HEIGHT),
                             obj->getStringAttribute(SUMO_ATTR_NAME),
                             obj->getParameters());
            } else {
                // build POIGEO
                buildPOIGeo(obj,
                            obj->getStringAttribute(SUMO_ATTR_ID),
                            obj->getStringAttribute(SUMO_ATTR_TYPE),
                            obj->getColorAttribute(SUMO_ATTR_COLOR),
                            obj->getDoubleAttribute(SUMO_ATTR_LON),
                            obj->getDoubleAttribute(SUMO_ATTR_LAT),
                            obj->getDoubleAttribute(SUMO_ATTR_LAYER),
                            obj->getDoubleAttribute(SUMO_ATTR_ANGLE),
                            obj->getStringAttribute(SUMO_ATTR_IMGFILE),
                            obj->getBoolAttribute(SUMO_ATTR_RELATIVEPATH),
                            obj->getDoubleAttribute(SUMO_ATTR_WIDTH),
                            obj->getDoubleAttribute(SUMO_ATTR_HEIGHT),
                            obj->getStringAttribute(SUMO_ATTR_NAME),
                            obj->getParameters());
            }
            break;
        default:
            break;
    }
    // now iterate over childrens
    for (const auto& child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
}


void
AdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
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
}


void
AdditionalHandler::myEndElement(int element) {
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
AdditionalHandler::parseBusStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, std::vector<std::string>());
    const int personCapacity = attrs.getOpt<int>(SUMO_ATTR_PERSON_CAPACITY, id.c_str(), parsedOk, 6);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), parsedOk, 0);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor(false));
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_BUS_STOP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PARKING_LENGTH, parkingLength);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseTrainStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, std::vector<std::string>());
    const int personCapacity = attrs.getOpt<int>(SUMO_ATTR_PERSON_CAPACITY, id.c_str(), parsedOk, 6);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), parsedOk, 0);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor(false));
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRAIN_STOP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PARKING_LENGTH, parkingLength);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseAccessAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, "", parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, "", parsedOk);
    // optional attributes
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, "", parsedOk, -1.00); /* in future updates, INVALID_DOUBLE */
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, "", parsedOk, false);
    // check parent
    checkParent(SUMO_TAG_ACCESS, {SUMO_TAG_BUS_STOP, SUMO_TAG_TRAIN_STOP}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ACCESS);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LENGTH, length);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseContainerStopAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, std::vector<std::string>());
    const int containerCapacity = attrs.getOpt<int>(SUMO_ATTR_CONTAINER_CAPACITY, id.c_str(), parsedOk, 6);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), parsedOk, 0);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor(false));
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONTAINER_STOP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_CONTAINER_CAPACITY, containerCapacity);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PARKING_LENGTH, parkingLength);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseChargingStationAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), parsedOk, std::vector<std::string>());
    const double chargingPower = attrs.getOpt<double>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), parsedOk, 22000);
    const double efficiency = attrs.getOpt<double>(SUMO_ATTR_EFFICIENCY, id.c_str(), parsedOk, 0.95);
    const bool chargeInTransit = attrs.getOpt<bool>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), parsedOk, 0);
    const SUMOTime chargeDelay = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CHARGEDELAY, id.c_str(), parsedOk, 0);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CHARGING_STATION);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_CHARGINGPOWER, chargingPower);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_EFFICIENCY, efficiency);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_CHARGEINTRANSIT, chargeInTransit);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_CHARGEDELAY, chargeDelay);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseParkingAreaAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    // optional attributes
    const double startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, INVALID_DOUBLE);
    const std::string departPos = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTPOS, id.c_str(), parsedOk, "");
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    const int roadSideCapacity = attrs.getOpt<int>(SUMO_ATTR_ROADSIDE_CAPACITY, id.c_str(), parsedOk, 0);
    const bool onRoad = attrs.getOpt<bool>(SUMO_ATTR_ONROAD, id.c_str(), parsedOk, false);
    const double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), parsedOk, 0);
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, id.c_str(), parsedOk, 0);
    const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), parsedOk, 0);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PARKING_AREA);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_ROADSIDE_CAPACITY, roadSideCapacity);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_ONROAD, onRoad);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_WIDTH, width);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LENGTH, length);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ANGLE, angle);
    }
}


void
AdditionalHandler::parseParkingSpaceAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const double x = attrs.get<double>(SUMO_ATTR_X, "", parsedOk);
    const double y = attrs.get<double>(SUMO_ATTR_Y, "", parsedOk);
    // optional attributes
    const double z = attrs.getOpt<double>(SUMO_ATTR_Z, "", parsedOk, 0);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, "", parsedOk, "");
    const std::string width = attrs.getOpt<std::string>(SUMO_ATTR_WIDTH, "", parsedOk, "");
    const std::string length = attrs.getOpt<std::string>(SUMO_ATTR_LENGTH, "", parsedOk, "");
    const std::string angle = attrs.getOpt<std::string>(SUMO_ATTR_ANGLE, "", parsedOk, "");
    const double slope = attrs.getOpt<double>(SUMO_ATTR_SLOPE, "", parsedOk, 0);
    // check parent
    checkParent(SUMO_TAG_PARKING_SPACE, {SUMO_TAG_PARKING_AREA}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PARKING_SPACE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_X, x);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_Y, y);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_Z, z);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_WIDTH, width);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LENGTH, length);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ANGLE, angle);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_SLOPE, slope);
    }
}


void
AdditionalHandler::parseE1Attributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk);
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_E1DETECTOR);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseE2Attributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // check that frequency and trafficLight aren't defined together
    if (!attrs.hasAttribute(SUMO_ATTR_FREQUENCY) && !attrs.hasAttribute(SUMO_ATTR_TLID)) {
        WRITE_ERROR("Define either '" + toString(SUMO_ATTR_FREQUENCY) + "' or '" + toString(SUMO_ATTR_TLID) + "' in a lane area detector.");
        parsedOk = false;
    }
    // check that lane and length are defined together
    if (attrs.hasAttribute(SUMO_ATTR_LANE) && !attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
        WRITE_ERROR("'lane' and 'length' must be defined together in a lane area detector.");
        parsedOk = false;
    }
    // check that lanes and endPos are defined together
    if (attrs.hasAttribute(SUMO_ATTR_LANES) && !attrs.hasAttribute(SUMO_ATTR_ENDPOS)) {
        WRITE_ERROR("'lanes' and 'endPos' must be defined together in a lane area detector.");
        parsedOk = false;
    }
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    // special attributes
    const std::string laneId = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, "");
    const std::vector<std::string> laneIds = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LANES, id.c_str(), parsedOk, std::vector<std::string>());
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, id.c_str(), parsedOk, 0);
    const double endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), parsedOk, 0);
    const SUMOTime frequency = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk, -1);
    const std::string trafficLight = attrs.getOpt<std::string>(SUMO_ATTR_TLID, id.c_str(), parsedOk, "");
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), parsedOk, TIME2STEPS(1));
    const double haltingSpeedThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), parsedOk, 1.39);
    const double jamDistThreshold = attrs.getOpt<double>(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), parsedOk, 10);
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_E2DETECTOR);
        // add attributes depending of Lane/Lanes
        if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LENGTH, length);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LANES, laneIds);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
        }
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TLID, trafficLight);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD, haltingTimeThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD, haltingSpeedThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD, jamDistThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseE3Attributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk);
    // optional attributes
    const Position pos = attrs.getOpt<Position>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, Position());
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), parsedOk, TIME2STEPS(1));
    const double haltingSpeedThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), parsedOk, 1.39);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_E3DETECTOR);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addPositionAttribute(SUMO_ATTR_POSITION, pos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD, haltingTimeThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD, haltingSpeedThreshold);
    }
}


void
AdditionalHandler::parseEntryAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, "", parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, "", parsedOk);
    // optional attributes
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, "", parsedOk, false);
    // check parent
    checkParent(SUMO_TAG_DET_ENTRY, {SUMO_TAG_E3DETECTOR}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_DET_ENTRY);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseExitAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, "", parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, "", parsedOk);
    // optional attributes
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, "", parsedOk, false);
    // check parent
    checkParent(SUMO_TAG_DET_EXIT, {SUMO_TAG_E3DETECTOR}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_DET_EXIT);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseE1InstantAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string laneId = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk);
    const double position = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_INSTANT_INDUCTION_LOOP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, laneId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, position);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
}


void
AdditionalHandler::parseTAZAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), parsedOk);
    // optional attributes
    const std::vector<std::string> edges = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk, std::vector<std::string>());
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor::RED);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TAZ);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addPositionVectorAttribute(SUMO_ATTR_SHAPE, shape);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
    }
}


void
AdditionalHandler::parseTAZSourceAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double weight = attrs.get<double>(SUMO_ATTR_WEIGHT, edgeID.c_str(), parsedOk);
    // check parent
    checkParent(SUMO_TAG_TAZSOURCE, {SUMO_TAG_TAZ}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TAZSOURCE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_WEIGHT, weight);
    }
}


void
AdditionalHandler::parseTAZSinkAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double weight = attrs.get<double>(SUMO_ATTR_WEIGHT, edgeID.c_str(), parsedOk);
    // check parent
    checkParent(SUMO_TAG_TAZSINK, {SUMO_TAG_TAZ}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TAZSINK);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_WEIGHT, weight);
    }
}


void
AdditionalHandler::parseVariableSpeedSignAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::vector<std::string> lanes = attrs.get<std::vector<std::string> >(SUMO_ATTR_LANES, id.c_str(), parsedOk);
    // optional attributes
    const Position pos = attrs.getOpt<Position>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, Position());
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VSS);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LANES, lanes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addPositionAttribute(SUMO_ATTR_POSITION, pos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
    }
}


void
AdditionalHandler::parseVariableSpeedSignStepAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const SUMOTime time = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, "", parsedOk);
    // optional attributes
    const std::string speed = attrs.getOpt<std::string>(SUMO_ATTR_SPEED, "", parsedOk, "");
    // check parent
    checkParent(SUMO_TAG_STEP, {SUMO_TAG_VSS}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_STEP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_TIME, time);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_SPEED, speed);
    }
}


void
AdditionalHandler::parseCalibratorAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // check that frecuency and trafficLight aren't defined together
    if ((attrs.hasAttribute(SUMO_ATTR_EDGE) && attrs.hasAttribute(SUMO_ATTR_LANE)) ||
            (!attrs.hasAttribute(SUMO_ATTR_EDGE) && !attrs.hasAttribute(SUMO_ATTR_LANE))) {
        WRITE_ERROR("Calibrators need either an edge or a lane");
        parsedOk = false;
    }
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double pos = attrs.get<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk);
    // special attributes
    const std::string edge = attrs.getOpt<std::string>(SUMO_ATTR_EDGE, id.c_str(), parsedOk, "");
    const std::string lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), parsedOk, "");
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const SUMOTime freq = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk, DELTA_T);
    const std::string routeProbe = attrs.getOpt<std::string>(SUMO_ATTR_ROUTEPROBE, id.c_str(), parsedOk, "");
    const double jamThreshold = attrs.getOpt<double>(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), parsedOk, 0.5);
    const std::string output = attrs.getOpt<std::string>(SUMO_ATTR_OUTPUT, id.c_str(), parsedOk, "");
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    // continue if flag is ok
    if (parsedOk) {
        // set tag depending of edge/lane
        if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CALIBRATOR);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EDGE, edge);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_LANECALIBRATOR);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, lane);
        }
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, pos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_FREQUENCY, freq);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ROUTEPROBE, routeProbe);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD, jamThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_OUTPUT, output);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
    }
}


void
AdditionalHandler::parseCalibratorFlowAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // check that frecuency and trafficLight aren't defined together
    if (!attrs.hasAttribute(SUMO_ATTR_TYPE) && !attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR) && !attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        WRITE_ERROR("CalibratorFlows need either the attribute vehsPerHour or speed or type (or any combination of these)");
        parsedOk = false;
    }
    // needed attributes
    const std::string route = attrs.get<std::string>(SUMO_ATTR_ROUTE, "", parsedOk);
    const SUMOTime begin = attrs.get<SUMOTime>(SUMO_ATTR_BEGIN, "", parsedOk);
    const SUMOTime end = attrs.get<SUMOTime>(SUMO_ATTR_BEGIN, "", parsedOk);
    // special attributes
    const std::string vType = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, "", parsedOk, "");
    const std::string vehsPerHour = attrs.getOpt<std::string>(SUMO_ATTR_VEHSPERHOUR, "", parsedOk, "");
    const std::string speed = attrs.getOpt<std::string>(SUMO_ATTR_SPEED, "", parsedOk, "");
    // optional attributes
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, "", parsedOk, RGBColor::YELLOW);
    const std::string departLane = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTLANE, "", parsedOk, "first");
    const std::string departPos = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTPOS, "", parsedOk, "base");
    const std::string departSpeed = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTSPEED, "", parsedOk, "0");
    const std::string arrivalLane = attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALLANE, "", parsedOk, "current");
    const std::string arrivalPos = attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, "max");
    const std::string arrivalSpeed = attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALSPEED, "", parsedOk, "current");
    const std::string line = attrs.getOpt<std::string>(SUMO_ATTR_LINE, "", parsedOk, "");
    const int personNumber = attrs.getOpt<int>(SUMO_ATTR_PERSON_NUMBER, "", parsedOk, 0);
    const int containerNumber = attrs.getOpt<int>(SUMO_ATTR_CONTAINER_NUMBER, "", parsedOk, 0);
    const bool reroute = attrs.getOpt<bool>(SUMO_ATTR_REROUTE, "", parsedOk, false);
    const std::string departPosLat = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTPOS_LAT, "", parsedOk, "center");
    const std::string arrivalPosLat = attrs.getOpt<std::string>(SUMO_ATTR_ARRIVALPOS_LAT, "", parsedOk, "");
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ROUTE, route);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_END, end);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TYPE, vType);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_VEHSPERHOUR, vehsPerHour);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_SPEED, speed);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DEPARTLANE, departLane);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DEPARTSPEED, departSpeed);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ARRIVALLANE, arrivalLane);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ARRIVALSPEED, arrivalSpeed);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LINE, line);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_PERSON_NUMBER, personNumber);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_CONTAINER_NUMBER, containerNumber);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_REROUTE, reroute);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DEPARTPOS_LAT, departPosLat);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ARRIVALPOS_LAT, arrivalPosLat);
    }
}


void
AdditionalHandler::parseRerouterAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::vector<std::string> edges = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk);
    // optional attributes
    const Position pos = attrs.getOpt<Position>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, Position());
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk, "");
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, id.c_str(), parsedOk, 1);
    SUMOTime timeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), parsedOk, 0);
    const std::vector<std::string> vehicleTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk, std::vector<std::string>());
    const bool off = attrs.getOpt<bool>(SUMO_ATTR_OFF, id.c_str(), parsedOk, false);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_REROUTER);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addPositionAttribute(SUMO_ATTR_POSITION, pos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD, timeThreshold);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vehicleTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_OFF, off);
    }
}


void
AdditionalHandler::parseRerouterIntervalAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, "", parsedOk);
    const SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, "", parsedOk);
    // check parent
    checkParent(SUMO_TAG_INTERVAL, {SUMO_TAG_REROUTER}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_INTERVAL);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_END, end);
    }
}


void
AdditionalHandler::parseClosingLaneRerouteAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string laneID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, "", parsedOk, "");
    const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, "", parsedOk, "");
    // check parent
    checkParent(SUMO_TAG_CLOSING_LANE_REROUTE, {SUMO_TAG_INTERVAL}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CLOSING_LANE_REROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, laneID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ALLOW, allow);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DISALLOW, disallow);
    }
}


void
AdditionalHandler::parseClosingRerouteAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, "", parsedOk, "");
    const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, "", parsedOk, "");
    // check parent
    checkParent(SUMO_TAG_CLOSING_REROUTE, {SUMO_TAG_INTERVAL}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CLOSING_REROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ALLOW, allow);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_DISALLOW, disallow);
    }
}


void
AdditionalHandler::parseDestProbRerouteAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double probability = attrs.get<double>(SUMO_ATTR_PROB, "", parsedOk);
    // check parent
    checkParent(SUMO_TAG_DEST_PROB_REROUTE, {SUMO_TAG_INTERVAL}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_DEST_PROB_REROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
    }
}


void
AdditionalHandler::parseParkingAreaRerouteAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string parkingAreaID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, "", parsedOk, 1);
    const bool visible = attrs.getOpt<bool>(SUMO_ATTR_VISIBLE, "", parsedOk, 1);
    // check parent
    checkParent(SUMO_TAG_PARKING_ZONE_REROUTE, {SUMO_TAG_INTERVAL}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PARKING_ZONE_REROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, parkingAreaID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_VISIBLE, visible);
    }
}


void
AdditionalHandler::parseRouteProbRerouteAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string routeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, "", parsedOk, 1);
    // check parent
    checkParent(SUMO_TAG_ROUTE_PROB_REROUTE, {SUMO_TAG_INTERVAL}, parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE_PROB_REROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, routeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
    }
}


void
AdditionalHandler::parseRouteProbeAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::string edge = attrs.get<std::string>(SUMO_ATTR_EDGE, id.c_str(), parsedOk);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), parsedOk);
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), parsedOk);
    SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), parsedOk, -1);
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTEPROBE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_EDGE, edge);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FILE, file);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_FREQUENCY, frequency);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
    }
}


void
AdditionalHandler::parseVaporizerAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string edgeID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, parsedOk);
    SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, nullptr, parsedOk);
    // optional attributes
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, edgeID.c_str(), parsedOk, "");
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VAPORIZER);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, edgeID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_END, end);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
    }
}


void
AdditionalHandler::parsePolyAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string polygonID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const PositionVector shapeStr = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, polygonID.c_str(), parsedOk);
    // optional attributes
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, polygonID.c_str(), parsedOk, RGBColor::RED);
    const bool geo = attrs.getOpt<bool>(SUMO_ATTR_GEO, polygonID.c_str(), parsedOk, false);
    const bool fill = attrs.getOpt<bool>(SUMO_ATTR_FILL, polygonID.c_str(), parsedOk, false);
    const double lineWidth = attrs.getOpt<double>(SUMO_ATTR_LINEWIDTH, polygonID.c_str(), parsedOk, Shape::DEFAULT_LINEWIDTH);
    const double layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, polygonID.c_str(), parsedOk, Shape::DEFAULT_LAYER);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, polygonID.c_str(), parsedOk, Shape::DEFAULT_TYPE);
    const std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, polygonID.c_str(), parsedOk, Shape::DEFAULT_IMG_FILE);
    const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, polygonID.c_str(), parsedOk, Shape::DEFAULT_ANGLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, polygonID.c_str(), parsedOk, "");
    const bool relativePath = attrs.getOpt<bool>(SUMO_ATTR_RELATIVEPATH, polygonID.c_str(), parsedOk, Shape::DEFAULT_RELATIVEPATH);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_POLY);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, polygonID);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addPositionVectorAttribute(SUMO_ATTR_SHAPE, shapeStr);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_GEO, geo);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FILL, fill);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LINEWIDTH, lineWidth);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LAYER, layer);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TYPE, type);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_IMGFILE, imgFile);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ANGLE, angle);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_RELATIVEPATH, relativePath);
    }
}


void
AdditionalHandler::parsePOIAttributes(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // check that x and y are defined together
    if ((attrs.hasAttribute(SUMO_ATTR_X) && !attrs.hasAttribute(SUMO_ATTR_Y)) ||
            (!attrs.hasAttribute(SUMO_ATTR_X) && attrs.hasAttribute(SUMO_ATTR_Y))) {
        WRITE_ERROR("X and Y must be be defined together in POIs");
        parsedOk = false;
    }
    // check that lane and pos are defined together
    if ((attrs.hasAttribute(SUMO_ATTR_LANE) && !attrs.hasAttribute(SUMO_ATTR_POSITION)) ||
            (!attrs.hasAttribute(SUMO_ATTR_LANE) && attrs.hasAttribute(SUMO_ATTR_POSITION))) {
        WRITE_ERROR("lane and position must be be defined together in POIs");
        parsedOk = false;
    }
    // check that lon and lat are defined together
    if ((attrs.hasAttribute(SUMO_ATTR_LON) && !attrs.hasAttribute(SUMO_ATTR_LAT)) ||
            (!attrs.hasAttribute(SUMO_ATTR_LON) && attrs.hasAttribute(SUMO_ATTR_LAT))) {
        WRITE_ERROR("lon and lat must be be defined together in POIs");
        parsedOk = false;
    }
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const RGBColor color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk);
    // special attributes
    const double x = attrs.getOpt<double>(SUMO_ATTR_X, id.c_str(), parsedOk, 0);
    const double y = attrs.getOpt<double>(SUMO_ATTR_Y, id.c_str(), parsedOk, 0);
    const std::string lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, "", parsedOk, "");
    const double pos = attrs.getOpt<double>(SUMO_ATTR_POSITION, id.c_str(), parsedOk, 0);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), parsedOk, false);
    const double posLat = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, id.c_str(), parsedOk, 0);
    const double lon = attrs.getOpt<double>(SUMO_ATTR_LON, id.c_str(), parsedOk, 0);
    const double lat = attrs.getOpt<double>(SUMO_ATTR_LAT, id.c_str(), parsedOk, 0);
    // optional attributes
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, "", parsedOk, Shape::DEFAULT_TYPE);
    const double layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, id.c_str(), parsedOk, Shape::DEFAULT_LAYER_POI);
    const std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, "", parsedOk, Shape::DEFAULT_IMG_FILE);
    const double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), parsedOk, Shape::DEFAULT_IMG_WIDTH);
    const double height = attrs.getOpt<double>(SUMO_ATTR_HEIGHT, id.c_str(), parsedOk, Shape::DEFAULT_IMG_HEIGHT);
    const double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), parsedOk, Shape::DEFAULT_ANGLE);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), parsedOk, "");
    const bool relativePath = attrs.getOpt<bool>(SUMO_ATTR_RELATIVEPATH, id.c_str(), parsedOk, Shape::DEFAULT_RELATIVEPATH);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_POI);
        // add attributes depending of Lane/Lanes
        if (attrs.hasAttribute(SUMO_ATTR_X) && attrs.hasAttribute(SUMO_ATTR_Y)) {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_X, x);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_Y, y);
        } else if (attrs.hasAttribute(SUMO_ATTR_LANE) && attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_LANE, lane);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION, pos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_POSITION_LAT, posLat);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LON, lon);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LAT, lat);
        }
        // add rest attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TYPE, type);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_LAYER, layer);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_IMGFILE, imgFile);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_WIDTH, width);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_HEIGHT, height);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ANGLE, angle);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_NAME, name);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addBoolAttribute(SUMO_ATTR_RELATIVEPATH, relativePath);
    }
}


void
AdditionalHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // get SumoBaseObject parent
    CommonXMLStructure::SumoBaseObject* SumoBaseObjectParent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    // check parent
    if (SumoBaseObjectParent == nullptr) {
        WRITE_ERROR("Parameters must be defined within an object.");
    }
    // check tag
    if (SumoBaseObjectParent->getTag() == SUMO_TAG_NOTHING) {
        WRITE_ERROR("Parameters cannot be defined in neither the additional file's root nor another parameter.");
    }
    // continue if key was sucesfully loaded
    if (parsedOk) {
        // get tag str
        const std::string parentTagStr = toString(SumoBaseObjectParent->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key cannot be empty.");
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            WRITE_WARNING("Error parsing key from " + parentTagStr + " generic parameter. Key contains invalid characters.");
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + parentTagStr);
            // insert parameter in SumoBaseObjectParent
            SumoBaseObjectParent->addParameter(key, value);
        }
    }
}


void
AdditionalHandler::checkParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok) const {
    // check that parent SUMOBaseObject's tag is the parentTag
    CommonXMLStructure::SumoBaseObject* const parent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    if ((parent != nullptr) &&
            (parentTags.size() > 0) &&
            (std::find(parentTags.begin(), parentTags.end(), parent->getTag()) == parentTags.end())) {
        const std::string id = parent->hasStringAttribute(SUMO_ATTR_ID) ? ", id: '" + parent->getStringAttribute(SUMO_ATTR_ID) + "'" : "";
        WRITE_ERROR("'" + toString(currentTag) + "' must be defined within the definition of a '" + toString(parentTags.front()) + "' (found '" + toString(parent->getTag()) + "'" + id + ").");
        ok = false;
    }
}

/****************************************************************************/
