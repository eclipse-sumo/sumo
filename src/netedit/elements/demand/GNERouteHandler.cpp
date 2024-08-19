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
/// @file    GNERouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Builds demand objects for netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <utils/common/StringTokenizer.h>

#include "GNEContainer.h"
#include "GNEPerson.h"
#include "GNEPersonTrip.h"
#include "GNERide.h"
#include "GNERoute.h"
#include "GNERouteDistribution.h"
#include "GNERouteHandler.h"
#include "GNEStop.h"
#include "GNETranship.h"
#include "GNETransport.h"
#include "GNEVehicle.h"
#include "GNEVType.h"
#include "GNEVTypeDistribution.h"
#include "GNEWalk.h"
#include "GNEStopPlan.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteHandler::GNERouteHandler(const std::string& file, GNENet* net, const bool allowUndoRedo, const bool overwrite) :
    RouteHandler(file, false),
    myNet(net),
    myPlanObject(new CommonXMLStructure::SumoBaseObject(nullptr)),
    myAllowUndoRedo(allowUndoRedo),
    myOverwrite(overwrite) {
}


GNERouteHandler::~GNERouteHandler() {
    delete myPlanObject;
}


void
GNERouteHandler::buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter) {
    // check if loaded type is a default type
    if (DEFAULT_VTYPES.count(vTypeParameter.id) > 0) {
        // overwrite default vehicle type
        GNEVType::overwriteVType(myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vTypeParameter.id, false), vTypeParameter, myNet->getViewNet()->getUndoList());
    } else if (!checkDuplicatedDemandElement(SUMO_TAG_VTYPE, vTypeParameter.id)) {
        writeError(TLF("There is another % with the same ID='%'.", toString(SUMO_TAG_VTYPE), vTypeParameter.id));
    } else {
        // create vType/pType using myCurrentVType
        GNEDemandElement* vType = new GNEVType(myNet, vTypeParameter);
        // check if add this vType to a distribution
        GNEDemandElement* vTypeDistribution = nullptr;
        if (sumoBaseObject->getParentSumoBaseObject() && sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
            vTypeDistribution = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
        }
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(vType, TL("add ") + vType->getTagStr() + " '" + vTypeParameter.id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
            if (vTypeDistribution) {
                vTypeDistribution->addDistributionKey(vType, vType->getAttributeDouble(SUMO_ATTR_PROB), myNet->getViewNet()->getUndoList());
            }
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(vType);
            if (vTypeDistribution) {
                vTypeDistribution->addDistributionKey(vType, vType->getAttributeDouble(SUMO_ATTR_PROB));
            }
            vType->incRef("buildVType");
        }
    }
}


void
GNERouteHandler::buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const int deterministic,
                                        const std::vector<std::string>& vTypeIDs, const std::vector<double>& probabilities) {
    // declare vector with vType and their probabilities
    std::vector<const GNEDemandElement*> vTypes;
    // first check conditions
    if (!checkDuplicatedDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, id)) {
        writeError(TLF("There is another % with the same ID='%'.", toString(SUMO_TAG_VTYPE)));
    } else if (getDistributionElements(sumoBaseObject, SUMO_TAG_VTYPE, vTypeIDs, probabilities, vTypes)) {
        // create distributions
        GNEVTypeDistribution* vTypeDistribution = new GNEVTypeDistribution(myNet, id, deterministic);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(vTypeDistribution, TL("add ") + vTypeDistribution->getTagStr() + " '" + id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vTypeDistribution, true), true);
            // add all distributions
            for (int i = 0; i < (int)vTypes.size(); i++) {
                vTypeDistribution->addDistributionKey(vTypes.at(i), probabilities.at(i), myNet->getViewNet()->getUndoList());
            }
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(vTypeDistribution);
            // add all distributions directly
            for (int i = 0; i < (int)vTypes.size(); i++) {
                vTypeDistribution->addDistributionKey(vTypes.at(i), probabilities.at(i));
            }
            vTypeDistribution->incRef("buildVTypeDistribution");
        }
    }
}


void
GNERouteHandler::buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, SUMOVehicleClass vClass,
                            const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                            const double probability, const Parameterised::Map& routeParameters) {
    // parse edges
    const auto edges = parseEdges(SUMO_TAG_ROUTE, edgeIDs);
    // check conditions
    if (!checkDuplicatedDemandElement(SUMO_TAG_ROUTE, id)) {
        writeError(TLF("There is another % with the same ID='%'.", toString(SUMO_TAG_ROUTE), id));
    } else if (edges.size() > 0) {
        // create GNERoute
        GNEDemandElement* route = new GNERoute(myNet, id, vClass, edges, color, repeat, cycleTime, routeParameters);
        // check if add this route to a distribution
        GNEDemandElement* routeDistribution = nullptr;
        if (sumoBaseObject && sumoBaseObject->getParentSumoBaseObject() && sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
            routeDistribution = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE_DISTRIBUTION, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
        }
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(route, TL("add ") + route->getTagStr() + " '" + id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            if (routeDistribution) {
                routeDistribution->addDistributionKey(route, probability, myNet->getViewNet()->getUndoList());
            }
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(route);
            for (const auto& edge : edges) {
                edge->addChildElement(route);
            }
            if (routeDistribution) {
                routeDistribution->addDistributionKey(route, probability);
            }
            route->incRef("buildRoute");
        }
    }
}


void
GNERouteHandler::buildEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::vector<std::string>& edgeIDs,
                                    const RGBColor& color, const int repeat, const SUMOTime cycleTime, const Parameterised::Map& routeParameters) {
    // first create vehicle/flow
    const SUMOVehicleParameter& vehicleParameters = sumoBaseObject->getParentSumoBaseObject()->getVehicleParameter();
    const SumoXMLTag vehicleTag = (sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_VEHICLE) ? GNE_TAG_VEHICLE_WITHROUTE :
                                  (sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_FLOW) ? GNE_TAG_FLOW_WITHROUTE :
                                  sumoBaseObject->getParentSumoBaseObject()->getTag();
    // parse route edges
    const auto edges = parseEdges(SUMO_TAG_ROUTE, edgeIDs);
    // check if ID is duplicated
    if ((edges.size() > 0) && !isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)edges.front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(vehicleTag, myNet, type, vehicleParameters);
            // create embedded route
            GNEDemandElement* route = new GNERoute(myNet, vehicle, edges, color, repeat, cycleTime, routeParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(route, TL("add ") + route->getTagStr() + " in '" + vehicle->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                myNet->getAttributeCarriers()->insertDemandElement(route);
                type->addChildElement(vehicle);
                vehicle->addChildElement(route);
                for (const auto& edge : edges) {
                    edge->addChildElement(route);
                }
                vehicle->incRef("buildEmbeddedRoute");
                route->incRef("buildEmbeddedRoute");
            }
        }
    }
}


void
GNERouteHandler::buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id,
                                        const std::vector<std::string>& routeIDs, const std::vector<double>& probabilities) {
    // declare vector with route and their probabilities
    std::vector<const GNEDemandElement*> routes;
    // first check conditions
    if (!checkDuplicatedDemandElement(SUMO_TAG_ROUTE_DISTRIBUTION, id)) {
        writeError(TLF("There is another % with the same ID='%'.", toString(SUMO_TAG_ROUTE)));
    } else if (getDistributionElements(sumoBaseObject, SUMO_TAG_ROUTE, routeIDs, probabilities, routes)) {
        // create distributions
        GNERouteDistribution* routeDistribution = new GNERouteDistribution(myNet, id);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(routeDistribution, TL("add ") + routeDistribution->getTagStr() + " '" + id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(routeDistribution, true), true);
            // add all distributions
            for (int i = 0; i < (int)routes.size(); i++) {
                routeDistribution->addDistributionKey(routes.at(i), probabilities.at(i), myNet->getViewNet()->getUndoList());
            }
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(routeDistribution);
            // add all distributions directly
            for (int i = 0; i < (int)routes.size(); i++) {
                routeDistribution->addDistributionKey(routes.at(i), probabilities.at(i));
            }
            routeDistribution->incRef("buildRouteDistribution");
        }
    }
}


void
GNERouteHandler::buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (route == nullptr) {
            writeError(TLF("Invalid route '%' used in % '%'.", vehicleParameters.routeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(SUMO_TAG_VEHICLE, myNet, type, route, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(vehicle, TL("add ") + vehicle->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                // set vehicle as child of type and Route
                type->addChildElement(vehicle);
                route->addChildElement(vehicle);
                vehicle->incRef("buildVehicleOverRoute");
            }
        }
    }
}


void
GNERouteHandler::buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (route == nullptr) {
            writeError(TLF("Invalid route '%' used in % '%'.", vehicleParameters.routeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create flow or trips using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_ROUTE, myNet, type, route, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set flow as child of type and Route
                type->addChildElement(flow);
                route->addChildElement(flow);
                flow->incRef("buildFlowOverRoute");
            }
        }
    }
}


void
GNERouteHandler::buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) {
    // parse edges
    const auto fromEdge = parseEdge(SUMO_TAG_TRIP, fromEdgeID);
    const auto toEdge = parseEdge(SUMO_TAG_TRIP, toEdgeID);
    // set via attribute
    if (sumoBaseObject && sumoBaseObject->hasStringListAttribute(SUMO_ATTR_VIA)) {
        vehicleParameters.via = sumoBaseObject->getStringListAttribute(SUMO_ATTR_VIA);
    }
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromEdge && toEdge && !isVehicleIdDuplicated(vehicleParameters.id) && isViaAttributeValid(vehicleParameters.via)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create trip or flow using tripParameters
            GNEDemandElement* trip = new GNEVehicle(SUMO_TAG_TRIP, myNet, type, fromEdge, toEdge, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(trip, TL("add ") + trip->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(trip);
                // set vehicle as child of type
                type->addChildElement(trip);
                trip->incRef("buildTrip");
                // add reference in all edges
                fromEdge->addChildElement(trip);
                toEdge->addChildElement(trip);
            }
        }
    }
}


void
GNERouteHandler::buildTripJunctions(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) {
    // parse junctions
    const auto fromJunction = parseJunction(SUMO_TAG_TRIP, fromJunctionID);
    const auto toJunction = parseJunction(SUMO_TAG_TRIP, toJunctionID);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromJunction && toJunction && !isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create trip using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_TRIP_JUNCTIONS, myNet, type, fromJunction, toJunction, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of type
                type->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all junctions
                fromJunction->addChildElement(flow);
                toJunction->addChildElement(flow);
            }
        }
    }
}


void
GNERouteHandler::buildTripTAZs(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
                               const std::string& fromTAZID, const std::string& toTAZID) {
    // parse TAZs
    const auto fromTAZ = parseTAZ(SUMO_TAG_TRIP, fromTAZID);
    const auto toTAZ = parseTAZ(SUMO_TAG_TRIP, toTAZID);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromTAZ && toTAZ && !isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create trip using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_TRIP_TAZS, myNet, type, fromTAZ, toTAZ, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of type
                type->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all TAZs
                fromTAZ->addChildElement(flow);
                toTAZ->addChildElement(flow);
            }
        }
    }
}


void
GNERouteHandler::buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) {
    // parse edges
    const auto fromEdge = parseEdge(SUMO_TAG_FLOW, fromEdgeID);
    const auto toEdge = parseEdge(SUMO_TAG_FLOW, toEdgeID);
    // set via attribute
    if (sumoBaseObject && sumoBaseObject->hasStringListAttribute(SUMO_ATTR_VIA)) {
        vehicleParameters.via = sumoBaseObject->getStringListAttribute(SUMO_ATTR_VIA);
    }
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromEdge && toEdge && !isVehicleIdDuplicated(vehicleParameters.id) && isViaAttributeValid(vehicleParameters.via)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create trip or flow using tripParameters
            GNEDemandElement* flow = new GNEVehicle(SUMO_TAG_FLOW, myNet, type, fromEdge, toEdge, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of type
                type->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all edges
                fromEdge->addChildElement(flow);
                toEdge->addChildElement(flow);
            }
        }
    }
}


void
GNERouteHandler::buildFlowJunctions(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) {
    // parse junctions
    const auto fromJunction = parseJunction(SUMO_TAG_TRIP, fromJunctionID);
    const auto toJunction = parseJunction(SUMO_TAG_TRIP, toJunctionID);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromJunction && toJunction && !isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create flow using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_JUNCTIONS, myNet, type, fromJunction, toJunction, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of type
                type->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all junctions
                fromJunction->addChildElement(flow);
                toJunction->addChildElement(flow);
            }
        }
    }
}


void
GNERouteHandler::buildFlowTAZs(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters,
                               const std::string& fromTAZID, const std::string& toTAZID) {
    // parse TAZs
    const auto fromTAZ = parseTAZ(SUMO_TAG_TRIP, fromTAZID);
    const auto toTAZ = parseTAZ(SUMO_TAG_TRIP, toTAZID);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (fromTAZ && toTAZ && !isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(vehicleParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", vehicleParameters.vtypeid, toString(vehicleParameters.tag), vehicleParameters.id));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than number of lanes", toString(SUMO_ATTR_DEPARTLANE), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departLane)));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (type->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TLF("Invalid % used in % '%'. % is greater than type %", toString(SUMO_ATTR_DEPARTSPEED), toString(vehicleParameters.tag), vehicleParameters.id, toString(vehicleParameters.departSpeed), toString(SUMO_ATTR_MAXSPEED)));
        } else {
            // create flow using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_TAZS, myNet, type, fromTAZ, toTAZ, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow, TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of type
                type->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all TAZs
                fromTAZ->addChildElement(flow);
                toTAZ->addChildElement(flow);
            }
        }
    }
}


void
GNERouteHandler::buildPerson(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& personParameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(personParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(personParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", personParameters.vtypeid, toString(personParameters.tag), personParameters.id));
        } else {
            // create person using personParameters
            GNEDemandElement* person = new GNEPerson(SUMO_TAG_PERSON, myNet, type, personParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(person, TL("add ") + person->getTagStr() + " '" + personParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(person, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(person);
                // set person as child of type
                type->addChildElement(person);
                person->incRef("buildPerson");
            }
        }
    }
}


void
GNERouteHandler::buildPersonFlow(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& personFlowParameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(personFlowParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(personFlowParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", personFlowParameters.vtypeid, toString(personFlowParameters.tag), personFlowParameters.id));
        } else {
            // create personFlow using personFlowParameters
            GNEDemandElement* personFlow = new GNEPerson(SUMO_TAG_PERSONFLOW, myNet, type, personFlowParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(personFlow, TL("add ") + personFlow->getTagStr() + " '" + personFlowParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personFlow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personFlow);
                // set personFlow as child of type
                type->addChildElement(personFlow);
                personFlow->incRef("buildPersonFlow");
            }
        }
    }
}


void
GNERouteHandler::buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                                 const std::string& fromEdgeID, const std::string& fromTAZID, const std::string& fromJunctionID, const std::string& fromBusStopID, const std::string& fromTrainStopID,
                                 const std::string& toEdgeID, const std::string& toTAZID, const std::string& toJunctionID, const std::string& toBusStopID, const std::string& toTrainStopID,
                                 double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes, const std::vector<std::string>& lines) {
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* fromTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, fromTAZID, false);
    GNEAdditional* toTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, toTAZID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* fromBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, fromBusStopID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    GNEAdditional* fromTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, fromTrainStopID, false);
    GNEAdditional* toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toTrainStopID, false);
    // chek if we're parsing the train stop as a busStop
    if ((toBusStop == nullptr) && (toTrainStop == nullptr)) {
        toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toTrainStopID, false);
        toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toBusStopID, false);
    }
    // check if overwritte previous elements
    if (getPreviousPlanEdge(sumoBaseObject)) {
        fromEdge = getPreviousPlanEdge(sumoBaseObject);
    }
    if (getPreviousPlanTAZ(sumoBaseObject)) {
        fromTAZ = getPreviousPlanTAZ(sumoBaseObject);
    }
    if (getPreviousPlanJunction(sumoBaseObject)) {
        fromJunction = getPreviousPlanJunction(sumoBaseObject);
    }
    if (getPreviousPlanBusStop(sumoBaseObject)) {
        fromBusStop = getPreviousPlanBusStop(sumoBaseObject);
    }
    if (getPreviousPlanTrainStop(sumoBaseObject)) {
        toTrainStop = getPreviousPlanTrainStop(sumoBaseObject);
    }
    // check conditions
    if (personParent == nullptr) {
        WRITE_WARNING(TL("invalid person parent"));
    } else if (GNEPersonTrip::getPersonTripTagIcon(fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction,
               fromBusStop, toBusStop, fromTrainStop, toTrainStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for personTrip"));
    } else {
        // build person trip
        GNEDemandElement* personTrip = GNEPersonTrip::buildPersonTrip(myNet, personParent,
                                       fromEdge, fromTAZ, fromJunction, fromBusStop, fromTrainStop,
                                       toEdge, toTAZ, toJunction, toBusStop, toTrainStop,
                                       arrivalPos, types, modes, lines);
        // continue depending of undo.redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(personTrip, TLF("add % in '%'", personTrip->getTagStr(), personParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(personTrip);
            // set child references
            personParent->addChildElement(personTrip);
            if (fromEdge) {
                fromEdge->addChildElement(personTrip);
            }
            if (fromTAZ) {
                fromTAZ->addChildElement(personTrip);
            }
            if (fromJunction) {
                fromJunction->addChildElement(personTrip);
            }
            if (fromBusStop) {
                fromBusStop->addChildElement(personTrip);
            }
            if (fromTrainStop) {
                fromTrainStop->addChildElement(personTrip);
            }
            if (toEdge) {
                toEdge->addChildElement(personTrip);
            }
            if (toTAZ) {
                toTAZ->addChildElement(personTrip);
            }
            if (toJunction) {
                toJunction->addChildElement(personTrip);
            }
            if (toBusStop) {
                toBusStop->addChildElement(personTrip);
            }
            if (toTrainStop) {
                toTrainStop->addChildElement(personTrip);
            }
            personTrip->incRef("buildPersonTrip");
        }
    }
}


void
GNERouteHandler::buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                           const std::string& fromEdgeID, const std::string& fromTAZID, const std::string& fromJunctionID, const std::string& fromBusStopID, const std::string& fromTrainStopID,
                           const std::string& toEdgeID, const std::string& toTAZID, const std::string& toJunctionID, const std::string& toBusStopID, const std::string& toTrainStopID,
                           const std::vector<std::string>& edgeIDs, const std::string& routeID, double arrivalPos) {
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* fromTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, fromTAZID, false);
    GNEAdditional* toTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, toTAZID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* fromBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, fromBusStopID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    GNEAdditional* fromTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, fromTrainStopID, false);
    GNEAdditional* toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toTrainStopID, false);
    GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, routeID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // chek if we're parsing the train stop as a busStop
    if ((toBusStop == nullptr) && (toTrainStop == nullptr)) {
        toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toTrainStopID, false);
        toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toBusStopID, false);
    }
    // avoid consecutive duplicated edges
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // check if overwritte previous elements
    if (getPreviousPlanEdge(sumoBaseObject)) {
        fromEdge = getPreviousPlanEdge(sumoBaseObject);
    }
    if (getPreviousPlanTAZ(sumoBaseObject)) {
        fromTAZ = getPreviousPlanTAZ(sumoBaseObject);
    }
    if (getPreviousPlanJunction(sumoBaseObject)) {
        fromJunction = getPreviousPlanJunction(sumoBaseObject);
    }
    if (getPreviousPlanBusStop(sumoBaseObject)) {
        fromBusStop = getPreviousPlanBusStop(sumoBaseObject);
    }
    if (getPreviousPlanTrainStop(sumoBaseObject)) {
        toTrainStop = getPreviousPlanTrainStop(sumoBaseObject);
    }
    // check conditions
    if (personParent == nullptr) {
        WRITE_WARNING(TL("invalid person parent"));
    } else if (GNEWalk::getWalkTagIcon(edges, route, fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction,
                                       fromBusStop, toBusStop, fromTrainStop, toTrainStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for personTrip"));
    } else {
        // build person trip
        GNEDemandElement* walk = GNEWalk::buildWalk(myNet, personParent,
                                 fromEdge, fromTAZ, fromJunction, fromBusStop, fromTrainStop,
                                 toEdge, toTAZ, toJunction, toBusStop, toTrainStop,
                                 edges, route, arrivalPos);
        // continue depending of undo.redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(walk, TLF("add % in '%'", walk->getTagStr(), personParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(walk);
            // set child references
            personParent->addChildElement(walk);
            if (fromEdge) {
                fromEdge->addChildElement(walk);
            }
            if (fromTAZ) {
                fromTAZ->addChildElement(walk);
            }
            if (fromJunction) {
                fromJunction->addChildElement(walk);
            }
            if (fromBusStop) {
                fromBusStop->addChildElement(walk);
            }
            if (fromTrainStop) {
                fromTrainStop->addChildElement(walk);
            }
            if (toEdge) {
                toEdge->addChildElement(walk);
            }
            if (toTAZ) {
                toTAZ->addChildElement(walk);
            }
            if (toJunction) {
                toJunction->addChildElement(walk);
            }
            if (toBusStop) {
                toBusStop->addChildElement(walk);
            }
            if (toTrainStop) {
                toTrainStop->addChildElement(walk);
            }
            for (const auto& edge : edges) {
                edge->addChildElement(walk);
            }
            if (route) {
                route->addChildElement(walk);
            }
            walk->incRef("buildWalk");
        }
    }
}


void
GNERouteHandler::buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                           const std::string& fromEdgeID, const std::string& fromBusStopID, const std::string& fromTrainStopID,
                           const std::string& toEdgeID, const std::string& toBusStopID, const std::string& toTrainStopID,
                           double arrivalPos, const std::vector<std::string>& lines) {
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* fromBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, fromBusStopID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    GNEAdditional* fromTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, fromTrainStopID, false);
    GNEAdditional* toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toTrainStopID, false);
    // chek if we're parsing the train stop as a busStop
    if ((toBusStop == nullptr) && (toTrainStop == nullptr)) {
        toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toTrainStopID, false);
        toTrainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, toBusStopID, false);
    }
    // check if overwritte previous elements
    if (getPreviousPlanEdge(sumoBaseObject)) {
        fromEdge = getPreviousPlanEdge(sumoBaseObject);
    }
    if (getPreviousPlanBusStop(sumoBaseObject)) {
        fromBusStop = getPreviousPlanBusStop(sumoBaseObject);
    }
    if (getPreviousPlanTrainStop(sumoBaseObject)) {
        toTrainStop = getPreviousPlanTrainStop(sumoBaseObject);
    }
    // check conditions
    if (personParent == nullptr) {
        WRITE_WARNING(TL("invalid person parent"));
    } else if (GNERide::getRideTagIcon(fromEdge, toEdge, fromBusStop, toBusStop, fromTrainStop, toTrainStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for ride"));
    } else {
        // build ride
        GNEDemandElement* ride = GNERide::buildRide(myNet, personParent,
                                 fromEdge, fromBusStop, fromTrainStop,
                                 toEdge, toBusStop, toTrainStop,
                                 arrivalPos, lines);
        // continue depending of undo-redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(ride, TLF("add % in '%'", ride->getTagStr(), personParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(ride, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(ride);
            // set child references
            personParent->addChildElement(ride);
            if (fromEdge) {
                fromEdge->addChildElement(ride);
            }
            if (toEdge) {
                toEdge->addChildElement(ride);
            }
            if (toBusStop) {
                toBusStop->addChildElement(ride);
            }
            if (toTrainStop) {
                toTrainStop->addChildElement(ride);
            }
            ride->incRef("buildRide");
        }
    }
}


void
GNERouteHandler::buildContainer(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& containerParameters) {
    // first check if ID is duplicated
    if (!isContainerIdDuplicated(containerParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(containerParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", containerParameters.vtypeid, toString(containerParameters.tag), containerParameters.id));
        } else {
            // create container using containerParameters
            GNEDemandElement* container = new GNEContainer(SUMO_TAG_CONTAINER, myNet, type, containerParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(container, TL("add ") + container->getTagStr() + " '" + container->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(container, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(container);
                // set container as child of type
                type->addChildElement(container);
                container->incRef("buildContainer");
            }
        }
    }
}


void
GNERouteHandler::buildContainerFlow(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& containerFlowParameters) {
    // first check if ID is duplicated
    if (!isContainerIdDuplicated(containerFlowParameters.id)) {
        // obtain  type
        GNEDemandElement* type = getType(containerFlowParameters.vtypeid);
        if (type == nullptr) {
            writeError(TLF("Invalid vehicle type '%' used in % '%'.", containerFlowParameters.vtypeid, toString(containerFlowParameters.tag), containerFlowParameters.id));
        } else {
            // create containerFlow using containerFlowParameters
            GNEDemandElement* containerFlow = new GNEContainer(SUMO_TAG_CONTAINERFLOW, myNet, type, containerFlowParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(containerFlow, TL("add ") + containerFlow->getTagStr() + " '" + containerFlow->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(containerFlow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(containerFlow);
                // set containerFlow as child of type
                type->addChildElement(containerFlow);
                containerFlow->incRef("buildContainerFlow");
            }
        }
    }
}


void
GNERouteHandler::buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                                const std::string& fromEdgeID, const std::string& fromTAZID, const std::string& fromJunctionID, const std::string& fromContainerStopID,
                                const std::string& toEdgeID, const std::string& toTAZID, const std::string& toJunctionID, const std::string& toContainerStopID,
                                const double arrivalPos, const std::vector<std::string>& lines) {
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* fromTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, fromTAZID, false);
    GNEAdditional* toTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, toTAZID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* fromContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, fromContainerStopID, false);
    GNEAdditional* toContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    // check if overwritte previous elements
    if (getPreviousPlanEdge(sumoBaseObject)) {
        fromEdge = getPreviousPlanEdge(sumoBaseObject);
    }
    if (getPreviousPlanContainerStop(sumoBaseObject)) {
        fromContainerStop = getPreviousPlanContainerStop(sumoBaseObject);
    }
    // check conditions
    if (containerParent == nullptr) {
        WRITE_WARNING(TL("invalid container parent"));
    } else if (GNETransport::getTransportTagIcon(fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction, fromContainerStop, toContainerStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for personTrip"));
    } else {
        // build transport
        GNEDemandElement* transport = GNETransport::buildTransport(myNet, containerParent,
                                      fromEdge, fromTAZ, fromJunction, fromContainerStop,
                                      toEdge, toTAZ, toJunction, toContainerStop,
                                      arrivalPos, lines);
        // continue depending of undo-redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(transport, TLF("add % in '%'", transport->getTagStr(), containerParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(transport, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(transport);
            // set child references
            containerParent->addChildElement(transport);
            if (fromEdge) {
                fromEdge->addChildElement(transport);
            }
            if (toEdge) {
                toEdge->addChildElement(transport);
            }
            if (fromContainerStop) {
                fromContainerStop->addChildElement(transport);
            }
            if (toContainerStop) {
                toContainerStop->addChildElement(transport);
            }
            transport->incRef("buildTransport");
        }
    }
}


void
GNERouteHandler::buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                               const std::string& fromEdgeID, const std::string& fromTAZID, const std::string& fromJunctionID, const std::string& fromContainerStopID,
                               const std::string& toEdgeID, const std::string& toTAZID, const std::string& toJunctionID, const std::string& toContainerStopID,
                               const std::vector<std::string>& edgeIDs, const double arrivalPosition, const double departPosition, const double speed) {
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* fromTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, fromTAZID, false);
    GNEAdditional* toTAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, toTAZID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* fromContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, fromContainerStopID, false);
    GNEAdditional* toContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // avoid consecutive duplicated edges
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // check if overwritte previous elements
    if (getPreviousPlanEdge(sumoBaseObject)) {
        fromEdge = getPreviousPlanEdge(sumoBaseObject);
    }
    if (getPreviousPlanContainerStop(sumoBaseObject)) {
        fromContainerStop = getPreviousPlanContainerStop(sumoBaseObject);
    }
    // check conditions
    if (containerParent == nullptr) {
        WRITE_WARNING(TL("invalid container parent"));
    } else if (GNETranship::getTranshipTagIcon(edges, fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction, fromContainerStop, toContainerStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for personTrip"));
    } else {
        // build tranship
        GNEDemandElement* tranship = GNETranship::buildTranship(myNet, containerParent,
                                     fromEdge, fromTAZ, fromJunction, fromContainerStop,
                                     toEdge, toTAZ, toJunction, toContainerStop,
                                     edges, arrivalPosition, departPosition, speed);
        // continue depending of undo-redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(tranship, TLF("add % in '%'", tranship->getTagStr(), containerParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(tranship);
            // set child references
            containerParent->addChildElement(tranship);
            if (fromEdge) {
                fromEdge->addChildElement(tranship);
            }
            if (toEdge) {
                toEdge->addChildElement(tranship);
            }
            if (fromContainerStop) {
                fromContainerStop->addChildElement(tranship);
            }
            if (toContainerStop) {
                toContainerStop->addChildElement(tranship);
            }
            for (const auto& edge : edges) {
                edge->addChildElement(tranship);
            }
            tranship->incRef("buildTranship");
        }
    }
}


void
GNERouteHandler::buildPersonStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID, const std::string& busStopID,
                                 const std::string& trainStopID, const double endPos, const SUMOTime duration, const SUMOTime until,
                                 const std::string& actType, const bool friendlyPos, const int parameterSet) {
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    GNEAdditional* busStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, busStopID, false);
    GNEAdditional* trainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, trainStopID, false);
    // check conditions
    if (personParent == nullptr) {
        WRITE_WARNING(TL("invalid person parent"));
    } else if (GNETranship::getPersonStopTagIcon(edge, busStop, trainStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for person stop"));
    } else {
        // build person stop
        GNEDemandElement* stopPlan = GNEStopPlan::buildPersonStopPlan(myNet, personParent, edge, busStop, trainStop,
                                     endPos, duration, until, actType, friendlyPos, parameterSet);
        // continue depending of undo-redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(stopPlan, TLF("add % in '%'", stopPlan->getTagStr(), personParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stopPlan, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(stopPlan);
            // set child references
            personParent->addChildElement(stopPlan);
            if (edge) {
                edge->addChildElement(stopPlan);
            }
            if (busStop) {
                busStop->addChildElement(stopPlan);
            }
            if (trainStop) {
                trainStop->addChildElement(stopPlan);
            }
            stopPlan->incRef("buildPersonStop");
        }
    }
}


void
GNERouteHandler::buildContainerStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID,
                                    const std::string& containerStopID, const double endPos, const SUMOTime duration,
                                    const SUMOTime until, const std::string& actType, const bool friendlyPos, const int parameterSet) {
    // parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    GNEAdditional* containerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, containerStopID, false);
    // check conditions
    if (containerParent == nullptr) {
        WRITE_WARNING(TL("invalid container parent"));
    } else if (GNETranship::getContainerStopTagIcon(edge, containerStop).first == SUMO_TAG_NOTHING) {
        WRITE_WARNING(TL("invalid combination for containerStop"));
    } else {
        // build container stop
        GNEDemandElement* stopPlan = GNEStopPlan::buildContainerStopPlan(myNet, containerParent, edge, containerStop,
                                     endPos, duration, until, actType, friendlyPos, parameterSet);
        // continue depending of undo-redo
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(stopPlan, TLF("add % in '%'", stopPlan->getTagStr(), containerParent->getID()));
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stopPlan, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(stopPlan);
            // set child references
            containerParent->addChildElement(stopPlan);
            if (edge) {
                edge->addChildElement(stopPlan);
            }
            if (containerStop) {
                containerStop->addChildElement(stopPlan);
            }
            stopPlan->incRef("buildContainerStop");
        }
    }
}


void
GNERouteHandler::buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) {
    // get obj parent
    const auto objParent = sumoBaseObject->getParentSumoBaseObject();
    // continue depending of objParent
    if (objParent == nullptr) {
        WRITE_WARNING(TL("Stops needs a parent"));
    } else if ((objParent->getTag() == SUMO_TAG_PERSON) || (objParent->getTag() == SUMO_TAG_PERSONFLOW)) {
        buildPersonStop(sumoBaseObject, stopParameters.edge, stopParameters.busstop, stopParameters.busstop, stopParameters.endPos,
                        stopParameters.duration, stopParameters.until, stopParameters.actType, stopParameters.friendlyPos, stopParameters.parametersSet);
    } else if ((objParent->getTag() == SUMO_TAG_CONTAINER) || (objParent->getTag() == SUMO_TAG_CONTAINERFLOW)) {
        buildContainerStop(sumoBaseObject, stopParameters.edge, stopParameters.containerstop, stopParameters.endPos,
                           stopParameters.duration, stopParameters.until, stopParameters.actType, stopParameters.friendlyPos, stopParameters.parametersSet);
    } else {
        // get vehicle tag
        SumoXMLTag vehicleTag = objParent->getTag();
        if (vehicleTag == SUMO_TAG_VEHICLE) {
            // check if vehicle is placed over route or with embedded route
            if (!objParent->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                vehicleTag = GNE_TAG_VEHICLE_WITHROUTE;
            }
        } else if (vehicleTag == SUMO_TAG_FLOW) {
            if (objParent->hasStringAttribute(SUMO_ATTR_ROUTE)) {
                vehicleTag = GNE_TAG_FLOW_ROUTE;
            } else if (objParent->hasStringAttribute(SUMO_ATTR_FROM) && objParent->hasStringAttribute(SUMO_ATTR_TO)) {
                vehicleTag = SUMO_TAG_FLOW;
            } else {
                vehicleTag = GNE_TAG_FLOW_WITHROUTE;
            }
        }
        // get stop parent
        GNEDemandElement* stopParent = myNet->getAttributeCarriers()->retrieveDemandElement(vehicleTag, objParent->getStringAttribute(SUMO_ATTR_ID), false);
        // check if stopParent exist
        if (stopParent) {
            // flag for waypoint (is like a stop, but with extra attribute speed)
            bool waypoint = false;
            // avoid waypoints for person and containers
            if (!stopParent->getTagProperty().isPerson() && !stopParent->getTagProperty().isContainer()) {
                waypoint = (sumoBaseObject->getStopParameter().parametersSet & STOP_SPEED_SET) || (sumoBaseObject->getStopParameter().speed > 0);
            }
            // declare pointers to parent elements
            GNEAdditional* stoppingPlace = nullptr;
            GNELane* lane = nullptr;
            GNEEdge* edge = nullptr;
            // declare stopTagType
            SumoXMLTag stopTagType = SUMO_TAG_NOTHING;
            bool validParentDemandElement = true;
            // check conditions
            if (stopParameters.busstop.size() > 0) {
                stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, stopParameters.busstop, false);
                stopTagType = waypoint ? GNE_TAG_WAYPOINT_BUSSTOP : GNE_TAG_STOP_BUSSTOP;
                // check if is a train stop
                if (stoppingPlace == nullptr) {
                    stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, stopParameters.busstop, false);
                    stopTagType = waypoint ? GNE_TAG_WAYPOINT_TRAINSTOP : GNE_TAG_STOP_TRAINSTOP;
                }
                // containers cannot stops in busStops
                if (stopParent->getTagProperty().isContainer()) {
                    writeError(TL("Containers don't support stops at busStops or trainStops"));
                    validParentDemandElement = false;
                }
            } else if (stopParameters.containerstop.size() > 0) {
                stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
                stopTagType = waypoint ? GNE_TAG_WAYPOINT_CONTAINERSTOP : GNE_TAG_STOP_CONTAINERSTOP;
                // persons cannot stops in containerStops
                if (stopParent->getTagProperty().isPerson()) {
                    writeError(TL("Persons don't support stops at containerStops"));
                    validParentDemandElement = false;
                }
            } else if (stopParameters.chargingStation.size() > 0) {
                stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
                stopTagType = waypoint ? GNE_TAG_WAYPOINT_CHARGINGSTATION : GNE_TAG_STOP_CHARGINGSTATION;
                // check person and containers
                if (stopParent->getTagProperty().isPerson()) {
                    writeError(TL("Persons don't support stops at chargingStations"));
                    validParentDemandElement = false;
                } else if (stopParent->getTagProperty().isContainer()) {
                    writeError(TL("Containers don't support stops at chargingStations"));
                    validParentDemandElement = false;
                }
            } else if (stopParameters.parkingarea.size() > 0) {
                stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stopParameters.parkingarea, false);
                stopTagType = waypoint ? GNE_TAG_WAYPOINT_PARKINGAREA : GNE_TAG_STOP_PARKINGAREA;
                // check person and containers
                if (stopParent->getTagProperty().isPerson()) {
                    writeError(TL("Persons don't support stops at parkingAreas"));
                    validParentDemandElement = false;
                } else if (stopParent->getTagProperty().isContainer()) {
                    writeError(TL("Containers don't support stops at parkingAreas"));
                    validParentDemandElement = false;
                }
            } else if (stopParameters.lane.size() > 0) {
                lane = myNet->getAttributeCarriers()->retrieveLane(stopParameters.lane, false);
                stopTagType = waypoint ? GNE_TAG_WAYPOINT_LANE : GNE_TAG_STOP_LANE;
            } else if (stopParameters.edge.size() > 0) {
                edge = myNet->getAttributeCarriers()->retrieveEdge(stopParameters.edge, false);
                // check vehicles
                if (stopParent->getTagProperty().isVehicle()) {
                    writeError(TL("vehicles don't support stops at edges"));
                    validParentDemandElement = false;
                }
            }
            // overwrite lane with edge parent if we're handling a personStop
            if (lane && (stopParent->getTagProperty().isPerson() || stopParent->getTagProperty().isContainer())) {
                edge = lane->getParentEdge();
                lane = nullptr;
            }
            // first check that parent is valid
            if (validParentDemandElement) {
                // check if values are correct
                if (stoppingPlace && lane && edge) {
                    writeError(TL("A stop must be defined either over a stoppingPlace, a edge or a lane"));
                } else if (!stoppingPlace && !lane && !edge) {
                    writeError(TL("A stop requires only a stoppingPlace, edge or lane"));
                } else if (stoppingPlace) {
                    // create stop using stopParameters and stoppingPlace
                    GNEDemandElement* stop = nullptr;
                    if (stopParent->getTagProperty().isPerson()) {
                        if (stoppingPlace->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
                            stop = new GNEStop(GNE_TAG_STOPPERSON_BUSSTOP, myNet, stopParent, stoppingPlace, stopParameters);
                        } else {
                            stop = new GNEStop(GNE_TAG_STOPPERSON_TRAINSTOP, myNet, stopParent, stoppingPlace, stopParameters);
                        }
                    } else if (stopParent->getTagProperty().isContainer()) {
                        stop = new GNEStop(GNE_TAG_STOPCONTAINER_CONTAINERSTOP, myNet, stopParent, stoppingPlace, stopParameters);
                    } else {
                        stop = new GNEStop(stopTagType, myNet, stopParent, stoppingPlace, stopParameters);
                    }
                    // add it depending of undoDemandElements
                    if (myAllowUndoRedo) {
                        myNet->getViewNet()->getUndoList()->begin(stop, TL("add ") + stop->getTagStr() + " in '" + stopParent->getID() + "'");
                        overwriteDemandElement();
                        myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                        myNet->getViewNet()->getUndoList()->end();
                    } else {
                        myNet->getAttributeCarriers()->insertDemandElement(stop);
                        stoppingPlace->addChildElement(stop);
                        stopParent->addChildElement(stop);
                        stop->incRef("buildStoppingPlaceStop");
                    }
                } else if (lane) {
                    // create stop using stopParameters and lane (only for vehicles)
                    GNEDemandElement* stop = new GNEStop(stopTagType, myNet, stopParent, lane, stopParameters);
                    // add it depending of undoDemandElements
                    if (myAllowUndoRedo) {
                        myNet->getViewNet()->getUndoList()->begin(stop, TL("add ") + stop->getTagStr() + " in '" + stopParent->getID() + "'");
                        overwriteDemandElement();
                        myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                        myNet->getViewNet()->getUndoList()->end();
                    } else {
                        myNet->getAttributeCarriers()->insertDemandElement(stop);
                        lane->addChildElement(stop);
                        stopParent->addChildElement(stop);
                        stop->incRef("buildLaneStop");
                    }
                }
            }
        }
    }
}


bool
GNERouteHandler::buildPersonPlan(const GNEDemandElement* planTemplate, GNEDemandElement* personParent,
                                 GNEAttributesCreator* personPlanAttributes, GNEPlanCreator* planCreator,
                                 const bool centerAfterCreation) {
    // first check if person is valid
    if (personParent == nullptr) {
        return false;
    }
    // clear and set person object
    myPlanObject->clear();
    myPlanObject->setTag(personParent->getTagProperty().getTag());
    myPlanObject->addStringAttribute(SUMO_ATTR_ID, personParent->getID());
    // declare personPlan object
    CommonXMLStructure::SumoBaseObject* personPlanObject = new CommonXMLStructure::SumoBaseObject(myPlanObject);
    // get person plan attributes
    personPlanAttributes->getAttributesAndValues(personPlanObject, true);
    // get attributes
    const std::vector<std::string> types = personPlanObject->hasStringListAttribute(SUMO_ATTR_VTYPES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_VTYPES) :
                                           personPlanObject->hasStringAttribute(SUMO_ATTR_VTYPES) ? GNEAttributeCarrier::parse<std::vector<std::string> >(personPlanObject->getStringAttribute(SUMO_ATTR_VTYPES)) :
                                           std::vector<std::string>();
    const std::vector<std::string> modes = personPlanObject->hasStringListAttribute(SUMO_ATTR_MODES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_MODES) :
                                           personPlanObject->hasStringAttribute(SUMO_ATTR_MODES) ? GNEAttributeCarrier::parse<std::vector<std::string> >(personPlanObject->getStringAttribute(SUMO_ATTR_MODES)) :
                                           std::vector<std::string>();
    const std::vector<std::string> lines = personPlanObject->hasStringListAttribute(SUMO_ATTR_LINES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_LINES) :
                                           personPlanObject->hasStringAttribute(SUMO_ATTR_LINES) ? GNEAttributeCarrier::parse<std::vector<std::string> >(personPlanObject->getStringAttribute(SUMO_ATTR_LINES)) :
                                           std::vector<std::string>();
    const double arrivalPos = personPlanObject->hasDoubleAttribute(SUMO_ATTR_ARRIVALPOS) ? personPlanObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS) :
                              personPlanObject->hasStringAttribute(SUMO_ATTR_ARRIVALPOS) ? GNEAttributeCarrier::parse<double>(personPlanObject->getStringAttribute(SUMO_ATTR_ARRIVALPOS)) :
                              -1;
    const double endPos = personPlanObject->hasDoubleAttribute(SUMO_ATTR_ENDPOS) ? personPlanObject->getDoubleAttribute(SUMO_ATTR_ENDPOS) :
                          personPlanObject->hasStringAttribute(SUMO_ATTR_ENDPOS) ? GNEAttributeCarrier::parse<double>(personPlanObject->getStringAttribute(SUMO_ATTR_ENDPOS)) :
                          planCreator->getClickedPositionOverLane();
    const SUMOTime duration = personPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION) ? personPlanObject->getTimeAttribute(SUMO_ATTR_DURATION) :
                              personPlanObject->hasStringAttribute(SUMO_ATTR_DURATION) ? GNEAttributeCarrier::parse<SUMOTime>(personPlanObject->getStringAttribute(SUMO_ATTR_DURATION)) :
                              0;
    const SUMOTime until = personPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL) ? personPlanObject->getTimeAttribute(SUMO_ATTR_UNTIL) :
                           personPlanObject->hasStringAttribute(SUMO_ATTR_UNTIL) ? GNEAttributeCarrier::parse<SUMOTime>(personPlanObject->getStringAttribute(SUMO_ATTR_UNTIL)) :
                           0;
    const std::string actType = personPlanObject->hasStringAttribute(SUMO_ATTR_ACTTYPE) ? personPlanObject->getStringAttribute(SUMO_ATTR_ACTTYPE) : "";
    const bool friendlyPos = personPlanObject->hasBoolAttribute(SUMO_ATTR_FRIENDLY_POS) ? personPlanObject->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS) :
                             personPlanObject->hasStringAttribute(SUMO_ATTR_FRIENDLY_POS) ? GNEAttributeCarrier::parse<bool>(personPlanObject->getStringAttribute(SUMO_ATTR_FRIENDLY_POS)) :
                             false;
    // get consecutive edges
    const auto consecutiveEdges = planCreator->getConsecutiveEdgeIDs();
    // get edges
    GNEEdge* fromEdge = planCreator->getFromEdge();
    GNEEdge* toEdge = planCreator->getToEdge();
    // get junctions
    GNEJunction* fromJunction = planCreator->getFromJunction();
    GNEJunction* toJunction = planCreator->getToJunction();
    // get TAZs
    GNEAdditional* fromTAZ = planCreator->getFromTAZ();
    GNEAdditional* toTAZ = planCreator->getToTAZ();
    // get busStops
    GNEAdditional* fromBusStop = planCreator->getFromBusStop();
    GNEAdditional* toBusStop = planCreator->getToBusStop();
    // get trainStops
    GNEAdditional* fromTrainStop = planCreator->getFromTrainStop();
    GNEAdditional* toTrainStop = planCreator->getToTrainStop();
    // get route
    GNEDemandElement* route = planCreator->getRoute();
    // get edge
    GNEEdge* edge = planCreator->getEdge();
    // get busStop
    GNEAdditional* busStop = planCreator->getBusStop();
    // get trainStop
    GNEAdditional* trainStop = planCreator->getTrainStop();
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromTAZ && !toTAZ && !fromJunction && !toJunction &&
            !fromBusStop && !toBusStop && !fromTrainStop && !toTrainStop &&
            consecutiveEdges.empty() && !route) {
        toEdge = fromEdge;
    }
    // create plans depending of elements
    if (consecutiveEdges.size() > 0) {
        // consecutive edges
        personPlanObject->setTag(GNE_TAG_WALK_EDGES);
        buildWalk(personPlanObject,
                  "", "", "", "", "",
                  "", "", "", "", "",
                  consecutiveEdges, "", arrivalPos);
    } else if (route) {
        // single route
        personPlanObject->setTag(GNE_TAG_WALK_ROUTE);
        buildWalk(personPlanObject,
                  "", "", "", "", "",
                  "", "", "", "", "",
                  {}, route->getID(), arrivalPos);
    } else if (planTemplate->getTagProperty().isPlanWalk()) {
        // set walk tag
        personPlanObject->setTag(GNEDemandElementPlan::getWalkTagIcon({}, route,
                                 fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction,
                                 fromBusStop, toBusStop, fromTrainStop, toTrainStop).first);
        // from edges
        if (fromEdge && toEdge) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      fromEdge->getID(), "", "", "", "",
                      toEdge->getID(), "", "", "", "",
                      {}, "", arrivalPos);
        } else if (fromEdge && toTAZ) {
            // walk: edge->TAZ
            buildWalk(personPlanObject,
                      fromEdge->getID(), "", "", "", "",
                      "", toTAZ->getID(), "", "", "",
                      {}, "", arrivalPos);
        } else if (fromEdge && toJunction) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      fromEdge->getID(), "", "", "", "",
                      "", "", toJunction->getID(), "", "",
                      {}, "", arrivalPos);
        } else if (fromEdge && toBusStop) {
            // walk: edge->busStop
            buildWalk(personPlanObject,
                      fromEdge->getID(), "", "", "", "",
                      "", "", "", toBusStop->getID(), "",
                      {}, "", arrivalPos);
        } else if (fromEdge && toTrainStop) {
            // walk: edge->trainStop
            buildWalk(personPlanObject,
                      fromEdge->getID(), "", "", "", "",
                      "", "", "", "", toTrainStop->getID(),
                      {}, "", arrivalPos);
            // from TAZs
        } else if (fromTAZ && toEdge) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", fromTAZ->getID(), "", "", "",
                      toEdge->getID(), "", "", "", "",
                      {}, "", arrivalPos);
        } else if (fromTAZ && toTAZ) {
            // walk: edge->TAZ
            buildWalk(personPlanObject,
                      "", fromTAZ->getID(), "", "", "",
                      "", toTAZ->getID(), "", "", "",
                      {}, "", arrivalPos);
        } else if (fromTAZ && toJunction) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", fromTAZ->getID(), "", "", "",
                      "", "", toJunction->getID(), "", "",
                      {}, "", arrivalPos);
        } else if (fromTAZ && toBusStop) {
            // walk: edge->busStop
            buildWalk(personPlanObject,
                      "", fromTAZ->getID(), "", "", "",
                      "", "", "", toBusStop->getID(), "",
                      {}, "", arrivalPos);
        } else if (fromTAZ && toTrainStop) {
            // walk: edge->trainStop
            buildWalk(personPlanObject,
                      "", fromTAZ->getID(), "", "", "",
                      "", "", "", "", toTrainStop->getID(),
                      {}, "", arrivalPos);
            // from junction
        } else if (fromJunction && toEdge) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", fromJunction->getID(), "", "",
                      toEdge->getID(), "", "", "", "",
                      {}, "", arrivalPos);
        } else if (fromJunction && toTAZ) {
            // walk: edge->TAZ
            buildWalk(personPlanObject,
                      "", "", fromJunction->getID(), "", "",
                      "", toTAZ->getID(), "", "", "",
                      {}, "", arrivalPos);
        } else if (fromJunction && toJunction) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", fromJunction->getID(), "", "",
                      "", "", toJunction->getID(), "", "",
                      {}, "", arrivalPos);
        } else if (fromJunction && toBusStop) {
            // walk: edge->busStop
            buildWalk(personPlanObject,
                      "", "", fromJunction->getID(), "", "",
                      "", "", "", toBusStop->getID(), "",
                      {}, "", arrivalPos);
        } else if (fromJunction && toTrainStop) {
            // walk: edge->trainStop
            buildWalk(personPlanObject,
                      "", "", fromJunction->getID(), "", "",
                      "", "", "", "", toTrainStop->getID(),
                      {}, "", arrivalPos);
            // from busStop
        } else if (fromBusStop && toEdge) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", "", fromBusStop->getID(), "",
                      toEdge->getID(), "", "", "", "",
                      {}, "", arrivalPos);
        } else if (fromBusStop && toTAZ) {
            // walk: edge->TAZ
            buildWalk(personPlanObject,
                      "", "", "", fromBusStop->getID(), "",
                      "", toTAZ->getID(), "", "", "",
                      {}, "", arrivalPos);
        } else if (fromBusStop && toJunction) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", "", fromBusStop->getID(), "",
                      "", "", toJunction->getID(), "", "",
                      {}, "", arrivalPos);
        } else if (fromBusStop && toBusStop) {
            // walk: edge->busStop
            buildWalk(personPlanObject,
                      "", "", "", fromBusStop->getID(), "",
                      "", "", "", toBusStop->getID(), "",
                      {}, "", arrivalPos);
        } else if (fromBusStop && toTrainStop) {
            // walk: edge->trainStop
            buildWalk(personPlanObject,
                      "", "", "", fromBusStop->getID(), "",
                      "", "", "", "", toTrainStop->getID(),
                      {}, "", arrivalPos);
            // from trainStop
        } else if (fromTrainStop && toEdge) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", "", "", fromTrainStop->getID(),
                      toEdge->getID(), "", "", "", "",
                      {}, "", arrivalPos);
        } else if (fromTrainStop && toTAZ) {
            // walk: edge->TAZ
            buildWalk(personPlanObject,
                      "", "", "", "", fromTrainStop->getID(),
                      "", toTAZ->getID(), "", "", "",
                      {}, "", arrivalPos);
        } else if (fromTrainStop && toJunction) {
            // walk: edge->edge
            buildWalk(personPlanObject,
                      "", "", "", "", fromTrainStop->getID(),
                      "", "", toJunction->getID(), "", "",
                      {}, "", arrivalPos);
        } else if (fromTrainStop && toBusStop) {
            // walk: edge->busStop
            buildWalk(personPlanObject,
                      "", "", "", "", fromTrainStop->getID(),
                      "", "", "", toBusStop->getID(), "",
                      {}, "", arrivalPos);
        } else if (fromTrainStop && toTrainStop) {
            // walk: edge->trainStop
            buildWalk(personPlanObject,
                      "", "", "", "", fromTrainStop->getID(),
                      "", "", "", "", toTrainStop->getID(),
                      {}, "", arrivalPos);
        }
    } else if (planTemplate->getTagProperty().isPersonTrip()) {
        // set person trip tag
        personPlanObject->setTag(GNEDemandElementPlan::getPersonTripTagIcon(fromEdge, toEdge,
                                 fromTAZ, toTAZ, fromJunction, toJunction, fromBusStop, toBusStop,
                                 fromTrainStop, toTrainStop).first);
        // from edges
        if (fromEdge && toEdge) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            fromEdge->getID(), "", "", "", "",
                            toEdge->getID(), "", "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromEdge && toTAZ) {
            // personTrip: edge->TAZ
            buildPersonTrip(personPlanObject,
                            fromEdge->getID(), "", "", "", "",
                            "", toTAZ->getID(), "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromEdge && toJunction) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            fromEdge->getID(), "", "", "", "",
                            "", "", toJunction->getID(), "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromEdge && toBusStop) {
            // personTrip: edge->busStop
            buildPersonTrip(personPlanObject,
                            fromEdge->getID(), "", "", "", "",
                            "", "", "", toBusStop->getID(), "",
                            arrivalPos, types, modes, lines);
        } else if (fromEdge && toTrainStop) {
            // personTrip: edge->trainStop
            buildPersonTrip(personPlanObject,
                            fromEdge->getID(), "", "", "", "",
                            "", "", "", "", toTrainStop->getID(),
                            arrivalPos, types, modes, lines);
            // from TAZs
        } else if (fromTAZ && toEdge) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", fromTAZ->getID(), "", "", "",
                            toEdge->getID(), "", "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTAZ && toTAZ) {
            // personTrip: edge->TAZ
            buildPersonTrip(personPlanObject,
                            "", fromTAZ->getID(), "", "", "",
                            "", toTAZ->getID(), "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTAZ && toJunction) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", fromTAZ->getID(), "", "", "",
                            "", "", toJunction->getID(), "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTAZ && toBusStop) {
            // personTrip: edge->busStop
            buildPersonTrip(personPlanObject,
                            "", fromTAZ->getID(), "", "", "",
                            "", "", "", toBusStop->getID(), "",
                            arrivalPos, types, modes, lines);
        } else if (fromTAZ && toTrainStop) {
            // personTrip: edge->trainStop
            buildPersonTrip(personPlanObject,
                            "", fromTAZ->getID(), "", "", "",
                            "", "", "", "", toTrainStop->getID(),
                            arrivalPos, types, modes, lines);
            // from junction
        } else if (fromJunction && toEdge) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", fromJunction->getID(), "", "",
                            toEdge->getID(), "", "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromJunction && toTAZ) {
            // personTrip: edge->TAZ
            buildPersonTrip(personPlanObject,
                            "", "", fromJunction->getID(), "", "",
                            "", toTAZ->getID(), "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromJunction && toJunction) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", fromJunction->getID(), "", "",
                            "", "", toJunction->getID(), "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromJunction && toBusStop) {
            // personTrip: edge->busStop
            buildPersonTrip(personPlanObject,
                            "", "", fromJunction->getID(), "", "",
                            "", "", "", toBusStop->getID(), "",
                            arrivalPos, types, modes, lines);
        } else if (fromJunction && toTrainStop) {
            // personTrip: edge->trainStop
            buildPersonTrip(personPlanObject,
                            "", "", fromJunction->getID(), "", "",
                            "", "", "", "", toTrainStop->getID(),
                            arrivalPos, types, modes, lines);
            // from busStop
        } else if (fromBusStop && toEdge) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", "", fromBusStop->getID(), "",
                            toEdge->getID(), "", "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromBusStop && toTAZ) {
            // personTrip: edge->TAZ
            buildPersonTrip(personPlanObject,
                            "", "", "", fromBusStop->getID(), "",
                            "", toTAZ->getID(), "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromBusStop && toJunction) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", "", fromBusStop->getID(), "",
                            "", "", toJunction->getID(), "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromBusStop && toBusStop) {
            // personTrip: edge->busStop
            buildPersonTrip(personPlanObject,
                            "", "", "", fromBusStop->getID(), "",
                            "", "", "", toBusStop->getID(), "",
                            arrivalPos, types, modes, lines);
        } else if (fromBusStop && toTrainStop) {
            // personTrip: edge->trainStop
            buildPersonTrip(personPlanObject,
                            "", "", "", fromBusStop->getID(), "",
                            "", "", "", "", toTrainStop->getID(),
                            arrivalPos, types, modes, lines);
            // from trainStop
        } else if (fromTrainStop && toEdge) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", "", "", fromTrainStop->getID(),
                            toEdge->getID(), "", "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTrainStop && toTAZ) {
            // personTrip: edge->TAZ
            buildPersonTrip(personPlanObject,
                            "", "", "", "", fromTrainStop->getID(),
                            "", toTAZ->getID(), "", "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTrainStop && toJunction) {
            // personTrip: edge->edge
            buildPersonTrip(personPlanObject,
                            "", "", "", "", fromTrainStop->getID(),
                            "", "", toJunction->getID(), "", "",
                            arrivalPos, types, modes, lines);
        } else if (fromTrainStop && toBusStop) {
            // personTrip: edge->busStop
            buildPersonTrip(personPlanObject,
                            "", "", "", "", fromTrainStop->getID(),
                            "", "", "", toBusStop->getID(), "",
                            arrivalPos, types, modes, lines);
        } else if (fromTrainStop && toTrainStop) {
            // personTrip: edge->trainStop
            buildPersonTrip(personPlanObject,
                            "", "", "", "", fromTrainStop->getID(),
                            "", "", "", "", toTrainStop->getID(),
                            arrivalPos, types, modes, lines);
        }
    } else if (planTemplate->getTagProperty().isPlanRide()) {
        // set ride tag
        personPlanObject->setTag(GNEDemandElementPlan::getRideTagIcon(fromEdge, toEdge,
                                 fromBusStop, toBusStop, fromTrainStop, toTrainStop).first);
        // from edges
        if (fromEdge && toEdge) {
            // ride: edge->edge
            buildRide(personPlanObject,
                      fromEdge->getID(), "", "",
                      toEdge->getID(), "", "",
                      arrivalPos, lines);
        } else if (fromEdge && toBusStop) {
            // ride: edge->busStop
            buildRide(personPlanObject,
                      fromEdge->getID(), "", "",
                      "", toBusStop->getID(), "",
                      arrivalPos, lines);
        } else if (fromEdge && toTrainStop) {
            // ride: edge->trainStop
            buildRide(personPlanObject,
                      fromEdge->getID(), "", "",
                      "", "", toTrainStop->getID(),
                      arrivalPos, lines);
            // from busStop
        } else if (fromBusStop && toEdge) {
            // ride: edge->edge
            buildRide(personPlanObject,
                      "", fromBusStop->getID(), "",
                      toEdge->getID(), "", "",
                      arrivalPos, lines);
        } else if (fromBusStop && toBusStop) {
            // ride: edge->busStop
            buildRide(personPlanObject,
                      "", fromBusStop->getID(), "",
                      "", toBusStop->getID(), "",
                      arrivalPos, lines);
        } else if (fromBusStop && toTrainStop) {
            // ride: edge->trainStop
            buildRide(personPlanObject,
                      "", fromBusStop->getID(), "",
                      "", "", toTrainStop->getID(),
                      arrivalPos, lines);
            // from trainStop
        } else if (fromTrainStop && toEdge) {
            // ride: edge->edge
            buildRide(personPlanObject,
                      "", "", fromTrainStop->getID(),
                      toEdge->getID(), "", "",
                      arrivalPos, lines);
        } else if (fromTrainStop && toBusStop) {
            // ride: edge->busStop
            buildRide(personPlanObject,
                      "", "", fromTrainStop->getID(),
                      "", toBusStop->getID(), "",
                      arrivalPos, lines);
        } else if (fromTrainStop && toTrainStop) {
            // ride: edge->trainStop
            buildRide(personPlanObject,
                      "", "", fromTrainStop->getID(),
                      "", "", toTrainStop->getID(),
                      arrivalPos, lines);
        }
    } else if (planTemplate->getTagProperty().isPlanStopPerson()) {
        // set ride tag
        personPlanObject->setTag(GNEDemandElementPlan::getPersonStopTagIcon(edge, busStop, trainStop).first);
        // set parameters
        int parameterSet = 0;
        if (personPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            parameterSet |= STOP_DURATION_SET;
        }
        if (personPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            parameterSet |= STOP_UNTIL_SET;
        }
        // from edges
        if (edge) {
            buildPersonStop(personPlanObject, edge->getID(), "", "", endPos, duration, until, actType, friendlyPos, parameterSet);
        } else if (busStop) {
            buildPersonStop(personPlanObject, "", busStop->getID(), "", endPos, duration, until, actType, friendlyPos, parameterSet);
        } else if (trainStop) {
            buildPersonStop(personPlanObject, "", "", trainStop->getID(), endPos, duration, until, actType, friendlyPos, parameterSet);
        }
    }
    // get person
    const auto person = myNet->getAttributeCarriers()->retrieveDemandElement(personPlanObject->getParentSumoBaseObject()->getTag(),
                        personPlanObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (person) {
        // center view after creation
        if (centerAfterCreation && !myNet->getViewNet()->getVisibleBoundary().around(person->getPositionInView())) {
            myNet->getViewNet()->centerTo(person->getPositionInView(), false);
        }
    }
    delete personPlanObject;
    return true;
}


bool
GNERouteHandler::buildContainerPlan(const GNEDemandElement* planTemplate, GNEDemandElement* containerParent,
                                    GNEAttributesCreator* containerPlanAttributes, GNEPlanCreator* planCreator,
                                    const bool centerAfterCreation) {
    // first check if container is valid
    if (containerParent == nullptr) {
        return false;
    }
    // clear and set container object
    myPlanObject->clear();
    myPlanObject->setTag(containerParent->getTagProperty().getTag());
    myPlanObject->addStringAttribute(SUMO_ATTR_ID, containerParent->getID());
    // declare containerPlan object
    CommonXMLStructure::SumoBaseObject* containerPlanObject = new CommonXMLStructure::SumoBaseObject(myPlanObject);
    // get container plan attributes
    containerPlanAttributes->getAttributesAndValues(containerPlanObject, true);
    // get attributes
    const double speed = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_SPEED) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_SPEED) :
                         containerPlanObject->hasStringAttribute(SUMO_ATTR_SPEED) ? GNEAttributeCarrier::parse<double>(containerPlanObject->getStringAttribute(SUMO_ATTR_SPEED)) :
                         0;
    const std::vector<std::string> lines = containerPlanObject->hasStringListAttribute(SUMO_ATTR_LINES) ? containerPlanObject->getStringListAttribute(SUMO_ATTR_LINES) :
                                           containerPlanObject->hasStringAttribute(SUMO_ATTR_LINES) ? GNEAttributeCarrier::parse<std::vector<std::string> >(containerPlanObject->getStringAttribute(SUMO_ATTR_LINES)) :
                                           std::vector<std::string>();
    const double departPos = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_DEPARTPOS) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_DEPARTPOS) :
                             containerPlanObject->hasStringAttribute(SUMO_ATTR_DEPARTPOS) ? GNEAttributeCarrier::parse<double>(containerPlanObject->getStringAttribute(SUMO_ATTR_DEPARTPOS)) :
                             -1;
    const double arrivalPos = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_ARRIVALPOS) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS) :
                              containerPlanObject->hasStringAttribute(SUMO_ATTR_ARRIVALPOS) ? GNEAttributeCarrier::parse<double>(containerPlanObject->getStringAttribute(SUMO_ATTR_ARRIVALPOS)) :
                              -1;
    const double endPos = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_ENDPOS) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_ENDPOS) :
                          containerPlanObject->hasStringAttribute(SUMO_ATTR_ENDPOS) ? GNEAttributeCarrier::parse<double>(containerPlanObject->getStringAttribute(SUMO_ATTR_ENDPOS)) :
                          planCreator->getClickedPositionOverLane();
    const SUMOTime duration = containerPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION) ? containerPlanObject->getTimeAttribute(SUMO_ATTR_DURATION) :
                              containerPlanObject->hasStringAttribute(SUMO_ATTR_DURATION) ? GNEAttributeCarrier::parse<SUMOTime>(containerPlanObject->getStringAttribute(SUMO_ATTR_DURATION)) :
                              0;
    const SUMOTime until = containerPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL) ? containerPlanObject->getTimeAttribute(SUMO_ATTR_UNTIL) :
                           containerPlanObject->hasStringAttribute(SUMO_ATTR_UNTIL) ? GNEAttributeCarrier::parse<SUMOTime>(containerPlanObject->getStringAttribute(SUMO_ATTR_UNTIL)) :
                           0;
    const std::string actType = containerPlanObject->hasStringAttribute(SUMO_ATTR_ACTTYPE) ? containerPlanObject->getStringAttribute(SUMO_ATTR_ACTTYPE) : "";
    const bool friendlyPos = containerPlanObject->hasBoolAttribute(SUMO_ATTR_FRIENDLY_POS) ? containerPlanObject->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS) :
                             containerPlanObject->hasStringAttribute(SUMO_ATTR_FRIENDLY_POS) ? GNEAttributeCarrier::parse<bool>(containerPlanObject->getStringAttribute(SUMO_ATTR_FRIENDLY_POS)) :
                             false;
    // get consecutive edges
    const auto consecutiveEdges = planCreator->getConsecutiveEdgeIDs();
    // get edges
    GNEEdge* fromEdge = planCreator->getFromEdge();
    GNEEdge* toEdge = planCreator->getToEdge();
    // get junctions
    GNEJunction* fromJunction = planCreator->getFromJunction();
    GNEJunction* toJunction = planCreator->getToJunction();
    // get TAZs
    GNEAdditional* fromTAZ = planCreator->getFromTAZ();
    GNEAdditional* toTAZ = planCreator->getToTAZ();
    // get containerStops
    GNEAdditional* fromContainerStop = planCreator->getFromContainerStop();
    GNEAdditional* toContainerStop = planCreator->getToContainerStop();
    // get edge
    GNEEdge* edge = planCreator->getEdge();
    // get containerStop
    GNEAdditional* containerStop = planCreator->getContainerStop();
    // special case for elements with from-to edge
    if (fromEdge && !toEdge && !fromContainerStop && !toContainerStop && consecutiveEdges.empty()) {
        toEdge = fromEdge;
    }
    // create plans depending of elements
    if (consecutiveEdges.size() > 0) {
        // consecutive edges
        containerPlanObject->setTag(GNE_TAG_TRANSHIP_EDGES);
        buildTranship(containerPlanObject,
                      "", "", "", "",
                      "", "", "", "",
                      consecutiveEdges, speed, departPos, arrivalPos);
    } else if (planTemplate->getTagProperty().isPlanTranship()) {
        // set tranship tag
        containerPlanObject->setTag(GNEDemandElementPlan::getTranshipTagIcon({},
                                    fromEdge, toEdge, fromTAZ, toTAZ, fromJunction, toJunction,
                                    fromContainerStop, toContainerStop).first);
        // from edges
        if (fromEdge && toEdge) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          fromEdge->getID(), "", "", "",
                          toEdge->getID(), "", "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromEdge && toTAZ) {
            // tranship: edge->TAZ
            buildTranship(containerPlanObject,
                          fromEdge->getID(), "", "", "",
                          "", toTAZ->getID(), "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromEdge && toJunction) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          fromEdge->getID(), "", "", "",
                          "", "", toJunction->getID(), "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromEdge && toContainerStop) {
            // tranship: edge->containerStop
            buildTranship(containerPlanObject,
                          fromEdge->getID(), "", "", "",
                          "", "", "", toContainerStop->getID(),
                          {}, arrivalPos, departPos, speed);
            // from TAZs
        } else if (fromTAZ && toEdge) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", fromTAZ->getID(), "", "",
                          toEdge->getID(), "", "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromTAZ && toTAZ) {
            // tranship: edge->TAZ
            buildTranship(containerPlanObject,
                          "", fromTAZ->getID(), "", "",
                          "", toTAZ->getID(), "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromTAZ && toJunction) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", fromTAZ->getID(), "", "",
                          "", "", toJunction->getID(), "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromTAZ && toContainerStop) {
            // tranship: edge->containerStop
            buildTranship(containerPlanObject,
                          "", fromTAZ->getID(), "", "",
                          "", "", "", toContainerStop->getID(),
                          {}, arrivalPos, departPos, speed);
            // from junction
        } else if (fromJunction && toEdge) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", "", fromJunction->getID(), "",
                          toEdge->getID(), "", "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromJunction && toTAZ) {
            // tranship: edge->TAZ
            buildTranship(containerPlanObject,
                          "", "", fromJunction->getID(), "",
                          "", toTAZ->getID(), "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromJunction && toJunction) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", "", fromJunction->getID(), "",
                          "", "", toJunction->getID(), "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromJunction && toContainerStop) {
            // tranship: edge->containerStop
            buildTranship(containerPlanObject,
                          "", "", fromJunction->getID(), "",
                          "", "", "", toContainerStop->getID(),
                          {}, arrivalPos, departPos, speed);
            // from containerStop
        } else if (fromContainerStop && toEdge) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", "", "", fromContainerStop->getID(),
                          toEdge->getID(), "", "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromContainerStop && toTAZ) {
            // tranship: edge->TAZ
            buildTranship(containerPlanObject,
                          "", "", "", fromContainerStop->getID(),
                          "", toTAZ->getID(), "", "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromContainerStop && toJunction) {
            // tranship: edge->edge
            buildTranship(containerPlanObject,
                          "", "", "", fromContainerStop->getID(),
                          "", "", toJunction->getID(), "",
                          {}, arrivalPos, departPos, speed);
        } else if (fromContainerStop && toContainerStop) {
            // tranship: edge->containerStop
            buildTranship(containerPlanObject,
                          "", "", "", fromContainerStop->getID(),
                          "", "", "", toContainerStop->getID(),
                          {}, arrivalPos, departPos, speed);
        }
    } else if (planTemplate->getTagProperty().isPlanTransport()) {
        // set container trip tag
        containerPlanObject->setTag(GNEDemandElementPlan::getTransportTagIcon(fromEdge, toEdge,
                                    fromTAZ, toTAZ, fromJunction, toJunction,
                                    fromContainerStop, toContainerStop).first);
        // from edges
        if (fromEdge && toEdge) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           fromEdge->getID(), "", "", "",
                           toEdge->getID(), "", "", "",
                           arrivalPos, lines);
        } else if (fromEdge && toTAZ) {
            // tranship: edge->TAZ
            buildTransport(containerPlanObject,
                           fromEdge->getID(), "", "", "",
                           "", toTAZ->getID(), "", "",
                           arrivalPos, lines);
        } else if (fromEdge && toJunction) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           fromEdge->getID(), "", "", "",
                           "", "", toJunction->getID(), "",
                           arrivalPos, lines);
        } else if (fromEdge && toContainerStop) {
            // tranship: edge->containerStop
            buildTransport(containerPlanObject,
                           fromEdge->getID(), "", "", "",
                           "", "", "", toContainerStop->getID(),
                           arrivalPos, lines);
            // from TAZs
        } else if (fromTAZ && toEdge) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", fromTAZ->getID(), "", "",
                           toEdge->getID(), "", "", "",
                           arrivalPos, lines);
        } else if (fromTAZ && toTAZ) {
            // tranship: edge->TAZ
            buildTransport(containerPlanObject,
                           "", fromTAZ->getID(), "", "",
                           "", toTAZ->getID(), "", "",
                           arrivalPos, lines);
        } else if (fromTAZ && toJunction) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", fromTAZ->getID(), "", "",
                           "", "", toJunction->getID(), "",
                           arrivalPos, lines);
        } else if (fromTAZ && toContainerStop) {
            // tranship: edge->containerStop
            buildTransport(containerPlanObject,
                           "", fromTAZ->getID(), "", "",
                           "", "", "", toContainerStop->getID(),
                           arrivalPos, lines);
            // from junction
        } else if (fromJunction && toEdge) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", "", fromJunction->getID(), "",
                           toEdge->getID(), "", "", "",
                           arrivalPos, lines);
        } else if (fromJunction && toTAZ) {
            // tranship: edge->TAZ
            buildTransport(containerPlanObject,
                           "", "", fromJunction->getID(), "",
                           "", toTAZ->getID(), "", "",
                           arrivalPos, lines);
        } else if (fromJunction && toJunction) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", "", fromJunction->getID(), "",
                           "", "", toJunction->getID(), "",
                           arrivalPos, lines);
        } else if (fromJunction && toContainerStop) {
            // tranship: edge->containerStop
            buildTransport(containerPlanObject,
                           "", "", fromJunction->getID(), "",
                           "", "", "", toContainerStop->getID(),
                           arrivalPos, lines);
            // from containerStop
        } else if (fromContainerStop && toEdge) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", "", "", fromContainerStop->getID(),
                           toEdge->getID(), "", "", "",
                           arrivalPos, lines);
        } else if (fromContainerStop && toTAZ) {
            // tranship: edge->TAZ
            buildTransport(containerPlanObject,
                           "", "", "", fromContainerStop->getID(),
                           "", toTAZ->getID(), "", "",
                           arrivalPos, lines);
        } else if (fromContainerStop && toJunction) {
            // tranship: edge->edge
            buildTransport(containerPlanObject,
                           "", "", "", fromContainerStop->getID(),
                           "", "", toJunction->getID(), "",
                           arrivalPos, lines);
        } else if (fromContainerStop && toContainerStop) {
            // tranship: edge->containerStop
            buildTransport(containerPlanObject,
                           "", "", "", fromContainerStop->getID(),
                           "", "", "", toContainerStop->getID(),
                           arrivalPos, lines);
        }
    } else if (planTemplate->getTagProperty().isPlanStopContainer()) {
        // set ride tag
        containerPlanObject->setTag(GNEDemandElementPlan::getContainerStopTagIcon(edge, containerStop).first);
        // set parameters
        int parameterSet = 0;
        if (containerPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            parameterSet |= STOP_DURATION_SET;
        }
        if (containerPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            parameterSet |= STOP_UNTIL_SET;
        }
        if (edge) {
            buildContainerStop(containerPlanObject, edge->getID(), "", endPos, duration, until, actType, friendlyPos, parameterSet);
        } else if (containerStop) {
            buildContainerStop(containerPlanObject, "", containerStop->getID(), endPos, duration, until, actType, friendlyPos, parameterSet);
        }
    }
    // get container
    const auto container = myNet->getAttributeCarriers()->retrieveDemandElement(containerPlanObject->getParentSumoBaseObject()->getTag(),
                           containerPlanObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (container) {
        // center view after creation
        if (centerAfterCreation && !myNet->getViewNet()->getVisibleBoundary().around(container->getPositionInView())) {
            myNet->getViewNet()->centerTo(container->getPositionInView(), false);
        }
    }
    delete containerPlanObject;
    return true;
}


void
GNERouteHandler::duplicatePlan(const GNEDemandElement* originalPlan, GNEDemandElement* newParent) {
    const auto& tagProperty = originalPlan->getTagProperty();
    // clear and set container object
    myPlanObject->clear();
    myPlanObject->setTag(newParent->getTagProperty().getTag());
    myPlanObject->addStringAttribute(SUMO_ATTR_ID, newParent->getID());
    // declare personPlan object for adding all attributes
    CommonXMLStructure::SumoBaseObject* planObject = new CommonXMLStructure::SumoBaseObject(myPlanObject);
    planObject->setTag(tagProperty.getTag());
    // from-to elements
    const std::string fromEdgeID = tagProperty.planFromEdge() ? originalPlan->getAttribute(SUMO_ATTR_FROM) : "";
    const std::string toEdgeID = tagProperty.planToEdge() ? originalPlan->getAttribute(SUMO_ATTR_TO) : "";
    const std::string fromJunctionID = tagProperty.planFromJunction() ? originalPlan->getAttribute(SUMO_ATTR_FROM_JUNCTION) : "";
    const std::string toJunctionID = tagProperty.planToJunction() ? originalPlan->getAttribute(SUMO_ATTR_TO_JUNCTION) : "";
    const std::string fromTAZID = tagProperty.planFromTAZ() ? originalPlan->getAttribute(SUMO_ATTR_FROM_TAZ) : "";
    const std::string toTAZID = tagProperty.planToTAZ() ? originalPlan->getAttribute(SUMO_ATTR_TO_TAZ) : "";
    const std::string fromBusStopID = tagProperty.planFromBusStop() ? originalPlan->getAttribute(GNE_ATTR_FROM_BUSSTOP) : "";
    const std::string toBusStopID = tagProperty.planToBusStop() ? originalPlan->getAttribute(SUMO_ATTR_BUS_STOP) : "";
    const std::string fromTrainStopID = tagProperty.planFromTrainStop() ? originalPlan->getAttribute(GNE_ATTR_FROM_TRAINSTOP) : "";
    const std::string toTrainStopID = tagProperty.planToTrainStop() ? originalPlan->getAttribute(SUMO_ATTR_TRAIN_STOP) : "";
    const std::string fromContainerStopID = tagProperty.planFromContainerStop() ? originalPlan->getAttribute(GNE_ATTR_FROM_CONTAINERSTOP) : "";
    const std::string toContainerStopID = tagProperty.planToContainerStop() ? originalPlan->getAttribute(SUMO_ATTR_CONTAINER_STOP) : "";
    // single elements
    const std::string edgeID = tagProperty.planEdge() ? originalPlan->getAttribute(SUMO_ATTR_EDGE) : "";
    const std::string busStopID = tagProperty.planBusStop() ? originalPlan->getAttribute(SUMO_ATTR_BUS_STOP) : "";
    const std::string trainStopID = tagProperty.planTrainStop() ? originalPlan->getAttribute(SUMO_ATTR_TRAIN_STOP) : "";
    const std::string containerStopID = tagProperty.planContainerStop() ? originalPlan->getAttribute(SUMO_ATTR_CONTAINER_STOP) : "";
    const std::string routeID = tagProperty.planRoute() ? originalPlan->getAttribute(SUMO_ATTR_ROUTE) : "";
    // path
    const std::vector<std::string> edgeIDs = GNEAttributeCarrier::parse<std::vector<std::string> >(tagProperty.planConsecutiveEdges() ? originalPlan->getAttribute(SUMO_ATTR_EDGES) : "");
    // other elements
    planObject->addTimeAttribute(SUMO_ATTR_DURATION, 60);
    planObject->addTimeAttribute(SUMO_ATTR_UNTIL, 0);
    planObject->addDoubleAttribute(SUMO_ATTR_DEPARTPOS, 0);
    planObject->addDoubleAttribute(SUMO_ATTR_ARRIVALPOS, -1);
    planObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, 0);
    planObject->addDoubleAttribute(SUMO_ATTR_SPEED, 1.39);
    planObject->addBoolAttribute(SUMO_ATTR_FRIENDLY_POS, false);
    // add rest of attributes
    for (const auto& attrProperty : tagProperty) {
        if (!planObject->hasStringAttribute(attrProperty.getAttr())) {
            if (attrProperty.isFloat()) {
                if (!originalPlan->getAttribute(attrProperty.getAttr()).empty()) {
                    planObject->addDoubleAttribute(attrProperty.getAttr(), originalPlan->getAttributeDouble(attrProperty.getAttr()));
                }
            } else if (attrProperty.isSUMOTime()) {
                if (!originalPlan->getAttribute(attrProperty.getAttr()).empty()) {
                    planObject->addTimeAttribute(attrProperty.getAttr(), GNEAttributeCarrier::parse<SUMOTime>(originalPlan->getAttribute(attrProperty.getAttr())));
                }
            } else if (attrProperty.isBool()) {
                planObject->addBoolAttribute(attrProperty.getAttr(), GNEAttributeCarrier::parse<bool>(originalPlan->getAttribute(attrProperty.getAttr())));
            } else if (attrProperty.isList()) {
                planObject->addStringListAttribute(attrProperty.getAttr(), GNEAttributeCarrier::parse<std::vector<std::string> >(originalPlan->getAttribute(attrProperty.getAttr())));
            } else {
                planObject->addStringAttribute(attrProperty.getAttr(), originalPlan->getAttribute(attrProperty.getAttr()));
            }
        }
    }
    // create plan
    if (tagProperty.isPersonTrip()) {
        buildPersonTrip(planObject,
                        fromEdgeID, fromTAZID, fromJunctionID, fromBusStopID, fromTrainStopID,
                        toEdgeID, toTAZID, toJunctionID, toBusStopID, toTrainStopID,
                        planObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                        planObject->getStringListAttribute(SUMO_ATTR_VTYPES),
                        planObject->getStringListAttribute(SUMO_ATTR_MODES),
                        planObject->getStringListAttribute(SUMO_ATTR_LINES));
    } else if (tagProperty.isPlanWalk()) {
        buildWalk(planObject,
                  fromEdgeID, fromTAZID, fromJunctionID, fromBusStopID, fromTrainStopID,
                  toEdgeID, toTAZID, toJunctionID, toBusStopID, toTrainStopID,
                  edgeIDs, routeID,
                  planObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS));
    } else if (tagProperty.isPlanRide()) {
        buildRide(planObject,
                  fromEdgeID, fromBusStopID, fromTrainStopID, toEdgeID, toBusStopID, toTrainStopID,
                  planObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                  planObject->getStringListAttribute(SUMO_ATTR_LINES));
    } else if (tagProperty.isPlanStopPerson()) {
        // set parameters
        int parameterSet = 0;
        if (planObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            parameterSet |= STOP_DURATION_SET;
        }
        if (planObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            parameterSet |= STOP_UNTIL_SET;
        }
        buildPersonStop(planObject, edgeID, busStopID, trainStopID,
                        planObject->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                        planObject->getTimeAttribute(SUMO_ATTR_DURATION),
                        planObject->getTimeAttribute(SUMO_ATTR_UNTIL),
                        planObject->getStringAttribute(SUMO_ATTR_ACTTYPE),
                        planObject->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                        parameterSet);
    } else if (tagProperty.isPlanTransport()) {
        buildTransport(planObject, fromEdgeID, fromTAZID, fromJunctionID, fromContainerStopID,
                       toEdgeID, toTAZID, toJunctionID, toContainerStopID,
                       planObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                       planObject->getStringListAttribute(SUMO_ATTR_LINES));
    } else if (tagProperty.isPlanTranship()) {
        buildTranship(planObject, fromEdgeID, fromTAZID, fromJunctionID, fromContainerStopID,
                      toEdgeID, toTAZID, toJunctionID, toContainerStopID, edgeIDs,
                      planObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS),
                      planObject->getDoubleAttribute(SUMO_ATTR_DEPARTPOS),
                      planObject->getDoubleAttribute(SUMO_ATTR_SPEED));
    } else if (tagProperty.isPlanStopContainer()) {
        // set parameters
        int parameterSet = 0;
        if (planObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            parameterSet |= STOP_DURATION_SET;
        }
        if (planObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            parameterSet |= STOP_UNTIL_SET;
        }
        buildContainerStop(planObject, edgeID, containerStopID,
                           planObject->getDoubleAttribute(SUMO_ATTR_ENDPOS),
                           planObject->getTimeAttribute(SUMO_ATTR_DURATION),
                           planObject->getTimeAttribute(SUMO_ATTR_UNTIL),
                           planObject->getStringAttribute(SUMO_ATTR_ACTTYPE),
                           planObject->getBoolAttribute(SUMO_ATTR_FRIENDLY_POS),
                           parameterSet);
    } else {
        throw ProcessError("Invalid plan for duplicating");
    }
}


bool
GNERouteHandler::isVehicleIdDuplicated(const std::string& id) {
    // declare vehicle tags vector
    const std::vector<SumoXMLTag> vehicleTags = {SUMO_TAG_VEHICLE, GNE_TAG_VEHICLE_WITHROUTE, SUMO_TAG_TRIP, GNE_TAG_TRIP_TAZS,
                                                 GNE_TAG_FLOW_ROUTE, GNE_TAG_FLOW_WITHROUTE, SUMO_TAG_FLOW, SUMO_TAG_FLOW, SUMO_TAG_FLOW, GNE_TAG_FLOW_JUNCTIONS
                                                };
    for (const auto& vehicleTag : vehicleTags) {
        if (!checkDuplicatedDemandElement(vehicleTag, id)) {
            writeError(TLF("There is another % with the same ID='%'.", toString(vehicleTag), id));
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isViaAttributeValid(const std::vector<std::string>& via) {
    for (const auto& edgeID : via) {
        if (myNet->getAttributeCarriers()->retrieveEdge(edgeID, false) == nullptr) {
            writeError(TLF("Via edge '%' doesn't exist.", edgeID));
            return false;
        }
    }
    return true;
}


bool
GNERouteHandler::isPersonIdDuplicated(const std::string& id) {
    for (SumoXMLTag personTag : std::vector<SumoXMLTag>({SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW})) {
        if (!checkDuplicatedDemandElement(personTag, id)) {
            writeError(TLF("There is another % with the same ID='%'.", toString(personTag), id));
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isContainerIdDuplicated(const std::string& id) {
    for (SumoXMLTag containerTag : std::vector<SumoXMLTag>({SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW})) {
        if (!checkDuplicatedDemandElement(containerTag, id)) {
            writeError(TLF("There is another % with the same ID='%'.", toString(containerTag), id));
            return true;
        }
    }
    return false;
}


void
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // make a copy of the vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // obtain vClass
    const auto vClass = originalVehicle->getVClass();
    // set "yellow" as original route color
    RGBColor routeColor = RGBColor::YELLOW;
    // declare edges
    std::vector<GNEEdge*> routeEdges;
    // obtain edges depending of tag
    if (originalVehicle->getTagProperty().vehicleRoute()) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().at(1)->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if (originalVehicle->getTagProperty().vehicleRouteEmbedded()) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleEdges()) {
        // calculate path using from-via-to edges
        routeEdges = originalVehicle->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(originalVehicle->getVClass(), originalVehicle->getParentEdges());
    }
    // declare edge IDs
    std::vector<std::string> edgeIDs;
    for (const auto& edge : routeEdges) {
        edgeIDs.push_back(edge->getID());
    }
    // only continue if edges are valid
    if (routeEdges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if new vehicle must have an embedded route
        if (createEmbeddedRoute) {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_VEHICLE_WITHROUTE;
            // create a flow with embebbed routes
            CommonXMLStructure::SumoBaseObject* vehicleBaseOBject = new CommonXMLStructure::SumoBaseObject(nullptr);
            CommonXMLStructure::SumoBaseObject* routeBaseOBject = new CommonXMLStructure::SumoBaseObject(vehicleBaseOBject);
            // fill parameters
            vehicleBaseOBject->setTag(SUMO_TAG_VEHICLE);
            vehicleBaseOBject->addStringAttribute(SUMO_ATTR_ID, vehicleParameters.id);
            vehicleBaseOBject->setVehicleParameter(&vehicleParameters);
            // build embedded route
            routeHandler.buildEmbeddedRoute(routeBaseOBject, edgeIDs, RGBColor::INVISIBLE, false, 0, {});
            // delete vehicle base object
            delete vehicleBaseOBject;
        } else {
            // change tag in vehicle parameters
            vehicleParameters.tag = SUMO_TAG_VEHICLE;
            // generate route ID
            const std::string routeID = net->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE);
            // build route
            routeHandler.buildRoute(nullptr, routeID, vClass, edgeIDs, routeColor, false, 0, 1.0, {});
            // set route ID in vehicle parameters
            vehicleParameters.routeid = routeID;
            // create vehicle
            routeHandler.buildVehicleOverRoute(nullptr, vehicleParameters);
        }
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToRouteFlow(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // obtain vClass
    const auto vClass = originalVehicle->getVClass();
    // set "yellow" as original route color
    RGBColor routeColor = RGBColor::YELLOW;
    // declare edges
    std::vector<GNEEdge*> routeEdges;
    // obtain edges depending of tag
    if (originalVehicle->getTagProperty().vehicleRoute()) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().back()->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if (originalVehicle->getTagProperty().vehicleRouteEmbedded()) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleEdges()) {
        // calculate path using from-via-to edges
        routeEdges = originalVehicle->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(originalVehicle->getVClass(), originalVehicle->getParentEdges());
    }
    // declare edge IDs
    std::vector<std::string> edgeIDs;
    for (const auto& edge : routeEdges) {
        edgeIDs.push_back(edge->getID());
    }
    // only continue if edges are valid
    if (routeEdges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // change depart
        if (!GNEAttributeCarrier::getTagProperty(vehicleParameters.tag).isFlow()) {
            // get template flow
            const auto templateFlow = net->getViewNet()->getViewParent()->getVehicleFrame()->getVehicleTagSelector()->getTemplateAC(GNE_TAG_FLOW_ROUTE);
            // set flow parameters
            vehicleParameters.repetitionEnd = vehicleParameters.depart + string2time("3600");
            vehicleParameters.repetitionNumber = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(SUMO_ATTR_NUMBER));
            vehicleParameters.repetitionOffset = string2time(templateFlow->getAttribute(SUMO_ATTR_PERIOD));
            vehicleParameters.repetitionProbability = GNEAttributeCarrier::parse<double>(templateFlow->getAttribute(SUMO_ATTR_PROB));
            // by default, number and end enabled
            vehicleParameters.parametersSet = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(GNE_ATTR_FLOWPARAMETERS));
        }
        // check if new vehicle must have an embedded route
        if (createEmbeddedRoute) {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_FLOW_WITHROUTE;
            // create a flow with embebbed routes
            CommonXMLStructure::SumoBaseObject* vehicleBaseOBject = new CommonXMLStructure::SumoBaseObject(nullptr);
            CommonXMLStructure::SumoBaseObject* routeBaseOBject = new CommonXMLStructure::SumoBaseObject(vehicleBaseOBject);
            // fill parameters
            vehicleBaseOBject->setTag(SUMO_TAG_FLOW);
            vehicleBaseOBject->addStringAttribute(SUMO_ATTR_ID, vehicleParameters.id);
            vehicleBaseOBject->setVehicleParameter(&vehicleParameters);
            // build embedded route
            routeHandler.buildEmbeddedRoute(routeBaseOBject, edgeIDs, RGBColor::INVISIBLE, false, 0, {});
            // delete vehicle base object
            delete vehicleBaseOBject;
        } else {
            // change tag in vehicle parameters
            vehicleParameters.tag = GNE_TAG_FLOW_ROUTE;
            // generate a new route id
            const std::string routeID = net->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE);
            // build route
            routeHandler.buildRoute(nullptr, routeID, vClass, edgeIDs, routeColor, false, 0, 1.0, {});
            // set route ID in vehicle parameters
            vehicleParameters.routeid = routeID;
            // create vehicle
            routeHandler.buildFlowOverRoute(nullptr, vehicleParameters);
        }
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToTrip(GNEVehicle* originalVehicle) {
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // get route
    GNEDemandElement* route = nullptr;
    // declare edges
    std::vector<GNEEdge*> edges;
    // obtain edges depending of tag
    if (originalVehicle->getTagProperty().vehicleRoute()) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleRouteEmbedded()) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleEdges()) {
        // just take parent edges (from and to)
        edges = originalVehicle->getParentEdges();
    }
    // only continue if edges are valid
    if (edges.size() < 2) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_TRIP));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if route has to be deleted
        if (route && route->getChildDemandElements().empty()) {
            net->deleteDemandElement(route, net->getViewNet()->getUndoList());
        }
        // change tag in vehicle parameters
        vehicleParameters.tag = SUMO_TAG_TRIP;
        // create trip
        routeHandler.buildTrip(nullptr, vehicleParameters, edges.front()->getID(), edges.back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToFlow(GNEVehicle* originalVehicle) {
    // get pointer to net
    GNENet* net = originalVehicle->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain vehicle parameters
    SUMOVehicleParameter vehicleParameters = *originalVehicle;
    // declare route
    GNEDemandElement* route = nullptr;
    // declare edges
    std::vector<GNEEdge*> edges;
    // obtain edges depending of tag
    if (originalVehicle->getTagProperty().vehicleRoute()) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleRouteEmbedded()) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if (originalVehicle->getTagProperty().vehicleEdges()) {
        // just take parent edges (from and to)
        edges = originalVehicle->getParentEdges();
    }
    // only continue if edges are valid
    if (edges.empty()) {
        // declare header
        const std::string header = "Problem transforming to vehicle";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of edges";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if route has to be deleted
        if (route && route->getChildDemandElements().empty()) {
            net->deleteDemandElement(route, net->getViewNet()->getUndoList());
        }
        // change depart
        if (!GNEAttributeCarrier::getTagProperty(vehicleParameters.tag).isFlow()) {
            // get template flow
            const auto templateFlow = net->getViewNet()->getViewParent()->getVehicleFrame()->getVehicleTagSelector()->getTemplateAC(GNE_TAG_FLOW_ROUTE);
            // set flow parameters
            vehicleParameters.repetitionEnd = vehicleParameters.depart + string2time("3600");
            vehicleParameters.repetitionNumber = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(SUMO_ATTR_NUMBER));
            vehicleParameters.repetitionOffset = string2time(templateFlow->getAttribute(SUMO_ATTR_PERIOD));
            vehicleParameters.repetitionProbability = GNEAttributeCarrier::parse<double>(templateFlow->getAttribute(SUMO_ATTR_PROB));
            // by default, number and end enabled
            vehicleParameters.parametersSet = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(GNE_ATTR_FLOWPARAMETERS));
        }
        // change tag in vehicle parameters
        vehicleParameters.tag = SUMO_TAG_FLOW;
        // create flow
        routeHandler.buildFlow(nullptr, vehicleParameters, edges.front()->getID(), edges.back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToTripJunctions(GNEVehicle* originalVehicle) {
    // only continue if number of junctions are valid
    if (originalVehicle->getParentJunctions().empty()) {
        // declare header
        const std::string header = "Problem transforming to trip over junctions";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of junctions";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // get pointer to net
        GNENet* net = originalVehicle->getNet();
        // check if transform after creation
        const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
        // declare route handler
        GNERouteHandler routeHandler("", net, true, false);
        // obtain vehicle parameters
        SUMOVehicleParameter vehicleParameters = *originalVehicle;
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(GNE_TAG_TRIP_JUNCTIONS));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // change tag in vehicle parameters
        vehicleParameters.tag = GNE_TAG_TRIP_JUNCTIONS;
        // create trip
        routeHandler.buildTripJunctions(nullptr, vehicleParameters, originalVehicle->getParentJunctions().front()->getID(), originalVehicle->getParentJunctions().back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToFlowJunctions(GNEVehicle* originalVehicle) {
    // only continue if number of junctions are valid
    if (originalVehicle->getParentJunctions().empty()) {
        // declare header
        const std::string header = "Problem transforming to flow over junctions";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of junctions";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // get pointer to net
        GNENet* net = originalVehicle->getNet();
        // check if transform after creation
        const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
        // declare route handler
        GNERouteHandler routeHandler("", net, true, false);
        // obtain vehicle parameters
        SUMOVehicleParameter vehicleParameters = *originalVehicle;
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(GNE_TAG_FLOW_JUNCTIONS));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // get template flow
        const auto templateFlow = net->getViewNet()->getViewParent()->getVehicleFrame()->getVehicleTagSelector()->getTemplateAC(GNE_TAG_FLOW_JUNCTIONS);
        // set flow parameters
        vehicleParameters.repetitionEnd = vehicleParameters.depart + string2time("3600");
        vehicleParameters.repetitionNumber = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(SUMO_ATTR_NUMBER));
        vehicleParameters.repetitionOffset = string2time(templateFlow->getAttribute(SUMO_ATTR_PERIOD));
        vehicleParameters.repetitionProbability = GNEAttributeCarrier::parse<double>(templateFlow->getAttribute(SUMO_ATTR_PROB));
        // by default, number and end enabled
        vehicleParameters.parametersSet = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(GNE_ATTR_FLOWPARAMETERS));
        // change tag in vehicle parameters
        vehicleParameters.tag = GNE_TAG_FLOW_JUNCTIONS;
        // create flow
        routeHandler.buildFlowJunctions(nullptr, vehicleParameters, originalVehicle->getParentJunctions().front()->getID(), originalVehicle->getParentJunctions().back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToTripTAZs(GNEVehicle* originalVehicle) {
    // only continue if number of junctions are valid
    if (originalVehicle->getParentAdditionals().empty()) {
        // declare header
        const std::string header = "Problem transforming to trip over TAZs";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of TAZs";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // get pointer to net
        GNENet* net = originalVehicle->getNet();
        // check if transform after creation
        const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
        // declare route handler
        GNERouteHandler routeHandler("", net, true, false);
        // obtain vehicle parameters
        SUMOVehicleParameter vehicleParameters = *originalVehicle;
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(GNE_TAG_TRIP_TAZS));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // change tag in vehicle parameters
        vehicleParameters.tag = GNE_TAG_TRIP_TAZS;
        // create trip
        routeHandler.buildTripTAZs(nullptr, vehicleParameters, originalVehicle->getParentAdditionals().front()->getID(), originalVehicle->getParentAdditionals().back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToFlowTAZs(GNEVehicle* originalVehicle) {
    // only continue if number of junctions are valid
    if (originalVehicle->getParentAdditionals().empty()) {
        // declare header
        const std::string header = "Problem transforming to flow over TAZs";
        // declare message
        const std::string message = "Vehicle cannot be transformed. Invalid number of TAZs";
        // write warning
        WRITE_DEBUG("Opened FXMessageBox " + header);
        // open message box
        FXMessageBox::warning(originalVehicle->getNet()->getViewNet()->getApp(), MBOX_OK, header.c_str(), "%s", message.c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox " + header);
    } else {
        // get pointer to net
        GNENet* net = originalVehicle->getNet();
        // check if transform after creation
        const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalVehicle);
        // declare route handler
        GNERouteHandler routeHandler("", net, true, false);
        // obtain vehicle parameters
        SUMOVehicleParameter vehicleParameters = *originalVehicle;
        // begin undo-redo operation
        net->getViewNet()->getUndoList()->begin(originalVehicle, "transform " + originalVehicle->getTagStr() + " to " + toString(GNE_TAG_FLOW_TAZS));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // get template flow
        const auto templateFlow = net->getViewNet()->getViewParent()->getVehicleFrame()->getVehicleTagSelector()->getTemplateAC(GNE_TAG_FLOW_TAZS);
        // set flow parameters
        vehicleParameters.repetitionEnd = vehicleParameters.depart + string2time("3600");
        vehicleParameters.repetitionNumber = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(SUMO_ATTR_NUMBER));
        vehicleParameters.repetitionOffset = string2time(templateFlow->getAttribute(SUMO_ATTR_PERIOD));
        vehicleParameters.repetitionProbability = GNEAttributeCarrier::parse<double>(templateFlow->getAttribute(SUMO_ATTR_PROB));
        // by default, number and end enabled
        vehicleParameters.parametersSet = GNEAttributeCarrier::parse<int>(templateFlow->getAttribute(GNE_ATTR_FLOWPARAMETERS));
        // change tag in vehicle parameters
        vehicleParameters.tag = GNE_TAG_FLOW_TAZS;
        // create flow
        routeHandler.buildFlowTAZs(nullptr, vehicleParameters, originalVehicle->getParentAdditionals().front()->getID(), originalVehicle->getParentAdditionals().back()->getID());
        // end undo-redo operation
        net->getViewNet()->getUndoList()->end();
        // check if inspect
        if (inspectAfterTransform) {
            // get created element
            auto transformedVehicle = net->getAttributeCarriers()->retrieveDemandElement(vehicleParameters.tag, vehicleParameters.id);
            // inspect it
            net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedVehicle);
        }
    }
}


void
GNERouteHandler::transformToPerson(GNEPerson* originalPerson) {
    // get pointer to net
    GNENet* net = originalPerson->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalPerson);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain person parameters
    SUMOVehicleParameter personParameters = *originalPerson;
    // save ID
    const auto ID = personParameters.id;
    // set dummy ID
    personParameters.id = "%dummyID%";
    // begin undo-redo operation
    net->getViewNet()->getUndoList()->begin(originalPerson, "transform " + originalPerson->getTagStr() + " to " + toString(SUMO_TAG_PERSON));
    // create personFlow and get it
    routeHandler.buildPerson(nullptr, personParameters);
    auto newPerson = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSON, "%dummyID%");
    // duplicate plans in new person
    for (const auto& personPlan : originalPerson->getChildDemandElements()) {
        routeHandler.duplicatePlan(personPlan, newPerson);
    }
    // delete original person plan
    net->deleteDemandElement(originalPerson, net->getViewNet()->getUndoList());
    // restore ID of new person plan
    newPerson->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(newPerson);
    }
}


void
GNERouteHandler::transformToPersonFlow(GNEPerson* originalPerson) {
    // get pointer to net
    GNENet* net = originalPerson->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalPerson);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain person parameters
    SUMOVehicleParameter personParameters = *originalPerson;
    // get person plans
    const auto personPlans = originalPerson->getChildDemandElements();
    // save ID
    const auto ID = personParameters.id;
    // set dummy ID
    personParameters.id = "%dummyID%";
    // begin undo-redo operation
    net->getViewNet()->getUndoList()->begin(originalPerson, "transform " + originalPerson->getTagStr() + " to " + toString(SUMO_TAG_PERSONFLOW));
    // create personFlow and get it
    routeHandler.buildPersonFlow(nullptr, personParameters);
    auto newPerson = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSONFLOW, "%dummyID%");
    // move all person plans to new person
    for (const auto& personPlan : personPlans) {
        routeHandler.duplicatePlan(personPlan, newPerson);
    }
    // delete original person plan
    net->deleteDemandElement(originalPerson, net->getViewNet()->getUndoList());
    // restore ID of new person plan
    newPerson->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // enable attributes
    newPerson->enableAttribute(SUMO_ATTR_END, net->getViewNet()->getUndoList());
    newPerson->enableAttribute(SUMO_ATTR_PERSONSPERHOUR, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(newPerson);
    }
}


void
GNERouteHandler::transformToContainer(GNEContainer* originalContainer) {
    // get pointer to net
    GNENet* net = originalContainer->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalContainer);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain container parameters
    SUMOVehicleParameter containerParameters = *originalContainer;
    // get container plans
    const auto containerPlans = originalContainer->getChildDemandElements();
    // save ID
    const auto ID = containerParameters.id;
    // set dummy ID
    containerParameters.id = "%dummyID%";
    // begin undo-redo operation
    net->getViewNet()->getUndoList()->begin(originalContainer, "transform " + originalContainer->getTagStr() + " to " + toString(SUMO_TAG_CONTAINER));
    // create containerFlow
    routeHandler.buildContainer(nullptr, containerParameters);
    // move all container plans to new container
    for (const auto& containerPlan : containerPlans) {
        containerPlan->setAttribute(GNE_ATTR_PARENT, "%dummyID%", net->getViewNet()->getUndoList());
    }
    // delete original container plan
    net->deleteDemandElement(originalContainer, net->getViewNet()->getUndoList());
    // restore ID of new container plan
    auto newContainer = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINER, "%dummyID%");
    newContainer->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(newContainer);
    }
}


void
GNERouteHandler::transformToContainerFlow(GNEContainer* originalContainer) {
    // get pointer to net
    GNENet* net = originalContainer->getNet();
    // check if transform after creation
    const bool inspectAfterTransform = net->getViewNet()->isAttributeCarrierInspected(originalContainer);
    // declare route handler
    GNERouteHandler routeHandler("", net, true, false);
    // obtain container parameters
    SUMOVehicleParameter containerParameters = *originalContainer;
    // get container plans
    const auto containerPlans = originalContainer->getChildDemandElements();
    // save ID
    const auto ID = containerParameters.id;
    // set dummy ID
    containerParameters.id = "%dummyID%";
    // begin undo-redo operation
    net->getViewNet()->getUndoList()->begin(originalContainer, "transform " + originalContainer->getTagStr() + " to " + toString(SUMO_TAG_CONTAINERFLOW));
    // create containerFlow
    routeHandler.buildContainerFlow(nullptr, containerParameters);
    // move all container plans to new container
    for (const auto& containerPlan : containerPlans) {
        containerPlan->setAttribute(GNE_ATTR_PARENT, "%dummyID%", net->getViewNet()->getUndoList());
    }
    // delete original container plan
    net->deleteDemandElement(originalContainer, net->getViewNet()->getUndoList());
    // restore ID of new container plan
    auto newContainer = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINERFLOW, "%dummyID%");
    newContainer->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // enable attributes
    newContainer->enableAttribute(SUMO_ATTR_END, net->getViewNet()->getUndoList());
    newContainer->enableAttribute(SUMO_ATTR_CONTAINERSPERHOUR, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(newContainer);
    }
}


bool
GNERouteHandler::canReverse(const GNEDemandElement* element) {
    // continue depending of element
    if (element->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
        return canReverse(element->getNet(), SVC_PEDESTRIAN, element->getParentEdges());
    } else if (element->getTagProperty().vehicleRoute()) {
        return canReverse(element->getNet(), element->getVClass(), element->getParentDemandElements().at(1)->getParentEdges());
    } else if (element->getTagProperty().vehicleRouteEmbedded()) {
        return canReverse(element->getNet(), element->getVClass(), element->getChildDemandElements().front()->getParentEdges());
    } else if (element->getTagProperty().vehicleEdges()) {
        return canReverse(element->getNet(), element->getVClass(), element->getParentEdges());
    } else if (element->getTagProperty().vehicleJunctions()) {
        return (element->getNet()->getPathManager()->getPathCalculator()->calculateDijkstraPath(element->getVClass(),
                element->getParentJunctions().back(), element->getParentJunctions().front()).size() > 0);
    } else if (element->getTagProperty().vehicleTAZs()) {
        return true;
    } else {
        return false;
    }
}


bool
GNERouteHandler::canReverse(GNENet* net, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges) {
    if (edges.empty()) {
        return false;
    } else {
        // obtain opposite edges
        std::vector<GNEEdge*> reverseEdges;
        for (const auto& edge : edges) {
            const auto oppositeEdges = edge->getOppositeEdges();
            // stop if there isn't opposite edges for the current edge
            if (oppositeEdges.empty()) {
                return false;
            } else {
                reverseEdges.push_back(oppositeEdges.front());
            }
        }
        // reverse edges
        std::reverse(reverseEdges.begin(), reverseEdges.end());
        // now check if exist a path
        return (net->getPathManager()->getPathCalculator()->calculateDijkstraPath(vClass, edges).size() > 0);
    }
}


void
GNERouteHandler::reverse(GNEDemandElement* element) {
    // get undo list
    auto undoList = element->getNet()->getViewNet()->getUndoList();
    // continue depending of element
    if (element->getTagProperty().vehicleRoute()) {
        // reverse parent route
        reverse(element->getParentDemandElements().at(1));
    } else if (element->getTagProperty().vehicleRouteEmbedded()) {
        // reverse embedded route
        reverse(element->getChildDemandElements().front());
    } else if (element->getTagProperty().vehicleJunctions()) {
        // get from to junctions
        const auto fromJunction = element->getAttribute(SUMO_ATTR_FROM_JUNCTION);
        const auto toJunction = element->getAttribute(SUMO_ATTR_TO_JUNCTION);
        // swap both attributes
        element->setAttribute(SUMO_ATTR_FROM_JUNCTION, toJunction, undoList);
        element->setAttribute(SUMO_ATTR_TO_JUNCTION, fromJunction, undoList);
    } else if (element->getTagProperty().vehicleTAZs()) {
        // get from to TAZs
        const auto fromTAZ = element->getAttribute(SUMO_ATTR_FROM_TAZ);
        const auto toTAZ = element->getAttribute(SUMO_ATTR_TO_TAZ);
        // swap both attributes
        element->setAttribute(SUMO_ATTR_FROM_TAZ, toTAZ, undoList);
        element->setAttribute(SUMO_ATTR_TO_TAZ, fromTAZ, undoList);
    } else {
        // extract and reverse opposite edges
        std::vector<GNEEdge*> oppositeEdges;
        for (const auto& edge : element->getParentEdges()) {
            oppositeEdges.push_back(edge->getOppositeEdges().front());
        }
        std::reverse(oppositeEdges.begin(), oppositeEdges.end());
        if (element->isRoute()) {
            element->setAttribute(SUMO_ATTR_EDGES, GNEAttributeCarrier::parseIDs(oppositeEdges), undoList);
        } else {
            // set from and to
            element->setAttribute(SUMO_ATTR_FROM, oppositeEdges.front()->getID(), undoList);
            element->setAttribute(SUMO_ATTR_TO, oppositeEdges.back()->getID(), undoList);
            // check if add via attribute
            oppositeEdges.erase(oppositeEdges.begin());
            oppositeEdges.pop_back();
            if (oppositeEdges.size() > 0) {
                element->setAttribute(SUMO_ATTR_VIA, GNEAttributeCarrier::parseIDs(oppositeEdges), undoList);
            }
        }
    }
}


void
GNERouteHandler::addReverse(GNEDemandElement* element) {
    GNEDemandElement* elementCopy = nullptr;
    if (element->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
        // make a copy of the route and reverse
        elementCopy = GNERoute::copyRoute(dynamic_cast<GNERoute*>(element));
    } else if (element->getTagProperty().isVehicle()) {
        // make a copy of the vehicle
        elementCopy = GNEVehicle::copyVehicle(dynamic_cast<GNEVehicle*>(element));
    }
    // reverse copied element
    reverse(elementCopy);
}

// ===========================================================================
// protected
// ===========================================================================

GNEJunction*
GNERouteHandler::parseJunction(const SumoXMLTag tag, const std::string& junctionID) {
    GNEJunction* junction = myNet->getAttributeCarriers()->retrieveJunction(junctionID, false);
    // empty junctions aren't allowed. If junction is empty, write error, clear junctions and stop
    if (junction == nullptr) {
        writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TLF("% doesn't exist.", toString(SUMO_TAG_JUNCTION)));
    }
    return junction;
}


GNEAdditional*
GNERouteHandler::parseTAZ(const SumoXMLTag tag, const std::string& TAZID) {
    GNEAdditional* TAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, TAZID, false);
    // empty TAZs aren't allowed. If TAZ is empty, write error, clear TAZs and stop
    if (TAZ == nullptr) {
        writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TLF("% doesn't exist.", toString(SUMO_TAG_TAZ)));
    }
    return TAZ;
}


GNEEdge*
GNERouteHandler::parseEdge(const SumoXMLTag tag, const std::string& edgeID) {
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
    if (edge == nullptr) {
        writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TLF("% doesn't exist.", toString(SUMO_TAG_EDGE)));
    }
    return edge;
}


std::vector<GNEEdge*>
GNERouteHandler::parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs) {
    std::vector<GNEEdge*> edges;
    for (const auto& edgeID : edgeIDs) {
        GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
        // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
        if (edge == nullptr) {
            writeError(TLF("Could not build % in netedit", toString(tag)) + std::string("; ") + TLF("% doesn't exist.", toString(SUMO_TAG_EDGE)));
            edges.clear();
            return edges;
        } else {
            edges.push_back(edge);
        }
    }
    return edges;
}


GNEDemandElement*
GNERouteHandler::getType(const std::string& id) const {
    GNEDemandElement* type = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, id, false);
    if (type == nullptr) {
        return myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, id, false);
    } else {
        return type;
    }
}


GNEDemandElement*
GNERouteHandler::getPersonParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // check that sumoBaseObject has parent
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        return nullptr;
    }
    if ((sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_PERSON) &&
            (sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_PERSONFLOW)) {
        return nullptr;
    }
    // try it with person
    GNEDemandElement* personParent = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSON, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    // if empty, try it with personFlow
    if (personParent == nullptr) {
        return myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSONFLOW, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    } else {
        return personParent;
    }
}


GNEDemandElement*
GNERouteHandler::getContainerParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // check that sumoBaseObject has parent
    if (sumoBaseObject->getParentSumoBaseObject() == nullptr) {
        return nullptr;
    }
    if ((sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_CONTAINER) &&
            (sumoBaseObject->getParentSumoBaseObject()->getTag() != SUMO_TAG_CONTAINERFLOW)) {
        return nullptr;
    }
    // try it with container
    GNEDemandElement* containerParent = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINER, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    // if empty, try it with containerFlow
    if (containerParent == nullptr) {
        return myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_CONTAINERFLOW, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    } else {
        return containerParent;
    }
}


const CommonXMLStructure::SumoBaseObject*
GNERouteHandler::getPreviousPlanObj(const CommonXMLStructure::SumoBaseObject* obj) const {
    // get parent object
    const CommonXMLStructure::SumoBaseObject* parentObject = obj->getParentSumoBaseObject();
    // check if object has parent
    if (parentObject == nullptr) {
        return nullptr;
    }
    // check number of children
    if (parentObject->getSumoBaseObjectChildren().size() < 2) {
        return nullptr;
    }
    // search position of the given plan obj in the parent children
    const auto itObj = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), obj);
    // if itObj is the first element, return null
    if (itObj == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    }
    // get pevious itObj
    return *(itObj - 1);
}


GNEEdge*
GNERouteHandler::getPreviousPlanEdge(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to edge
    if (previousPlanObj == nullptr) {
        return nullptr;
    } else {
        // continue depending of attributes
        if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO)) {
            return myNet->getAttributeCarriers()->retrieveEdge(previousPlanObj->getStringAttribute(SUMO_ATTR_TO), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_EDGE)) {
            return myNet->getAttributeCarriers()->retrieveEdge(previousPlanObj->getStringAttribute(SUMO_ATTR_EDGE), false);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_BUS_STOP)) {
            // busStop
            const auto busStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
            if (busStop) {
                return busStop->getParentLanes().front()->getParentEdge();
            }
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TRAIN_STOP)) {
            // trainStop
            const auto trainStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP), false);
            if (trainStop) {
                return trainStop->getParentLanes().front()->getParentEdge();
            }
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP)) {
            // containerStop
            const auto containerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP), false);
            if (containerStop) {
                return containerStop->getParentLanes().front()->getParentEdge();
            }
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_EDGES)) {
            const auto edges = StringTokenizer(previousPlanObj->getStringAttribute(SUMO_ATTR_EDGE)).getVector();
            if (edges.size() > 0) {
                return myNet->getAttributeCarriers()->retrieveEdge(edges.back(), false);
            }
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
            auto route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, previousPlanObj->getStringAttribute(SUMO_ATTR_ROUTE), false);
            if (route) {
                return route->getParentEdges().back();
            }
        }
        // no previous edge
        return nullptr;
    }
}


GNEAdditional*
GNERouteHandler::getPreviousPlanTAZ(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to TAZ
    if (previousPlanObj && previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_TAZ)) {
        return myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, previousPlanObj->getStringAttribute(SUMO_ATTR_TO_TAZ), false);
    } else {
        return nullptr;
    }
}


GNEJunction*
GNERouteHandler::getPreviousPlanJunction(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to junction
    if (previousPlanObj && previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_JUNCTION)) {
        return myNet->getAttributeCarriers()->retrieveJunction(previousPlanObj->getStringAttribute(SUMO_ATTR_TO_JUNCTION), false);
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNERouteHandler::getPreviousPlanBusStop(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to busStop
    if (previousPlanObj && previousPlanObj->hasStringAttribute(SUMO_ATTR_BUS_STOP)) {
        return myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNERouteHandler::getPreviousPlanTrainStop(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to trainStop
    if (previousPlanObj && previousPlanObj->hasStringAttribute(SUMO_ATTR_TRAIN_STOP)) {
        return myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRAIN_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP), false);
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNERouteHandler::getPreviousPlanContainerStop(const CommonXMLStructure::SumoBaseObject* obj) const {
    const auto previousPlanObj = getPreviousPlanObj(obj);
    // check if previous plan object has to containerStop
    if (previousPlanObj && previousPlanObj->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP)) {
        return myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP), false);
    } else {
        return nullptr;
    }
}


bool
GNERouteHandler::getDistributionElements(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SumoXMLTag distributionElementTag,
        const std::vector<std::string>& distributionElementIDs, const std::vector<double>& probabilities,
        std::vector<const GNEDemandElement*>& elements) {
    // get distribution tag and ID
    std::string distributionTag = toString(sumoBaseObject->getTag());
    std::string distributionID = sumoBaseObject->getStringAttribute(SUMO_ATTR_ID);
    // first parse vType IDs
    for (const auto& distributionElementID : distributionElementIDs) {
        auto distributionElement = myNet->getAttributeCarriers()->retrieveDemandElement(distributionElementTag, distributionElementID, false);
        if (distributionElement) {
            elements.push_back(distributionElement);
        } else {
            writeError(TLF("% with id '%' doesn't exist in % '%'", toString(distributionElementTag), distributionElementID, distributionTag, distributionID));
            return false;

        }
    }
    // check probabilities
    for (const auto& probability : probabilities) {
        if (probability < 0) {
            writeError(TLF("invalid probability % in % '%'", toString(probability), distributionTag, distributionID));
            return false;
        }
    }
    // check that number of elements and probabilities is the same
    if (elements.size() != probabilities.size()) {
        writeError(TLF("Invalid type distribution probabilities in % '%'. Must have the same number of elements", distributionTag, distributionID));
        return false;
    } else {
        return true;
    }
}


bool
GNERouteHandler::checkDuplicatedDemandElement(const SumoXMLTag tag, const std::string& id) {
    // retrieve demand element
    auto demandElement = myNet->getAttributeCarriers()->retrieveDemandElement(tag, id, false);
    // if demand exist, check if overwrite (delete)
    if (demandElement) {
        if (!myAllowUndoRedo) {
            // only overwrite if allow undo-redo
            return false;
        } else if (myOverwrite) {
            // update demand to overwrite
            myDemandToOverwrite = demandElement;
            return true;
        } else {
            // duplicated demand
            return false;
        }
    } else {
        // demand with these id doesn't exist, then all ok
        return true;
    }
}


void
GNERouteHandler::overwriteDemandElement() {
    if (myDemandToOverwrite) {
        // remove element
        myNet->deleteDemandElement(myDemandToOverwrite, myNet->getViewNet()->getUndoList());
        // reset pointer
        myDemandToOverwrite = nullptr;
    }
}

/****************************************************************************/
