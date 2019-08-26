/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMORouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser for routes during their loading
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <map>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLSubSys.h>
#include "SUMORouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
SUMORouteHandler::SUMORouteHandler(const std::string& file, const std::string& expectedRoot, const bool hardFail) :
    SUMOSAXHandler(file, XMLSubSys::isValidating() ? expectedRoot : ""),
    myHardFail(hardFail),
    myVehicleParameter(nullptr),
    myLastDepart(-1),
    myActiveRouteColor(nullptr),
    myCurrentCosts(0.),
    myCurrentVType(nullptr),
    myBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myEndDefault(string2time(OptionsCont::getOptions().getString("end"))),
    myFirstDepart(-1),
    myInsertStopEdgesAt(-1) {
}


SUMORouteHandler::~SUMORouteHandler() {
    delete myCurrentVType;
}


bool
SUMORouteHandler::checkLastDepart() {
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        if (myVehicleParameter->depart < myLastDepart) {
            WRITE_WARNING("Route file should be sorted by departure time, ignoring '" + myVehicleParameter->id + "'!");
            return false;
        }
    }
    return true;
}


void
SUMORouteHandler::registerLastDepart() {
    // register only non public transport to parse all public transport lines in advance
    if (myVehicleParameter->line == "" && myVehicleParameter->departProcedure == DEPART_GIVEN) {
        myLastDepart = myVehicleParameter->depart;
        if (myFirstDepart == -1) {
            myFirstDepart = myLastDepart;
        }
    }
    // else: we don't know when this vehicle will depart. keep the previous known depart time
}


void
SUMORouteHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_VEHICLE:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // create a new vehicle
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, myHardFail);
            break;
        case SUMO_TAG_PERSON:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // create a new person
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, myHardFail, false, false, true);
            addPerson(attrs);
            break;
        case SUMO_TAG_CONTAINER:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // create a new container
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, myHardFail);
            addContainer(attrs);
            break;
        case SUMO_TAG_FLOW:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // parse vehicle parameters
            myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes(attrs, myHardFail, myBeginDefault, myEndDefault);
            // check if myVehicleParameter was sucesfully created
            if (myVehicleParameter) {
                // open a flow (using openTrip function)
                openTrip(attrs);
            }
            break;
        case SUMO_TAG_PERSONFLOW:
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // create a new flow
            myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes(attrs, myHardFail, myBeginDefault, myEndDefault, true);
            break;
        case SUMO_TAG_VTYPE:
            // delete if myCurrentVType isn't null
            if (myCurrentVType != nullptr) {
                delete myCurrentVType;
                myCurrentVType = nullptr;
            }
            // create a new vType
            myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs, myHardFail, getFileName());
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
            addStop(attrs);
            break;
        case SUMO_TAG_TRIP: {
            // delete if myVehicleParameter isn't null
            if (myVehicleParameter) {
                delete myVehicleParameter;
            }
            // parse vehicle parameters
            myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, myHardFail);
            // check if myVehicleParameter was sucesfully created
            if (myVehicleParameter) {
                myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
                myActiveRouteID = "!" + myVehicleParameter->id;
                // open trip
                openTrip(attrs);
            }
            break;
        }
        case SUMO_TAG_PERSONTRIP:
        case SUMO_TAG_WALK:
            if (attrs.hasAttribute(SUMO_ATTR_EDGES) || attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
                addWalk(attrs);
            } else {
                addPersonTrip(attrs);
            }
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
                WRITE_WARNING("Defining car following parameters in a nested element is deprecated in vType '" + myCurrentVType->id + "', use attributes instead!");
                if (!SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, (SumoXMLTag)element, attrs, myHardFail)) {
                    if (myHardFail) {
                        throw ProcessError("Invalid parsing embedded VType");
                    } else {
                        WRITE_ERROR("Invalid parsing embedded VType");
                    }
                }
            }
            break;
    }
}


void
SUMORouteHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_ROUTE:
            closeRoute();
            break;
        case SUMO_TAG_VTYPE:
            closeVType();
            delete myCurrentVType;
            myCurrentVType = nullptr;
            break;
        case SUMO_TAG_PERSON:
            closePerson();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            break;
        case SUMO_TAG_PERSONFLOW:
            closePersonFlow();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            break;
        case SUMO_TAG_CONTAINER:
            closeContainer();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
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
                break;
            }
        case SUMO_TAG_FLOW:
            closeFlow();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myInsertStopEdgesAt = -1;
            break;
        case SUMO_TAG_TRIP:
            closeTrip();
            delete myVehicleParameter;
            myVehicleParameter = nullptr;
            myInsertStopEdgesAt = -1;
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            closeVehicleTypeDistribution();
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            closeRouteDistribution();
            break;
        case SUMO_TAG_INTERVAL:
            myBeginDefault = string2time(OptionsCont::getOptions().getString("begin"));
            myEndDefault = string2time(OptionsCont::getOptions().getString("end"));
            break;
        default:
            break;
    }
}


bool
SUMORouteHandler::checkStopPos(double& startPos, double& endPos, const double laneLength,
                               const double minLength, const bool friendlyPos) {
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
    // circumventing empty string test
    const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
    // add parameter in current created element, or in myLoadedParameterised
    if (myVehicleParameter != nullptr) {
        myVehicleParameter->setParameter(key, val);
    } else if (myCurrentVType != nullptr) {
        myCurrentVType->setParameter(key, val);
    } else {
        myLoadedParameterised.setParameter(key, val);
    }
}


bool
SUMORouteHandler::parseStop(SUMOVehicleParameter::Stop& stop, const SUMOSAXAttributes& attrs, std::string errorSuffix, MsgHandler* const errorOutput) {
    stop.parametersSet = 0;
    if (attrs.hasAttribute(SUMO_ATTR_ENDPOS)) {
        stop.parametersSet |= STOP_END_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_STARTPOS)) {
        stop.parametersSet |= STOP_START_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TRIGGERED)) {
        stop.parametersSet |= STOP_TRIGGER_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_TRIGGERED)) {
        stop.parametersSet |= STOP_CONTAINER_TRIGGER_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_PARKING)) {
        stop.parametersSet |= STOP_PARKING_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EXPECTED)) {
        stop.parametersSet |= STOP_EXPECTED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EXPECTED_CONTAINERS)) {
        stop.parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_TRIP_ID)) {
        stop.parametersSet |= STOP_TRIP_ID_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        stop.parametersSet |= STOP_LINE_SET;
    }
    bool ok = true;
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");
    stop.chargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, nullptr, ok, "");
    stop.containerstop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, nullptr, ok, "");
    stop.parkingarea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, nullptr, ok, "");
    if (stop.busstop != "") {
        errorSuffix = " at '" + stop.busstop + "'" + errorSuffix;
    } else if (stop.chargingStation != "") {
        errorSuffix = " at '" + stop.chargingStation + "'" + errorSuffix;
    } else if (stop.containerstop != "") {
        errorSuffix = " at '" + stop.containerstop + "'" + errorSuffix;
    } else if (stop.parkingarea != "") {
        errorSuffix = " at '" + stop.parkingarea + "'" + errorSuffix;
    } else {
        errorSuffix = " on lane '" + stop.lane + "'" + errorSuffix;
    }
    // get the standing duration
    if (!attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL)) {
        if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_TRIGGERED)) {
            stop.containerTriggered = attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, true);
            stop.triggered = attrs.getOpt<bool>(SUMO_ATTR_TRIGGERED, nullptr, ok, false);
        } else {
            stop.triggered = attrs.getOpt<bool>(SUMO_ATTR_TRIGGERED, nullptr, ok, true);
            stop.containerTriggered = attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, false);
        }
        stop.duration = -1;
        stop.until = -1;
    } else {
        stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, nullptr, ok, -1);
        stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, nullptr, ok, -1);
        if (!ok || (stop.duration < 0 && stop.until < 0)) {
            errorOutput->inform("Invalid duration or end time is given for a stop" + errorSuffix);
            return false;
        }
        stop.triggered = attrs.getOpt<bool>(SUMO_ATTR_TRIGGERED, nullptr, ok, false);
        stop.containerTriggered = attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, false);
    }
    stop.parking = attrs.getOpt<bool>(SUMO_ATTR_PARKING, nullptr, ok, stop.triggered || stop.containerTriggered || stop.parkingarea != "");
    if (stop.parkingarea != "" && !stop.parking) {
        WRITE_WARNING("Stop at parkingarea overrides attribute 'parking' for stop" + errorSuffix);
        stop.parking = true;
    }
    if (!ok) {
        errorOutput->inform("Invalid bool for 'triggered', 'containerTriggered' or 'parking' for stop" + errorSuffix);
        return false;
    }

    // expected persons
    const std::vector<std::string>& expected = attrs.getOptStringVector(SUMO_ATTR_EXPECTED, nullptr, ok);
    stop.awaitedPersons.insert(expected.begin(), expected.end());
    if (stop.awaitedPersons.size() > 0 && (stop.parametersSet & STOP_TRIGGER_SET) == 0) {
        stop.triggered = true;
        if ((stop.parametersSet & STOP_PARKING_SET) == 0) {
            stop.parking = true;
        }
    }

    // expected containers
    const std::vector<std::string>& expectedContainers = attrs.getOptStringVector(SUMO_ATTR_EXPECTED_CONTAINERS, nullptr, ok);
    stop.awaitedContainers.insert(expectedContainers.begin(), expectedContainers.end());
    if (stop.awaitedContainers.size() > 0 && (stop.parametersSet & STOP_CONTAINER_TRIGGER_SET) == 0) {
        stop.containerTriggered = true;
        if ((stop.parametersSet & STOP_PARKING_SET) == 0) {
            stop.parking = true;
        }
    }
    // public transport trip id
    stop.tripId = attrs.getOpt<std::string>(SUMO_ATTR_TRIP_ID, nullptr, ok, "");
    stop.line = attrs.getOpt<std::string>(SUMO_ATTR_LINE, nullptr, ok, "");

    const std::string idx = attrs.getOpt<std::string>(SUMO_ATTR_INDEX, nullptr, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.get<int>(SUMO_ATTR_INDEX, nullptr, ok);
        if (!ok || stop.index < 0) {
            errorOutput->inform("Invalid 'index' for stop" + errorSuffix);
            return false;
        }
    }
    return true;
}

/****************************************************************************/
