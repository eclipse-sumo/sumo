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
    myBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myEndDefault(string2time(OptionsCont::getOptions().getString("end"))) {
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
                obj->getStringAttribute(SUMO_ATTR_ID),
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
RouteHandler::buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, const std::vector<std::string> &edges, 
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
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject 
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            case SUMO_TAG_ROUTE:
                parseRoute(attrs);
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
RouteHandler::parseRoute(const SUMOSAXAttributes& attrs) {
    // first check if this is an embedded route
    if (myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject() && attrs.hasAttribute(SUMO_ATTR_ID)) {
        WRITE_ERROR("either define a route within a vehicle or define it with ID");
    } else {
        // declare Ok Flag
        bool parsedOk = true;
        // special case for ID
        const std::string id = attrs.getOpt(SUMO_ATTR_ID, "", parsedOk, "");
        // needed attributes
        const std::vector<std::string> edges = attrs.getOptStringVector(SUMO_ATTR_EDGES, id.c_str(), parsedOk);
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
    SUMOVehicleParameter* flowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_FLOW, attrs, myHardFail, myBeginDefault, myEndDefault);
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
    //
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
    SUMOVehicleParameter* personFlowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, attrs, myHardFail, myBeginDefault, myEndDefault);
    if (personFlowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(personFlowParameter);
        // delete person flow parameter (because in XMLStructure we have a copy)
        delete personFlowParameter;
    }
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
