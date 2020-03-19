/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/additional/GNEChargingStation.h>
#include <netedit/elements/additional/GNEContainerStop.h>
#include <netedit/elements/additional/GNEParkingArea.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>

#include "GNEPerson.h"
#include "GNEPersonTrip.h"
#include "GNERide.h"
#include "GNERoute.h"
#include "GNERouteHandler.h"
#include "GNEStop.h"
#include "GNEVehicle.h"
#include "GNEVehicleType.h"
#include "GNEWalk.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERouteHandler::RouteParameter - methods
// ---------------------------------------------------------------------------

GNERouteHandler::RouteParameter::RouteParameter() :
    loadedID(false),
    vClass(SVC_PASSENGER),
    color(RGBColor::BLACK) {
}


GNERouteHandler::RouteParameter::RouteParameter(GNEDemandElement* originalDemandElement) :
    routeID(originalDemandElement->getTagProperty().isRoute() ?
        originalDemandElement->getID() :
        originalDemandElement->getViewNet()->getNet()->generateDemandElementID(originalDemandElement->getID(), SUMO_TAG_ROUTE)),
    loadedID(false),
    edges(originalDemandElement->getParentEdges()),
    vClass(originalDemandElement->getVClass()),
    color(originalDemandElement->getColor()) {
}


void
GNERouteHandler::RouteParameter::setEdges(GNEViewNet* viewNet, const std::string& edgeIDs) {
    // clear edges
    edges.clear();
    // obtain edges (And show warnings if isn't valid)
    if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(viewNet->getNet(), edgeIDs, true)) {
        edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(viewNet->getNet(), edgeIDs);
    }
}


void
GNERouteHandler::RouteParameter::setEdges(GNEViewNet* viewNet, const std::string& vehicleID, const std::string& fromID, const std::string& toID, const std::string& viaIDs) {
    // clear edges
    edges.clear();
    // only continue if at least one of the edges is defined
    if (fromID.size() + toID.size() > 0) {
        // obtain from and to edges
        GNEEdge* from = viewNet->getNet()->retrieveEdge(fromID, false);
        GNEEdge* to = viewNet->getNet()->retrieveEdge(toID, false);
        // check if edges are valid
        if (from == nullptr) {
            WRITE_ERROR("Invalid from-edge '" + fromID + "' used in trip '" + vehicleID + "'.");
        } else if (to == nullptr) {
            WRITE_ERROR("Invalid to-edge '" + toID + "' used in trip '" + vehicleID + "'.");
        } else if (!GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(viewNet->getNet(), viaIDs, false)) {
            WRITE_ERROR("Invalid 'via' edges used in trip '" + vehicleID + "'.");
        } else {
            // obtain via
            std::vector<GNEEdge*> viaEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(viewNet->getNet(), viaIDs);
            // build edges (from - via - to)
            edges.push_back(from);
            for (const auto& i : viaEdges) {
                edges.push_back(i);
            }
            // check that from and to edge are different
            if (from != to) {
                edges.push_back(to);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// GNERouteHandler - methods
// ---------------------------------------------------------------------------

GNERouteHandler::GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements) :
    SUMORouteHandler(file, "", false),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements),
    myLoadedVehicleWithEmbebbedRoute(nullptr),
    myAbort(false) {
}


GNERouteHandler::~GNERouteHandler() {}


bool
GNERouteHandler::isVehicleIdDuplicated(GNEViewNet* viewNet, const std::string& id) {
    for (SumoXMLTag vehicleTag : std::vector<SumoXMLTag>({SUMO_TAG_VEHICLE, SUMO_TAG_TRIP, SUMO_TAG_ROUTEFLOW, SUMO_TAG_FLOW})) {
        if (viewNet->getNet()->retrieveDemandElement(vehicleTag, id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(vehicleTag) + " with the same ID='" + id + "'.");
            return true;
        }
    }
    return false;
}


bool
GNERouteHandler::isPersonIdDuplicated(GNEViewNet* viewNet, const std::string& id) {
    for (SumoXMLTag personTag : std::vector<SumoXMLTag>({SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW})) {
        if (viewNet->getNet()->retrieveDemandElement(personTag, id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(personTag) + " with the same ID='" + id + "'.");
            return true;
        }
    }
    return false;
}


void
GNERouteHandler::buildVehicleOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_VEHICLE);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (route == nullptr) {
            WRITE_ERROR("Invalid route '" + vehicleParameters.routeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create vehicle using vehicleParameters
            GNEVehicle* vehicle = new GNEVehicle(viewNet, vType, route, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + vehicle->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, vehicle);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(vehicle);
                // set vehicle as child of vType and Route
                vType->addChildDemandElement(vehicle);
                route->addChildDemandElement(vehicle);
                vehicle->incRef("buildVehicleOverRoute");
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, vehicle);
                }
            }
        }
    }
}


void
GNERouteHandler::buildFlowOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_ROUTEFLOW);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters.routeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (route == nullptr) {
            WRITE_ERROR("Invalid route '" + vehicleParameters.routeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN) && ((int)route->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // create flow or trips using vehicleParameters
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, route, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, flow);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(flow);
                // set flow as child of vType and Route
                vType->addChildDemandElement(flow);
                route->addChildDemandElement(flow);
                flow->incRef("buildFlowOverRoute");
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, flow);
                }
            }
        }
    }
}


void
GNERouteHandler::buildVehicleWithEmbeddedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embeddedRouteCopy) {
    // Check tags
    assert(vehicleParameters.tag == SUMO_TAG_VEHICLE);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain vType
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN) && ((int)embeddedRouteCopy->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // generate a new route ID and add it to vehicleParameters
            vehicleParameters.routeid = viewNet->getNet()->generateDemandElementID(vehicleParameters.id, SUMO_TAG_ROUTE);
            // due vehicle was loaded without a route, change tag
            vehicleParameters.tag = SUMO_TAG_VEHICLE;
            // create vehicle or trips using myTemporalVehicleParameter without a route
            GNEVehicle* vehicle = new GNEVehicle(viewNet, vType, vehicleParameters);
            // creaste embedded route
            GNERoute* embeddedRoute = new GNERoute(viewNet, vehicle, RouteParameter(embeddedRouteCopy));
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(vehicle);
                // set vehicle as child of vType
                vType->addChildDemandElement(vehicle);
                vehicle->incRef("buildVehicleWithEmbeddedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(embeddedRoute);
                for (const auto& i : embeddedRouteCopy->getParentEdges()) {
                    i->addChildDemandElement(vehicle);
                }
                // set route as child of vehicle
                vehicle->addChildDemandElement(embeddedRoute);
                embeddedRoute->incRef("buildVehicleWithEmbeddedRoute");
            }
        }
    }
}


void
GNERouteHandler::buildFlowWithEmbeddedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embeddedRouteCopy) {
    // Check tags
    assert(vehicleParameters.tag == SUMO_TAG_ROUTEFLOW);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain vType
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN) && ((int)embeddedRouteCopy->getParentEdges().front()->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // generate a new route ID and add it to vehicleParameters
            vehicleParameters.routeid = viewNet->getNet()->generateDemandElementID(vehicleParameters.id, SUMO_TAG_ROUTE);
            // due vehicle was loaded without a route, change tag
            vehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
            // create vehicle or trips using myTemporalVehicleParameter without a route
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, vehicleParameters);
            // creaste embedded route
            GNERoute* embeddedRoute = new GNERoute(viewNet, flow, RouteParameter(embeddedRouteCopy));
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildDemandElement(flow);
                flow->incRef("buildFlowWithEmbeddedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(embeddedRoute);
                for (const auto& i : embeddedRouteCopy->getParentEdges()) {
                    i->addChildDemandElement(flow);
                }
                // set route as child of flow
                flow->addChildDemandElement(embeddedRoute);
                embeddedRoute->incRef("buildFlowWithEmbeddedRoute");
            }
        }
    }
}


void
GNERouteHandler::buildTrip(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_TRIP);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && ((vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN)) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // add "via" edges in vehicleParameters
            for (const auto& viaEdge : via) {
                vehicleParameters.via.push_back(viaEdge->getID());
            }
            // create trip or flow using tripParameters
            GNEVehicle* trip = new GNEVehicle(viewNet, vType, fromEdge, toEdge, via, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + trip->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, trip);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(trip);
                // set vehicle as child of vType
                vType->addChildDemandElement(trip);
                trip->incRef("buildTrip");
                // add reference in all edges
                fromEdge->addChildDemandElement(trip);
                toEdge->addChildDemandElement(trip);
                for (const auto& viaEdge : via) {
                    viaEdge->addChildDemandElement(trip);
                }
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, trip);
                }
            }
        }
    }
}


void
GNERouteHandler::buildFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_FLOW);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTLANE_SET) && (vehicleParameters.departLaneProcedure == DEPART_LANE_GIVEN) && ((int)fromEdge->getLanes().size() < vehicleParameters.departLane)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTLANE) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departLane) + " is greater than number of lanes");
        } else if (vehicleParameters.wasSet(VEHPARS_DEPARTSPEED_SET) && (vehicleParameters.departSpeedProcedure == DEPART_SPEED_GIVEN) && (vType->getAttributeDouble(SUMO_ATTR_MAXSPEED) < vehicleParameters.departSpeed)) {
            WRITE_ERROR("Invalid " + toString(SUMO_ATTR_DEPARTSPEED) + " used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'. " + toString(vehicleParameters.departSpeed) + " is greater than vType" + toString(SUMO_ATTR_MAXSPEED));
        } else {
            // add "via" edges in vehicleParameters
            for (const auto& viaEdge : via) {
                vehicleParameters.via.push_back(viaEdge->getID());
            }
            // create trip or flow using tripParameters
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, fromEdge, toEdge, via, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, flow);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addChildDemandElement(flow);
                flow->incRef("buildFlow");
                // add reference in all edges
                fromEdge->addChildDemandElement(flow);
                toEdge->addChildDemandElement(flow);
                for (const auto& viaEdge : via) {
                    viaEdge->addChildDemandElement(flow);
                }
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, flow);
                }
            }
        }
    }
}


void
GNERouteHandler::buildStop(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent) {
    // declare pointers to stopping place  and lane and obtain it
    GNEAdditional* stoppingPlace = nullptr;
    GNELane* lane = nullptr;
    SumoXMLTag stopTagType = SUMO_TAG_NOTHING;
    bool validParentDemandElement = true;
    if (stopParameters.busstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stopParameters.busstop, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            stopTagType = SUMO_TAG_PERSONSTOP_BUSSTOP;
        } else {
            stopTagType = SUMO_TAG_STOP_BUSSTOP;
        }
    } else if (stopParameters.containerstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons doesn't support stops over container stops");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_CONTAINERSTOP;
        }
    } else if (stopParameters.chargingStation.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons doesn't support stops over charging stations");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_CHARGINGSTATION;
        }
    } else if (stopParameters.parkingarea.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stopParameters.parkingarea, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            WRITE_ERROR("Persons doesn't support stops over parking areas");
            validParentDemandElement = false;
        } else {
            stopTagType = SUMO_TAG_STOP_PARKINGAREA;
        }
    } else if (stopParameters.lane.size() > 0) {
        lane = viewNet->getNet()->retrieveLane(stopParameters.lane, false);
        // distinguish between stop for vehicles and stops for persons
        if (stopParent->getTagProperty().isPerson()) {
            stopTagType = SUMO_TAG_PERSONSTOP_LANE;
        } else {
            stopTagType = SUMO_TAG_STOP_LANE;
        }
    }
    // first check that parent is valid
    if (validParentDemandElement) {
        // check if values are correct
        if (stoppingPlace && lane) {
            WRITE_ERROR("A stop must be defined either over a stoppingPlace or over a lane");
        } else if (!stoppingPlace && !lane) {
            WRITE_ERROR("A stop requires a stoppingPlace or a lane");
        } else if (stoppingPlace) {
            // create stop using stopParameters and stoppingPlace
            GNEStop* stop = new GNEStop(stopTagType, viewNet, stopParameters, stoppingPlace, stopParent);
            // add it depending of undoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + stop->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(stop);
                stoppingPlace->addChildDemandElement(stop);
                stopParent->addChildDemandElement(stop);
                stop->incRef("buildStoppingPlaceStop");
            }
        } else {
            // create stop using stopParameters and lane
            GNEStop* stop = new GNEStop(viewNet, stopParameters, lane, stopParent);
            // add it depending of undoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + stop->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(stop);
                lane->addChildDemandElement(stop);
                stopParent->addChildDemandElement(stop);
                stop->incRef("buildLaneStop");
            }
        }
    }
}


void
GNERouteHandler::buildPerson(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& personParameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(viewNet, personParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_PTYPE, personParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid person type '" + personParameters.vtypeid + "' used in " + toString(personParameters.tag) + " '" + personParameters.id + "'.");
        } else {
            // create person using personParameters
            GNEPerson* person = new GNEPerson(SUMO_TAG_PERSON, viewNet, pType, personParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + person->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(person, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(person);
                // set person as child of pType and Route
                pType->addChildDemandElement(person);
                person->incRef("buildPerson");
            }
        }
    }
}


void
GNERouteHandler::buildPersonFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& personFlowParameters) {
    // first check if ID is duplicated
    if (!isPersonIdDuplicated(viewNet, personFlowParameters.id)) {
        // obtain routes and vtypes
        GNEDemandElement* pType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_PTYPE, personFlowParameters.vtypeid, false);
        if (pType == nullptr) {
            WRITE_ERROR("Invalid personFlow type '" + personFlowParameters.vtypeid + "' used in " + toString(personFlowParameters.tag) + " '" + personFlowParameters.id + "'.");
        } else {
            // create personFlow using personFlowParameters
            GNEPerson* personFlow = new GNEPerson(SUMO_TAG_PERSONFLOW, viewNet, pType, personFlowParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + personFlow->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(personFlow, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->getAttributeCarriers()->insertDemandElement(personFlow);
                // set personFlow as child of pType and Route
                pType->addChildDemandElement(personFlow);
                personFlow->incRef("buildPersonFlow");
            }
        }
    }
}


void
GNERouteHandler::buildPersonTripFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPos,
                                       const std::vector<std::string>& types, const std::vector<std::string>& modes) {
    // create personTripFromTo
    GNEPersonTrip* personTripFromTo = new GNEPersonTrip(viewNet, personParent, fromEdge, toEdge, {}, arrivalPos, types, modes);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PERSONTRIP_FROMTO) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(personTripFromTo, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrPersonTripFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(personTripFromTo);
        personParent->addChildDemandElement(personTripFromTo);
        personTripFromTo->incRef("buildPersonTripFromTo");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildPersonTripBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop,
                                        const std::vector<std::string>& types, const std::vector<std::string>& modes) {
    // create personTripBusStop
    GNEPersonTrip* personTripBusStop = new GNEPersonTrip(viewNet, personParent, fromEdge, busStop, {}, types, modes);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PERSONTRIP_BUSSTOP) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(personTripBusStop, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrPersonTripFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(personTripBusStop);
        personParent->addChildDemandElement(personTripBusStop);
        busStop->addChildDemandElement(personTripBusStop);
        fromEdge->addChildDemandElement(personTripBusStop);
        personTripBusStop->incRef("buildPersonTripBusStop");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildWalkEdges(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, double arrivalPos) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A walk needs at least one edge. " + toString(SUMO_TAG_WALK_EDGES) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = viewNet->getNet()->getPathCalculator()->calculatePath(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create walkEdges
        GNEWalk* walkEdges = new GNEWalk(viewNet, personParent, pathEdges, arrivalPos);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_EDGES) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(walkEdges, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrWalkEdgesFlow in net and in their vehicle type parent
            viewNet->getNet()->getAttributeCarriers()->insertDemandElement(walkEdges);
            personParent->addChildDemandElement(walkEdges);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addChildDemandElement(walkEdges);
            }
            walkEdges->incRef("buildWalkEdges");
        }
        // update geometry
        personParent->updateGeometry();
    }
}


void
GNERouteHandler::buildWalkFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPos) {
    // create walkFromTo
    GNEWalk* walkFromTo = new GNEWalk(viewNet, personParent, fromEdge, toEdge, {}, arrivalPos);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_FROMTO) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(walkFromTo, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrWalkFromToFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(walkFromTo);
        personParent->addChildDemandElement(walkFromTo);
        // add reference in all edges
        fromEdge->addChildDemandElement(walkFromTo);
        toEdge->addChildDemandElement(walkFromTo);
        walkFromTo->incRef("buildWalkFromTo");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildWalkBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop) {
    // create walkBusStop
    GNEWalk* walkBusStop = new GNEWalk(viewNet, personParent, fromEdge, busStop, {});
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_BUSSTOP) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(walkBusStop, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrWalkBusStopFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(walkBusStop);
        personParent->addChildDemandElement(walkBusStop);
        busStop->addChildDemandElement(walkBusStop);
        fromEdge->addChildDemandElement(walkBusStop);
        walkBusStop->incRef("buildWalkBusStop");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildWalkRoute(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEDemandElement* route, double arrivalPos) {
    // create walkRoute
    GNEWalk* walkRoute = new GNEWalk(viewNet, personParent, route, arrivalPos);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_ROUTE) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(walkRoute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrWalkBusStopFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(walkRoute);
        personParent->addChildDemandElement(walkRoute);
        route->addChildDemandElement(walkRoute);
        walkRoute->incRef("buildWalkRoute");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildRideFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<std::string>& lines, double arrivalPos) {
    // create rideFromTo
    GNERide* rideFromTo = new GNERide(viewNet, personParent, fromEdge, toEdge, {}, arrivalPos, lines);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_RIDE_FROMTO) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(rideFromTo, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrRideFromToFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(rideFromTo);
        personParent->addChildDemandElement(rideFromTo);
        // add reference in all edges
        fromEdge->addChildDemandElement(rideFromTo);
        toEdge->addChildDemandElement(rideFromTo);
        rideFromTo->incRef("buildRideFromTo");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::buildRideBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop, const std::vector<std::string>& lines) {
    // create rideBusStop
    GNERide* rideBusStop = new GNERide(viewNet, personParent, fromEdge, busStop, {}, lines);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_RIDE_BUSSTOP) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(rideBusStop, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrRideBusStopFlow in net and in their vehicle type parent
        viewNet->getNet()->getAttributeCarriers()->insertDemandElement(rideBusStop);
        personParent->addChildDemandElement(rideBusStop);
        busStop->addChildDemandElement(rideBusStop);
        // add reference in first edge
        fromEdge->addChildDemandElement(rideBusStop);
        rideBusStop->incRef("buildRideBusStop");
    }
    // update geometry
    personParent->updateGeometry();
}


void
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // first check that given vehicle isn't already a vehicle
    if (originalVehicle->getTagProperty().getTag() != SUMO_TAG_VEHICLE) {
        // get pointer to undo list (due originalVehicle will be deleted)
        GNEUndoList* undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) &&
                (originalVehicle->getParentDemandElements().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
        }
        // obtain VType of original vehicle
        GNEDemandElement* vType = originalVehicle->getParentDemandElements().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_VEHICLE;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // obtain vehicle's route (it always exist due call to function separateEmbeddedRoute(...)
            GNEDemandElement* route = originalVehicle->getParentDemandElements().at(1);
            // create Vehicle using values of original vehicle
            GNEVehicle* vehicle = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove original vehicle (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new vehicle
            undoList->add(new GNEChange_DemandElement(vehicle, true), true);
            // as last step change vehicle's route to embedded route if createEmbeddedRoute is enabled
            if (createEmbeddedRoute) {
                embebbeRoute(vehicle, undoList);
            }
            // check if vehicle has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(vehicle, vehicle->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create route using values of originalVehicle flow/trip
            GNERoute* route = new GNERoute(originalVehicle->getViewNet(), RouteParameter(originalVehicle));
            // create Vehicle using values of original vehicle (including ID)
            GNEVehicle* vehicle = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove flow/trip (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add both new vehicle and route
            undoList->add(new GNEChange_DemandElement(route, true), true);
            undoList->add(new GNEChange_DemandElement(vehicle, true), true);
            // check if vehicle has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(vehicle, vehicle->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        }
        // end undo-redo operation
        undoList->p_end();
    }
}


void
GNERouteHandler::transformToRouteFlow(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // first check that given vehicle isn't already a routeflow
    if (originalVehicle->getTagProperty().getTag() != SUMO_TAG_ROUTEFLOW) {
        // get pointer to undo list (due originalVehicle will be deleted)
        GNEUndoList* undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_ROUTEFLOW));
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) &&
                (originalVehicle->getParentDemandElements().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
        }
        // obtain VType of original vehicle
        GNEDemandElement* vType = originalVehicle->getParentDemandElements().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // obtain vehicle's route (it always exist due call to function separateEmbeddedRoute(...)
            GNEDemandElement* route = originalVehicle->getParentDemandElements().at(1);
            // create flow using newVehicleParameters
            GNEVehicle* flow = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove original vehicle (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new vehicle
            undoList->add(new GNEChange_DemandElement(flow, true), true);
            // as last step change vehicle's route to embedded route if createEmbeddedRoute is enabled
            if (createEmbeddedRoute) {
                embebbeRoute(flow, undoList);
            }
            // check if flow has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(flow, flow->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create route using values of originalVehicle flow/trip
            GNERoute* route = new GNERoute(originalVehicle->getViewNet(), RouteParameter(originalVehicle));
            // create flow using values of original vehicle (including ID)
            GNEVehicle* flow = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove flow/trip (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add both new vehicle and route
            undoList->add(new GNEChange_DemandElement(route, true), true);
            undoList->add(new GNEChange_DemandElement(flow, true), true);
            // check if flow has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(flow, flow->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        }
        // end undo-redo operation
        undoList->p_end();
    }
}


void
GNERouteHandler::transformToTrip(GNEVehicle* originalVehicle) {
    // first check that given vehicle isn't already a trip
    if (originalVehicle->getTagProperty().getTag() != SUMO_TAG_TRIP) {
        // get pointer to undo list (due originalVehicle will be deleted)
        GNEUndoList* undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
        // declare pointer to get embedded route if is created
        GNEDemandElement* separatedEmbeddedRoute = nullptr;
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) &&
                (originalVehicle->getParentDemandElements().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
        }
        // obtain VType of original vehicle
        GNEDemandElement* vType = originalVehicle->getParentDemandElements().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_TRIP;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // create trip using values of original vehicle (including ID) and route's edges
            GNEVehicle* trip = new GNEVehicle(originalVehicle->getViewNet(), vType,
                                              originalVehicle->getParentDemandElements().at(1)->getParentEdges().front(),
                                              originalVehicle->getParentDemandElements().at(1)->getParentEdges().back(),
                                              originalVehicle->getParentDemandElements().at(1)->getMiddleParentEdges(),
                                              newVehicleParameters);
            // first remove vehicle (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new vehicle
            undoList->add(new GNEChange_DemandElement(trip, true), true);
            // check if trip has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(trip, trip->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create trip using values of original vehicle (including ID)
            GNEVehicle* trip = new GNEVehicle(originalVehicle->getViewNet(), vType,
                                              originalVehicle->getParentEdges().front(),
                                              originalVehicle->getParentEdges().back(),
                                              originalVehicle->getMiddleParentEdges(),
                                              newVehicleParameters);
            // remove originalVehicle
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new trip
            undoList->add(new GNEChange_DemandElement(trip, true), true);
            // check if trip has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(trip, trip->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        }
        // check if separatedEmbeddedRoute has to be removed
        if (separatedEmbeddedRoute) {
            undoList->add(new GNEChange_DemandElement(separatedEmbeddedRoute, false), true);
        }
        // end undo-redo operation
        undoList->p_end();
    }
}


void
GNERouteHandler::transformToFlow(GNEVehicle* originalVehicle) {
    // first check that given vehicle isn't already a flow
    if (originalVehicle->getTagProperty().getTag() != SUMO_TAG_FLOW) {
        // get pointer to undo list (due originalVehicle will be deleted)
        GNEUndoList* undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
        // declare pointer to get embedded route if is created
        GNEDemandElement* separatedEmbeddedRoute = nullptr;
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) &&
                (originalVehicle->getParentDemandElements().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
            separatedEmbeddedRoute = originalVehicle->getParentDemandElements().at(1);
        }
        // obtain VType of original vehicle
        GNEDemandElement* vType = originalVehicle->getParentDemandElements().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_FLOW;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // create Vehicle using values of original vehicle (including ID) and route's edges
            GNEVehicle* flow = new GNEVehicle(originalVehicle->getViewNet(), vType,
                                              originalVehicle->getParentDemandElements().at(1)->getParentEdges().front(),
                                              originalVehicle->getParentDemandElements().at(1)->getParentEdges().back(),
                                              originalVehicle->getParentDemandElements().at(1)->getMiddleParentEdges(),
                                              newVehicleParameters);
            // first remove vehicle (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new flow
            undoList->add(new GNEChange_DemandElement(flow, true), true);
        } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create flow using values of original vehicle (including ID)
            GNEVehicle* flow = new GNEVehicle(originalVehicle->getViewNet(), vType,
                                              originalVehicle->getParentEdges().front(),
                                              originalVehicle->getParentEdges().back(),
                                              originalVehicle->getMiddleParentEdges(),
                                              newVehicleParameters);
            // remove originalVehicle
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add new flow
            undoList->add(new GNEChange_DemandElement(flow, true), true);
            // check if flow has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(flow, flow->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        }
        // check if separatedEmbeddedRoute has to be removed
        if (separatedEmbeddedRoute) {
            undoList->add(new GNEChange_DemandElement(separatedEmbeddedRoute, false), true);
        }
        // end undo-redo operation
        undoList->p_end();
    }
}


void
GNERouteHandler::transformToPerson(GNEPerson* /*originalPerson*/) {
    //
}


void
GNERouteHandler::transformToPersonFlow(GNEPerson* /*originalPerson*/) {
    //
}


void
GNERouteHandler::setFlowParameters(const SumoXMLAttr attribute, int& parameters) {
    // modify parametersSetCopy depending of given Flow attribute
    switch (attribute) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parameters = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parameters ^= VEHPARS_END_SET;
            parameters |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parameters |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set period
            parameters |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parameters & VEHPARS_END_SET) && (parameters & VEHPARS_NUMBER_SET)) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_END_SET) {
                parameters = VEHPARS_END_SET;
            } else if (parameters & VEHPARS_NUMBER_SET) {
                parameters = VEHPARS_NUMBER_SET;
            }
            // set probability
            parameters |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
}

// ===========================================================================
// protected
// ===========================================================================

void
GNERouteHandler::embebbeRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // create a copy of vehicle with the same attributes but without embedded route
    GNEVehicle* vehicleWithEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getParentDemandElements().at(0), *vehicle);
    // create a embeddedRoute based on parameters of vehicle's route
    GNERoute* embeddedRoute = new GNERoute(vehicleWithEmbebbeRoute->getViewNet(), vehicleWithEmbebbeRoute, RouteParameter(vehicle->getParentDemandElements().at(1)));
    // remove vehicle, but NOT route
    undoList->add(new GNEChange_DemandElement(vehicle, false), true);
    // now add bot vehicleWithEmbebbeRoute and embeddedRoute
    undoList->add(new GNEChange_DemandElement(vehicleWithEmbebbeRoute, true), true);
    undoList->add(new GNEChange_DemandElement(embeddedRoute, true), true);
}


GNEVehicle*
GNERouteHandler::separateEmbeddedRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // first create a Route based on the parameters of vehicle's embedded route
    GNERoute* nonEmbeddedRoute = new GNERoute(vehicle->getChildDemandElements().at(0));
    // create a copy of vehicle with the same attributes but with the nonEmbeddedRoute
    GNEVehicle* vehicleWithoutEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getParentDemandElements().at(0), nonEmbeddedRoute, *vehicle);
    // remove embedded route andvehicle (because a embebbbed route without vehicle cannot exist)
    undoList->add(new GNEChange_DemandElement(vehicle->getChildDemandElements().at(0), false), true);
    undoList->add(new GNEChange_DemandElement(vehicle, false), true);
    // now add bot nonEmbeddedRoute and vehicleWithoutEmbebbeRoute
    undoList->add(new GNEChange_DemandElement(nonEmbeddedRoute, true), true);
    undoList->add(new GNEChange_DemandElement(vehicleWithoutEmbebbeRoute, true), true);
    // return vehicleWithoutEmbebbeRoute
    return vehicleWithoutEmbebbeRoute;
}


void
GNERouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::closeVehicleTypeDistribution() {
    // currently unused
}


void
GNERouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    // change abort flag
    myAbort = false;
    // check if route has ID (due embebbed routes)
    if (attrs.hasAttribute(SUMO_ATTR_ID)) {
        myRouteParameter.routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE, SUMO_ATTR_ID, myAbort);
        myRouteParameter.loadedID = true;
    } else {
        myRouteParameter.routeID.clear();
        myRouteParameter.loadedID = false;
    }
    // parse other attributes
    myRouteParameter.setEdges(myViewNet, GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myRouteParameter.routeID, SUMO_TAG_ROUTE, SUMO_ATTR_EDGES, myAbort));
    myRouteParameter.color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, myRouteParameter.routeID, SUMO_TAG_ROUTE, SUMO_ATTR_COLOR, myAbort);
}


void
GNERouteHandler::openFlow(const SUMOSAXAttributes& attrs) {
    // change abort flag
    myAbort = false;
    // parse flow attributes
    myRouteParameter.setEdges(myViewNet, myVehicleParameter->id,
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_FROM, myAbort),
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_TO, myAbort),
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_VIA, myAbort));
}


void
GNERouteHandler::openTrip(const SUMOSAXAttributes& attrs) {
    // change abort flag
    myAbort = false;
    // parse trips attributes
    myRouteParameter.setEdges(myViewNet, myVehicleParameter->id,
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_FROM, myAbort),
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_TO, myAbort),
                              GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myVehicleParameter->id, SUMO_TAG_TRIP, SUMO_ATTR_VIA, myAbort));
}


void
GNERouteHandler::closeRoute(const bool /* mayBeDisconnected */) {
    // first copy parameters from SUMORouteHanlder to myRouteParameter
    myRouteParameter.parameters = myLoadedParameterised;
    // clear loaded parameters after set
    myLoadedParameterised.clearParameter();
    // we have two possibilities: Either create a route with their own ID, or create a route within a vehicle
    if (myVehicleParameter) {
        // extra warning for embebbed routes
        if (myRouteParameter.loadedID) {
            WRITE_WARNING("Attribute ID ignored in embedded routes");
        }
        // check edges
        if (myRouteParameter.edges.size() == 0) {
            WRITE_ERROR("A route needs at least one edge. Vehicle with ID='" + myVehicleParameter->id + "' cannot be created");
        } else {
            // obtain vType
            GNEDemandElement* vType = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, myVehicleParameter->vtypeid, false);
            if (vType == nullptr) {
                WRITE_ERROR("Invalid vehicle type '" + myVehicleParameter->vtypeid + "' used in " + toString(myVehicleParameter->tag) + " '" + myVehicleParameter->id + "'.");
            } else {
                // generate a new route ID and add it to myVehicleParameter
                myVehicleParameter->routeid = myViewNet->getNet()->generateDemandElementID(myVehicleParameter->id, SUMO_TAG_ROUTE);
                // due vehicle was loaded without a route, change tag
                myVehicleParameter->tag = (myVehicleParameter->tag == SUMO_TAG_FLOW) ? SUMO_TAG_ROUTEFLOW : SUMO_TAG_VEHICLE;
                // create vehicle or trips using myTemporalVehicleParameter without a route
                myLoadedVehicleWithEmbebbedRoute = new GNEVehicle(myViewNet, vType, *myVehicleParameter);
                // creaste embedded route
                GNERoute* embeddedRoute = new GNERoute(myViewNet, myLoadedVehicleWithEmbebbedRoute, myRouteParameter);
                // add both to net depending of myUndoDemandElements
                if (myUndoDemandElements) {
                    myViewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                    // add both in net using undoList
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(myLoadedVehicleWithEmbebbedRoute, true), true);
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
                    // iterate over stops of myActiveRouteStops and create stops associated with it
                    for (const auto& i : myActiveRouteStops) {
                        buildStop(myViewNet, true, i, myLoadedVehicleWithEmbebbedRoute);
                    }
                } else {
                    // add vehicleOrRouteFlow in net and in their vehicle type parent
                    myViewNet->getNet()->getAttributeCarriers()->insertDemandElement(myLoadedVehicleWithEmbebbedRoute);
                    vType->addChildDemandElement(myLoadedVehicleWithEmbebbedRoute);
                    myLoadedVehicleWithEmbebbedRoute->incRef("buildVehicleAndRoute");
                    // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                    myViewNet->getNet()->getAttributeCarriers()->insertDemandElement(embeddedRoute);
                    for (const auto& i : myRouteParameter.edges) {
                        i->addChildDemandElement(myLoadedVehicleWithEmbebbedRoute);
                    }
                    myLoadedVehicleWithEmbebbedRoute->addChildDemandElement(embeddedRoute);
                    embeddedRoute->incRef("buildVehicleAndRoute");
                    // iterate over stops of myActiveRouteStops and create stops associated with it
                    for (const auto& i : myActiveRouteStops) {
                        buildStop(myViewNet, false, i, myLoadedVehicleWithEmbebbedRoute);
                    }
                }
            }
        }
    } else if (myRouteParameter.loadedID == false) {
        WRITE_ERROR(toString(SUMO_TAG_ROUTE) + " needs a valid ID.");
    } else if (!SUMOXMLDefinitions::isValidVehicleID(myRouteParameter.routeID)) {
        WRITE_ERROR(toString(SUMO_TAG_ROUTE) + " ID='" + myRouteParameter.routeID + "' contains invalid characters.");
    } else if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteParameter.routeID, false) != nullptr) {
        WRITE_ERROR("There is another " + toString(SUMO_TAG_ROUTE) + " with the same ID='" + myRouteParameter.routeID + "'.");
    } else if (myRouteParameter.edges.size() == 0) {
        WRITE_ERROR("A route needs at least one edge.");
    } else {
        // creaste GNERoute
        GNERoute* route = new GNERoute(myViewNet, myRouteParameter);
        if (myUndoDemandElements) {
            myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
            myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            // iterate over stops of myActiveRouteStops and create stops associated with it
            for (const auto& i : myActiveRouteStops) {
                buildStop(myViewNet, true, i, route);
            }
            myViewNet->getUndoList()->p_end();
        } else {
            myViewNet->getNet()->getAttributeCarriers()->insertDemandElement(route);
            for (const auto& i : myRouteParameter.edges) {
                i->addChildDemandElement(route);
            }
            route->incRef("buildRoute");
            // iterate over stops of myActiveRouteStops and create stops associated with it
            for (const auto& i : myActiveRouteStops) {
                buildStop(myViewNet, false, i, route);
            }
        }
    }
}


void
GNERouteHandler::openRouteDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::closeRouteDistribution() {
    // currently unused
}


void
GNERouteHandler::closeVehicle() {
    // first check if we're closing a vehicle with embebbed routes and stops
    if (myLoadedVehicleWithEmbebbedRoute) {
        myLoadedVehicleWithEmbebbedRoute = nullptr;
        // check if current command group size has to be ended
        if (myViewNet->getUndoList()->currentCommandGroupSize() > 0) {
            myViewNet->getUndoList()->p_end();
        }
    } else {
        // now check if myVehicleParameter was sucesfully created
        if (myVehicleParameter) {
            // build vehicle over route
            buildVehicleOverRoute(myViewNet, myUndoDemandElements, *myVehicleParameter);
        }
    }
}


void
GNERouteHandler::closeVType() {
    // first check that VType was sucesfully created
    if (myCurrentVType) {
        // first check if we're creating a vType or a pType
        SumoXMLTag vTypeTag = (myCurrentVType->vehicleClass == SVC_PEDESTRIAN) ? SUMO_TAG_PTYPE : SUMO_TAG_VTYPE;
        // check if loaded vType/pType is a default vtype
        if ((myCurrentVType->id == DEFAULT_VTYPE_ID) || (myCurrentVType->id == DEFAULT_PEDTYPE_ID) || (myCurrentVType->id == DEFAULT_BIKETYPE_ID)) {
            // overwrite default vehicle type
            GNEVehicleType::overwriteVType(myViewNet->getNet()->retrieveDemandElement(vTypeTag, myCurrentVType->id, false), myCurrentVType, myViewNet->getUndoList());
        } else if (myViewNet->getNet()->retrieveDemandElement(vTypeTag, myCurrentVType->id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(vTypeTag) + " with the same ID='" + myCurrentVType->id + "'.");
        } else {
            // create vType/pType using myCurrentVType
            GNEVehicleType* vType = new GNEVehicleType(myViewNet, *myCurrentVType, vTypeTag);
            if (myUndoDemandElements) {
                myViewNet->getUndoList()->p_begin("add " + vType->getTagStr());
                myViewNet->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
                myViewNet->getUndoList()->p_end();
            } else {
                myViewNet->getNet()->getAttributeCarriers()->insertDemandElement(vType);
                vType->incRef("buildVType");
            }
        }
    }
}


void
GNERouteHandler::closePerson() {
    // first check if myVehicleParameter was sucesfully created
    if (myVehicleParameter) {
        // first check if ID is duplicated
        if (!isPersonIdDuplicated(myViewNet, myVehicleParameter->id)) {
            // obtain ptype
            GNEDemandElement* pType = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_PTYPE, myVehicleParameter->vtypeid, false);
            if (pType == nullptr) {
                WRITE_ERROR("Invalid person type '" + myVehicleParameter->vtypeid + "' used in " + toString(myVehicleParameter->tag) + " '" + myVehicleParameter->vtypeid + "'.");
            } else {
                // declare flag to abort person plans creation
                bool abortPersonPlans = false;
                // create person using personParameters
                GNEPerson* person = new GNEPerson(SUMO_TAG_PERSON, myViewNet, pType, *myVehicleParameter);
                // begin undo-list creation
                myViewNet->getUndoList()->p_begin("add " + person->getTagStr());
                // add person
                myViewNet->getUndoList()->add(new GNEChange_DemandElement(person, true), true);
                // iterate over all personplan childs and add it
                for (auto i = myPersonPlanValues.begin(); (i != myPersonPlanValues.end()) && !abortPersonPlans; i++) {
                    switch (i->tag) {
                        case SUMO_TAG_PERSONTRIP_FROMTO:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from && i->to) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEPersonTrip(myViewNet, person, i->from, i->to, {}, i->arrivalPos, i->vTypes, i->modes), true), true);
                            } else if ((i != myPersonPlanValues.begin()) && i->to) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEPersonTrip(myViewNet, person, i->from, i->to, {}, i->arrivalPos, i->vTypes, i->modes), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_PERSONTRIP_BUSSTOP:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEPersonTrip(myViewNet, person, i->from, i->busStop, {}, i->vTypes, i->modes), true), true);
                            } else if (i != myPersonPlanValues.begin()) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEPersonTrip(myViewNet, person, i->from, i->busStop, {}, i->vTypes, i->modes), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_RIDE_FROMTO:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from && i->to) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNERide(myViewNet, person, i->from, i->to, {}, i->arrivalPos, i->lines), true), true);
                            } else if ((i != myPersonPlanValues.begin()) && i->to) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNERide(myViewNet, person, i->from, i->to, {}, i->arrivalPos, i->lines), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_RIDE_BUSSTOP:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNERide(myViewNet, person, i->from, i->busStop, {}, i->lines), true), true);
                            } else if (i != myPersonPlanValues.begin()) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNERide(myViewNet, person, i->from, i->busStop, {}, i->lines), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_WALK_EDGES:
                            myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->edges, i->arrivalPos), true), true);
                            break;
                        case SUMO_TAG_WALK_FROMTO:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from && i->to) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->from, i->to, {}, i->arrivalPos), true), true);
                            } else if ((i != myPersonPlanValues.begin()) && i->to) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->from, i->to, {}, i->arrivalPos), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_WALK_BUSSTOP:
                            // check if "from" attribute was loaded, or it must be taked fron previous personPlan values
                            if (i->from) {
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->from, i->busStop, {}), true), true);
                            } else if (i != myPersonPlanValues.begin()) {
                                // update 'from' edge using 'to' edge of last personPlan element
                                i->from = (i - 1)->getLastEdge();
                                myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->from, i->busStop, {}), true), true);
                            } else {
                                WRITE_ERROR("The first person plan of type '" + toString(i->tag) + "' needs a from edge. Person cannot be created.");
                                // abort last command group (to remove created person)
                                myViewNet->getUndoList()->p_abortLastCommandGroup();
                                // abort person plan creation
                                abortPersonPlans = true;
                            }
                            break;
                        case SUMO_TAG_WALK_ROUTE:
                            myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEWalk(myViewNet, person, i->route, i->arrivalPos), true), true);
                            break;
                        case SUMO_TAG_PERSONSTOP_LANE:
                            myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEStop(myViewNet, i->stopParameters, i->laneStop, person), true), true);
                            break;
                        case SUMO_TAG_PERSONSTOP_BUSSTOP:
                            myViewNet->getUndoList()->add(new GNEChange_DemandElement(new GNEStop(i->tag, myViewNet, i->stopParameters, i->busStop, person), true), true);
                            break;
                        default:
                            break;
                    }
                }
                // end undo-list depending of abortPersonPlans
                if (!abortPersonPlans) {
                    myViewNet->getUndoList()->p_end();
                }
            }
        }

    }
    // clear person plan values clear
    myPersonPlanValues.clear();
}

void
GNERouteHandler::closePersonFlow() {
    // first check if myVehicleParameter was sucesfully created
    if (myVehicleParameter) {
        // build person flow
        buildPersonFlow(myViewNet, myUndoDemandElements, *myVehicleParameter);
    }
}

void
GNERouteHandler::closeContainer() {
    // currently unused
}


void
GNERouteHandler::closeFlow() {
    // first check if myVehicleParameter was sucesfully created
    if (myVehicleParameter) {
        // check if we're creating a flow or a routeFlow over route
        if (myRouteParameter.edges.size() > 1) {
            // extract via edges
            std::vector<GNEEdge*> viaEdges;
            for (int i = 1; i < ((int)myRouteParameter.edges.size() - 1); i++) {
                viaEdges.push_back(myRouteParameter.edges.at(i));
            }
            // build flow
            buildFlow(myViewNet, true, *myVehicleParameter, myRouteParameter.edges.front(), myRouteParameter.edges.back(), viaEdges);
        } else if (myRouteParameter.edges.size() == 0) {
            // build flow over route
            buildFlowOverRoute(myViewNet, myUndoDemandElements, *myVehicleParameter);
        }
    }
}


void
GNERouteHandler::closeTrip() {
    // first check if myVehicleParameter was sucesfully created
    if (myVehicleParameter && (myRouteParameter.edges.size() > 1)) {
        // force reroute
        myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
        // extract via edges
        std::vector<GNEEdge*> viaEdges;
        for (int i = 1; i < ((int)myRouteParameter.edges.size() - 1); i++) {
            viaEdges.push_back(myRouteParameter.edges.at(i));
        }
        // build trip
        buildTrip(myViewNet, true, *myVehicleParameter, myRouteParameter.edges.front(), myRouteParameter.edges.back(), viaEdges);
    }
}


void
GNERouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    // declare a personStop
    PersonPlansValues stop;
    std::string errorSuffix;
    if (myVehicleParameter != nullptr) {
        errorSuffix = " in " + toString(myVehicleParameter->tag) + " '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    }
    // try to parse stop
    myAbort = parseStop(stop.stopParameters, attrs, errorSuffix, MsgHandler::getErrorInstance());
    if (!myAbort) {
        return;
    }
    // try to parse the assigned bus stop
    if (stop.stopParameters.busstop != "") {
        // ok, we have a busStop
        GNEBusStop* bs = dynamic_cast<GNEBusStop*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stop.stopParameters.busstop, false));
        if (bs == nullptr) {
            WRITE_ERROR(toString(SUMO_TAG_BUS_STOP) + " '" + stop.stopParameters.busstop + "' is not known" + errorSuffix);
            return;
        }
        // save lane
        stop.stopParameters.lane = bs->getAttribute(SUMO_ATTR_LANE);
        // save stoping place in stop
        stop.busStop = bs;
        // set tag
        stop.tag = SUMO_TAG_STOP_BUSSTOP;
        // special case for persons
        if ((myVehicleParameter != nullptr) && ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW))) {
            stop.tag = SUMO_TAG_PERSONSTOP_BUSSTOP;
        }
    } else if (stop.stopParameters.containerstop != "") {
        // special case for persons
        if ((myVehicleParameter != nullptr) && ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW))) {
            WRITE_ERROR("Persons don't support " + toString(SUMO_TAG_CONTAINER_STOP) + "s");
            return;
        }
        // ok, we have a containerStop
        GNEContainerStop* cs = dynamic_cast<GNEContainerStop*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stop.stopParameters.containerstop, false));
        if (cs == nullptr) {
            WRITE_ERROR(toString(SUMO_TAG_CONTAINER_STOP) + " '" + stop.stopParameters.containerstop + "' is not known" + errorSuffix);
            return;
        }
        // save lane
        stop.stopParameters.lane = cs->getAttribute(SUMO_ATTR_LANE);
        // save stoping place in stop
        stop.containerStop = cs;
        // set tag
        stop.tag = SUMO_TAG_STOP_CONTAINERSTOP;

    } else if (stop.stopParameters.chargingStation != "") {
        // special case for persons
        if ((myVehicleParameter != nullptr) && ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW))) {
            WRITE_ERROR("Persons don't support " + toString(SUMO_TAG_CHARGING_STATION) + "s");
            return;
        }
        // ok, we have a chargingStation
        GNEChargingStation* cs = dynamic_cast<GNEChargingStation*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stop.stopParameters.chargingStation, false));
        if (cs == nullptr) {
            WRITE_ERROR(toString(SUMO_TAG_CHARGING_STATION) + " '" + stop.stopParameters.chargingStation + "' is not known" + errorSuffix);
            return;
        }
        // save lane
        stop.stopParameters.lane = cs->getAttribute(SUMO_ATTR_LANE);
        // save stoping place in stop
        stop.chargingStation = cs;
        // set tag
        stop.tag = SUMO_TAG_STOP_CHARGINGSTATION;
    } else if (stop.stopParameters.parkingarea != "") {
        // special case for persons
        if ((myVehicleParameter != nullptr) && ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW))) {
            WRITE_ERROR("Persons don't support " + toString(SUMO_TAG_PARKING_AREA) + "s");
            return;
        }
        // ok, we have a parkingArea
        GNEParkingArea* pa = dynamic_cast<GNEParkingArea*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stop.stopParameters.parkingarea, false));
        if (pa == nullptr) {
            WRITE_ERROR(toString(SUMO_TAG_PARKING_AREA) + " '" + stop.stopParameters.parkingarea + "' is not known" + errorSuffix);
            return;
        }
        // save lane
        stop.stopParameters.lane = pa->getAttribute(SUMO_ATTR_LANE);
        // save stoping place in stop
        stop.parkingArea = pa;
        // set tag
        stop.tag = SUMO_TAG_STOP_PARKINGAREA;
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.stopParameters.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, nullptr, myAbort, "");
        stop.laneStop = myViewNet->getNet()->retrieveLane(stop.stopParameters.lane, false);
        // check if lane is valid
        if (myAbort && stop.stopParameters.lane != "") {
            if (stop.laneStop == nullptr) {
                WRITE_ERROR("The lane '" + stop.stopParameters.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            WRITE_ERROR("A stop must be placed over a " + toString(SUMO_TAG_BUS_STOP) + ", a " + toString(SUMO_TAG_CONTAINER_STOP) +
                        ", a " + toString(SUMO_TAG_CHARGING_STATION) + ", a " + toString(SUMO_TAG_PARKING_AREA) + " or a " + toString(SUMO_TAG_LANE) + errorSuffix);
            return;
        }
        // calculate start and end position
        stop.stopParameters.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, nullptr, myAbort, stop.laneStop->getLaneParametricLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_ERROR("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.stopParameters.endPos = attrs.getOpt<double>(SUMO_ATTR_POSITION, nullptr, myAbort, stop.stopParameters.endPos);
        }
        stop.stopParameters.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, nullptr, myAbort, MAX2(0., stop.stopParameters.endPos - 2 * POSITION_EPS));
        stop.stopParameters.friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, myAbort, false);
        if (!myAbort || (checkStopPos(stop.stopParameters.startPos, stop.stopParameters.endPos, stop.laneStop->getLaneParametricLength(), POSITION_EPS, stop.stopParameters.friendlyPos) != STOPPOS_VALID)) {
            WRITE_ERROR("Invalid start or end position for stop on lane '" + stop.stopParameters.lane + "'" + errorSuffix);
            return;
        }
        // set tag
        stop.tag = SUMO_TAG_STOP_LANE;
        // special case for persons
        if ((myVehicleParameter != nullptr) && ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW))) {
            stop.tag = SUMO_TAG_PERSONSTOP_LANE;
        }
    }
    // now create or store stop
    if (myLoadedVehicleWithEmbebbedRoute) {
        buildStop(myViewNet, true, stop.stopParameters, myLoadedVehicleWithEmbebbedRoute);
    } else if (myVehicleParameter != nullptr) {
        if ((myVehicleParameter->tag == SUMO_TAG_PERSON) || (myVehicleParameter->tag == SUMO_TAG_PERSONFLOW)) {
            myPersonPlanValues.push_back(stop);
        } else {
            myVehicleParameter->stops.push_back(stop.stopParameters);
        }
    } else {
        myActiveRouteStops.push_back(stop.stopParameters);
    }
}


void
GNERouteHandler::addPersonTrip(const SUMOSAXAttributes& attrs) {
    // SUMORouteHandler handle certain walks configurations as PersonTrips, then it needs a manually call to addWalk(...)
    if (attrs.getObjectType() == "walk") {
        addWalk(attrs);
    } else {
        // change abort flag
        myAbort = false;
        // declare value for saving loaded values
        PersonPlansValues personTripValuesLoaded;
        // first set tag
        if (attrs.hasAttribute(SUMO_ATTR_TO)) {
            // set tag
            personTripValuesLoaded.tag = SUMO_TAG_PERSONTRIP_FROMTO;
            // declare a flag to check if values are valid
            bool validValues = true;
            // extract rest of parameters
            if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
                personTripValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
                if (personTripValuesLoaded.from == nullptr) {
                    WRITE_ERROR("Invalid edge from in " + toString(personTripValuesLoaded.tag));
                    validValues = false;
                }
            }
            personTripValuesLoaded.to = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_TO, myAbort), false);
            personTripValuesLoaded.vTypes = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_VTYPES, myAbort);
            personTripValuesLoaded.modes = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_MODES, myAbort);
            if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
                personTripValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
            }
            // check that all parameters are correct
            if (personTripValuesLoaded.to == nullptr) {
                WRITE_ERROR("Invalid edge to in " + toString(personTripValuesLoaded.tag));
                validValues = false;
            }
            // check modes
            for (const auto& i : personTripValuesLoaded.modes) {
                if ((i != "public") && (i != "car") && (i != "bicycle")) {
                    validValues = false;
                }
            }
            if (validValues) {
                // remove duplicated modes
                std::sort(personTripValuesLoaded.modes.begin(), personTripValuesLoaded.modes.end());
                personTripValuesLoaded.modes.erase(unique(personTripValuesLoaded.modes.begin(), personTripValuesLoaded.modes.end()), personTripValuesLoaded.modes.end());
            } else {
                WRITE_ERROR("A person trip mode can be only a combination of 'public', 'car' or 'bicycle'");
            }
            for (const auto& i : personTripValuesLoaded.vTypes) {
                if (!SUMOXMLDefinitions::isValidTypeID(i)) {
                    WRITE_ERROR("Invalid vehicle type '" + i + "' used in " + toString(personTripValuesLoaded.tag));
                    validValues = false;
                }
            }
            // save loaded values in container only if all parameters are valid
            if (validValues) {
                myPersonPlanValues.push_back(personTripValuesLoaded);
            }
        } else if (attrs.hasAttribute(SUMO_ATTR_BUS_STOP)) {
            // set tag
            personTripValuesLoaded.tag = SUMO_TAG_PERSONTRIP_BUSSTOP;
            // declare a flag to check if values are valid
            bool validValues = true;
            // extract rest of parameters
            if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
                personTripValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
                if (personTripValuesLoaded.from == nullptr) {
                    WRITE_ERROR("Invalid edge from in " + toString(personTripValuesLoaded.tag));
                    validValues = false;
                }
            }
            personTripValuesLoaded.busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_BUS_STOP, myAbort), false);
            personTripValuesLoaded.vTypes = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_VTYPES, myAbort);
            personTripValuesLoaded.modes = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_MODES, myAbort);
            if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
                personTripValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", personTripValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
            }
            // check that all parameters are correct
            if (personTripValuesLoaded.busStop == nullptr) {
                WRITE_ERROR("Invalid busStop to in " + toString(personTripValuesLoaded.tag));
                validValues = false;
            }
            // check modes
            for (const auto& i : personTripValuesLoaded.modes) {
                if ((i != "public") && (i != "car") && (i != "bicycle")) {
                    validValues = false;
                }
            }
            if (validValues) {
                // remove duplicated modes
                std::sort(personTripValuesLoaded.modes.begin(), personTripValuesLoaded.modes.end());
                personTripValuesLoaded.modes.erase(unique(personTripValuesLoaded.modes.begin(), personTripValuesLoaded.modes.end()), personTripValuesLoaded.modes.end());
            } else {
                WRITE_ERROR("A person trip mode can be only a combination of 'public', 'car' or 'bicycle'");
            }
            for (const auto& i : personTripValuesLoaded.vTypes) {
                if (!SUMOXMLDefinitions::isValidTypeID(i)) {
                    WRITE_ERROR("Invalid vehicle type '" + i + "' used in " + toString(personTripValuesLoaded.tag));
                    validValues = false;
                }
            }
            // save loaded values in container only if all parameters are valid
            if (validValues) {
                myPersonPlanValues.push_back(personTripValuesLoaded);
            }
        } else {
            WRITE_ERROR("A personTrip requires either a from-to edges or a from edge and a busStop");
        }
    }
}


void
GNERouteHandler::addWalk(const SUMOSAXAttributes& attrs) {
    // change abort flag
    myAbort = false;
    // declare value for saving loaded values
    PersonPlansValues walkValuesLoaded;
    // first set tag
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        // set tag
        walkValuesLoaded.tag = SUMO_TAG_WALK_EDGES;
        // parse edges
        std::string edgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_EDGES, myAbort);
        if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs, true)) {
            walkValuesLoaded.edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs);
        }
        // extract rest of parameters
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            walkValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check that all parameters are correct
        if (walkValuesLoaded.edges.empty()) {
            WRITE_ERROR("Invalid edges of " + toString(walkValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(walkValuesLoaded);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_TO)) {
        // set tag
        walkValuesLoaded.tag = SUMO_TAG_WALK_FROMTO;
        // extract rest of parameters
        if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
            walkValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
            if (walkValuesLoaded.from == nullptr) {
                WRITE_ERROR("Invalid edge from in " + toString(walkValuesLoaded.tag));
            }
        }
        walkValuesLoaded.to = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_TO, myAbort), false);
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            walkValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check that all parameters are correct
        if (walkValuesLoaded.to == nullptr) {
            WRITE_ERROR("Invalid edge to in " + toString(walkValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(walkValuesLoaded);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_BUS_STOP)) {
        // set tag
        walkValuesLoaded.tag = SUMO_TAG_WALK_BUSSTOP;
        // extract rest of parameters
        if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
            walkValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
            if (walkValuesLoaded.from == nullptr) {
                WRITE_ERROR("Invalid edge from in " + toString(walkValuesLoaded.tag));
            }
        }
        walkValuesLoaded.busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_BUS_STOP, myAbort), false);
        // use edge of busstop's lane as to edge
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            walkValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check that all parameters are correct
        if (walkValuesLoaded.busStop == nullptr) {
            WRITE_ERROR("Invalid busStop to in " + toString(walkValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(walkValuesLoaded);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        // set tag
        walkValuesLoaded.tag = SUMO_TAG_WALK_ROUTE;
        // extract rest of parameters
        walkValuesLoaded.route = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_ROUTE, myAbort), false);
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            walkValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", walkValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check that all parameters are correct
        if (walkValuesLoaded.route == nullptr) {
            WRITE_ERROR("Invalid route from in " + toString(walkValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(walkValuesLoaded);
        }
    } else {
        WRITE_ERROR("A walk requires either a from-to edges, a from edge and a busStop or a route");
    }
}


void
GNERouteHandler::addPerson(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addContainer(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addRide(const SUMOSAXAttributes& attrs) {
    // change abort flag
    myAbort = false;
    // declare value for saving loaded values
    PersonPlansValues rideValuesLoaded;
    // first set tag
    if (attrs.hasAttribute(SUMO_ATTR_TO)) {
        // set tag
        rideValuesLoaded.tag = SUMO_TAG_RIDE_FROMTO;
        // extract rest of parameters
        if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
            rideValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
            if (rideValuesLoaded.from == nullptr) {
                WRITE_ERROR("Invalid edge from in " + toString(rideValuesLoaded.tag));
            }
        }
        rideValuesLoaded.to = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_TO, myAbort), false);
        rideValuesLoaded.lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_LINES, myAbort);
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            rideValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check lines
        if (rideValuesLoaded.lines.empty()) {
            rideValuesLoaded.lines.push_back("ANY");
        }
        // check that all parameters are correct
        if (rideValuesLoaded.to == nullptr) {
            WRITE_ERROR("Invalid edge to in " + toString(rideValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(rideValuesLoaded);
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_BUS_STOP)) {
        // set tag
        rideValuesLoaded.tag = SUMO_TAG_RIDE_BUSSTOP;
        // extract rest of parameters
        if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
            rideValuesLoaded.from = myViewNet->getNet()->retrieveEdge(GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_FROM, myAbort), false);
            if (rideValuesLoaded.from == nullptr) {
                WRITE_ERROR("Invalid edge from in " + toString(rideValuesLoaded.tag));
            }
        }
        rideValuesLoaded.busStop = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_BUS_STOP, myAbort), false);
        rideValuesLoaded.lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_LINES, myAbort);
        if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
            rideValuesLoaded.arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", rideValuesLoaded.tag, SUMO_ATTR_ARRIVALPOS, myAbort);
        }
        // check lines
        if (rideValuesLoaded.lines.empty()) {
            rideValuesLoaded.lines.push_back("ANY");
        }
        // check that all parameters are correct
        if (rideValuesLoaded.busStop == nullptr) {
            WRITE_ERROR("Invalid busStop to in " + toString(rideValuesLoaded.tag));
        } else {
            // save loaded values in container
            myPersonPlanValues.push_back(rideValuesLoaded);
        }
    } else {
        WRITE_ERROR("A ride requires either a from-to edges or a from edge and a busStop");
    }
}


void
GNERouteHandler::addTransport(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addTranship(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}

// ===========================================================================
// private members
// ===========================================================================

GNERouteHandler::PersonPlansValues::PersonPlansValues() :
    tag(SUMO_TAG_NOTHING),
    from(nullptr),
    to(nullptr),
    busStop(nullptr),
    containerStop(nullptr),
    chargingStation(nullptr),
    parkingArea(nullptr),
    route(nullptr),
    arrivalPos(-1),
    laneStop(nullptr) {
}


GNEEdge*
GNERouteHandler::PersonPlansValues::getLastEdge() const {
    if (edges.size() > 0) {
        return edges.back();
    } else if (route) {
        return route->getParentEdges().back();
    } else if (busStop) {
        return busStop->getParentLanes().front()->getParentEdge();
    } else if (laneStop) {
        return laneStop->getParentEdge();
    } else if (to) {
        return to;
    } else {
        return nullptr;
    }
}


/****************************************************************************/
