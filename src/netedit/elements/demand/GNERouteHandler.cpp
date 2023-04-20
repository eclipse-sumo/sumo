/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <config.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/common/GNEInspectorFrame.h>

#include "GNEContainer.h"
#include "GNEPerson.h"
#include "GNEPersonTrip.h"
#include "GNERide.h"
#include "GNERoute.h"
#include "GNERouteHandler.h"
#include "GNEStop.h"
#include "GNETranship.h"
#include "GNETransport.h"
#include "GNEVehicle.h"
#include "GNEVType.h"
#include "GNEVTypeDistribution.h"
#include "GNEWalk.h"


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
    // check vTypeDistribution
    const bool vTypeDistribution = sumoBaseObject->getParentSumoBaseObject() && (sumoBaseObject->getParentSumoBaseObject()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION);
    // check if loaded type is a default type
    if (DEFAULT_VTYPES.count(vTypeParameter.id) > 0) {
        // overwrite default vehicle type
        GNEVType::overwriteVType(myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vTypeParameter.id, false), vTypeParameter, myNet->getViewNet()->getUndoList());
    } else if (!checkDuplicatedDemandElement(SUMO_TAG_VTYPE, vTypeParameter.id)) {
        writeError(TL("There is another ") + toString(SUMO_TAG_VTYPE) + TL(" with the same ID='") + vTypeParameter.id + "'.");
    } else {
        // create vType/pType using myCurrentVType
        GNEDemandElement* vType = new GNEVType(myNet, vTypeParameter);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::VTYPE, TL("add ") + vType->getTagStr() + " '" + vTypeParameter.id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
            // check if place this vType within a vTypeDistribution
            if (vTypeDistribution) {
                vType->setAttribute(GNE_ATTR_VTYPE_DISTRIBUTION, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), myNet->getViewNet()->getUndoList());
            }
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(vType);
            if (vTypeDistribution) {
                vType->setAttribute(GNE_ATTR_VTYPE_DISTRIBUTION, sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
            }
            vType->incRef("buildVType");
        }
    }
}


void
GNERouteHandler::buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id,
                                        const int deterministic, const std::vector<std::string>& vTypes) {
    // first check conditions
    if (!checkDuplicatedDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, id)) {
        writeError(TL("There is another ") + toString(SUMO_TAG_VTYPE) + TL(" with the same ID='") + id + "'.");
    } else if (vTypes.empty() && sumoBaseObject->getSumoBaseObjectChildren().empty()) {
        writeError(toString(SUMO_TAG_VTYPE_DISTRIBUTION) + TL(" need at least one ") + toString(SUMO_TAG_VTYPE));
    } else {
        bool checkVTypesOK = true;
        // check vTypes
        for (const auto& vType : vTypes) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vType, false) == nullptr) {
                writeError(toString(SUMO_TAG_VTYPE) + TL(" with id '") + vType + TL("' doesn't exist in ") + toString(SUMO_TAG_VTYPE_DISTRIBUTION) + " '" + id + "'");
                checkVTypesOK = false;
            }
        }
        // now check childrens
        for (const auto& child : sumoBaseObject->getSumoBaseObjectChildren()) {
            if (child->hasStringAttribute(SUMO_ATTR_ID) == false) {
                writeError(TL("Invalid definition for ") + toString(SUMO_TAG_VTYPE) + TL(" in ") + toString(SUMO_TAG_VTYPE_DISTRIBUTION) + " '" + id + "'");
                checkVTypesOK = false;
            } else if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, child->getStringAttribute(SUMO_ATTR_ID), false) != nullptr) {
                writeError(toString(SUMO_TAG_VTYPE) + TL(" with id '") + child->getStringAttribute(SUMO_ATTR_ID) + TL("' cannot be created in ") + toString(SUMO_TAG_VTYPE_DISTRIBUTION) + " '" + id + "'");
                checkVTypesOK = false;
            }
        }
        // if all ok, then create vTypeDistribution
        if (checkVTypesOK) {
            GNEVTypeDistribution* vTypeDistribution = new GNEVTypeDistribution(myNet, id, deterministic);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(GUIIcon::VTYPEDISTRIBUTION, TL("add ") + vTypeDistribution->getTagStr() + " '" + id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vTypeDistribution, true), true);
                // set this vTypeDistribution as parent of the other vTypes
                for (const auto& vTypeID : vTypes) {
                    auto vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vTypeID);
                    vType->setAttribute(GNE_ATTR_VTYPE_DISTRIBUTION, id, myNet->getViewNet()->getUndoList());
                }
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vTypeDistribution);
                vTypeDistribution->incRef("buildVType");
                for (const auto& vTypeID : vTypes) {
                    auto vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vTypeID);
                    vType->setAttribute(GNE_ATTR_VTYPE_DISTRIBUTION, id);
                }
            }
        }
    }
}


void
GNERouteHandler::buildRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& id, SUMOVehicleClass vClass,
                            const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                            const Parameterised::Map& routeParameters) {
    // parse edges
    const auto edges = parseEdges(SUMO_TAG_ROUTE, edgeIDs);
    // check conditions
    if (!checkDuplicatedDemandElement(SUMO_TAG_ROUTE, id)) {
        writeError(TL("There is another ") + toString(SUMO_TAG_ROUTE) + TL(" with the same ID='") + id + "'.");
    } else if (edges.size() > 0) {
        // create GNERoute
        GNEDemandElement* route = new GNERoute(myNet, id, vClass, edges, color, repeat, cycleTime, routeParameters);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::ROUTE, TL("add ") + route->getTagStr() + " '" + id + "'");
            overwriteDemandElement();
            myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            myNet->getAttributeCarriers()->insertDemandElement(route);
            for (const auto& edge : edges) {
                edge->addChildElement(route);
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
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)edges.front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(vehicleTag, myNet, vType, vehicleParameters);
            // create embedded route
            GNEDemandElement* route = new GNERoute(myNet, vehicle, edges, color, repeat, cycleTime, routeParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(GUIIcon::ROUTE, TL("add ") + route->getTagStr() + " in '" + vehicle->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                myNet->getAttributeCarriers()->insertDemandElement(route);
                vType->addChildElement(vehicle);
                vehicle->addChildElement(route);
                for (const auto& edge : edges) {
                    edge->addChildElement(route);
                }
                vehicle->incRef("buildEmbeddedRoute");
                route->incRef("buildEmbeddedRoute");
            }
            // compute path
            vehicle->computePathElement();
        }
    }
}


void
GNERouteHandler::buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string& /*id*/) {
    // unsupported
    writeError(TL("netedit doesn't support route distributions"));
}


void
GNERouteHandler::buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (route == nullptr) {
            writeError(TL("Invalid route '") + vehicleParameters.routeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create vehicle using vehicleParameters
            GNEDemandElement* vehicle = new GNEVehicle(SUMO_TAG_VEHICLE, myNet, vType, route, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(vehicle->getTagProperty().getGUIIcon(), TL("add ") + vehicle->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(vehicle);
                // set vehicle as child of vType and Route
                vType->addChildElement(vehicle);
                route->addChildElement(vehicle);
                vehicle->incRef("buildVehicleOverRoute");
            }
            // compute path
            vehicle->computePathElement();
        }
    }
}


void
GNERouteHandler::buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& vehicleParameters) {
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (route == nullptr) {
            writeError(TL("Invalid route '") + vehicleParameters.routeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + TL(" is greater than number of lanes"));
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + TL(" is greater than vType") + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create flow or trips using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_ROUTE, myNet, vType, route, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow->getTagProperty().getGUIIcon(), TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set flow as child of vType and Route
                vType->addChildElement(flow);
                route->addChildElement(flow);
                flow->incRef("buildFlowOverRoute");
            }
            // compute path
            flow->computePathElement();
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
        // obtain  vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create trip or flow using tripParameters
            GNEDemandElement* trip = new GNEVehicle(SUMO_TAG_TRIP, myNet, vType, fromEdge, toEdge, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(trip->getTagProperty().getGUIIcon(), TL("add ") + trip->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(trip);
                // set vehicle as child of vType
                vType->addChildElement(trip);
                trip->incRef("buildTrip");
                // add reference in all edges
                fromEdge->addChildElement(trip);
                toEdge->addChildElement(trip);
            }
            // compute path
            trip->computePathElement();
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
        // obtain  vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create trip using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_TRIP_JUNCTIONS, myNet, vType, fromJunction, toJunction, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow->getTagProperty().getGUIIcon(), TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all junctions
                fromJunction->addChildElement(flow);
                toJunction->addChildElement(flow);
            }
            // compute path
            flow->computePathElement();
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
        // obtain  vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create trip or flow using tripParameters
            GNEDemandElement* flow = new GNEVehicle(SUMO_TAG_FLOW, myNet, vType, fromEdge, toEdge, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow->getTagProperty().getGUIIcon(), TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all edges
                fromEdge->addChildElement(flow);
                toEdge->addChildElement(flow);
            }
            // compute path
            flow->computePathElement();
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
        // obtain  vtypes
        GNEDemandElement* vType = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, vehicleParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid vehicle type '") + vehicleParameters.vtypeid + TL("' used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
            }
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DepartLaneDefinition::GIVEN)) && (vehicleParameters.departLane > 0)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTLANE) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DepartSpeedDefinition::GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            writeError(TL("Invalid ") + toString(SUMO_ATTR_DEPARTSPEED) + TL(" used in ") + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create flow using vehicleParameters
            GNEDemandElement* flow = new GNEVehicle(GNE_TAG_FLOW_JUNCTIONS, myNet, vType, fromJunction, toJunction, vehicleParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(flow->getTagProperty().getGUIIcon(), TL("add ") + flow->getTagStr() + " '" + vehicleParameters.id + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildElement(flow);
                flow->incRef("buildFlow");
                // add reference in all junctions
                fromJunction->addChildElement(flow);
                toJunction->addChildElement(flow);
            }
            // compute path
            flow->computePathElement();
        }
    }
}


void
GNERouteHandler::buildPerson(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& personParameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(personParameters.id)) {
        // obtain type
        GNEDemandElement* type = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, personParameters.vtypeid, false);
        if (type == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, personParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid person type '") + personParameters.vtypeid + TL("' used in ") + toString(personParameters.tag) + " '" + personParameters.id + "'.");
            }
        } else {
            // create person using personParameters
            GNEDemandElement* person = new GNEPerson(SUMO_TAG_PERSON, myNet, type, personParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(person->getTagProperty().getGUIIcon(), TL("add ") + person->getTagStr() + " '" + personParameters.id + "'");
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
        // obtain type
        GNEDemandElement* type = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, personFlowParameters.vtypeid, false);
        if (type == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, personFlowParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError(TL("Invalid personFlow type '") + personFlowParameters.vtypeid + TL("' used in ") + toString(personFlowParameters.tag) + " '" + personFlowParameters.id + "'.");
            }
        } else {
            // create personFlow using personFlowParameters
            GNEDemandElement* personFlow = new GNEPerson(SUMO_TAG_PERSONFLOW, myNet, type, personFlowParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(personFlow->getTagProperty().getGUIIcon(), TL("add ") + personFlow->getTagStr() + " '" + personFlowParameters.id + "'");
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
GNERouteHandler::buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                                 const std::string& fromJunctionID, const std::string& toJunctionID, const std::string& toBusStopID, double arrivalPos,
                                 const std::vector<std::string>& types, const std::vector<std::string>& modes, const std::vector<std::string>& lines) {
    // get previous plan edge
    const auto previousEdge = getPreviousPlanEdge(true, sumoBaseObject);
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    // check from edge
    if ((fromEdge == nullptr) && previousEdge) {
        fromEdge = previousEdge;
    }
    // check from junction
    if (fromJunction == nullptr) {
        if (previousEdge) {
            fromJunction = previousEdge->getToJunction();
        } else {
            fromJunction = getPreviousPlanJunction(true, sumoBaseObject);
        }
    }
    // check conditions
    if (personParent) {
        if (fromEdge && toEdge) {
            // create personTrip from->to (edges)
            GNEDemandElement* personTrip = new GNEPersonTrip(myNet, personParent, fromEdge, toEdge, arrivalPos, types, modes, lines);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(personTrip->getTagProperty().getGUIIcon(), TL("add ") + personTrip->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personTrip);
                // set child references
                personParent->addChildElement(personTrip);
                fromEdge->addChildElement(personTrip);
                toEdge->addChildElement(personTrip);
                personTrip->incRef("buildPersonTripFromTo");
            }
        } else if (fromEdge && toBusStop) {
            // create personTrip from->busStop
            GNEDemandElement* personTrip = new GNEPersonTrip(myNet, personParent, fromEdge, toBusStop, arrivalPos, types, modes, lines);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(personTrip->getTagProperty().getGUIIcon(), TL("add ") + personTrip->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personTrip);
                // set child references
                personParent->addChildElement(personTrip);
                fromEdge->addChildElement(personTrip);
                toBusStop->addChildElement(personTrip);
                personTrip->incRef("buildPersonTripFromBusStop");
            }
        } else if (fromJunction && toJunction) {
            // create personTrip from->to (junctions)
            GNEDemandElement* personTrip = new GNEPersonTrip(myNet, personParent, fromJunction, toJunction, arrivalPos, types, modes, lines);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(personTrip->getTagProperty().getGUIIcon(), TL("add ") + personTrip->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(personTrip, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(personTrip);
                // set child references
                personParent->addChildElement(personTrip);
                fromJunction->addChildElement(personTrip);
                toJunction->addChildElement(personTrip);
                personTrip->incRef("buildPersonTripFromTo");
            }
        }
    }
}


void
GNERouteHandler::buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                           const std::string& fromJunctionID, const std::string& toJunctionID, const std::string& toBusStopID,
                           const std::vector<std::string>& edgeIDs, const std::string& routeID, double arrivalPos) {
    // get previous plan edge
    const auto previousEdge = getPreviousPlanEdge(true, sumoBaseObject);
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEJunction* fromJunction = myNet->getAttributeCarriers()->retrieveJunction(fromJunctionID, false);
    GNEJunction* toJunction = myNet->getAttributeCarriers()->retrieveJunction(toJunctionID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    GNEDemandElement* route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, routeID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // avoid consecutive duplicated edges
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // check from edge
    if ((fromEdge == nullptr) && previousEdge) {
        fromEdge = previousEdge;
    }
    // check from junction
    if (fromJunction == nullptr) {
        if (previousEdge) {
            fromJunction = previousEdge->getToJunction();
        } else {
            fromJunction = getPreviousPlanJunction(true, sumoBaseObject);
        }
    }
    // check conditions
    if (personParent) {
        if (edges.size() > 0) {
            // create walk edges
            GNEDemandElement* walk = new GNEWalk(myNet, personParent, edges, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(walk->getTagProperty().getGUIIcon(), TL("add ") + walk->getTagStr() + " '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                for (const auto& edge : edges) {
                    edge->addChildElement(walk);
                }
                walk->incRef("buildWalkEdges");
            }
        } else if (route) {
            // create walk over route
            GNEDemandElement* walk = new GNEWalk(myNet, personParent, route, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(walk->getTagProperty().getGUIIcon(), TL("add ") + walk->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                route->addChildElement(walk);
                walk->incRef("buildWalkRoute");
            }
        } else if (fromEdge && toEdge) {
            // create walk from->to (edges)
            GNEDemandElement* walk = new GNEWalk(myNet, personParent, fromEdge, toEdge, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(walk->getTagProperty().getGUIIcon(), TL("add ") + walk->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                fromEdge->addChildElement(walk);
                toEdge->addChildElement(walk);
                walk->incRef("buildWalkFromTo");
            }
        } else if (fromEdge && toBusStop) {
            // create walk from->busStop
            GNEDemandElement* walk = new GNEWalk(myNet, personParent, fromEdge, toBusStop, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(walk->getTagProperty().getGUIIcon(), TL("add ") + walk->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                fromEdge->addChildElement(walk);
                toBusStop->addChildElement(walk);
                walk->incRef("buildWalkFromBusStop");
            }
        } else if (fromJunction && toJunction) {
            // create walk from->to (junction)
            GNEDemandElement* walk = new GNEWalk(myNet, personParent, fromJunction, toJunction, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(walk->getTagProperty().getGUIIcon(), TL("add ") + walk->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(walk, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(walk);
                // set child references
                personParent->addChildElement(walk);
                fromJunction->addChildElement(walk);
                toJunction->addChildElement(walk);
                walk->incRef("buildWalkFromTo");
            }
        }
    }
}


void
GNERouteHandler::buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                           const std::string& toBusStopID, double arrivalPos, const std::vector<std::string>& lines) {
    // get previous plan edge
    const auto previousEdge = getPreviousPlanEdge(true, sumoBaseObject);
    // parse parents
    GNEDemandElement* personParent = getPersonParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* toBusStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, toBusStopID, false);
    // check from edge
    if ((fromEdge == nullptr) && previousEdge) {
        fromEdge = previousEdge;
    }
    // check conditions
    if (personParent && fromEdge) {
        if (toEdge) {
            // create ride from->to
            GNEDemandElement* ride = new GNERide(myNet, personParent, fromEdge, toEdge, arrivalPos, lines);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(ride->getTagProperty().getGUIIcon(), TL("add ") + ride->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(ride, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(ride);
                // set child references
                personParent->addChildElement(ride);
                fromEdge->addChildElement(ride);
                toEdge->addChildElement(ride);
                ride->incRef("buildRideFromTo");
            }
        } else if (toBusStop) {
            // create ride from->busStop
            GNEDemandElement* ride = new GNERide(myNet, personParent, fromEdge, toBusStop, arrivalPos, lines);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(ride->getTagProperty().getGUIIcon(), TL("add ") + ride->getTagStr() + " in '" + personParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(ride, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(ride);
                // set child references
                personParent->addChildElement(ride);
                fromEdge->addChildElement(ride);
                toBusStop->addChildElement(ride);
                ride->incRef("buildRideFromBusStop");
            }
        }
    }
}


void
GNERouteHandler::buildContainer(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const SUMOVehicleParameter& containerParameters) {
    // first check if ID is duplicated
    if (!isContainerIdDuplicated(containerParameters.id)) {
        // obtain type
        GNEDemandElement* type = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, containerParameters.vtypeid, false);
        if (type == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, containerParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError("Invalid container type '" + containerParameters.vtypeid + TL("' used in ") + toString(containerParameters.tag) + " '" + containerParameters.id + "'.");
            }
        } else {
            // create container using containerParameters
            GNEDemandElement* container = new GNEContainer(SUMO_TAG_CONTAINER, myNet, type, containerParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(GUIIcon::CONTAINER, TL("add ") + container->getTagStr() + " '" + container->getID() + "'");
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
        // obtain type
        GNEDemandElement* type = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, containerFlowParameters.vtypeid, false);
        if (type == nullptr) {
            if (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, containerFlowParameters.vtypeid, false)) {
                WRITE_WARNING(TL("VType distributions currently unsupported in netedit"));
            } else {
                writeError("Invalid containerFlow type '" + containerFlowParameters.vtypeid + TL("' used in ") + toString(containerFlowParameters.tag) + " '" + containerFlowParameters.id + "'.");
            }
        } else {
            // create containerFlow using containerFlowParameters
            GNEDemandElement* containerFlow = new GNEContainer(SUMO_TAG_CONTAINERFLOW, myNet, type, containerFlowParameters);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(GUIIcon::CONTAINERFLOW, TL("add ") + containerFlow->getTagStr() + " '" + containerFlow->getID() + "'");
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
GNERouteHandler::buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                                const std::string& toContainerStopID, const std::vector<std::string>& lines, const double arrivalPos) {
    // get previous plan edge
    const auto previousEdge = getPreviousPlanEdge(false, sumoBaseObject);
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* toContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    // check from edge
    if ((fromEdge == nullptr) && previousEdge) {
        fromEdge = previousEdge;
    }
    // check conditions
    if (containerParent && fromEdge) {
        if (toEdge) {
            // create transport from->to
            GNEDemandElement* transport = new GNETransport(myNet, containerParent, fromEdge, toEdge, lines, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(transport->getTagProperty().getGUIIcon(), TL("add ") + transport->getTagStr() + " in '" + containerParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(transport, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(transport);
                // set child references
                containerParent->addChildElement(transport);
                fromEdge->addChildElement(transport);
                toEdge->addChildElement(transport);
                transport->incRef("buildTransportFromTo");
            }
        } else if (toContainerStop) {
            // create transport from->containerStop
            GNEDemandElement* transport = new GNETransport(myNet, containerParent, fromEdge, toContainerStop, lines, arrivalPos);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(transport->getTagProperty().getGUIIcon(), TL("add ") + transport->getTagStr() + " in '" + containerParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(transport, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(transport);
                // set child references
                containerParent->addChildElement(transport);
                fromEdge->addChildElement(transport);
                toContainerStop->addChildElement(transport);
                transport->incRef("buildTransportFromContainerStop");
            }
        }
    }
}


void
GNERouteHandler::buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                               const std::string& toContainerStopID, const std::vector<std::string>& edgeIDs, const double speed, const double departPosition, const double arrivalPosition) {
    // get previous plan edge
    const auto previousEdge = getPreviousPlanEdge(false, sumoBaseObject);
    // first parse parents
    GNEDemandElement* containerParent = getContainerParent(sumoBaseObject);
    GNEEdge* fromEdge = myNet->getAttributeCarriers()->retrieveEdge(fromEdgeID, false);
    GNEEdge* toEdge = myNet->getAttributeCarriers()->retrieveEdge(toEdgeID, false);
    GNEAdditional* toContainerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, toContainerStopID, false);
    std::vector<GNEEdge*> edges = parseEdges(SUMO_TAG_WALK, edgeIDs);
    // avoid consecutive duplicated edges
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    // check from edge
    if ((fromEdge == nullptr) && previousEdge) {
        fromEdge = previousEdge;
    }
    // check conditions
    if (containerParent && (fromEdge || (edges.size() > 0))) {
        if (edges.size() > 0) {
            // create tranship edges
            GNEDemandElement* tranship = new GNETranship(myNet, containerParent, edges, speed, departPosition, arrivalPosition);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(tranship->getTagProperty().getGUIIcon(), TL("add ") + tranship->getTagStr() + " in '" + containerParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                for (const auto& edge : edges) {
                    edge->addChildElement(tranship);
                }
                tranship->incRef("buildTranshipEdges");
            }
        } else if (toEdge) {
            // create tranship from->to
            GNEDemandElement* tranship = new GNETranship(myNet, containerParent, fromEdge, toEdge, speed, departPosition, arrivalPosition);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(tranship->getTagProperty().getGUIIcon(), TL("add ") + tranship->getTagStr() + " in '" + containerParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                fromEdge->addChildElement(tranship);
                toEdge->addChildElement(tranship);
                tranship->incRef("buildTranshipFromTo");
            }
        } else if (toContainerStop) {
            // create tranship from->containerStop
            GNEDemandElement* tranship = new GNETranship(myNet, containerParent, fromEdge, toContainerStop, speed, departPosition, arrivalPosition);
            if (myAllowUndoRedo) {
                myNet->getViewNet()->getUndoList()->begin(tranship->getTagProperty().getGUIIcon(), TL("add ") + tranship->getTagStr() + " '" + containerParent->getID() + "'");
                overwriteDemandElement();
                myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(tranship, true), true);
                myNet->getViewNet()->getUndoList()->end();
            } else {
                myNet->getAttributeCarriers()->insertDemandElement(tranship);
                // set child references
                containerParent->addChildElement(tranship);
                fromEdge->addChildElement(tranship);
                toContainerStop->addChildElement(tranship);
                tranship->incRef("buildTranshipFromContainerStop");
            }
        }
    }
}


void
GNERouteHandler::buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) {
    // get obj parent
    const auto objParent = sumoBaseObject->getParentSumoBaseObject();
    // set tag
    SumoXMLTag tag = objParent->getTag();
    if (!objParent->hasStringAttribute(SUMO_ATTR_ROUTE) && !objParent->hasStringAttribute(SUMO_ATTR_FROM) && !objParent->hasStringAttribute(SUMO_ATTR_TO)) {
        if (tag == SUMO_TAG_VEHICLE) {
            tag = GNE_TAG_VEHICLE_WITHROUTE;
        } else if (tag == SUMO_TAG_FLOW) {
            tag = GNE_TAG_FLOW_WITHROUTE;
        }
    }
    // special case for flows over route
    if (objParent->hasStringAttribute(SUMO_ATTR_ROUTE) && (objParent->getTag() == SUMO_TAG_FLOW)) {
        tag = GNE_TAG_FLOW_ROUTE;
    }
    // get stop parent
    GNEDemandElement* stopParent = myNet->getAttributeCarriers()->retrieveDemandElement(tag, objParent->getStringAttribute(SUMO_ATTR_ID), false);
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
            stopTagType = waypoint ? GNE_TAG_WAYPOINT_BUSSTOP : SUMO_TAG_STOP_BUSSTOP;
            // containers cannot stops in busStops
            if (stopParent->getTagProperty().isContainer()) {
                writeError(TL("Containers don't support stops at busStops"));
                validParentDemandElement = false;
            }
        } else if (stopParameters.containerstop.size() > 0) {
            stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
            stopTagType = waypoint ? GNE_TAG_WAYPOINT_CONTAINERSTOP : SUMO_TAG_STOP_CONTAINERSTOP;
            // persons cannot stops in containerStops
            if (stopParent->getTagProperty().isPerson()) {
                writeError(TL("Persons don't support stops at containerStops"));
                validParentDemandElement = false;
            }
        } else if (stopParameters.chargingStation.size() > 0) {
            stoppingPlace = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
            stopTagType = waypoint ? GNE_TAG_WAYPOINT_CHARGINGSTATION : SUMO_TAG_STOP_CHARGINGSTATION;
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
            stopTagType = waypoint ? GNE_TAG_WAYPOINT_PARKINGAREA : SUMO_TAG_STOP_PARKINGAREA;
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
            stopTagType = waypoint ? GNE_TAG_WAYPOINT_LANE : SUMO_TAG_STOP_LANE;
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
                    stop = new GNEStop(GNE_TAG_STOPPERSON_BUSSTOP, myNet, stopParent, stoppingPlace, stopParameters);
                } else if (stopParent->getTagProperty().isContainer()) {
                    stop = new GNEStop(GNE_TAG_STOPCONTAINER_CONTAINERSTOP, myNet, stopParent, stoppingPlace, stopParameters);
                } else {
                    stop = new GNEStop(stopTagType, myNet, stopParent, stoppingPlace, stopParameters);
                }
                // add it depending of undoDemandElements
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(stop->getTagProperty().getGUIIcon(), TL("add ") + stop->getTagStr() + " in '" + stopParent->getID() + "'");
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
                    myNet->getViewNet()->getUndoList()->begin(stop->getTagProperty().getGUIIcon(), TL("add ") + stop->getTagStr() + " in '" + stopParent->getID() + "'");
                    overwriteDemandElement();
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertDemandElement(stop);
                    lane->addChildElement(stop);
                    stopParent->addChildElement(stop);
                    stop->incRef("buildLaneStop");
                }
            } else {
                // create stop using stopParameters and edge
                GNEDemandElement* stop = nullptr;
                if (stopParent->getTagProperty().isPerson()) {
                    stop = new GNEStop(GNE_TAG_STOPPERSON_EDGE, myNet, stopParent, edge, stopParameters);
                } else {
                    stop = new GNEStop(GNE_TAG_STOPCONTAINER_EDGE, myNet, stopParent, edge, stopParameters);
                }
                // add it depending of undoDemandElements
                if (myAllowUndoRedo) {
                    myNet->getViewNet()->getUndoList()->begin(stop->getTagProperty().getGUIIcon(), TL("add ") + stop->getTagStr() + " in '" + stopParent->getID() + "'");
                    overwriteDemandElement();
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                    myNet->getViewNet()->getUndoList()->end();
                } else {
                    myNet->getAttributeCarriers()->insertDemandElement(stop);
                    edge->addChildElement(stop);
                    stopParent->addChildElement(stop);
                    stop->incRef("buildLaneStop");
                }
            }
        }
    }
}


bool
GNERouteHandler::buildPersonPlan(SumoXMLTag tag, GNEDemandElement* personParent, GNEAttributesCreator* personPlanAttributes,
                                 GNEPathCreator* pathCreator, const bool centerAfterCreation) {
    // clear and set person object
    myPlanObject->clear();
    myPlanObject->setTag(personParent->getTagProperty().getTag());
    myPlanObject->addStringAttribute(SUMO_ATTR_ID, personParent->getID());
    // declare personPlan object
    CommonXMLStructure::SumoBaseObject* personPlanObject = new CommonXMLStructure::SumoBaseObject(myPlanObject);
    personPlanObject->setTag(tag);
    // get person plan attributes
    personPlanAttributes->getAttributesAndValues(personPlanObject, true);
    // get attributes
    const std::vector<std::string> types = personPlanObject->hasStringListAttribute(SUMO_ATTR_VTYPES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_VTYPES) : std::vector<std::string>();
    const std::vector<std::string> modes = personPlanObject->hasStringListAttribute(SUMO_ATTR_MODES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_MODES) : std::vector<std::string>();
    const std::vector<std::string> lines = personPlanObject->hasStringListAttribute(SUMO_ATTR_LINES) ? personPlanObject->getStringListAttribute(SUMO_ATTR_LINES) : std::vector<std::string>();
    const double arrivalPos = personPlanObject->hasDoubleAttribute(SUMO_ATTR_ARRIVALPOS) ? personPlanObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS) : -1;
    // get stop parameters
    SUMOVehicleParameter::Stop stopParameters;
    // fill stops parameters
    if ((tag == GNE_TAG_STOPPERSON_BUSSTOP) || (tag == GNE_TAG_STOPPERSON_EDGE)) {
        stopParameters.actType = personPlanObject->getStringAttribute(SUMO_ATTR_ACTTYPE);
        if (personPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            stopParameters.duration = personPlanObject->getTimeAttribute(SUMO_ATTR_DURATION);
            stopParameters.parametersSet |= STOP_DURATION_SET;
        }
        if (personPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            stopParameters.until = personPlanObject->getTimeAttribute(SUMO_ATTR_UNTIL);
            stopParameters.parametersSet |= STOP_UNTIL_SET;
        }
    }
    // get edges
    GNEEdge* fromEdge = (pathCreator->getSelectedEdges().size() > 0) ? pathCreator->getSelectedEdges().front() : nullptr;
    GNEEdge* toEdge = (pathCreator->getSelectedEdges().size() > 0) ? pathCreator->getSelectedEdges().back() : nullptr;
    // get junctions
    GNEJunction* fromJunction = (pathCreator->getSelectedJunctions().size() > 0) ? pathCreator->getSelectedJunctions().front() : nullptr;
    GNEJunction* toJunction = (pathCreator->getSelectedJunctions().size() > 0) ? pathCreator->getSelectedJunctions().back() : nullptr;
    // get busStop
    GNEAdditional* toBusStop = pathCreator->getToStoppingPlace(SUMO_TAG_BUS_STOP);
    // get path edges
    std::vector<std::string> edges;
    for (const auto& path : pathCreator->getPath()) {
        for (const auto& edge : path.getSubPath()) {
            edges.push_back(edge->getID());
        }
    }
    // get route
    GNEDemandElement* route = pathCreator->getRoute();
    // check what PersonPlan we're creating
    switch (tag) {
        // Person Trips
        case GNE_TAG_PERSONTRIP_EDGE: {
            // check if person trip busStop->edge can be created
            if (fromEdge && toEdge) {
                buildPersonTrip(personPlanObject, fromEdge->getID(), toEdge->getID(), "", "", "", arrivalPos, types, modes, lines);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A person trip from edge to edge needs two edges edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_PERSONTRIP_BUSSTOP: {
            // check if person trip busStop->busStop can be created
            if (fromEdge && toBusStop) {
                buildPersonTrip(personPlanObject, fromEdge->getID(), "", "", "", toBusStop->getID(), arrivalPos, types, modes, lines);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A person trip from edge to busStop needs one edge and one busStop"));
                return false;
            }
            break;
        }
        case GNE_TAG_PERSONTRIP_JUNCTIONS: {
            // check if person trip busStop->junction can be created
            if (fromJunction && toJunction) {
                buildPersonTrip(personPlanObject, "", "", fromJunction->getID(), toJunction->getID(), "", arrivalPos, types, modes, lines);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A person trip from junction to junction needs two junctions junction"));
                return false;
            }
            break;
        }
        // Walks
        case GNE_TAG_WALK_EDGE: {
            // check if transport busStop->edge can be created
            if (fromEdge && toEdge) {
                buildWalk(personPlanObject, fromEdge->getID(), toEdge->getID(), "", "", "", {}, "", arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from busStop to edge needs a busStop and an edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_WALK_BUSSTOP: {
            // check if transport busStop->busStop can be created
            if (fromEdge && toBusStop) {
                buildWalk(personPlanObject, fromEdge->getID(), "", "", "", toBusStop->getID(), {}, "", arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A transport from busStop to busStop needs two busStops"));
                return false;
            }
            break;
        }
        case GNE_TAG_WALK_EDGES: {
            // check if transport edges can be created
            if (edges.size() > 0) {
                buildWalk(personPlanObject, "", "", "", "", "", edges, "", arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A transport with edges attribute needs a list of edges"));
                return false;
            }
            break;
        }
        case GNE_TAG_WALK_ROUTE: {
            // check if transport edges can be created
            if (route) {
                buildWalk(personPlanObject, "", "", "", "", "", {}, route->getID(), arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A route transport needs a route"));
                return false;
            }
            break;
        }
        case GNE_TAG_WALK_JUNCTIONS: {
            // check if transport busStop->junction can be created
            if (fromJunction && toJunction) {
                buildWalk(personPlanObject, "", "", fromJunction->getID(), toJunction->getID(), "", {}, "", arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from busStop to junction needs a busStop and an junction"));
                return false;
            }
            break;
        }
        // Rides
        case GNE_TAG_RIDE_EDGE: {
            // check if ride busStop->edge can be created
            if (fromEdge && toEdge) {
                buildRide(personPlanObject, fromEdge->getID(), toEdge->getID(), "", arrivalPos, lines);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from edge to edge needs two edges edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_RIDE_BUSSTOP: {
            // check if ride busStop->busStop can be created
            if (fromEdge && toBusStop) {
                buildRide(personPlanObject, fromEdge->getID(), "", toBusStop->getID(), arrivalPos, lines);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from edge to busStop needs one edge and one busStop"));
                return false;
            }
            break;
        }
        // stops
        case GNE_TAG_STOPPERSON_EDGE: {
            // check if ride busStop->busStop can be created
            if (fromEdge) {
                stopParameters.edge = fromEdge->getID();
                stopParameters.endPos = fromEdge->getLanes().front()->getLaneShape().nearest_offset_to_point2D(myNet->getViewNet()->getPositionInformation());
                stopParameters.parametersSet |= STOP_END_SET;
                buildStop(personPlanObject, stopParameters);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A stop has to be placed over an edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_STOPPERSON_BUSSTOP: {
            // check if ride busStop->busStop can be created
            if (toBusStop) {
                stopParameters.busstop = toBusStop->getID();
                buildStop(personPlanObject, stopParameters);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A stop has to be placed over a busStop"));
                return false;
            }
            break;
        }
        default:
            throw InvalidArgument("Invalid person plan tag");
    }
    // get person
    const auto person = myNet->getAttributeCarriers()->retrieveDemandElement(personPlanObject->getParentSumoBaseObject()->getTag(),
                        personPlanObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (person) {
        // compute person (and all person plans)
        person->computePathElement();
        // center view after creation
        if (centerAfterCreation && !myNet->getViewNet()->getVisibleBoundary().around(person->getPositionInView())) {
            myNet->getViewNet()->centerTo(person->getPositionInView(), false);
        }
    }
    return true;
}


bool
GNERouteHandler::buildContainerPlan(SumoXMLTag tag, GNEDemandElement* containerParent, GNEAttributesCreator* containerPlanAttributes, GNEPathCreator* pathCreator, const bool centerAfterCreation) {
    // clear and set planObject
    myPlanObject->clear();
    myPlanObject->setTag(containerParent->getTagProperty().getTag());
    myPlanObject->addStringAttribute(SUMO_ATTR_ID, containerParent->getID());
    // declare containerPlan object
    CommonXMLStructure::SumoBaseObject* containerPlanObject = new CommonXMLStructure::SumoBaseObject(myPlanObject);
    containerPlanObject->setTag(tag);
    // get attributes and values
    containerPlanAttributes->getAttributesAndValues(containerPlanObject, true);
    // get attributes
    const std::vector<std::string> lines = containerPlanObject->hasStringListAttribute(SUMO_ATTR_LINES) ? containerPlanObject->getStringListAttribute(SUMO_ATTR_LINES) : std::vector<std::string>();
    const double speed = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_SPEED) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_SPEED) : 1.39;
    const double departPos = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_DEPARTPOS) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_DEPARTPOS) : -1;
    const double arrivalPos = containerPlanObject->hasDoubleAttribute(SUMO_ATTR_ARRIVALPOS) ? containerPlanObject->getDoubleAttribute(SUMO_ATTR_ARRIVALPOS) : -1;
    // get stop parameters
    SUMOVehicleParameter::Stop stopParameters;
    // fill stops parameters
    if ((tag == GNE_TAG_STOPCONTAINER_EDGE) || (tag == GNE_TAG_STOPCONTAINER_CONTAINERSTOP)) {
        stopParameters.actType = containerPlanObject->getStringAttribute(SUMO_ATTR_ACTTYPE);
        if (containerPlanObject->hasTimeAttribute(SUMO_ATTR_DURATION)) {
            stopParameters.duration = containerPlanObject->getTimeAttribute(SUMO_ATTR_DURATION);
            stopParameters.parametersSet |= STOP_DURATION_SET;
        }
        if (containerPlanObject->hasTimeAttribute(SUMO_ATTR_UNTIL)) {
            stopParameters.until = containerPlanObject->getTimeAttribute(SUMO_ATTR_UNTIL);
            stopParameters.parametersSet |= STOP_UNTIL_SET;
        }
    }
    // get edges
    GNEEdge* fromEdge = (pathCreator->getSelectedEdges().size() > 0) ? pathCreator->getSelectedEdges().front() : nullptr;
    GNEEdge* toEdge = (pathCreator->getSelectedEdges().size() > 0) ? pathCreator->getSelectedEdges().back() : nullptr;
    // get containerStop
    GNEAdditional* toContainerStop = pathCreator->getToStoppingPlace(SUMO_TAG_CONTAINER_STOP);
    // get path edges
    std::vector<std::string> edges;
    for (const auto& path : pathCreator->getPath()) {
        for (const auto& edge : path.getSubPath()) {
            edges.push_back(edge->getID());
        }
    }
    // check what ContainerPlan we're creating
    switch (tag) {
        // Transports
        case GNE_TAG_TRANSPORT_EDGE: {
            // check if transport busStop->edge can be created
            if (fromEdge && toEdge) {
                buildTransport(containerPlanObject, fromEdge->getID(), toEdge->getID(), "", lines, arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from busStop to edge needs a busStop and an edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_TRANSPORT_CONTAINERSTOP: {
            // check if transport busStop->busStop can be created
            if (fromEdge && toContainerStop) {
                buildTransport(containerPlanObject, fromEdge->getID(), "", toContainerStop->getID(), lines, arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A transport from busStop to busStop needs two busStops"));
                return false;
            }
            break;
        }
        // Tranships
        case GNE_TAG_TRANSHIP_EDGE: {
            // check if tranship busStop->edge can be created
            if (fromEdge && toEdge) {
                buildTranship(containerPlanObject, fromEdge->getID(), toEdge->getID(), "", {}, speed, departPos, arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A ride from busStop to edge needs a busStop and an edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_TRANSHIP_CONTAINERSTOP: {
            // check if tranship busStop->busStop can be created
            if (fromEdge && toContainerStop) {
                buildTranship(containerPlanObject, fromEdge->getID(), "", toContainerStop->getID(), {}, speed, departPos, arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A tranship from busStop to busStop needs two busStops"));
                return false;
            }
            break;
        }
        case GNE_TAG_TRANSHIP_EDGES: {
            // check if tranship edges can be created
            if (edges.size() > 0) {
                buildTranship(containerPlanObject, "", "", "", edges, speed, departPos, arrivalPos);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A tranship with edges attribute needs a list of edges"));
                return false;
            }
            break;
        }
        // stops
        case GNE_TAG_STOPCONTAINER_EDGE: {
            // check if ride busStop->busStop can be created
            if (fromEdge) {
                stopParameters.edge = fromEdge->getID();
                stopParameters.endPos = fromEdge->getLanes().front()->getLaneShape().nearest_offset_to_point2D(myNet->getViewNet()->getPositionInformation());
                stopParameters.parametersSet |= STOP_END_SET;
                buildStop(containerPlanObject, stopParameters);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A stop has to be placed over an edge"));
                return false;
            }
            break;
        }
        case GNE_TAG_STOPCONTAINER_CONTAINERSTOP: {
            // check if ride busStop->busStop can be created
            if (toContainerStop) {
                stopParameters.containerstop = toContainerStop->getID();
                buildStop(containerPlanObject, stopParameters);
            } else {
                myNet->getViewNet()->setStatusBarText(TL("A stop has to be placed over a busStop"));
                return false;
            }
            break;
        }
        default:
            throw InvalidArgument("Invalid container plan tag");
    }
    // get container
    const auto container = myNet->getAttributeCarriers()->retrieveDemandElement(containerPlanObject->getParentSumoBaseObject()->getTag(),
                           containerPlanObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID), false);
    if (container) {
        // compute container (and all container plans)
        container->computePathElement();
        // center view after creation
        if (centerAfterCreation && !myNet->getViewNet()->getVisibleBoundary().around(container->getPositionInView())) {
            myNet->getViewNet()->centerTo(container->getPositionInView(), false);
        }
    }
    return true;
}


bool
GNERouteHandler::isVehicleIdDuplicated(const std::string& id) {
    // declare vehicle tags vector
    std::vector<SumoXMLTag> vehicleTags = {SUMO_TAG_VEHICLE, GNE_TAG_VEHICLE_WITHROUTE, SUMO_TAG_TRIP, GNE_TAG_FLOW_ROUTE, GNE_TAG_FLOW_WITHROUTE, SUMO_TAG_FLOW};
    for (const auto& vehicleTag : vehicleTags) {
        if (!checkDuplicatedDemandElement(vehicleTag, id)) {
            writeError(TL("There is another ") + toString(vehicleTag) + TL(" with the same ID='") + id + "'.");
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isViaAttributeValid(const std::vector<std::string>& via) {
    for (const auto& edgeID : via) {
        if (myNet->getAttributeCarriers()->retrieveEdge(edgeID, false) == nullptr) {
            writeError(TL("Via edge '") + edgeID + TL("' doesn't exist."));
            return false;
        }
    }
    return true;
}


bool
GNERouteHandler::isPersonIdDuplicated(const std::string& id) {
    for (SumoXMLTag personTag : std::vector<SumoXMLTag>({SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW})) {
        if (!checkDuplicatedDemandElement(personTag, id)) {
            writeError(TL("There is another ") + toString(personTag) + TL(" with the same ID='") + id + "'.");
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isContainerIdDuplicated(const std::string& id) {
    for (SumoXMLTag containerTag : std::vector<SumoXMLTag>({SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW})) {
        if (!checkDuplicatedDemandElement(containerTag, id)) {
            writeError(TL("There is another ") + toString(containerTag) + TL(" with the same ID='") + id + "'.");
            return true;
        }
    }
    return false;
}


void
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
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
    if ((tag == GNE_TAG_FLOW_ROUTE) || (tag == SUMO_TAG_VEHICLE)) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().back()->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if (originalVehicle->getTagProperty().hasEmbeddedRoute()) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
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
        net->getViewNet()->getUndoList()->begin(originalVehicle->getTagProperty().getGUIIcon(), "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
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
            routeHandler.buildRoute(nullptr, routeID, vClass, edgeIDs, routeColor, false, 0, {});
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
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
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
    if ((tag == GNE_TAG_FLOW_ROUTE) || (tag == SUMO_TAG_VEHICLE)) {
        // get route edges
        routeEdges = originalVehicle->getParentDemandElements().back()->getParentEdges();
        // get original route color
        routeColor = originalVehicle->getParentDemandElements().back()->getColor();
    } else if (originalVehicle->getTagProperty().hasEmbeddedRoute()) {
        // get embedded route edges
        routeEdges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
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
        net->getViewNet()->getUndoList()->begin(originalVehicle->getTagProperty().getGUIIcon(), "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // change depart
        if ((vehicleParameters.tag == SUMO_TAG_TRIP) || (vehicleParameters.tag == SUMO_TAG_VEHICLE) || (vehicleParameters.tag == GNE_TAG_VEHICLE_WITHROUTE)) {
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
            routeHandler.buildRoute(nullptr, routeID, vClass, edgeIDs, routeColor, false, 0, {});
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
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
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
    if ((tag == SUMO_TAG_VEHICLE) || (tag == GNE_TAG_FLOW_ROUTE)) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if (originalVehicle->getTagProperty().hasEmbeddedRoute()) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
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
        net->getViewNet()->getUndoList()->begin(originalVehicle->getTagProperty().getGUIIcon(), "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_TRIP));
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
    // get original vehicle tag
    SumoXMLTag tag = originalVehicle->getTagProperty().getTag();
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
    if ((tag == SUMO_TAG_VEHICLE) || (tag == GNE_TAG_FLOW_ROUTE)) {
        // set route
        route = originalVehicle->getParentDemandElements().back();
        // get route edges
        edges = route->getParentEdges();
    } else if (originalVehicle->getTagProperty().hasEmbeddedRoute()) {
        // get embedded route edges
        edges = originalVehicle->getChildDemandElements().front()->getParentEdges();
    } else if ((tag == SUMO_TAG_TRIP) || (tag == SUMO_TAG_FLOW)) {
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
        net->getViewNet()->getUndoList()->begin(originalVehicle->getTagProperty().getGUIIcon(), "transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // first delete vehicle
        net->deleteDemandElement(originalVehicle, net->getViewNet()->getUndoList());
        // check if route has to be deleted
        if (route && route->getChildDemandElements().empty()) {
            net->deleteDemandElement(route, net->getViewNet()->getUndoList());
        }
        // change depart
        if ((vehicleParameters.tag == SUMO_TAG_TRIP) || (vehicleParameters.tag == SUMO_TAG_VEHICLE) || (vehicleParameters.tag == GNE_TAG_VEHICLE_WITHROUTE)) {
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
GNERouteHandler::transformToPerson(GNEPerson* originalPerson) {
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
    net->getViewNet()->getUndoList()->begin(originalPerson->getTagProperty().getGUIIcon(), "transform " + originalPerson->getTagStr() + " to " + toString(SUMO_TAG_PERSON));
    // create personFlow
    routeHandler.buildPerson(nullptr, personParameters);
    // move all person plans to new person
    for (const auto& personPlan : personPlans) {
        personPlan->setAttribute(GNE_ATTR_PARENT, "%dummyID%", net->getViewNet()->getUndoList());
    }
    // delete original person plan
    net->deleteDemandElement(originalPerson, net->getViewNet()->getUndoList());
    // restore ID of new person plan
    auto newPerson = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSON, "%dummyID%");
    newPerson->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        // get created element
        auto transformedPerson = net->getAttributeCarriers()->retrieveDemandElement(personParameters.tag, personParameters.id);
        // inspect it
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedPerson);
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
    net->getViewNet()->getUndoList()->begin(originalPerson->getTagProperty().getGUIIcon(), "transform " + originalPerson->getTagStr() + " to " + toString(SUMO_TAG_PERSONFLOW));
    // create personFlow
    routeHandler.buildPersonFlow(nullptr, personParameters);
    // move all person plans to new person
    for (const auto& personPlan : personPlans) {
        personPlan->setAttribute(GNE_ATTR_PARENT, "%dummyID%", net->getViewNet()->getUndoList());
    }
    // delete original person plan
    net->deleteDemandElement(originalPerson, net->getViewNet()->getUndoList());
    // restore ID of new person plan
    auto newPerson = net->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_PERSONFLOW, "%dummyID%");
    newPerson->setAttribute(SUMO_ATTR_ID, ID, net->getViewNet()->getUndoList());
    // enable attributes
    newPerson->enableAttribute(SUMO_ATTR_END, net->getViewNet()->getUndoList());
    newPerson->enableAttribute(SUMO_ATTR_PERSONSPERHOUR, net->getViewNet()->getUndoList());
    // finish undoList
    net->getViewNet()->getUndoList()->end();
    // check if inspect
    if (inspectAfterTransform) {
        // get created element
        auto transformedPerson = net->getAttributeCarriers()->retrieveDemandElement(personParameters.tag, personParameters.id);
        // inspect it
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedPerson);
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
    net->getViewNet()->getUndoList()->begin(originalContainer->getTagProperty().getGUIIcon(), "transform " + originalContainer->getTagStr() + " to " + toString(SUMO_TAG_CONTAINER));
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
        // get created element
        auto transformedContainer = net->getAttributeCarriers()->retrieveDemandElement(containerParameters.tag, containerParameters.id);
        // inspect it
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedContainer);
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
    net->getViewNet()->getUndoList()->begin(originalContainer->getTagProperty().getGUIIcon(), "transform " + originalContainer->getTagStr() + " to " + toString(SUMO_TAG_CONTAINERFLOW));
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
        // get created element
        auto transformedContainer = net->getAttributeCarriers()->retrieveDemandElement(containerParameters.tag, containerParameters.id);
        // inspect it
        net->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(transformedContainer);
    }
}

// ===========================================================================
// protected
// ===========================================================================

GNEJunction*
GNERouteHandler::parseJunction(const SumoXMLTag tag, const std::string& junctionID) {
    GNEJunction* junction = myNet->getAttributeCarriers()->retrieveJunction(junctionID, false);
    // empty junctions aren't allowed. If junction is empty, write error, clear junctions and stop
    if (junction == nullptr) {
        writeError("Could not build " + toString(tag) + " in netedit; " +  toString(SUMO_TAG_JUNCTION) + " doesn't exist.");
    }
    return junction;
}


GNEEdge*
GNERouteHandler::parseEdge(const SumoXMLTag tag, const std::string& edgeID) {
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    // empty edges aren't allowed. If edge is empty, write error, clear edges and stop
    if (edge == nullptr) {
        writeError("Could not build " + toString(tag) + " in netedit; " +  toString(SUMO_TAG_EDGE) + " doesn't exist.");
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
            writeError("Could not build " + toString(tag) + " in netedit; " +  toString(SUMO_TAG_EDGE) + " doesn't exist.");
            edges.clear();
            return edges;
        } else {
            edges.push_back(edge);
        }
    }
    return edges;
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


GNEEdge*
GNERouteHandler::getPreviousPlanEdge(const bool person, const CommonXMLStructure::SumoBaseObject* obj) const {
    if (obj->getParentSumoBaseObject() == nullptr) {
        // no parent defined
        return nullptr;
    }
    // get parent object
    const CommonXMLStructure::SumoBaseObject* parentObject = obj->getParentSumoBaseObject();
    // check person/container conditions
    if (person && (parentObject->getTag() != SUMO_TAG_PERSON) && (parentObject->getTag() != SUMO_TAG_PERSONFLOW)) {
        // invalid parent
        return nullptr;
    }
    if (!person && (parentObject->getTag() != SUMO_TAG_CONTAINER) && (parentObject->getTag() != SUMO_TAG_CONTAINERFLOW)) {
        // invalid parent
        return nullptr;
    }
    // search previous child
    const auto it = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), obj);
    if (it == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    }
    // get last children
    const CommonXMLStructure::SumoBaseObject* previousPlan = *(it - 1);
    // check conditions
    if (person && (previousPlan->getTag() != SUMO_TAG_WALK) && (previousPlan->getTag() != SUMO_TAG_RIDE) &&
            (previousPlan->getTag() != SUMO_TAG_PERSONTRIP) && (previousPlan->getTag() != SUMO_TAG_STOP)) {
        // invalid last child
        return nullptr;
    }
    if (!person && (previousPlan->getTag() != SUMO_TAG_TRANSPORT) && (previousPlan->getTag() != SUMO_TAG_TRANSHIP)) {
        // invalid last child
        return nullptr;
    }
    // ends in an edge (only for stops)
    if (previousPlan->hasStringAttribute(SUMO_ATTR_EDGE)) {
        return myNet->getAttributeCarriers()->retrieveEdge(previousPlan->getStringAttribute(SUMO_ATTR_EDGE), false);
    }
    // ends in a lane (only for stops)
    if (previousPlan->hasStringAttribute(SUMO_ATTR_LANE)) {
        const auto lane = myNet->getAttributeCarriers()->retrieveLane(previousPlan->getStringAttribute(SUMO_ATTR_LANE), false);
        if (lane) {
            return lane->getParentEdge();
        } else {
            return nullptr;
        }
    }
    // ends in a route (walk)
    if (previousPlan->hasStringAttribute(SUMO_ATTR_ROUTE) &&
            !previousPlan->getStringAttribute(SUMO_ATTR_ROUTE).empty()) {
        const auto route = myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, previousPlan->getStringAttribute(SUMO_ATTR_ROUTE), false);
        if (route) {
            return route->getParentEdges().back();
        } else {
            return nullptr;
        }
    }
    // ends in a list of edges (walk)
    if (previousPlan->hasStringListAttribute(SUMO_ATTR_EDGES) &&
            !previousPlan->getStringListAttribute(SUMO_ATTR_EDGES).empty()) {
        return myNet->getAttributeCarriers()->retrieveEdge(previousPlan->getStringListAttribute(SUMO_ATTR_EDGES).back(), false);
    }
    // ends in a "to" edge
    if (previousPlan->hasStringAttribute(SUMO_ATTR_TO) &&
            !previousPlan->getStringAttribute(SUMO_ATTR_TO).empty()) {
        return myNet->getAttributeCarriers()->retrieveEdge(previousPlan->getStringAttribute(SUMO_ATTR_TO), false);
    }
    // ends in a "busStop"
    if (previousPlan->hasStringAttribute(SUMO_ATTR_BUS_STOP) &&
            !previousPlan->getStringAttribute(SUMO_ATTR_BUS_STOP).empty()) {
        const auto busStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_BUS_STOP, previousPlan->getStringAttribute(SUMO_ATTR_BUS_STOP), false);
        if (busStop) {
            return busStop->getParentLanes().front()->getParentEdge();
        } else {
            return nullptr;
        }
    }
    // ends in a "containerStop"
    if (previousPlan->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP) &&
            !previousPlan->getStringAttribute(SUMO_ATTR_CONTAINER_STOP).empty()) {
        const auto containerStop = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, previousPlan->getStringAttribute(SUMO_ATTR_CONTAINER_STOP), false);
        if (containerStop) {
            return containerStop->getParentLanes().front()->getParentEdge();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}


GNEJunction*
GNERouteHandler::getPreviousPlanJunction(const bool /* person */, const CommonXMLStructure::SumoBaseObject* obj) const {
    if (obj->getParentSumoBaseObject() == nullptr) {
        // no parent defined
        return nullptr;
    }
    // get parent object
    const CommonXMLStructure::SumoBaseObject* parentObject = obj->getParentSumoBaseObject();
    // search previous child
    const auto it = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), obj);
    if (it == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    }
    // get last child
    const CommonXMLStructure::SumoBaseObject* previousPersonPlan = *(it - 1);
    // ends in an junction (only for stops)
    if (previousPersonPlan->hasStringAttribute(SUMO_ATTR_TOJUNCTION)) {
        return myNet->getAttributeCarriers()->retrieveJunction(previousPersonPlan->getStringAttribute(SUMO_ATTR_TOJUNCTION), false);
    }
    return nullptr;
}


bool
GNERouteHandler::checkDuplicatedDemandElement(const SumoXMLTag tag, const std::string& id) {
    // retrieve demand element
    auto demandElement = myNet->getAttributeCarriers()->retrieveDemandElement(tag, id, false);
    // if demand exist, check if overwrite (delete)
    if (demandElement) {
        if (myAllowUndoRedo == false) {
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
