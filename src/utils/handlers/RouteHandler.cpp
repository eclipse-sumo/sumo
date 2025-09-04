/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/xml/NamespaceIDs.h>

#include "RouteHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

RouteHandler::RouteHandler(const std::string& filename, const bool hardFail) :
    CommonHandler(filename),
    myHardFail(hardFail),
    myFlowBeginDefault(string2time(OptionsCont::getOptions().getString("begin"))),
    myFlowEndDefault(string2time(OptionsCont::getOptions().getString("end"))) {
}


RouteHandler::~RouteHandler() {}


bool
RouteHandler::beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    // open SUMOBaseOBject
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // vTypes
            case SUMO_TAG_VTYPE:
                // continue depeding if we're parsing a vType or a reference
                if ((myCommonXMLStructure.getSumoBaseObjectRoot()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) &&
                        attrs.hasAttribute(SUMO_ATTR_REFID)) {
                    parseVTypeRef(attrs);
                } else {
                    parseVType(attrs);
                }
                break;
            case SUMO_TAG_VTYPE_DISTRIBUTION:
                parseVTypeDistribution(attrs);
                break;
            // routes
            case SUMO_TAG_ROUTE: {
                // continue depeding if we're parsing a basic route, an embedded route, or a distribution ref
                const auto parentTag = myCommonXMLStructure.getSumoBaseObjectRoot()->getTag();
                // this is temporal, until #16476
                if (parentTag != SUMO_TAG_ERROR) {
                    if ((parentTag == SUMO_TAG_VEHICLE) || (parentTag == SUMO_TAG_FLOW)) {
                        parseRouteEmbedded(attrs);
                    } else if ((parentTag == SUMO_TAG_ROUTE_DISTRIBUTION) && attrs.hasAttribute(SUMO_ATTR_REFID)) {
                        parseRouteRef(attrs);
                    } else {
                        parseRoute(attrs);
                    }
                }
                break;
            }
            case SUMO_TAG_ROUTE_DISTRIBUTION:
                parseRouteDistribution(attrs);
                break;
            // vehicles
            case SUMO_TAG_TRIP:
                parseTrip(attrs);
                break;
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
                // get vehicle type Base object
                const auto vTypeObject = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
                // parse embedded car following model information
                if (vTypeObject && (vTypeObject->getTag() == SUMO_TAG_VTYPE)) {
                    // nested CFM attributes
                    return parseNestedCFM(tag, attrs, vTypeObject);
                } else {
                    // tag cannot be parsed in routeHandler
                    myCommonXMLStructure.abortSUMOBaseOBject();
                    return false;
                }
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
    return true;
}


void
RouteHandler::endParseAttributes() {
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // check tag
    if (obj) {
        // close SUMOBaseOBject
        myCommonXMLStructure.closeSUMOBaseOBject();
        switch (obj->getTag()) {
            case SUMO_TAG_ROUTE:
                // special case, because embedded and distribution routes are created within other elements
                if (obj->getParentSumoBaseObject() == nullptr) {
                    // parse object and all their childrens
                    parseSumoBaseObject(obj);
                    // delete object (and all of their childrens)
                    delete obj;
                }
                break;
            case SUMO_TAG_ROUTE_DISTRIBUTION:
                // overwrite probabilities in children
                for (int i = 0; i < (int)obj->getStringListAttribute(SUMO_ATTR_ROUTES).size(); i++) {
                    const auto& routeID = obj->getStringListAttribute(SUMO_ATTR_ROUTES).at(i);
                    if (i < (int)obj->getDoubleListAttribute(SUMO_ATTR_PROBS).size()) {
                        const double probability = obj->getDoubleListAttribute(SUMO_ATTR_PROBS).at(i);
                        // find child
                        for (auto objChild : obj->getSumoBaseObjectChildren()) {
                            if (objChild->hasStringAttribute(SUMO_ATTR_ID) && (objChild->getStringAttribute(SUMO_ATTR_ID) == routeID)) {
                                // routes
                                objChild->addDoubleAttribute(SUMO_ATTR_PROB, probability);
                            } else if (objChild->hasStringAttribute(SUMO_ATTR_REFID) && (objChild->getStringAttribute(SUMO_ATTR_REFID) == routeID)) {
                                // routeReferences
                                objChild->addDoubleAttribute(SUMO_ATTR_PROB, probability);
                            }
                        }
                    }
                }
                // parse object and all their childrens
                parseSumoBaseObject(obj);
                // delete object (and all of their childrens)
                delete obj;
                break;
            case SUMO_TAG_VTYPE_DISTRIBUTION:
                // overwrite probabilities in children
                for (int i = 0; i < (int)obj->getStringListAttribute(SUMO_ATTR_VTYPES).size(); i++) {
                    const auto& vTypeID = obj->getStringListAttribute(SUMO_ATTR_VTYPES).at(i);
                    if (i < (int)obj->getDoubleListAttribute(SUMO_ATTR_PROBS).size()) {
                        const double probability = obj->getDoubleListAttribute(SUMO_ATTR_PROBS).at(i);
                        // find child
                        for (auto objChild : obj->getSumoBaseObjectChildren()) {
                            if (objChild->hasStringAttribute(SUMO_ATTR_ID) && (objChild->getStringAttribute(SUMO_ATTR_ID) == vTypeID)) {
                                // vTypes
                                objChild->addDoubleAttribute(SUMO_ATTR_PROB, probability);
                            } else if (objChild->hasStringAttribute(SUMO_ATTR_REFID) && (objChild->getStringAttribute(SUMO_ATTR_REFID) == vTypeID)) {
                                // vTypeReferences
                                objChild->addDoubleAttribute(SUMO_ATTR_PROB, probability);
                            }
                        }
                    }
                }
                // parse object and all their childrens
                parseSumoBaseObject(obj);
                // delete object (and all of their childrens)
                delete obj;
                break;
            case SUMO_TAG_VTYPE:
                // special case, because embedded and distribution routes are created within other elements
                if (obj->getParentSumoBaseObject() == nullptr) {
                    // parse object and all their childrens
                    parseSumoBaseObject(obj);
                    // delete object (and all of their childrens)
                    delete obj;
                }
                break;
            case SUMO_TAG_TRIP:
            case SUMO_TAG_VEHICLE:
            case SUMO_TAG_FLOW:
            case SUMO_TAG_PERSON:
            case SUMO_TAG_PERSONFLOW:
            case SUMO_TAG_CONTAINER:
            case SUMO_TAG_CONTAINERFLOW:
                // parse object and all their childrens
                parseSumoBaseObject(obj);
                // delete object (and all of their childrens)
                delete obj;
                break;
            default:
                break;
        }
    }
}


void
RouteHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // check if loading was aborted
    if (!myAbortLoading) {
        // switch tag
        switch (obj->getTag()) {
            // vTypes
            case SUMO_TAG_VTYPE:
                // check if parse vType or Ref
                if (obj->hasStringAttribute(SUMO_ATTR_REFID)) {
                    if (buildVTypeRef(obj,
                                      obj->getStringAttribute(SUMO_ATTR_REFID),
                                      obj->getDoubleAttribute(SUMO_ATTR_PROB))) {
                        obj->markAsCreated();
                    }
                } else {
                    if (buildVType(obj,
                                   obj->getVehicleTypeParameter())) {
                        obj->markAsCreated();
                    }
                }
                break;
            case SUMO_TAG_VTYPE_DISTRIBUTION:
                if (buildVTypeDistribution(obj,
                                           obj->getStringAttribute(SUMO_ATTR_ID),
                                           obj->getIntAttribute(SUMO_ATTR_DETERMINISTIC))) {
                    obj->markAsCreated();
                }
                break;
            // route
            case SUMO_TAG_ROUTE:
                // embedded routes are created in build<Vehicle/Flow>EmbeddedRoute
                if (obj->hasStringAttribute(SUMO_ATTR_REFID)) {
                    if (buildRouteRef(obj,
                                      obj->getStringAttribute(SUMO_ATTR_REFID),
                                      obj->getDoubleAttribute(SUMO_ATTR_PROB))) {
                        obj->markAsCreated();
                    }
                } else if (obj->hasStringAttribute(SUMO_ATTR_ID)) {
                    if (buildRoute(obj,
                                   obj->getStringAttribute(SUMO_ATTR_ID),
                                   obj->getVClass(),
                                   obj->getStringListAttribute(SUMO_ATTR_EDGES),
                                   obj->getColorAttribute(SUMO_ATTR_COLOR),
                                   obj->getIntAttribute(SUMO_ATTR_REPEAT),
                                   obj->getTimeAttribute(SUMO_ATTR_CYCLETIME),
                                   obj->getDoubleAttribute(SUMO_ATTR_PROB),
                                   obj->getParameters())) {
                        obj->markAsCreated();
                    }
                }
                break;
            case SUMO_TAG_ROUTE_DISTRIBUTION:
                if (buildRouteDistribution(obj,
                                           obj->getStringAttribute(SUMO_ATTR_ID))) {
                    obj->markAsCreated();
                }
                break;
            // vehicles
            case SUMO_TAG_TRIP:
                if (checkVehicleParents(obj)) {
                    if (isOverFromToEdges(obj)) {
                        // build trip with from-to edges
                        if (buildTrip(obj,
                                      obj->getVehicleParameter(),
                                      obj->hasStringAttribute(SUMO_ATTR_FROM) ? obj->getStringAttribute(SUMO_ATTR_FROM) : "",
                                      obj->hasStringAttribute(SUMO_ATTR_TO) ? obj->getStringAttribute(SUMO_ATTR_TO) : "")) {
                            obj->markAsCreated();
                        }
                    } else if (isOverFromToJunctions(obj)) {
                        // build trip with from-to junctions
                        if (buildTripJunctions(obj,
                                               obj->getVehicleParameter(),
                                               obj->getStringAttribute(SUMO_ATTR_FROM_JUNCTION),
                                               obj->getStringAttribute(SUMO_ATTR_TO_JUNCTION))) {
                            obj->markAsCreated();
                        }
                    } else if (isOverFromToTAZs(obj)) {
                        // build trip with from-to TAZs
                        if (buildTripTAZs(obj,
                                          obj->getVehicleParameter(),
                                          obj->getStringAttribute(SUMO_ATTR_FROM_TAZ),
                                          obj->getStringAttribute(SUMO_ATTR_TO_TAZ))) {
                            obj->markAsCreated();
                        }
                    }
                }
                break;
            case SUMO_TAG_VEHICLE:
                if (checkVehicleParents(obj)) {
                    if (obj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                        // build vehicle over route
                        if (buildVehicleOverRoute(obj,
                                                  obj->getVehicleParameter())) {
                            obj->markAsCreated();
                        }
                    } else {
                        const auto embeddedRoute = getEmbeddedRoute(obj);
                        if (embeddedRoute) {
                            // build vehicle with embedded route
                            if (buildVehicleEmbeddedRoute(obj,
                                                          obj->getVehicleParameter(),
                                                          embeddedRoute->getStringListAttribute(SUMO_ATTR_EDGES),
                                                          embeddedRoute->getColorAttribute(SUMO_ATTR_COLOR),
                                                          embeddedRoute->getIntAttribute(SUMO_ATTR_REPEAT),
                                                          embeddedRoute->getTimeAttribute(SUMO_ATTR_CYCLETIME),
                                                          embeddedRoute->getParameters())) {
                                obj->markAsCreated();
                            }
                        }
                    }
                }
                break;
            // flows
            case SUMO_TAG_FLOW:
                if (checkVehicleParents(obj)) {
                    if (obj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                        // build flow over route
                        if (buildFlowOverRoute(obj,
                                               obj->getVehicleParameter())) {
                            obj->markAsCreated();
                        }
                    } else if (isOverFromToEdges(obj)) {
                        // build flow with from-to edges
                        if (buildFlow(obj,
                                      obj->getVehicleParameter(),
                                      obj->getStringAttribute(SUMO_ATTR_FROM),
                                      obj->getStringAttribute(SUMO_ATTR_TO))) {
                            obj->markAsCreated();
                        }
                    } else if (isOverFromToJunctions(obj)) {
                        // build flow with from-to junctions
                        if (buildFlowJunctions(obj,
                                               obj->getVehicleParameter(),
                                               obj->getStringAttribute(SUMO_ATTR_FROM_JUNCTION),
                                               obj->getStringAttribute(SUMO_ATTR_TO_JUNCTION))) {
                            obj->markAsCreated();
                        }
                    } else if (isOverFromToTAZs(obj)) {
                        // build flow with from-to TAZs
                        if (buildFlowTAZs(obj,
                                          obj->getVehicleParameter(),
                                          obj->getStringAttribute(SUMO_ATTR_FROM_TAZ),
                                          obj->getStringAttribute(SUMO_ATTR_TO_TAZ))) {
                            obj->markAsCreated();
                        }

                    } else {
                        const auto embeddedRoute = getEmbeddedRoute(obj);
                        if (embeddedRoute) {
                            // build flow with embedded route
                            if (buildFlowEmbeddedRoute(obj,
                                                       obj->getVehicleParameter(),
                                                       embeddedRoute->getStringListAttribute(SUMO_ATTR_EDGES),
                                                       embeddedRoute->getColorAttribute(SUMO_ATTR_COLOR),
                                                       embeddedRoute->getIntAttribute(SUMO_ATTR_REPEAT),
                                                       embeddedRoute->getTimeAttribute(SUMO_ATTR_CYCLETIME),
                                                       embeddedRoute->getParameters())) {
                                obj->markAsCreated();
                            }
                        }
                    }
                }
                break;
            // persons
            case SUMO_TAG_PERSON:
                if (buildPerson(obj,
                                obj->getVehicleParameter())) {
                    obj->markAsCreated();
                }
                break;
            case SUMO_TAG_PERSONFLOW:
                if (buildPersonFlow(obj,
                                    obj->getVehicleParameter())) {
                    obj->markAsCreated();
                }
                break;
            // person plans
            case SUMO_TAG_PERSONTRIP:
                if (checkPersonPlanParents(obj)) {
                    if (buildPersonTrip(obj,
                                        obj->getPlanParameters(),
                                        obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                                        obj->getStringListAttribute(SUMO_ATTR_VTYPES),
                                        obj->getStringListAttribute(SUMO_ATTR_MODES),
                                        obj->getStringListAttribute(SUMO_ATTR_LINES),
                                        obj->getDoubleAttribute(SUMO_ATTR_WALKFACTOR),
                                        obj->getStringAttribute(SUMO_ATTR_GROUP))) {
                        obj->markAsCreated();
                    }
                }
                break;
            case SUMO_TAG_RIDE:
                if (checkPersonPlanParents(obj)) {
                    if (buildRide(obj,
                                  obj->getPlanParameters(),
                                  obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                                  obj->getStringListAttribute(SUMO_ATTR_LINES),
                                  obj->getStringAttribute(SUMO_ATTR_GROUP))) {
                        obj->markAsCreated();
                    }
                }
                break;
            case SUMO_TAG_WALK:
                if (checkPersonPlanParents(obj)) {
                    if (buildWalk(obj,
                                  obj->getPlanParameters(),
                                  obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                                  obj->getDoubleAttribute(SUMO_ATTR_SPEED),
                                  obj->getTimeAttribute(SUMO_ATTR_DURATION))) {
                        obj->markAsCreated();
                    }
                }
                break;
            // container
            case SUMO_TAG_CONTAINER:
                if (buildContainer(obj,
                                   obj->getVehicleParameter())) {
                    obj->markAsCreated();
                }
                break;
            case SUMO_TAG_CONTAINERFLOW:
                if (buildContainerFlow(obj,
                                       obj->getVehicleParameter())) {
                    obj->markAsCreated();
                }
                break;
            // container plans
            case SUMO_TAG_TRANSPORT:
                if (checkContainerPlanParents(obj)) {
                    if (buildTransport(obj,
                                       obj->getPlanParameters(),
                                       obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                                       obj->getStringListAttribute(SUMO_ATTR_LINES),
                                       obj->getStringAttribute(SUMO_ATTR_GROUP))) {
                        obj->markAsCreated();
                    }
                }
                break;
            case SUMO_TAG_TRANSHIP:
                if (checkContainerPlanParents(obj)) {
                    if (buildTranship(obj,
                                      obj->getPlanParameters(),
                                      obj->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                                      obj->getDoubleAttribute(SUMO_ATTR_DEPARTPOS),
                                      obj->getDoubleAttribute(SUMO_ATTR_SPEED),
                                      obj->getTimeAttribute(SUMO_ATTR_DURATION))) {
                        obj->markAsCreated();
                    }
                }
                break;
            // stops
            case SUMO_TAG_STOP:
                if (checkStopParents(obj)) {
                    if (buildStop(obj,
                                  obj->getPlanParameters(),
                                  obj->getStopParameter())) {
                        obj->markAsCreated();
                    }
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
}


void
RouteHandler::parseVType(const SUMOSAXAttributes& attrs) {
    // parse vehicleType
    SUMOVTypeParameter* vehicleTypeParameter = SUMOVehicleParserHelper::beginVTypeParsing(attrs, myHardFail, myFilename);
    if (vehicleTypeParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VTYPE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleTypeParameter(vehicleTypeParameter);
        // delete vehicleType parameter (because in XMLStructure we have a copy)
        delete vehicleTypeParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseVTypeRef(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // special case for ID
    const std::string refId = attrs.get<std::string>(SUMO_ATTR_REFID, "", parsedOk);
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, refId.c_str(), parsedOk, 1.0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VTYPE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_REFID, refId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseVTypeDistribution(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const int deterministic = attrs.getOpt<int>(SUMO_ATTR_DETERMINISTIC, id.c_str(), parsedOk, -1);
    const std::vector<std::string> vTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, id.c_str(), parsedOk);
    const std::vector<double> probabilities = attrs.getOpt<std::vector<double> >(SUMO_ATTR_PROBS, id.c_str(), parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VTYPE_DISTRIBUTION);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_DETERMINISTIC, deterministic);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleListAttribute(SUMO_ATTR_PROBS, probabilities);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseRoute(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const std::vector<std::string> edges = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), parsedOk);
    // optional attributes
    SUMOVehicleClass vClass = SUMOVehicleParserHelper::parseVehicleClass(attrs, id);
    const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), parsedOk, RGBColor::INVISIBLE);
    const int repeat = attrs.getOpt<int>(SUMO_ATTR_REPEAT, id.c_str(), parsedOk, 0);
    const SUMOTime cycleTime = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CYCLETIME, id.c_str(), parsedOk, 0);
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, id.c_str(), parsedOk, 1.0);
    // check attributes
    if (!checkNegative(SUMO_TAG_ROUTE, id, SUMO_ATTR_CYCLETIME, cycleTime, true)) {
        parsedOk = false;
    }
    if (!checkNegative(SUMO_TAG_ROUTE, id, SUMO_ATTR_REPEAT, repeat, true)) {
        parsedOk = false;
    }
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVClass(vClass);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_REPEAT, repeat);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_CYCLETIME, cycleTime);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseRouteRef(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // special case for ID
    const std::string refId = attrs.get<std::string>(SUMO_ATTR_REFID, "", parsedOk);
    const double probability = attrs.getOpt<double>(SUMO_ATTR_PROB, refId.c_str(), parsedOk, 1.0);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_REFID, refId);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_PROB, probability);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseRouteEmbedded(const SUMOSAXAttributes& attrs) {
    // first check if this is an embedded route
    if (attrs.hasAttribute(SUMO_ATTR_ID)) {
        writeError(TL("an embedded route cannot have their own ID"));
    } else {
        // declare Ok Flag
        bool parsedOk = true;
        // special case for ID
        const std::string vehicleID = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject()->getVehicleParameter().id;
        // needed attributes
        const std::vector<std::string> edges = attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, vehicleID.c_str(), parsedOk);
        // optional attributes
        SUMOVehicleClass vClass = SUMOVehicleParserHelper::parseVehicleClass(attrs, vehicleID);
        const RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, vehicleID.c_str(), parsedOk, RGBColor::INVISIBLE);
        const int repeat = attrs.getOpt<int>(SUMO_ATTR_REPEAT, vehicleID.c_str(), parsedOk, 0);
        const SUMOTime cycleTime = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CYCLETIME, vehicleID.c_str(), parsedOk, 0);
        // check attributes
        if (!checkNegative(SUMO_TAG_ROUTE, vehicleID, SUMO_ATTR_CYCLETIME, cycleTime, true)) {
            parsedOk = false;
        }
        if (!checkNegative(SUMO_TAG_ROUTE, vehicleID, SUMO_ATTR_REPEAT, repeat, true)) {
            parsedOk = false;
        }
        if (parsedOk) {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->setVClass(vClass);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_EDGES, edges);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addColorAttribute(SUMO_ATTR_COLOR, color);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addIntAttribute(SUMO_ATTR_REPEAT, repeat);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_CYCLETIME, cycleTime);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        }
    }
}


void
RouteHandler::parseRouteDistribution(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // optional attributes
    const std::vector<std::string> routes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_ROUTES, id.c_str(), parsedOk);
    const std::vector<double> probabilities = attrs.getOpt<std::vector<double> >(SUMO_ATTR_PROBS, id.c_str(), parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ROUTE_DISTRIBUTION);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_ROUTES, routes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleListAttribute(SUMO_ATTR_PROBS, probabilities);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseTrip(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // parse vehicle
    SUMOVehicleParameter* tripParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_TRIP, attrs, myHardFail);
    if (tripParameter) {
        // check from/to edge/junction
        if ((attrs.hasAttribute(SUMO_ATTR_FROM) + attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) + attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)) > 1) {
            writeError(TL("Attributes 'from', 'fromJunction' and 'fromTaz' cannot be defined together"));
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        } else if ((attrs.hasAttribute(SUMO_ATTR_TO) + attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION) + attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) > 1) {
            writeError(TL("Attributes 'to', 'toJunction' and 'toTaz' cannot be defined together"));
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        } else if (attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) && attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION)) {
            // from-to attributes
            const std::string fromJunction = attrs.get<std::string>(SUMO_ATTR_FROM_JUNCTION, tripParameter->id.c_str(), parsedOk);
            const std::string toJunction = attrs.get<std::string>(SUMO_ATTR_TO_JUNCTION, tripParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRIP);
                // set vehicle parameters
                myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(tripParameter);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM_JUNCTION, fromJunction);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO_JUNCTION, toJunction);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else if (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ) && attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) {
            // from-to attributes
            const std::string fromJunction = attrs.get<std::string>(SUMO_ATTR_FROM_TAZ, tripParameter->id.c_str(), parsedOk);
            const std::string toJunction = attrs.get<std::string>(SUMO_ATTR_TO_TAZ, tripParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRIP);
                // set vehicle parameters
                myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(tripParameter);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM_TAZ, fromJunction);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO_TAZ, toJunction);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else {
            // from-to attributes
            const std::string from = attrs.getOpt<std::string>(SUMO_ATTR_FROM, tripParameter->id.c_str(), parsedOk, "");
            const std::string to = attrs.getOpt<std::string>(SUMO_ATTR_TO, tripParameter->id.c_str(), parsedOk, "");
            // optional attributes
            const std::vector<std::string> via = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VIA, tripParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRIP);
                // set vehicle parameters
                myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(tripParameter);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VIA, via);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        }
        // delete trip parameter (because in XMLStructure we have a copy)
        delete tripParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseVehicle(const SUMOSAXAttributes& attrs) {
    // first parse vehicle
    SUMOVehicleParameter* vehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_VEHICLE, attrs, myHardFail);
    if (vehicleParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_VEHICLE);
        // set vehicle parameters
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(vehicleParameter);
        // delete vehicle parameter (because in XMLStructure we have a copy)
        delete vehicleParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseFlow(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // first parse flow
    SUMOVehicleParameter* flowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_FLOW, attrs, myHardFail, true, myFlowBeginDefault, myFlowEndDefault);
    if (flowParameter) {
        // set vehicle parameters
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(flowParameter);
        // check from/to edge/junction
        if ((attrs.hasAttribute(SUMO_ATTR_FROM) + attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) + attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)) > 1) {
            writeError(TL("Attributes 'from', 'fromJunction' and 'fromTaz' cannot be defined together"));
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        } else if ((attrs.hasAttribute(SUMO_ATTR_TO) + attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION) + attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) > 1) {
            writeError(TL("Attributes 'to', 'toJunction' and 'toTaz' cannot be defined together"));
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        } else if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
            // from-to attributes
            const std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, flowParameter->id.c_str(), parsedOk);
            const std::string to = attrs.get<std::string>(SUMO_ATTR_TO, flowParameter->id.c_str(), parsedOk);
            // optional attributes
            const std::vector<std::string> via = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VIA, flowParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VIA, via);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else if (attrs.hasAttribute(SUMO_ATTR_FROM_JUNCTION) && attrs.hasAttribute(SUMO_ATTR_TO_JUNCTION)) {
            // from-to attributes
            const std::string fromJunction = attrs.get<std::string>(SUMO_ATTR_FROM_JUNCTION, flowParameter->id.c_str(), parsedOk);
            const std::string toJunction = attrs.get<std::string>(SUMO_ATTR_TO_JUNCTION, flowParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM_JUNCTION, fromJunction);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO_JUNCTION, toJunction);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else if (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ) && attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) {
            // from-to attributes
            const std::string fromJunction = attrs.get<std::string>(SUMO_ATTR_FROM_TAZ, flowParameter->id.c_str(), parsedOk);
            const std::string toJunction = attrs.get<std::string>(SUMO_ATTR_TO_TAZ, flowParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM_TAZ, fromJunction);
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO_TAZ, toJunction);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
            // from-to attributes
            const std::string route = attrs.get<std::string>(SUMO_ATTR_ROUTE, flowParameter->id.c_str(), parsedOk);
            if (parsedOk) {
                // set tag
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
                // add other attributes
                myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ROUTE, route);
            } else {
                myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
            }
        } else {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_FLOW);
        }
        // delete flow parameter (because in XMLStructure we have a copy)
        delete flowParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseStop(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // declare stop
    SUMOVehicleParameter::Stop stop;
    // plan parameters
    const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
    // get parents
    std::vector<SumoXMLTag> stopParents;
    stopParents.insert(stopParents.end(), NamespaceIDs::routes.begin(), NamespaceIDs::routes.end());
    stopParents.insert(stopParents.end(), NamespaceIDs::vehicles.begin(), NamespaceIDs::vehicles.end());
    stopParents.insert(stopParents.end(), NamespaceIDs::persons.begin(), NamespaceIDs::persons.end());
    stopParents.insert(stopParents.end(), NamespaceIDs::containers.begin(), NamespaceIDs::containers.end());
    //  check parents
    checkParsedParent(SUMO_TAG_STOP, stopParents, parsedOk);
    // parse stop
    if (parsedOk && parseStopParameters(stop, attrs)) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_STOP);
        // add stop attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
        myCommonXMLStructure.getCurrentSumoBaseObject()->setStopParameter(stop);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parsePerson(const SUMOSAXAttributes& attrs) {
    // first parse vehicle
    SUMOVehicleParameter* personParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, attrs, myHardFail);
    if (personParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PERSON);
        // set vehicle parameter
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(personParameter);
        // delete person parameter (because in XMLStructure we have a copy)
        delete personParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parsePersonFlow(const SUMOSAXAttributes& attrs) {
    // first parse flow
    SUMOVehicleParameter* personFlowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, attrs, myHardFail, true, myFlowBeginDefault, myFlowEndDefault);
    if (personFlowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PERSONFLOW);
        // set vehicle parameter
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(personFlowParameter);
        // delete person flow parameter (because in XMLStructure we have a copy)
        delete personFlowParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parsePersonTrip(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // plan parameters
    const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
    // optional attributes
    const std::vector<std::string> via = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VIA, "", parsedOk);
    const std::vector<std::string> vTypes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_VTYPES, "", parsedOk);
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, "", parsedOk);
    std::vector<std::string> modes = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_MODES, "", parsedOk);
    const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, -1);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, -1);
    const double walkFactor = attrs.getOpt<double>(SUMO_ATTR_WALKFACTOR, "", parsedOk, 0);
    const std::string group = attrs.getOpt<std::string>(SUMO_ATTR_GROUP, "", parsedOk, "");
    // check modes
    SVCPermissions dummyModeSet;
    std::string dummyError;
    if (!SUMOVehicleParameter::parsePersonModes(toString(modes), toString(SUMO_TAG_PERSONTRIP), "", dummyModeSet, dummyError)) {
        WRITE_WARNING(dummyError);
        modes.clear();
    }
    // check parents
    checkParsedParent(SUMO_TAG_PERSONTRIP, NamespaceIDs::persons, parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_PERSONTRIP);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_VTYPES, vTypes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_MODES, modes);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_WALKFACTOR, walkFactor);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_GROUP, group);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseWalk(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_SPEED) && attrs.hasAttribute(SUMO_ATTR_DURATION)) {
        writeError(TL("Speed and duration attributes cannot be defined together in walks"));
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    } else {
        // declare Ok Flag
        bool parsedOk = true;
        // plan parameters
        const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
        // optional attributes
        const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, -1);
        const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, -1);
        const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, "", parsedOk, 1.39);
        const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, "", parsedOk, 0);
        // check parents
        checkParsedParent(SUMO_TAG_WALK, NamespaceIDs::persons, parsedOk);
        if (parsedOk) {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_WALK);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_SPEED, speed);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_DURATION, duration);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        }
    }
}


void
RouteHandler::parseRide(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // plan parameters
    const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
    // optional attributes
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, "", parsedOk);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, -1);
    const std::string group = attrs.getOpt<std::string>(SUMO_ATTR_GROUP, "", parsedOk, "");
    // check parents
    checkParsedParent(SUMO_TAG_RIDE, NamespaceIDs::persons, parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_RIDE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_GROUP, group);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseContainer(const SUMOSAXAttributes& attrs) {
    // first parse container
    SUMOVehicleParameter* containerParameter = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_CONTAINER, attrs, myHardFail);
    if (containerParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONTAINER);
        // set vehicle parameter
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(containerParameter);
        // delete container parameter (because in XMLStructure we have a copy)
        delete containerParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseContainerFlow(const SUMOSAXAttributes& attrs) {
    // first parse flow
    SUMOVehicleParameter* containerFlowParameter = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_CONTAINERFLOW, attrs, myHardFail, true, myFlowBeginDefault, myFlowEndDefault);
    if (containerFlowParameter) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_CONTAINERFLOW);
        // set vehicle parameter
        myCommonXMLStructure.getCurrentSumoBaseObject()->setVehicleParameter(containerFlowParameter);
        // delete container flow parameter (because in XMLStructure we have a copy)
        delete containerFlowParameter;
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseTransport(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // plan parameters
    const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
    // optional attributes
    const std::vector<std::string> lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, "", parsedOk);
    const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, -1);
    const std::string group = attrs.getOpt<std::string>(SUMO_ATTR_GROUP, "", parsedOk, "");
    // check parents
    checkParsedParent(SUMO_TAG_TRANSPORT, NamespaceIDs::containers, parsedOk);
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRANSPORT);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringListAttribute(SUMO_ATTR_LINES, lines);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_GROUP, group);
    } else {
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    }
}


void
RouteHandler::parseTranship(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_SPEED) && attrs.hasAttribute(SUMO_ATTR_DURATION)) {
        writeError(TL("Speed and duration attributes cannot be defined together in tranships"));
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
    } else {
        // declare Ok Flag
        bool parsedOk = true;
        // plan parameters
        const auto planParameters = CommonXMLStructure::PlanParameters(myCommonXMLStructure.getCurrentSumoBaseObject(), attrs, parsedOk);
        // optional attributes
        const double arrivalPos = attrs.getOpt<double>(SUMO_ATTR_ARRIVALPOS, "", parsedOk, -1);
        const double departPos = attrs.getOpt<double>(SUMO_ATTR_DEPARTPOS, "", parsedOk, -1);
        const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, "", parsedOk, 1.39);
        const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, "", parsedOk, 0);
        // check parents
        checkParsedParent(SUMO_TAG_TRANSHIP, NamespaceIDs::containers, parsedOk);
        if (parsedOk) {
            // set tag
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TRANSHIP);
            // add all attributes
            myCommonXMLStructure.getCurrentSumoBaseObject()->setPlanParameters(planParameters);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, arrivalPos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, departPos);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_SPEED, speed);
            myCommonXMLStructure.getCurrentSumoBaseObject()->addTimeAttribute(SUMO_ATTR_DURATION, duration);
        } else {
            myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_ERROR);
        }
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


bool
RouteHandler::parseNestedCFM(const SumoXMLTag tag, const SUMOSAXAttributes& attrs, CommonXMLStructure::SumoBaseObject* vTypeObject) {
    // write warning info
    WRITE_WARNINGF(TL("Defining car-following parameters in a nested element is deprecated in vType '%', use attributes instead!"), vTypeObject->getStringAttribute(SUMO_ATTR_ID));
    // get vType to modify it
    auto vType = vTypeObject->getVehicleTypeParameter();
    // parse nested CFM attributes
    if (SUMOVehicleParserHelper::parseCFMParams(&vType, tag, attrs, true)) {
        vTypeObject->setVehicleTypeParameter(&vType);
        return true;
    } else if (myHardFail) {
        throw ProcessError(TL("Invalid parsing embedded VType"));
    } else {
        return writeError(TL("Invalid parsing embedded VType"));
    }
    return false;
}


bool
RouteHandler::parseStopParameters(SUMOVehicleParameter::Stop& stop, const SUMOSAXAttributes& attrs) {
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
    if (attrs.hasAttribute(SUMO_ATTR_ONDEMAND)) {
        stop.parametersSet |= STOP_ONDEMAND_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_JUMP)) {
        stop.parametersSet |= STOP_JUMP_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_JUMP_UNTIL)) {
        stop.parametersSet |= STOP_JUMP_UNTIL_SET;
    }
    // get parameters
    bool ok = true;
    // edge/lane
    stop.edge = attrs.getOpt<std::string>(SUMO_ATTR_EDGE, nullptr, ok, "");
    stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, nullptr, ok, stop.busstop);
    // check errors
    if (!stop.edge.empty() && !stop.lane.empty()) {
        return writeError(TL("A stop must be defined either with an edge or with an lane, not both"));
    }
    // stopping places
    stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, nullptr, ok, "");
    if (stop.busstop.empty()) {
        stop.busstop = attrs.getOpt<std::string>(SUMO_ATTR_TRAIN_STOP, nullptr, ok, stop.busstop);
    }
    stop.chargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, nullptr, ok, "");
    stop.overheadWireSegment = attrs.getOpt<std::string>(SUMO_ATTR_OVERHEAD_WIRE_SEGMENT, nullptr, ok, "");
    stop.containerstop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, nullptr, ok, "");
    stop.parkingarea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, nullptr, ok, "");
    //check stopping places
    const int numStoppingPlaces = !stop.busstop.empty() + !stop.chargingStation.empty() + !stop.overheadWireSegment.empty() +
                                  !stop.containerstop.empty() + !stop.parkingarea.empty();
    if (numStoppingPlaces > 1) {
        return writeError(TL("A stop must be defined only in a StoppingPlace"));
    } else if ((numStoppingPlaces == 0) && stop.edge.empty() && stop.lane.empty()) {
        return writeError(TL("A stop must be defined in an edge, a lane, or in a StoppingPlace"));
    }
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
    } else if (stop.edge != "") {
        errorSuffix = " at '" + stop.edge + "'" + errorSuffix;
    } else {
        errorSuffix = " on lane '" + stop.lane + "'" + errorSuffix;
    }
    // speed for counting as stopped
    stop.speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, 0);
    if (stop.speed < 0) {
        return writeError("Speed cannot be negative for stop" + errorSuffix);
        return false;
    }
    // get the standing duration
    bool expectTrigger = !attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL) && !attrs.hasAttribute(SUMO_ATTR_SPEED);
    std::vector<std::string> triggers = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_TRIGGERED, nullptr, ok);
    // legacy
    if (attrs.getOpt<bool>(SUMO_ATTR_CONTAINER_TRIGGERED, nullptr, ok, false)) {
        triggers.push_back(toString(SUMO_TAG_CONTAINER));
    };
    SUMOVehicleParameter::parseStopTriggers(triggers, expectTrigger, stop);
    stop.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, nullptr, ok, 0);
    stop.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, nullptr, ok, 0);
    stop.friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, ok, false);
    stop.arrival = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ARRIVAL, nullptr, ok, -1);
    stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, nullptr, ok, -1);
    stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, nullptr, ok, -1);
    if (!expectTrigger && (!ok || (stop.duration < 0 && stop.until < 0 && stop.speed == 0))) {
        return writeError("Invalid duration or end time is given for a stop" + errorSuffix);
        return false;
    }
    stop.extension = attrs.getOptSUMOTimeReporting(SUMO_ATTR_EXTENSION, nullptr, ok, -1);
    const bool defaultParking = (stop.triggered || stop.containerTriggered || stop.parkingarea != "");
    stop.parking = attrs.getOpt<ParkingType>(SUMO_ATTR_PARKING, nullptr, ok, defaultParking ? ParkingType::OFFROAD : ParkingType::ONROAD);
    if ((stop.parkingarea != "") && (stop.parking == ParkingType::ONROAD)) {
        WRITE_WARNING("Stop at parkingarea overrides attribute 'parking' for stop" + errorSuffix);
        stop.parking = ParkingType::OFFROAD;
    }
    if (!ok) {
        return writeError("Invalid bool for 'triggered', 'containerTriggered' or 'parking' for stop" + errorSuffix);
        return false;
    }
    // expected persons
    const std::vector<std::string>& expected = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EXPECTED, nullptr, ok);
    stop.awaitedPersons.insert(expected.begin(), expected.end());
    if (stop.awaitedPersons.size() > 0 && (stop.parametersSet & STOP_TRIGGER_SET) == 0) {
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
    if (stop.awaitedContainers.size() > 0 && (stop.parametersSet & STOP_CONTAINER_TRIGGER_SET) == 0) {
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
    // index
    const std::string idx = attrs.getOpt<std::string>(SUMO_ATTR_INDEX, nullptr, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.get<int>(SUMO_ATTR_INDEX, nullptr, ok);
        if (!ok || stop.index < 0) {
            return writeError("Invalid 'index' for stop" + errorSuffix);
            return false;
        }
    }
    stop.started = attrs.getOptSUMOTimeReporting(SUMO_ATTR_STARTED, nullptr, ok, -1);
    stop.ended = attrs.getOptSUMOTimeReporting(SUMO_ATTR_ENDED, nullptr, ok, -1);
    stop.posLat = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, nullptr, ok, INVALID_DOUBLE);
    stop.actType = attrs.getOpt<std::string>(SUMO_ATTR_ACTTYPE, nullptr, ok, "");
    stop.onDemand = attrs.getOpt<bool>(SUMO_ATTR_ONDEMAND, nullptr, ok, false);
    stop.jump = attrs.getOptSUMOTimeReporting(SUMO_ATTR_JUMP, nullptr, ok, -1);
    stop.jumpUntil = attrs.getOptSUMOTimeReporting(SUMO_ATTR_JUMP_UNTIL, nullptr, ok, -1);
    return true;
}


bool
RouteHandler::isOverFromToEdges(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    return sumoBaseObject->hasStringAttribute(SUMO_ATTR_FROM) && sumoBaseObject->hasStringAttribute(SUMO_ATTR_TO);
}


bool
RouteHandler::isOverFromToJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    return sumoBaseObject->hasStringAttribute(SUMO_ATTR_FROM_JUNCTION) && sumoBaseObject->hasStringAttribute(SUMO_ATTR_TO_JUNCTION);
}


bool
RouteHandler::isOverFromToTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    return sumoBaseObject->hasStringAttribute(SUMO_ATTR_FROM_TAZ) && sumoBaseObject->hasStringAttribute(SUMO_ATTR_TO_TAZ);
}

/****************************************************************************/
