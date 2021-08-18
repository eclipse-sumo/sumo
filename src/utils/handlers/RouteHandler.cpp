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
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/options/OptionsCont.h>
#include <utils/shapes/Shape.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/XMLSubSys.h>

#include "RouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

RouteHandler::RouteHandler(const std::string& file) :
    SUMOSAXHandler(file),
    myHardFail(true),
    myFlowBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myFlowEndDefault(string2time(OptionsCont::getOptions().getString("end"))) {
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
        // vTypes
         case SUMO_TAG_VTYPE:
            buildVType(obj,
                obj->getVehicleTypeParameter(),
                obj->getParameters());
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            buildVTypeDistribution(obj,
                obj->getStringAttribute(SUMO_ATTR_ID));
            break;
        // route
        case SUMO_TAG_ROUTE:
            buildRoute(obj,
                obj->getStringAttribute(SUMO_ATTR_ID),
                obj->getStringListAttribute(SUMO_ATTR_EDGES),
                obj->getColorAttribute(SUMO_ATTR_COLOR),
                obj->getIntAttribute(SUMO_ATTR_REPEAT),
                obj->getTimeAttribute(SUMO_ATTR_CYCLETIME),
                obj->getParameters());
            break;
        case SUMO_TAG_ROUTE_DISTRIBUTION:
            buildRouteDistribution(obj,
                obj->getStringAttribute(SUMO_ATTR_ID));
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
RouteHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject 
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // vTypes
            case SUMO_TAG_VTYPE:
                parseVType(attrs);
                break;
            case SUMO_TAG_VTYPE_DISTRIBUTION:
                parseVTypeDistribution(attrs);
                break;
            // routes
            case SUMO_TAG_ROUTE:
                parseRoute(attrs);
                break;
            case SUMO_TAG_ROUTE_DISTRIBUTION:
                parseRouteDistribution(attrs);
                break;
            // vehicles
            case SUMO_TAG_TRIP:
            case SUMO_TAG_VEHICLE:
                parseVehicle(attrs);
                break;
            // flows
            case SUMO_TAG_FLOW:
                parseFlow(attrs);
                break;
            // stop
            case SUMO_TAG_STOP:
                parseStop(attrs);
                break;
            // persons
            case SUMO_TAG_PERSON:
                parsePerson(attrs);
                break;
            case SUMO_TAG_PERSONFLOW:
                parsePersonFlow(attrs);
                break;
            // person plans
            case SUMO_TAG_PERSONTRIP:
                parsePersonTrip(attrs);
                break;
            case SUMO_TAG_RIDE:
                parseRide(attrs);
                break;
            case SUMO_TAG_WALK:
                parseWalk(attrs);
                break;
            // container
            case SUMO_TAG_CONTAINER:
                parseContainer(attrs);
                break;
            case SUMO_TAG_CONTAINERFLOW:
                parseContainerFlow(attrs);
                break;
            // container plans
            case SUMO_TAG_TRANSPORT:
                parseTransport(attrs);
                break;
            case SUMO_TAG_TRANSHIP:
                parseTranship(attrs);
                break;
            // parameters
            case SUMO_TAG_PARAM:
                parseParameters(attrs);
                break;
            // other
            case SUMO_TAG_INTERVAL: {
                parseInterval(attrs);
                break;
            }
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
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
        case SUMO_TAG_ROUTE:
        case SUMO_TAG_TRIP:
        case SUMO_TAG_VEHICLE:
        case SUMO_TAG_FLOW:
        case SUMO_TAG_STOP:
        case SUMO_TAG_PERSON:
        case SUMO_TAG_PERSONFLOW:
        case SUMO_TAG_PERSONTRIP:
        case SUMO_TAG_RIDE:
        case SUMO_TAG_WALK:
        case SUMO_TAG_CONTAINER:
        case SUMO_TAG_CONTAINERFLOW:
        case SUMO_TAG_TRANSPORT:
        case SUMO_TAG_TRANSHIP:
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
RouteHandler::parseVType(const SUMOSAXAttributes& attrs) {
    // parse vehicleType
    SUMOVTypeParameter* vehicleTypeParameter = SUMOVehicleParserHelper::beginVTypeParsing(attrs, myHardFail, getFileName());
    if (vehicleTypeParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VTYPE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleTypeParameter(vehicleTypeParameter);
        // delete vehicleType parameter (because in XMLStructure we have a copy)
        delete vehicleTypeParameter;
    }
}


void 
RouteHandler::parseVTypeDistribution(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string ID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VTYPE_DISTRIBUTION);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, ID);
    }
}


void 
RouteHandler::parseRoute(const SUMOSAXAttributes& attrs) {
    // first check if this is an embedded route
    if (myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject() && attrs.hasAttribute(SUMO_ATTR_ID)) {
        WRITE_ERROR("either define a route within a vehicle or with an ID");
    } else {
        // declare Ok Flag
        bool parsedOk = true;
        // special case for ID
        const std::string id = attrs.getOpt(SUMO_ATTR_ID, "", parsedOk, "");
        // needed attributes
        const std::vector<std::string> edges = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk);
        // optional attributes
        const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor::YELLOW);
        const int repeat = attrs.getOpt<int>(SUMO_ATTR_REPEAT, id.c_str(), parsedOk, 0);
        const SUMOTime cycleTime = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CYCLETIME, id.c_str(), parsedOk, 0);
        if (parsedOk) {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_REPEAT, repeat);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_CYCLETIME, cycleTime);
        }
    }
}


void 
RouteHandler::parseRouteDistribution(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string ID = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE_DISTRIBUTION);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, ID);
    }
}


void 
RouteHandler::parseTrip(const SUMOSAXAttributes& attrs) {
    // first parse vehicle
    SUMOVehicleParameter* vehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_VEHICLE, attrs, myHardFail);
    if (vehicleParameter) {
        // declare Ok Flag
        bool parsedOk = true;
        // needed attributes
        const std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, vehicleParameter->id.c_str(), parsedOk);
        const std::string to = attrs.get<std::string>(SUMO_ATTR_TO, vehicleParameter->id.c_str(), parsedOk);
        // optional attributes
        const std::vector<std::string> via = attrs.getOptStringVector(SUMO_ATTR_VIA, vehicleParameter->id.c_str(), parsedOk);
        if (parsedOk) {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRIP);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(vehicleParameter);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VIA, via);
        }
        // delete vehicle parameter (because in XMLStructure we have a copy)
        delete vehicleParameter;
    }
}


void
RouteHandler::parseVehicle(const SUMOSAXAttributes& attrs) {
    // first parse vehicle
    SUMOVehicleParameter* vehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_VEHICLE, attrs, myHardFail);
    if (vehicleParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VEHICLE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(vehicleParameter);
        // delete vehicle parameter (because in XMLStructure we have a copy)
        delete vehicleParameter;
    }
}


void
RouteHandler::parseFlow(const SUMOSAXAttributes& attrs) {
    // first parse flow
    SUMOVehicleParameter* flowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_FLOW, attrs, myHardFail, myFlowBeginDefault, myFlowEndDefault);
    if (flowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(flowParameter);
        // delete flow parameter (because in XMLStructure we have a copy)
        delete flowParameter;
    }
}


void
RouteHandler::parseStop(const SUMOSAXAttributes& attrs) {
    // declare stop
    SUMOVehicleParameter::Stop stop;
    // parse stop
    if (parseStopParameters(stop, attrs)) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_STOP);
        // add stop attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setStopParameter(stop);
    }
}


void
RouteHandler::parsePerson(const SUMOSAXAttributes& attrs) {
    // first parse vehicle
    SUMOVehicleParameter* personParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, attrs, myHardFail);
    if (personParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VEHICLE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(personParameter);
        // delete person parameter (because in XMLStructure we have a copy)
        delete personParameter;
    }
}


void
RouteHandler::parsePersonFlow(const SUMOSAXAttributes& attrs) {
    // first parse flow
    SUMOVehicleParameter* personFlowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, attrs, myHardFail, myFlowBeginDefault, myFlowEndDefault);
    if (personFlowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PERSONFLOW);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(personFlowParameter);
        // delete person flow parameter (because in XMLStructure we have a copy)
        delete personFlowParameter;
    }
}


void
RouteHandler::parsePersonTrip(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // optional attributes
    const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, "", parsedOk, "");
    const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "", parsedOk, "");
    const std::vector<std::string> via = attrs.getOptStringVector(SUMO_ATTR_VIA, "", parsedOk);
    const std::string busStop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, "", parsedOk, "");
    const std::vector<std::string> vTypes = attrs.getOptStringVector(SUMO_ATTR_VTYPES, "", parsedOk);
    const std::vector<std::string> modes = attrs.getOptStringVector(SUMO_ATTR_MODES, "", parsedOk);
    const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, 0);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, 0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PERSONTRIP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VIA, via);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_BUS_STOP, busStop);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_MODES, modes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
    }
}


void
RouteHandler::parseWalk(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // optional attributes
    const std::string route = attrs.getOpt<std::string>(SUMO_ATTR_ROUTE, "", parsedOk, "");
    const std::vector<std::string> edges = attrs.getOptStringVector(SUMO_ATTR_EDGES, "", parsedOk);
    const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, "", parsedOk, "");
    const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "", parsedOk, "");
    const std::string busStop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, "", parsedOk, "");
    const double duration = attrs.getOpt<double>(SUMO_ATTR_DURATION, "", parsedOk, 0);
    const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, "", parsedOk, 0);
    const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, 0);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, 0);
    const double departPosLat = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS_LAT, "", parsedOk, 0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_WALK);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ROUTE, route);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_BUS_STOP, busStop);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DURATION, duration);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_SPEED, speed);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS_LAT, departPosLat);
    }
}


void
RouteHandler::parseRide(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // optional attributes
    const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, "", parsedOk, "");
    const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "", parsedOk, "");
    const std::string busStop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, "", parsedOk, "");
    const std::vector<std::string> lines = attrs.getOptStringVector(SUMO_ATTR_LINES, "", parsedOk);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, 0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_RIDE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_BUS_STOP, busStop);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
    }
}


void
RouteHandler::parseContainer(const SUMOSAXAttributes& attrs) {
    // first parse container
    SUMOVehicleParameter* containerParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_CONTAINER, attrs, myHardFail);
    if (containerParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONTAINER);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(containerParameter);
        // delete container parameter (because in XMLStructure we have a copy)
        delete containerParameter;
    }
}


void
RouteHandler::parseContainerFlow(const SUMOSAXAttributes& attrs) {
    // first parse flow
    SUMOVehicleParameter* containerFlowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_CONTAINERFLOW, attrs, myHardFail, myFlowBeginDefault, myFlowEndDefault);
    if (containerFlowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONTAINERFLOW);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(containerFlowParameter);
        // delete container flow parameter (because in XMLStructure we have a copy)
        delete containerFlowParameter;
    }
}


void
RouteHandler::parseTransport(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // optional attributes
    const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, "", parsedOk, "");
    const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "", parsedOk, "");
    const std::string containerStop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, "", parsedOk, "");
    const std::vector<std::string> lines = attrs.getOptStringVector(SUMO_ATTR_LINES, "", parsedOk);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, 0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRANSPORT);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_CONTAINER_STOP, containerStop);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
    }
}


void
RouteHandler::parseTranship(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // optional attributes
    const std::vector<std::string> edges = attrs.getOptStringVector(SUMO_ATTR_EDGES, "", parsedOk);
    const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, "", parsedOk, "");
    const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, "", parsedOk, "");
    const std::string containerStop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, "", parsedOk, "");
    const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, "", parsedOk, 0);
    const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, 0);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, 0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRANSHIP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_CONTAINER_STOP, containerStop);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_SPEED, speed);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
    }
}


void
RouteHandler::parseInterval(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // just parse begin and end default
    myFlowBeginDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, parsedOk);
    myFlowEndDefault = attrs.getSUMOTimeReporting(SUMO_ATTR_END, nullptr, parsedOk);
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


bool
RouteHandler::parseStopParameters(SUMOVehicleParameter::Stop &stop, const SUMOSAXAttributes& attrs) const {
    // check stop parameters
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
    // get parameters
    bool ok = true;
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_TRAIN_STOP, nullptr, ok, stop.busstop);
    stop.chargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, nullptr, ok, "");
    stop.overheadWireSegment = attrs.getOpt<std::string>(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, nullptr, ok, "");
    stop.containerstop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, nullptr, ok, "");
    stop.parkingarea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, nullptr, ok, "");
    // declare error suffix
    std::string errorSuffix;
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
    } else {
        errorSuffix = " on lane '" + stop.lane + "'" + errorSuffix;
    }
    // speed for counting as stopped
    stop.speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, 0);
    if (stop.speed < 0) {
        WRITE_ERROR("Speed cannot be negative for stop" + errorSuffix);
        return false;
    }
    // get the standing duration
    bool expectTrigger = !attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL) && !attrs.hasAttribute(SUMO_ATTR_SPEED);
    std::vector<std::string> triggers = attrs.getOptStringVector(SUMO_ATTR_TRIGGERED, nullptr, ok);
    // legacy
    if (attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, false)) {
        triggers.push_back(toString(SUMO_TAG_CONTAINER));
    };
    SUMOVehicleParameter::parseStopTriggers(triggers, expectTrigger, stop);
    stop.arrival = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ARRIVAL, nullptr, ok, -1);
    stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, nullptr, ok, -1);
    stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, nullptr, ok, -1);
    if (!expectTrigger && (!ok || (stop.duration < 0 && stop.until < 0 && stop.speed == 0))) {
        WRITE_ERROR("Invalid duration or end time is given for a stop" + errorSuffix);
        return false;
    }
    stop.extension = attrs.getOptSUMOTimeReporting(SUMO_ATTR_EXTENSION, nullptr, ok, -1);
    stop.parking = attrs.getOpt<bool>(SUMO_ATTR_PARKING, nullptr, ok, stop.triggered || stop.containerTriggered || stop.parkingarea != "");
    if (stop.parkingarea != "" && !stop.parking) {
        WRITE_WARNING("Stop at parkingarea overrides attribute 'parking' for stop" + errorSuffix);
        stop.parking = true;
    }
    if (!ok) {
        WRITE_ERROR("Invalid bool for 'triggered', 'containerTriggered' or 'parking' for stop" + errorSuffix);
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
    // permitted transportables
    const std::vector<std::string>& permitted = attrs.getOptStringVector(SUMO_ATTR_PERMITTED, nullptr, ok);
    stop.permitted.insert(permitted.begin(), permitted.end());
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
    stop.split = attrs.getOpt<std::string>(SUMO_ATTR_SPLIT, nullptr, ok, "");
    stop.join = attrs.getOpt<std::string>(SUMO_ATTR_JOIN, nullptr, ok, "");
    stop.line = attrs.getOpt<std::string>(SUMO_ATTR_LINE, nullptr, ok, "");
    // index
    const std::string idx = attrs.getOpt<std::string>(SUMO_ATTR_INDEX, nullptr, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.get<int>(SUMO_ATTR_INDEX, nullptr, ok);
        if (!ok || stop.index < 0) {
            WRITE_ERROR("Invalid 'index' for stop" + errorSuffix);
            return false;
        }
    }
    stop.started = attrs.getOptSUMOTimeReporting(SUMO_ATTR_STARTED, nullptr, ok, -1);
    stop.ended = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ENDED, nullptr, ok, -1);
    stop.posLat = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, nullptr, ok, INVALID_DOUBLE);
    return true;
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
