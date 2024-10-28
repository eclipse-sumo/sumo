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
/// @file    SUMORouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
///
// Parser for routes during their loading
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/XMLSubSys.h>

#include "SUMORouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

SUMORouteHandler::SUMORouteHandler(const std::string& file, const std::string& expectedRoot, const bool hardFail) :
    SUMOSAXHandler(file, expectedRoot),
    myHardFail(hardFail),
    myVehicleParameter(nullptr),
    myLastDepart(-1),
    myActiveRouteColor(nullptr),
    myCurrentCosts(0.),
    myCurrentVType(nullptr),
    myBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myEndDefault(string2time(OptionsCont::getOptions().getString("end"))),
    myFirstDepart(-1),
    myInsertStopEdgesAt(-1),
    myAllowInternalRoutes(false) {
}


SUMORouteHandler::~SUMORouteHandler() {
    delete myVehicleParameter;
    delete myCurrentVType;
}


bool
SUMORouteHandler::checkLastDepart() {
    if (myVehicleParameter->departProcedure == DepartDefinition::GIVEN) {
        if (myVehicleParameter->depart < myLastDepart) {
            WRITE_WARNINGF(TL("Route file should be sorted by departure time, ignoring '%'!"), myVehicleParameter->id);
            return false;
        }
    }
    return true;
}


void
SUMORouteHandler::registerLastDepart() {
    // register only non public transport to parse all public transport lines in advance
    if (myVehicleParameter && myVehicleParameter->line == "" && myVehicleParameter->departProcedure == DepartDefinition::GIVEN) {
        myLastDepart = myVehicleParameter->depart;
        if (myFirstDepart == -1) {
            myFirstDepart = myLastDepart;
        }
    }
    // else: we don't know when this vehicle will depart. keep the previous known depart time
}


void
SUMORouteHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myElementStack.push_back(element);
    switch (element) {
        case SUMO_TAG_VEHICLE:
        case SUMO_TAG_PERSON:
        case SUMO_TAG_CONTAINER:
            // if myVehicleParameter is nullptr this will do nothing
            delete myVehicleParameter;
            // we set to nullptr to have a consistent state if the parsing fails
            myVehicleParameter = nullptr;
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(element, attrs, myHardFail, false, false, myAllowInternalRoutes);
            myParamStack.push_back(myVehicleParameter);
            if (element != SUMO_TAG_VEHICLE) {
                addTransportable(attrs, element == SUMO_TAG_PERSON);
            }
            break;
        case SUMO_TAG_FLOW:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
                myVehicleParameter = nullptr;
            }
            // parse vehicle parameters
            // might be called to parse vehicles from additional file in the
            // context of quickReload. In this case, rerouter flows must be ignored
            if (myElementStack.size() == 1 || myElementStack[myElementStack.size() - 2] != SUMO_TAG_CALIBRATOR) {
                myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_FLOW, attrs, myHardFail, true, myBeginDefault, myEndDefault, myAllowInternalRoutes);
            }
            // check if myVehicleParameter was successfully created
            if (myVehicleParameter) {
                // check tag
                if (myVehicleParameter->routeid.empty()) {
                    // open a route flow (It could be a flow with embedded route)
                    openFlow(attrs);
                } else {
                    // open a route flow
                    openRouteFlow(attrs);
                }
                myParamStack.push_back(myVehicleParameter);
            }
            break;
        case SUMO_TAG_PERSONFLOW:
        case SUMO_TAG_CONTAINERFLOW:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
                myVehicleParameter = nullptr;
            }
            // create a new flow
            myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes((SumoXMLTag)element, attrs, myHardFail, true, myBeginDefault, myEndDefault, myAllowInternalRoutes);
            myParamStack.push_back(myVehicleParameter);
            break;
        case SUMO_TAG_VTYPE:
            // delete if myCurrentVType isn't null
            if (myCurrentVType != nullptr) {
                delete myCurrentVType;
                myCurrentVType = nullptr;
            }
            // create a new vType
            myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs, myHardFail, getFileName());
            myParamStack.push_back(myCurrentVType);
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            openVehicleTypeDistribution(attrs);
            break;
        case SUMO_TAG_ROUTE:
            openRoute(attrs);
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            openRouteDistribution(attrs);
            break;
        case SUMO_TAG_STOP:
            myParamStack.push_back(addStop(attrs));
            break;
        case SUMO_TAG_TRIP: {
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
                myVehicleParameter = nullptr;
            }
            // parse vehicle parameters
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(element, attrs, myHardFail, false, false, myAllowInternalRoutes);
            // check if myVehicleParameter was successfully created
            if (myVehicleParameter) {
                myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
                myActiveRouteID = "!" + myVehicleParameter->id;
                // open trip
                openTrip(attrs);
                myParamStack.push_back(myVehicleParameter);
            }
            break;
        }
        case SUMO_TAG_PERSONTRIP:
            addPersonTrip(attrs);
            break;
        case SUMO_TAG_WALK:
            addWalk(attrs);
            break;
        case SUMO_TAG_INTERVAL: {
            bool ok;
            myBeginDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, ok);
            myEndDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_END, nullptr, ok);
            break;
        }
        case SUMO_TAG_RIDE:
            addRide(attrs);
            break;
        case SUMO_TAG_TRANSPORT:
            addTransport(attrs);
            break;
        case SUMO_TAG_TRANSHIP:
            addTranship(attrs);
            break;
        case SUMO_TAG_PARAM:
            addParam(attrs);
            break;
        default:
            // parse embedded car following model information
            if (myCurrentVType != nullptr) {
                WRITE_WARNINGF(TL("Defining car-following parameters in a nested element is deprecated in vType '%', use attributes instead!"), myCurrentVType->id);
                if (!SUMOVehicleParserHelper::parseCFMParams(myCurrentVType, (SumoXMLTag)element, attrs, true)) {
                    if (myHardFail) {
                        throw ProcessError(TL("Invalid parsing embedded VType"));
                    } else {
                        WRITE_ERROR(TL("Invalid parsing embedded VType"));
                    }
                }
            }
            break;
    }
}


void
SUMORouteHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_STOP:
            myParamStack.pop_back();
            break;
        case SUMO_TAG_ROUTE:
            closeRoute();
            break;
        case SUMO_TAG_VTYPE:
            closeVType();
            delete myCurrentVType;
            myCurrentVType = nullptr;
            myParamStack.pop_back();
            break;
        case SUMO_TAG_PERSON:
            closePerson();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myParamStack.pop_back();
            break;
        case SUMO_TAG_PERSONFLOW:
            closePersonFlow();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myParamStack.pop_back();
            break;
        case SUMO_TAG_CONTAINER:
            closeContainer();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myParamStack.pop_back();
            break;
        case SUMO_TAG_CONTAINERFLOW:
            closeContainerFlow();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myParamStack.pop_back();
            break;
        case SUMO_TAG_VEHICLE:
            if (myVehicleParameter == nullptr) {
                break;
            }
            if (myVehicleParameter->repetitionNumber > 0) {
                myVehicleParameter->repetitionNumber++; // for backwards compatibility
                // it is a flow, thus no break here
                FALLTHROUGH;
            } else {
                closeVehicle();
                delete myVehicleParameter;
                myVehicleParameter = nullptr;
                myParamStack.pop_back();
                break;
            }
        case SUMO_TAG_FLOW:
            if (myVehicleParameter) {
                closeFlow();
                delete myVehicleParameter;
                myParamStack.pop_back();
            }
            myVehicleParameter = nullptr;
            myInsertStopEdgesAt = -1;
            break;
        case SUMO_TAG_TRIP:
            closeTrip();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myParamStack.pop_back();
            myInsertStopEdgesAt = -1;
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            closeVehicleTypeDistribution();
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            closeRouteDistribution();
            break;
        case SUMO_TAG_PERSONTRIP:
        case SUMO_TAG_RIDE:
        case SUMO_TAG_TRANSPORT:
        case SUMO_TAG_TRANSHIP:
        case SUMO_TAG_WALK:
            if (myParamStack.size() == 2) {
                myParamStack.pop_back();
            }
            break;
        case SUMO_TAG_INTERVAL:
            myBeginDefault = string2time(OptionsCont::getOptions().getString("begin"));
            myEndDefault = string2time(OptionsCont::getOptions().getString("end"));
            break;
        default:
            break;
    }
    myElementStack.pop_back();
}


SUMORouteHandler::StopPos
SUMORouteHandler::checkStopPos(double& startPos, double& endPos, const double laneLength, const double minLength, const bool friendlyPos) {
    if (minLength > laneLength) {
        return STOPPOS_INVALID_LANELENGTH;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if ((endPos < minLength) || (endPos > laneLength)) {
        if (!friendlyPos) {
            return STOPPOS_INVALID_ENDPOS;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if ((startPos < 0) || (startPos > (endPos - minLength))) {
        if (!friendlyPos) {
            return STOPPOS_INVALID_STARTPOS;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > (endPos - minLength)) {
            startPos = endPos - minLength;
        }
    }
    return STOPPOS_VALID;
}


bool
SUMORouteHandler::isStopPosValid(const double startPos, const double endPos, const double laneLength, const double minLength, const bool friendlyPos) {
    // declare dummy start and end positions
    double dummyStartPos = startPos;
    double dummyEndPos = endPos;
    // return checkStopPos
    return (checkStopPos(dummyStartPos, dummyEndPos, laneLength, minLength, friendlyPos) == STOPPOS_VALID);
}


SUMOTime
SUMORouteHandler::getFirstDepart() const {
    return myFirstDepart;
}


SUMOTime
SUMORouteHandler::getLastDepart() const {
    return myLastDepart;
}


void
SUMORouteHandler::addParam(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
    // only continue if key isn't empty
    if (ok && (key.size() > 0)) {
        // circumventing empty string test
        const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // add parameter in current created element
        if (!myParamStack.empty()) {
            myParamStack.back()->setParameter(key, val);
        }
    }
}


bool
SUMORouteHandler::parseStop(SUMOVehicleParameter::Stop& stop, const SUMOSAXAttributes& attrs, std::string errorSuffix, MsgHandler* const errorOutput) {
    stop.parametersSet = 0;
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVAL)) {
        stop.parametersSet |= STOP_ARRIVAL_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_DURATION)) {
        stop.parametersSet |= STOP_DURATION_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_UNTIL)) {
        stop.parametersSet |= STOP_UNTIL_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_STARTED)) {
        stop.parametersSet |= STOP_STARTED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_ENDED)) {
        stop.parametersSet |= STOP_ENDED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EXTENSION)) {
        stop.parametersSet |= STOP_EXTENSION_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_ENDPOS)) {
        stop.parametersSet |= STOP_END_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_STARTPOS)) {
        stop.parametersSet |= STOP_START_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_POSITION_LAT)) {
        stop.parametersSet |= STOP_POSLAT_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TRIGGERED)) {
        stop.parametersSet |= STOP_TRIGGER_SET;
    }
    // legacy attribute
    if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_TRIGGERED)) {
        stop.parametersSet |= STOP_TRIGGER_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_PARKING)) {
        stop.parametersSet |= STOP_PARKING_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EXPECTED)) {
        stop.parametersSet |= STOP_EXPECTED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERMITTED)) {
        stop.parametersSet |= STOP_PERMITTED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EXPECTED_CONTAINERS)) {
        stop.parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TRIP_ID)) {
        stop.parametersSet |= STOP_TRIP_ID_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPLIT)) {
        stop.parametersSet |= STOP_SPLIT_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_JOIN)) {
        stop.parametersSet |= STOP_JOIN_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        stop.parametersSet |= STOP_LINE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        stop.parametersSet |= STOP_SPEED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_ONDEMAND)) {
        stop.parametersSet |= STOP_ONDEMAND_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_JUMP)) {
        stop.parametersSet |= STOP_JUMP_SET;
    }
    bool ok = true;
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_TRAIN_STOP, nullptr, ok, stop.busstop);
    stop.chargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, nullptr, ok, "");
    stop.overheadWireSegment = attrs.getOpt<std::string>(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, nullptr, ok, "");
    stop.containerstop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, nullptr, ok, "");
    stop.parkingarea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, nullptr, ok, "");
    if (stop.busstop != "") {
        errorSuffix = " at '" + stop.busstop + "'" + errorSuffix;
    } else if (stop.chargingStation != "") {
        errorSuffix = " at '" + stop.chargingStation + "'" + errorSuffix;
    } else if (stop.overheadWireSegment != "") {
        errorSuffix = " at '" + stop.overheadWireSegment + "'" + errorSuffix;
    } else if (stop.containerstop != "") {
        errorSuffix = " at '" + stop.containerstop + "'" + errorSuffix;
    } else if (stop.parkingarea != "") {
        errorSuffix = " at '" + stop.parkingarea + "'" + errorSuffix;
    } else if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        errorSuffix = " on lane '" + attrs.get<std::string>(SUMO_ATTR_LANE, nullptr, ok) + "'" + errorSuffix;
    } else if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        errorSuffix = " on edge '" + attrs.get<std::string>(SUMO_ATTR_EDGE, nullptr, ok) + "'" + errorSuffix;
    } else {
        errorSuffix = " at undefined location" + errorSuffix;
    }
    // speed for counting as stopped
    stop.speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, 0);
    if (stop.speed < 0) {
        errorOutput->inform(TLF("Speed cannot be negative for stop%.", errorSuffix));
        return false;
    }

    // get the standing duration
    bool expectTrigger = !attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL) && !attrs.hasAttribute(SUMO_ATTR_SPEED);
    std::vector<std::string> triggers = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_TRIGGERED, nullptr, ok);
    // legacy
    if (attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, false)) {
        triggers.push_back(toString(SUMO_TAG_CONTAINER));
    }
    SUMOVehicleParameter::parseStopTriggers(triggers, expectTrigger, stop);
    stop.arrival = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ARRIVAL, nullptr, ok, -1);
    stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, nullptr, ok, -1);
    stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, nullptr, ok, -1);
    if (!expectTrigger && (!ok || (stop.duration < 0 && stop.until < 0 && stop.speed == 0))) {
        errorOutput->inform(TLF("Invalid duration or end time is given for a stop%.", errorSuffix));
        return false;
    }
    if (triggers.size() > 0 && stop.speed > 0) {
        errorOutput->inform(TLF("Triggers and waypoints cannot be combined%.", errorSuffix));
        return false;
    }
    stop.extension = attrs.getOptSUMOTimeReporting(SUMO_ATTR_EXTENSION, nullptr, ok, -1);
    const bool defaultParking = (stop.triggered || stop.containerTriggered || stop.parkingarea != "");
    stop.parking = attrs.getOpt<ParkingType>(SUMO_ATTR_PARKING, nullptr, ok, defaultParking ? ParkingType::OFFROAD : ParkingType::ONROAD);
    if ((stop.parkingarea != "") && (stop.parking == ParkingType::ONROAD)) {
        WRITE_WARNINGF(TL("Stop at parkingArea overrides attribute 'parking' for stop%."), errorSuffix);
        stop.parking = ParkingType::OFFROAD;
    }

    // expected persons
    const std::vector<std::string>& expected = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EXPECTED, nullptr, ok);
    stop.awaitedPersons.insert(expected.begin(), expected.end());
    if (stop.awaitedPersons.size() > 0) {
        stop.triggered = true;
        if ((stop.parametersSet & STOP_PARKING_SET) == 0) {
            stop.parking = ParkingType::OFFROAD;
        }
    }

    // permitted transportables
    const std::vector<std::string>& permitted = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_PERMITTED, nullptr, ok);
    stop.permitted.insert(permitted.begin(), permitted.end());

    // expected containers
    const std::vector<std::string>& expectedContainers = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EXPECTED_CONTAINERS, nullptr, ok);
    stop.awaitedContainers.insert(expectedContainers.begin(), expectedContainers.end());
    if (stop.awaitedContainers.size() > 0) {
        stop.containerTriggered = true;
        if ((stop.parametersSet & STOP_PARKING_SET) == 0) {
            stop.parking = ParkingType::OFFROAD;
        }
    }
    // public transport trip id
    stop.tripId = attrs.getOpt<std::string>(SUMO_ATTR_TRIP_ID, nullptr, ok, "");
    stop.split = attrs.getOpt<std::string>(SUMO_ATTR_SPLIT, nullptr, ok, "");
    stop.join = attrs.getOpt<std::string>(SUMO_ATTR_JOIN, nullptr, ok, "");
    stop.line = attrs.getOpt<std::string>(SUMO_ATTR_LINE, nullptr, ok, "");

    const std::string idx = attrs.getOpt<std::string>(SUMO_ATTR_INDEX, nullptr, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.get<int>(SUMO_ATTR_INDEX, nullptr, ok);
        if (!ok || stop.index < 0) {
            errorOutput->inform(TLF("Invalid 'index' for stop%.", errorSuffix));
            return false;
        }
    }
    stop.started = attrs.getOptSUMOTimeReporting(SUMO_ATTR_STARTED, nullptr, ok, -1);
    stop.ended = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ENDED, nullptr, ok, -1);
    stop.posLat = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, nullptr, ok, INVALID_DOUBLE);
    stop.actType = attrs.getOpt<std::string>(SUMO_ATTR_ACTTYPE, nullptr, ok, "");
    stop.onDemand = attrs.getOpt<bool>(SUMO_ATTR_ONDEMAND, nullptr, ok, false);
    stop.jump = attrs.getOptSUMOTimeReporting(SUMO_ATTR_JUMP, nullptr, ok, -1);
    stop.collision = attrs.getOpt<bool>(SUMO_ATTR_COLLISION, nullptr, ok, false);
    return true;
}


/****************************************************************************/
