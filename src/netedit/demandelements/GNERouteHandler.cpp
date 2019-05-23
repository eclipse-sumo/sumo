/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEBusStop.h>
#include <netedit/additionals/GNEChargingStation.h>
#include <netedit/additionals/GNEContainerStop.h>
#include <netedit/additionals/GNEParkingArea.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/router/DijkstraRouter.h>

#include "GNERouteHandler.h"
#include "GNERoute.h"
#include "GNEStop.h"
#include "GNEVehicle.h"
#include "GNEVehicleType.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteHandler::GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements) :
    SUMORouteHandler(file, ""),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements) {
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


void
GNERouteHandler::buildVehicleOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters) {
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
        } else {
            // create vehicle using vehicleParameters
            GNEVehicle* vehicle = new GNEVehicle(viewNet, vType, route, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + vehicle->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, vehicle, false);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->insertDemandElement(vehicle);
                // set vehicle as child of vType and Route
                vType->addDemandElementChild(vehicle);
                route->addDemandElementChild(vehicle);
                vehicle->incRef("buildVehicleOverRoute");
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, vehicle, false);
                }
            }
        }
    }
}


void
GNERouteHandler::buildFlowOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters) {
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
        } else {
            // create flow or trips using vehicleParameters
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, route, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, flow, false);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->insertDemandElement(flow);
                // set flow as child of vType and Route
                vType->addDemandElementChild(flow);
                route->addDemandElementChild(flow);
                flow->incRef("buildFlowOverRoute");
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, flow, false);
                }
            }
        }
    }
}


void
GNERouteHandler::buildVehicleWithEmbebbedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embebbedRouteCopy) {
    // Check tags
    assert(vehicleParameters.tag == SUMO_TAG_VEHICLE);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain vType
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else {
            // generate a new route ID and add it to vehicleParameters
            vehicleParameters.routeid = viewNet->getNet()->generateDemandElementID(vehicleParameters.id, SUMO_TAG_ROUTE);
            // due vehicle was loaded without a route, change tag
            vehicleParameters.tag = SUMO_TAG_VEHICLE;
            // create vehicle or trips using myTemporalVehicleParameter without a route
            GNEVehicle* vehicle = new GNEVehicle(viewNet, vType, vehicleParameters);
            // creaste embebbed route
            GNERoute* embebbedRoute = new GNERoute(viewNet, vehicle, embebbedRouteCopy->getEdgeParents(), embebbedRouteCopy->getColor(), SVC_PASSENGER);
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embebbedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embebbedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->insertDemandElement(vehicle);
                // set vehicle as child of vType
                vType->addDemandElementChild(vehicle);
                vehicle->incRef("buildVehicleWithEmbebbedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->insertDemandElement(embebbedRoute);
                for (const auto& i : embebbedRouteCopy->getEdgeParents()) {
                    i->addDemandElementChild(vehicle);
                }
                 // set route as child of vehicle
                vehicle->addDemandElementChild(embebbedRoute);
                embebbedRoute->incRef("buildVehicleWithEmbebbedRoute");
            }
        }
    }
}


void
GNERouteHandler::buildFlowWithEmbebbedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embebbedRouteCopy) {
    // Check tags
    assert(vehicleParameters.tag == SUMO_TAG_ROUTEFLOW);
    // first check if ID is duplicated
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain vType
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else {
            // generate a new route ID and add it to vehicleParameters
            vehicleParameters.routeid = viewNet->getNet()->generateDemandElementID(vehicleParameters.id, SUMO_TAG_ROUTE);
            // due vehicle was loaded without a route, change tag
            vehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
            // create vehicle or trips using myTemporalVehicleParameter without a route
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, vehicleParameters);
            // creaste embebbed route
            GNERoute* embebbedRoute = new GNERoute(viewNet, flow, embebbedRouteCopy->getEdgeParents(), embebbedRouteCopy->getColor(), SVC_PASSENGER);
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embebbedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embebbedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addDemandElementChild(flow);
                flow->incRef("buildFlowWithEmbebbedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->insertDemandElement(embebbedRoute);
                for (const auto& i : embebbedRouteCopy->getEdgeParents()) {
                    i->addDemandElementChild(flow);
                }
                // set route as child of flow
                flow->addDemandElementChild(embebbedRoute);
                embebbedRoute->incRef("buildFlowWithEmbebbedRoute");
            }
        }
    }
}


void
GNERouteHandler::buildTrip(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters, const std::vector<GNEEdge*>& edges) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_TRIP);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else {
            // add "via" edges in vehicleParameters
            for (int i = 1; i < ((int)edges.size() - 1); i++) {
                vehicleParameters.via.push_back(edges.at(i)->getID());
            }
            // obtain route between edges
            std::vector<GNEEdge*> routeEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(vType->getVClass(), edges);
            // create trip or flow using tripParameters
            GNEVehicle* trip = new GNEVehicle(viewNet, vType, routeEdges, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + trip->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, trip, false);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->insertDemandElement(trip);
                // set vehicle as child of vType
                vType->addDemandElementChild(trip);
                trip->incRef("buildTrip");
                // add reference in all edges
                for (const auto& i : routeEdges) {
                    i->addDemandElementChild(trip);
                }
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, trip, false);
                }
            }
        }
    }
}


void
GNERouteHandler::buildFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters, const std::vector<GNEEdge*>& edges) {
    // Check tag
    assert(vehicleParameters.tag == SUMO_TAG_FLOW);
    // check if exist another vehicle with the same ID (note: Vehicles, Flows and Trips share namespace)
    if (!isVehicleIdDuplicated(viewNet, vehicleParameters.id)) {
        // obtain  vtypes
        GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters.vtypeid, false);
        if (vType == nullptr) {
            WRITE_ERROR("Invalid vehicle type '" + vehicleParameters.vtypeid + "' used in " + toString(vehicleParameters.tag) + " '" + vehicleParameters.id + "'.");
        } else {
            // add "via" edges in vehicleParameters
            for (int i = 1; i < ((int)edges.size() - 1); i++) {
                vehicleParameters.via.push_back(edges.at(i)->getID());
            }
            // obtain route between edges
            std::vector<GNEEdge*> routeEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(vType->getVClass(), edges);
            // create trip or flow using tripParameters
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, routeEdges, vehicleParameters);
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, true, i, flow, false);
                }
                viewNet->getUndoList()->p_end();
            } else {
                viewNet->getNet()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addDemandElementChild(flow);
                flow->incRef("buildFlow");
                // add reference in all edges
                for (const auto& i : routeEdges) {
                    i->addDemandElementChild(flow);
                }
                // iterate over stops of vehicleParameters and create stops associated with it
                for (const auto& i : vehicleParameters.stops) {
                    buildStop(viewNet, false, i, flow, false);
                }
            }
        }
    }
}


void
GNERouteHandler::buildStop(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent, bool friendlyPosition) {
    // declare pointers to stopping place  and lane and obtain it
    GNEAdditional* stoppingPlace = nullptr;
    GNELane* lane = nullptr;
    SumoXMLTag stopTagType = SUMO_TAG_NOTHING;
    if (stopParameters.busstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stopParameters.busstop, false);
        stopTagType = SUMO_TAG_STOP_BUSSTOP;
    } else if (stopParameters.containerstop.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stopParameters.containerstop, false);
        stopTagType = SUMO_TAG_STOP_CONTAINERSTOP;
    } else if (stopParameters.chargingStation.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stopParameters.chargingStation, false);
        stopTagType = SUMO_TAG_STOP_CHARGINGSTATION;
    } else if (stopParameters.parkingarea.size() > 0) {
        stoppingPlace = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stopParameters.parkingarea, false);
        stopTagType = SUMO_TAG_STOP_PARKINGAREA;
    } else if (stopParameters.lane.size() > 0) {
        lane = viewNet->getNet()->retrieveLane(stopParameters.lane, false);
        stopTagType = SUMO_TAG_STOP_LANE;
    }
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
            viewNet->getNet()->insertDemandElement(stop);
            stoppingPlace->addDemandElementChild(stop);
            stopParent->addDemandElementChild(stop);
            stop->incRef("buildStoppingPlaceStop");
        }
    } else {
        // create stop using stopParameters and lane
        GNEStop* stop = new GNEStop(viewNet, stopParameters, lane, friendlyPosition, stopParent);
        // add it depending of undoDemandElements
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + stop->getTagStr());
            viewNet->getUndoList()->add(new GNEChange_DemandElement(stop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertDemandElement(stop);
            lane->addDemandElementChild(stop);
            stopParent->addDemandElementChild(stop);
            stop->incRef("buildLaneStop");
        }
    }
}


void 
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbebbedRoute) {
    // get pointer to undo list (due originalVehicle will be deleted)
    GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
    // begin undo-redo operation
    undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
    // first check if originalVehicle has an embebbed route, and if true, separate it
    if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) && 
        (originalVehicle->getDemandElementParents().size() == 1)) {
        originalVehicle = separateEmbebbedRoute(originalVehicle, undoList);
    }
    // obtain VType of original vehicle
    GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
    // extract vehicleParameters of originalVehicle
    SUMOVehicleParameter newVehicleParameters = *originalVehicle;
    // change tag in newVehicleParameters (needed for GNEVehicle constructor)
    newVehicleParameters.tag = SUMO_TAG_VEHICLE;
    // make transformation depending of vehicle tag
    if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
        // obtain vehicle's route (it always exist due call to function separateEmbebbedRoute(...)
        GNEDemandElement *route = originalVehicle->getDemandElementParents().at(1);
        // create Vehicle using values of original vehicle
        GNEVehicle *vehicle = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
        // remove original vehicle (to avoid problem with ID)
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new vehicle
        undoList->add(new GNEChange_DemandElement(vehicle, true), true);
        // as last step change vehicle's route to embebbed route if createEmbebbedRoute is enabled
        if (createEmbebbedRoute) {
            embebbeRoute(vehicle, undoList);
        }
    } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create route using values of originalVehicle flow/trip
            GNERoute *route = new GNERoute(originalVehicle->getViewNet(), 
                originalVehicle->getViewNet()->getNet()->generateDemandElementID(originalVehicle->getID(), SUMO_TAG_ROUTE), 
                originalVehicle->getEdgeParents(), 
                originalVehicle->getColor(), 
                originalVehicle->getVClass());
            // create Vehicle using values of original vehicle (including ID)
            GNEVehicle *vehicle = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove flow/trip (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add both new vehicle and route
            undoList->add(new GNEChange_DemandElement(route, true), true);
            undoList->add(new GNEChange_DemandElement(vehicle, true), true);
    }
    // end undo-redo operation
    undoList->p_end();
}


void 
GNERouteHandler::transformToRouteFlow(GNEVehicle* originalVehicle, bool createEmbebbedRoute) {
    // get pointer to undo list (due originalVehicle will be deleted)
    GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
    // begin undo-redo operation
    undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_ROUTEFLOW));
    // first check if originalVehicle has an embebbed route, and if true, separate it
    if(originalVehicle->getDemandElementParents().size() == 1) {
        originalVehicle = separateEmbebbedRoute(originalVehicle, undoList);
    }
    // obtain VType of original vehicle
    GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
    // extract vehicleParameters of originalVehicle
    SUMOVehicleParameter newVehicleParameters = *originalVehicle;
    // change tag in newVehicleParameters (needed for GNEVehicle constructor)
    newVehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
    // make transformation depending of vehicle tag
    if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
        // obtain vehicle's route (it always exist due call to function separateEmbebbedRoute(...)
        GNEDemandElement *route = originalVehicle->getDemandElementParents().at(1);
        // create flow using newVehicleParameters
        GNEVehicle *flow = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
        // remove original vehicle (to avoid problem with ID)
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new vehicle
        undoList->add(new GNEChange_DemandElement(flow, true), true);
        // as last step change vehicle's route to embebbed route if createEmbebbedRoute is enabled
        if (createEmbebbedRoute) {
            embebbeRoute(flow, undoList);
        }
    } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create route using values of originalVehicle flow/trip
            GNERoute *route = new GNERoute(originalVehicle->getViewNet(), 
                originalVehicle->getViewNet()->getNet()->generateDemandElementID(originalVehicle->getID(), SUMO_TAG_ROUTE), 
                originalVehicle->getEdgeParents(), 
                originalVehicle->getColor(), 
                originalVehicle->getVClass());
            // create flow using values of original vehicle (including ID)
            GNEVehicle *flow = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
            // remove flow/trip (to avoid problem with ID)
            undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
            // add both new vehicle and route
            undoList->add(new GNEChange_DemandElement(route, true), true);
            undoList->add(new GNEChange_DemandElement(flow, true), true);
    }
    // end undo-redo operation
    undoList->p_end();
}


void 
GNERouteHandler::transformToTrip(GNEVehicle* originalVehicle) {
    // get pointer to undo list (due originalVehicle will be deleted)
    GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
    // begin undo-redo operation
    undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
    // declare pointer to get embebbed route if is created
    GNEDemandElement *separatedEmbebbedRoute = nullptr;
    // first check if originalVehicle has an embebbed route, and if true, separate it
    if(originalVehicle->getDemandElementParents().size() == 1) {
        originalVehicle = separateEmbebbedRoute(originalVehicle, undoList);
        separatedEmbebbedRoute = originalVehicle->getDemandElementParents().at(1);
    }
    // obtain VType of original vehicle
    GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
    // extract vehicleParameters of originalVehicle
    SUMOVehicleParameter newVehicleParameters = *originalVehicle;
    // change tag in newVehicleParameters (needed for GNEVehicle constructor)
    newVehicleParameters.tag = SUMO_TAG_TRIP;
    // make transformation depending of vehicle tag
    if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
        // create trip using values of original vehicle (including ID) and route's edges
        GNEVehicle *trip = new GNEVehicle(originalVehicle->getViewNet(), vType, originalVehicle->getDemandElementParents().at(1)->getEdgeParents(), newVehicleParameters);
        // first remove vehicle (to avoid problem with ID)
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new vehicle
        undoList->add(new GNEChange_DemandElement(trip, true), true);
    } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
        // create trip using values of original vehicle (including ID)
        GNEVehicle *trip = new GNEVehicle(originalVehicle->getViewNet(), vType, originalVehicle->getEdgeParents(), newVehicleParameters);
        // remove originalVehicle
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new trip
        undoList->add(new GNEChange_DemandElement(trip, true), true);
    }
    // check if separatedEmbebbedRoute has to be removed
    if (separatedEmbebbedRoute) {
        undoList->add(new GNEChange_DemandElement(separatedEmbebbedRoute, false), true);
    }
    // end undo-redo operation
    undoList->p_end();
}


void 
GNERouteHandler::transformToFlow(GNEVehicle* originalVehicle) {
    // get pointer to undo list (due originalVehicle will be deleted)
    GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
    // begin undo-redo operation
    undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
    // declare pointer to get embebbed route if is created
    GNEDemandElement *separatedEmbebbedRoute = nullptr;
    // first check if originalVehicle has an embebbed route, and if true, separate it
    if(originalVehicle->getDemandElementParents().size() == 1) {
        originalVehicle = separateEmbebbedRoute(originalVehicle, undoList);
        separatedEmbebbedRoute = originalVehicle->getDemandElementParents().at(1);
    }
    // obtain VType of original vehicle
    GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
    // extract vehicleParameters of originalVehicle
    SUMOVehicleParameter newVehicleParameters = *originalVehicle;
    // change tag in newVehicleParameters (needed for GNEVehicle constructor)
    newVehicleParameters.tag = SUMO_TAG_FLOW;
    // make transformation depending of vehicle tag
    if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
        // create Vehicle using values of original vehicle (including ID) and route's edges
        GNEVehicle *flow = new GNEVehicle(originalVehicle->getViewNet(), vType, originalVehicle->getDemandElementParents().at(1)->getEdgeParents(), newVehicleParameters);
        // first remove vehicle (to avoid problem with ID)
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new flow
        undoList->add(new GNEChange_DemandElement(flow, true), true);
    } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
        // create flow using values of original vehicle (including ID)
        GNEVehicle *flow = new GNEVehicle(originalVehicle->getViewNet(), vType, originalVehicle->getEdgeParents(), newVehicleParameters);
        // remove originalVehicle
        undoList->add(new GNEChange_DemandElement(originalVehicle, false), true);
        // add new flow
        undoList->add(new GNEChange_DemandElement(flow, true), true);
    }
    // check if separatedEmbebbedRoute has to be removed
    if (separatedEmbebbedRoute) {
        undoList->add(new GNEChange_DemandElement(separatedEmbebbedRoute, false), true);
    }
    // end undo-redo operation
    undoList->p_end();
}

// ===========================================================================
// protected
// ===========================================================================

void
GNERouteHandler::embebbeRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // create a copy of vehicle with the same attributes but without embebbed route
    GNEVehicle* vehicleWithEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getDemandElementParents().at(0), *vehicle);
    // create a embebbedRoute based on parameters of vehicle's route
    GNERoute* embebbedRoute = new GNERoute(vehicleWithEmbebbeRoute->getViewNet(), vehicleWithEmbebbeRoute,
        vehicle->getDemandElementParents().at(1)->getEdgeParents(),
        vehicle->getDemandElementParents().at(1)->getColor(),
        vehicle->getDemandElementParents().at(1)->getVClass());
    // remove vehicle, but NOT route
    undoList->add(new GNEChange_DemandElement(vehicle, false), true);
    // now add bot vehicleWithEmbebbeRoute and embebbedRoute
    undoList->add(new GNEChange_DemandElement(vehicleWithEmbebbeRoute, true), true);
    undoList->add(new GNEChange_DemandElement(embebbedRoute, true), true);
}


GNEVehicle* 
GNERouteHandler::separateEmbebbedRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // first create a Route based on the parameters of vehicle's embebbed route
    GNERoute* nonEmbebbedRoute = new GNERoute(vehicle->getDemandElementChilds().at(0));
    // create a copy of vehicle with the same attributes but with the nonEmbebbedRoute
    GNEVehicle* vehicleWithoutEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getDemandElementParents().at(0), nonEmbebbedRoute, *vehicle);
    // remove embebbed route andvehicle (because a embebbbed route without vehicle cannot exist)
    undoList->add(new GNEChange_DemandElement(vehicle->getDemandElementChilds().at(0), false), true);
    undoList->add(new GNEChange_DemandElement(vehicle, false), true);
    // now add bot nonEmbebbedRoute and vehicleWithoutEmbebbeRoute
    undoList->add(new GNEChange_DemandElement(nonEmbebbedRoute, true), true);
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
    myAbort = false;
    // parse attribute of routes
    myRouteID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE, SUMO_ATTR_ID, myAbort);
    myEdgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_EDGES, myAbort);
    myRouteColor = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_COLOR, myAbort);

}


void
GNERouteHandler::openFlow(const SUMOSAXAttributes& attrs) {
    myAbort = false;
    // parse attributes of Trips
    myFromID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_FROM, myAbort);
    myToID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_TO, myAbort);
    // attribute VIA is optional
    if (attrs.hasAttribute(SUMO_ATTR_VIA)) {
        myViaIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_VIA, myAbort);
    } else {
        myViaIDs.clear();
    }
}


void
GNERouteHandler::openTrip(const SUMOSAXAttributes& attrs) {
    myAbort = false;
    // parse attributes of Trips
    myFromID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_FROM, myAbort);
    myToID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_TO, myAbort);
    // attribute VIA is optional
    if (attrs.hasAttribute(SUMO_ATTR_VIA)) {
        myViaIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TRIP, SUMO_ATTR_VIA, myAbort);
    } else {
        myViaIDs.clear();
    }
}


void
GNERouteHandler::closeRoute(const bool /* mayBeDisconnected */) {
    // obtain edges (And show warnings if isn't valid)
    std::vector<GNEEdge*> edges;
    if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, true)) {
        edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs);
    }
    // we have two possibilities: Either create a route with their own ID, or create a route within a vehicle
    if (myVehicleParameter) {
        if (edges.size() == 0) {
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
                myVehicleParameter->tag = (myVehicleParameter->tag == SUMO_TAG_FLOW)? SUMO_TAG_ROUTEFLOW : SUMO_TAG_VEHICLE;
                // create vehicle or trips using myTemporalVehicleParameter without a route
                GNEVehicle* vehicleOrRouteFlow = new GNEVehicle(myViewNet, vType, *myVehicleParameter);
                // creaste embebbed route
                GNERoute* embebbedRoute = new GNERoute(myViewNet, vehicleOrRouteFlow, edges, myRouteColor, SVC_PASSENGER);
                // add both to net depending of myUndoDemandElements
                if (myUndoDemandElements) {
                    myViewNet->getUndoList()->p_begin("add vehicle and " + embebbedRoute->getTagStr());
                    // add both in net using undoList
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleOrRouteFlow, true), true);
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(embebbedRoute, true), true);
                    // iterate over stops of myActiveRouteStops and create stops associated with it
                    for (const auto& i : myActiveRouteStops) {
                        buildStop(myViewNet, true, i, vehicleOrRouteFlow, false);
                    }
                    myViewNet->getUndoList()->p_end();
                } else {
                    // add vehicleOrRouteFlow in net and in their vehicle type parent
                    myViewNet->getNet()->insertDemandElement(vehicleOrRouteFlow);
                    vType->addDemandElementChild(vehicleOrRouteFlow);
                    vehicleOrRouteFlow->incRef("buildVehicleAndRoute");
                    // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                    myViewNet->getNet()->insertDemandElement(embebbedRoute);
                    for (const auto& i : edges) {
                        i->addDemandElementChild(vehicleOrRouteFlow);
                    }
                    vehicleOrRouteFlow->addDemandElementChild(embebbedRoute);
                    embebbedRoute->incRef("buildVehicleAndRoute");
                    // iterate over stops of myActiveRouteStops and create stops associated with it
                    for (const auto& i : myActiveRouteStops) {
                        buildStop(myViewNet, false, i, vehicleOrRouteFlow, false);
                    }
                }
            }
        }
        // delete myVehicleParameter because at the end of this function closeVehicle()/closeFlow() will be called
        delete myVehicleParameter;
        myVehicleParameter = nullptr;
    } else if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteID, false) != nullptr) {
        WRITE_ERROR("There is another " + toString(SUMO_TAG_ROUTE) + " with the same ID='" + myRouteID + "'.");
    } else if (edges.size() == 0) {
        WRITE_ERROR("A route needs at least one edge.");
    } else {
        // creaste GNERoute
        GNERoute* route = new GNERoute(myViewNet, myRouteID, edges, myRouteColor, SVC_PASSENGER);
        if (myUndoDemandElements) {
            myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
            myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            // iterate over stops of myActiveRouteStops and create stops associated with it
            for (const auto& i : myActiveRouteStops) {
                buildStop(myViewNet, true, i, route, false);
            }
            myViewNet->getUndoList()->p_end();
        } else {
            myViewNet->getNet()->insertDemandElement(route);
            for (const auto& i : edges) {
                i->addDemandElementChild(route);
            }
            route->incRef("buildRoute");
            // iterate over stops of myActiveRouteStops and create stops associated with it
            for (const auto& i : myActiveRouteStops) {
                buildStop(myViewNet, false, i, route, false);
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
    // first check if myVehicleParameter was sucesfully created
    if(myVehicleParameter) {
        // build vehicle over route
        buildVehicleOverRoute(myViewNet, myUndoDemandElements, *myVehicleParameter);
    }
}


void
GNERouteHandler::closeVType() {
    // first check that VType was sucesfully created
    if (myCurrentVType) {
        // first check if loaded VType is a default vtype
        if ((myCurrentVType->id == DEFAULT_VTYPE_ID) || (myCurrentVType->id == DEFAULT_PEDTYPE_ID) || (myCurrentVType->id == DEFAULT_BIKETYPE_ID)) {
            // overwrite default vehicle type
            GNEVehicleType::overwriteVType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, myCurrentVType->id, false), myCurrentVType, myViewNet->getUndoList());
        } else if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, myCurrentVType->id, false) != nullptr) {
            WRITE_ERROR("There is another " + toString(SUMO_TAG_VTYPE) + " with the same ID='" + myCurrentVType->id + "'.");
        } else {
            // create VType using myCurrentVType
            GNEVehicleType* vType = new GNEVehicleType(myViewNet, *myCurrentVType);
            if (myUndoDemandElements) {
                myViewNet->getUndoList()->p_begin("add " + vType->getTagStr());
                myViewNet->getUndoList()->add(new GNEChange_DemandElement(vType, true), true);
                myViewNet->getUndoList()->p_end();
            } else {
                myViewNet->getNet()->insertDemandElement(vType);
                vType->incRef("buildVType");
            }
        }
    }
}


void
GNERouteHandler::closePerson() {
    // currently unused
}

void
GNERouteHandler::closePersonFlow() {
    // currently unused
}

void
GNERouteHandler::closeContainer() {
    // currently unused
}


void
GNERouteHandler::closeFlow() {
    // first check if myVehicleParameter was sucesfully created
    if(myVehicleParameter) {
        // check if we're creating a flow or a routeFlow over route
        if(!myFromID.empty() || !myToID.empty()) {
            // force reroute
            myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
            // obtain from and to edges
            GNEEdge* from = myViewNet->getNet()->retrieveEdge(myFromID, false);
            GNEEdge* to = myViewNet->getNet()->retrieveEdge(myToID, false);
            // check if edges are valid
            if (from == nullptr) {
                WRITE_ERROR("Invalid 'from' edge used in routeFlow '" + myVehicleParameter->id + "'.");
            } else if (to == nullptr) {
                WRITE_ERROR("Invalid 'to' edge used in routeFlow '" + myVehicleParameter->id + "'.");
            } else if (!GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, false)) {
                WRITE_ERROR("Invalid 'via' edges used in routeFlow '" + myVehicleParameter->id + "'.");
            } else {
                // obtain via
                std::vector<GNEEdge*> viaEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myViaIDs);
                // build edges (from - via - to)
                std::vector<GNEEdge*> edges;
                edges.push_back(from);
                for (const auto& i : viaEdges) {
                    edges.push_back(i);
                }
                // check that from and to edge are different
                if (from != to) {
                    edges.push_back(to);
                }
                // build flow
                buildFlow(myViewNet, true, *myVehicleParameter, edges);
            }
        } else {
            // build flow over route
            buildFlowOverRoute(myViewNet, myUndoDemandElements, *myVehicleParameter);
        }
    }
}


void
GNERouteHandler::closeTrip() {
    // first check if myVehicleParameter was sucesfully created
    if(myVehicleParameter) {
        // force reroute
        myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
        // obtain from and to edges
        GNEEdge* from = myViewNet->getNet()->retrieveEdge(myFromID, false);
        GNEEdge* to = myViewNet->getNet()->retrieveEdge(myToID, false);
        // check if edges are valid
        if (from == nullptr) {
            WRITE_ERROR("Invalid 'from' edge used in trip '" + myVehicleParameter->id + "'.");
        } else if (to == nullptr) {
            WRITE_ERROR("Invalid 'to' edge used in trip '" + myVehicleParameter->id + "'.");
        } else if (!GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, false)) {
            WRITE_ERROR("Invalid 'via' edges used in trip '" + myVehicleParameter->id + "'.");
        } else {
            // obtain via
            std::vector<GNEEdge*> viaEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myViaIDs);
            // build edges (from - via - to)
            std::vector<GNEEdge*> edges;
            edges.push_back(from);
            for (const auto& i : viaEdges) {
                edges.push_back(i);
            }
            // check that from and to edge are different
            if (from != to) {
                edges.push_back(to);
            }
            // build trip
            buildTrip(myViewNet, true, *myVehicleParameter, edges);
        }
    }
}


void
GNERouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    std::string errorSuffix;
    if (myVehicleParameter != nullptr) {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    bool ok = parseStop(stop, attrs, errorSuffix, MsgHandler::getErrorInstance());
    if (!ok) {
        return;
    }
    // try to parse the assigned bus stop
    if (stop.busstop != "") {
        // ok, we have a bus stop
        GNEBusStop* bs = dynamic_cast<GNEBusStop*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, stop.busstop, false));
        if (bs == nullptr) {
            WRITE_ERROR("The busStop '" + stop.busstop + "' is not known" + errorSuffix);
            return;
        }
        // obtain lane
        stop.lane = bs->getAttribute(SUMO_ATTR_LANE);
    } //try to parse the assigned container stop
    else if (stop.containerstop != "") {
        // ok, we have obviously a container stop
        GNEAdditional* cs = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, stop.containerstop, false);
        if (cs == nullptr) {
            WRITE_ERROR("The containerStop '" + stop.containerstop + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = cs->getAttribute(SUMO_ATTR_LANE);
    } //try to parse the assigned parking area
    else if (stop.parkingarea != "") {
        // ok, we have obviously a parking area
        GNEAdditional* pa = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, stop.parkingarea, false);
        if (pa == nullptr) {
            WRITE_ERROR("The parkingArea '" + stop.parkingarea + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = pa->getAttribute(SUMO_ATTR_LANE);
    } else if (stop.chargingStation != "") {
        // ok, we have a charging station
        GNEAdditional* cs = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, stop.chargingStation, false);
        if (cs == nullptr) {
            WRITE_ERROR("The chargingStation '" + stop.chargingStation + "' is not known" + errorSuffix);
            return;
        }
        stop.lane = cs->getAttribute(SUMO_ATTR_LANE);
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, nullptr, ok, "");
        GNELane* lane = myViewNet->getNet()->retrieveLane(stop.lane, false);
        // check if lane is valid
        if (ok && stop.lane != "") {
            if (lane == nullptr) {
                WRITE_ERROR("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            WRITE_ERROR("A stop must be placed on a busStop, a chargingStation, a containerStop a parkingArea or a lane" + errorSuffix);
            return;
        }
        // calculate start and end position
        stop.endPos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, nullptr, ok, lane->getLaneParametricLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_ERROR("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOpt<double>(SUMO_ATTR_POSITION, nullptr, ok, stop.endPos);
        }
        stop.startPos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, nullptr, ok, MAX2(0., stop.endPos - 2 * POSITION_EPS));
        const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, ok, false);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, lane->getLaneParametricLength(), POSITION_EPS, friendlyPos)) {
            WRITE_ERROR("Invalid start or end position for stop on lane '" + stop.lane + "'" + errorSuffix);
            return;
        }
    }
    if (myVehicleParameter != nullptr) {
        myVehicleParameter->stops.push_back(stop);
    } else {
        myActiveRouteStops.push_back(stop);
    }
}


void
GNERouteHandler::addPersonTrip(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addWalk(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
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
GNERouteHandler::addRide(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addTransport(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void
GNERouteHandler::addTranship(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}

/****************************************************************************/
