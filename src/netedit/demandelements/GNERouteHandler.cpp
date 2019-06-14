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
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/router/DijkstraRouter.h>

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
GNERouteHandler::buildVehicleWithEmbeddedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embeddedRouteCopy) {
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
            // creaste embedded route
            GNERoute* embeddedRoute = new GNERoute(viewNet, vehicle, embeddedRouteCopy->getEdgeParents(), embeddedRouteCopy->getColor(), SVC_PASSENGER);
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->insertDemandElement(vehicle);
                // set vehicle as child of vType
                vType->addDemandElementChild(vehicle);
                vehicle->incRef("buildVehicleWithEmbeddedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->insertDemandElement(embeddedRoute);
                for (const auto& i : embeddedRouteCopy->getEdgeParents()) {
                    i->addDemandElementChild(vehicle);
                }
                 // set route as child of vehicle
                vehicle->addDemandElementChild(embeddedRoute);
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
        } else {
            // generate a new route ID and add it to vehicleParameters
            vehicleParameters.routeid = viewNet->getNet()->generateDemandElementID(vehicleParameters.id, SUMO_TAG_ROUTE);
            // due vehicle was loaded without a route, change tag
            vehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
            // create vehicle or trips using myTemporalVehicleParameter without a route
            GNEVehicle* flow = new GNEVehicle(viewNet, vType, vehicleParameters);
            // creaste embedded route
            GNERoute* embeddedRoute = new GNERoute(viewNet, flow, embeddedRouteCopy->getEdgeParents(), embeddedRouteCopy->getColor(), SVC_PASSENGER);
            // add both to net depending of myUndoDemandElements
            if (undoDemandElements) {
                viewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                // add both in net using undoList
                viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                viewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
                viewNet->getUndoList()->p_end();
            } else {
                // add vehicleOrRouteFlow in net and in their vehicle type parent
                viewNet->getNet()->insertDemandElement(flow);
                // set vehicle as child of vType
                vType->addDemandElementChild(flow);
                flow->incRef("buildFlowWithEmbeddedRoute");
                // add route manually in net, and in all of their edges and in vehicleOrRouteFlow
                viewNet->getNet()->insertDemandElement(embeddedRoute);
                for (const auto& i : embeddedRouteCopy->getEdgeParents()) {
                    i->addDemandElementChild(flow);
                }
                // set route as child of flow
                flow->addDemandElementChild(embeddedRoute);
                embeddedRoute->incRef("buildFlowWithEmbeddedRoute");
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
            // check if obtained routes correspond to a valid route
            if (routeEdges.size() == 0) {
                routeEdges = edges;
            } else if (routeEdges.size() == 1) {
                routeEdges.push_back(routeEdges.front());
            }
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
GNERouteHandler::buildPerson(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &personParameters) {
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
                viewNet->getNet()->insertDemandElement(person);
                // set person as child of pType and Route
                pType->addDemandElementChild(person);
                person->incRef("buildPerson");
            }
        }
    }
}


void 
GNERouteHandler::buildPersonFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &personFlowParameters) {
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
                viewNet->getNet()->insertDemandElement(personFlow);
                // set personFlow as child of pType and Route
                pType->addDemandElementChild(personFlow);
                personFlow->incRef("buildPersonFlow");
            }
        }
    }
}


void 
GNERouteHandler::buildPersonTripFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, 
    const std::vector<std::string> &types, const std::vector<std::string> &modes, double arrivalPos) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A personTrip needs at least one edge. " + toString(SUMO_TAG_PERSONTRIP_FROMTO) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create personTripFromTo
        GNEPersonTrip* personTripFromTo = new GNEPersonTrip(viewNet, personParent, pathEdges, types, modes, arrivalPos);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PERSONTRIP_FROMTO) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(personTripFromTo, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrPersonTripFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(personTripFromTo);
            personParent->addDemandElementChild(personTripFromTo);
            personTripFromTo->incRef("buildPersonTripFromTo");
        }
    }
}


void
GNERouteHandler::buildPersonTripBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, 
    GNEAdditional *busStop, const std::vector<std::string> &types, const std::vector<std::string> &modes) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A personTrip needs at least one edge. " + toString(SUMO_TAG_PERSONTRIP_BUSSTOP) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create personTripBusStop
        GNEPersonTrip* personTripBusStop = new GNEPersonTrip(viewNet, personParent, pathEdges, busStop, types, modes);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PERSONTRIP_BUSSTOP) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(personTripBusStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrPersonTripFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(personTripBusStop);
            personParent->addDemandElementChild(personTripBusStop);
            busStop->addDemandElementChild(personTripBusStop);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(personTripBusStop);
            }
            personTripBusStop->incRef("buildPersonTripBusStop");
        }
    }
}


void 
GNERouteHandler::buildWalkEdges(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, double arrivalPos) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A walk needs at least one edge. " + toString(SUMO_TAG_WALK_EDGES) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
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
            viewNet->getNet()->insertDemandElement(walkEdges);
            personParent->addDemandElementChild(walkEdges);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(walkEdges);
            }
            walkEdges->incRef("buildWalkEdges");
        }
    }
}


void 
GNERouteHandler::buildWalkFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, double arrivalPos) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A walk needs at least one edge. " + toString(SUMO_TAG_WALK_FROMTO) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create walkFromTo
        GNEWalk* walkFromTo = new GNEWalk(viewNet, personParent, pathEdges, arrivalPos);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_FROMTO) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(walkFromTo, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrWalkFromToFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(walkFromTo);
            personParent->addDemandElementChild(walkFromTo);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(walkFromTo);
            }
            walkFromTo->incRef("buildWalkFromTo");
        }
    }
}


void 
GNERouteHandler::buildWalkBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, GNEAdditional *busStop) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A walk needs at least one edge. " + toString(SUMO_TAG_WALK_BUSSTOP) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create walkBusStop
        GNEWalk* walkBusStop = new GNEWalk(viewNet, personParent, pathEdges, busStop);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_BUSSTOP) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(walkBusStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrWalkBusStopFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(walkBusStop);
            personParent->addDemandElementChild(walkBusStop);
            busStop->addDemandElementChild(walkBusStop);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(walkBusStop);
            }
            walkBusStop->incRef("buildWalkBusStop");
        }
    }
}


void 
GNERouteHandler::buildWalkRoute(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEDemandElement* routeParent, double arrivalPos) {
    // create walkRoute
    GNEWalk* walkRoute = new GNEWalk(viewNet, personParent, routeParent, arrivalPos);
    // add element using undo list or directly, depending of undoDemandElements flag
    if (undoDemandElements) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_WALK_ROUTE) + " within person '" + personParent->getID() + "'");
        viewNet->getUndoList()->add(new GNEChange_DemandElement(walkRoute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        // add vehicleOrWalkBusStopFlow in net and in their vehicle type parent
        viewNet->getNet()->insertDemandElement(walkRoute);
        personParent->addDemandElementChild(walkRoute);
        routeParent->addDemandElementChild(walkRoute);
        walkRoute->incRef("buildWalkRoute");
    }
}


void 
GNERouteHandler::buildRideFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, 
    const std::vector<std::string> &lines, double arrivalPos) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A ride needs at least one edge. " + toString(SUMO_TAG_RIDE_FROMTO) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create rideFromTo
        GNERide* rideFromTo = new GNERide(viewNet, personParent, pathEdges, arrivalPos, lines);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_RIDE_FROMTO) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(rideFromTo, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrRideFromToFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(rideFromTo);
            personParent->addDemandElementChild(rideFromTo);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(rideFromTo);
            }
            rideFromTo->incRef("buildRideFromTo");
        }
    }
}


void 
GNERouteHandler::buildRideBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, 
    GNEAdditional *busStop, const std::vector<std::string> &lines) {
    // check that at least there is an edge
    if (edges.size() == 0) {
        WRITE_ERROR("A ride needs at least one edge. " + toString(SUMO_TAG_RIDE_BUSSTOP) + " within person with ID='" + personParent->getID() + "' cannot be created");
    } else {
        // obtain path between edges
        std::vector<GNEEdge*> pathEdges = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(personParent->getVClass(), edges);
        // check if obtained path is valid
        if (pathEdges.size() == 0) {
            pathEdges = edges;
        }
        // create rideBusStop
        GNERide* rideBusStop = new GNERide(viewNet, personParent, pathEdges, busStop, lines);
        // add element using undo list or directly, depending of undoDemandElements flag
        if (undoDemandElements) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_RIDE_BUSSTOP) + " within person '" + personParent->getID() + "'");
            viewNet->getUndoList()->add(new GNEChange_DemandElement(rideBusStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            // add vehicleOrRideBusStopFlow in net and in their vehicle type parent
            viewNet->getNet()->insertDemandElement(rideBusStop);
            personParent->addDemandElementChild(rideBusStop);
            busStop->addDemandElementChild(rideBusStop);
            // add reference in all edges
            for (const auto& i : edges) {
                i->addDemandElementChild(rideBusStop);
            }
            rideBusStop->incRef("buildRideBusStop");
        }
    }
}


void 
GNERouteHandler::transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute) {
    // first check that given vehicle isn't already a vehicle
    if (originalVehicle->getTagProperty().getTag() != SUMO_TAG_VEHICLE) {
        // get pointer to undo list (due originalVehicle will be deleted)
        GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_VEHICLE));
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) && 
            (originalVehicle->getDemandElementParents().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
        }
        // obtain VType of original vehicle
        GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_VEHICLE;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // obtain vehicle's route (it always exist due call to function separateEmbeddedRoute(...)
            GNEDemandElement *route = originalVehicle->getDemandElementParents().at(1);
            // create Vehicle using values of original vehicle
            GNEVehicle *vehicle = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
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
        GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_ROUTEFLOW));
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) && 
            (originalVehicle->getDemandElementParents().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
        }
        // obtain VType of original vehicle
        GNEDemandElement *vType = originalVehicle->getDemandElementParents().at(0);
        // extract vehicleParameters of originalVehicle
        SUMOVehicleParameter newVehicleParameters = *originalVehicle;
        // change tag in newVehicleParameters (needed for GNEVehicle constructor)
        newVehicleParameters.tag = SUMO_TAG_ROUTEFLOW;
        // make transformation depending of vehicle tag
        if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) {
            // obtain vehicle's route (it always exist due call to function separateEmbeddedRoute(...)
            GNEDemandElement *route = originalVehicle->getDemandElementParents().at(1);
            // create flow using newVehicleParameters
            GNEVehicle *flow = new GNEVehicle(originalVehicle->getViewNet(), vType, route, newVehicleParameters);
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
        GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
        // declare pointer to get embedded route if is created
        GNEDemandElement *separatedEmbeddedRoute = nullptr;
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if (((originalVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW)) && 
            (originalVehicle->getDemandElementParents().size() == 1)) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
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
            // check if trip has to be selected
            if (selected) {
                undoList->p_add(new GNEChange_Attribute(trip, trip->getViewNet()->getNet(), GNE_ATTR_SELECTED, "true"));
            }
        } else if ((originalVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) || (originalVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP)) {
            // create trip using values of original vehicle (including ID)
            GNEVehicle *trip = new GNEVehicle(originalVehicle->getViewNet(), vType, originalVehicle->getEdgeParents(), newVehicleParameters);
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
        GNEUndoList *undoList = originalVehicle->getViewNet()->getUndoList();
        // begin undo-redo operation
        undoList->p_begin("transform " + originalVehicle->getTagStr() + " to " + toString(SUMO_TAG_FLOW));
        // declare pointer to get embedded route if is created
        GNEDemandElement *separatedEmbeddedRoute = nullptr;
        // declare flag to save if vehicle is selected
        bool selected = originalVehicle->isAttributeCarrierSelected();
        // first check if originalVehicle has an embedded route, and if true, separate it
        if(originalVehicle->getDemandElementParents().size() == 1) {
            originalVehicle = separateEmbeddedRoute(originalVehicle, undoList);
            separatedEmbeddedRoute = originalVehicle->getDemandElementParents().at(1);
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

// ===========================================================================
// protected
// ===========================================================================

void
GNERouteHandler::embebbeRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // create a copy of vehicle with the same attributes but without embedded route
    GNEVehicle* vehicleWithEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getDemandElementParents().at(0), *vehicle);
    // create a embeddedRoute based on parameters of vehicle's route
    GNERoute* embeddedRoute = new GNERoute(vehicleWithEmbebbeRoute->getViewNet(), vehicleWithEmbebbeRoute,
        vehicle->getDemandElementParents().at(1)->getEdgeParents(),
        vehicle->getDemandElementParents().at(1)->getColor(),
        vehicle->getDemandElementParents().at(1)->getVClass());
    // remove vehicle, but NOT route
    undoList->add(new GNEChange_DemandElement(vehicle, false), true);
    // now add bot vehicleWithEmbebbeRoute and embeddedRoute
    undoList->add(new GNEChange_DemandElement(vehicleWithEmbebbeRoute, true), true);
    undoList->add(new GNEChange_DemandElement(embeddedRoute, true), true);
}


GNEVehicle* 
GNERouteHandler::separateEmbeddedRoute(GNEVehicle* vehicle, GNEUndoList* undoList) {
    // first create a Route based on the parameters of vehicle's embedded route
    GNERoute* nonEmbeddedRoute = new GNERoute(vehicle->getDemandElementChilds().at(0));
    // create a copy of vehicle with the same attributes but with the nonEmbeddedRoute
    GNEVehicle* vehicleWithoutEmbebbeRoute = new GNEVehicle(vehicle->getViewNet(), vehicle->getDemandElementParents().at(0), nonEmbeddedRoute, *vehicle);
    // remove embedded route andvehicle (because a embebbbed route without vehicle cannot exist)
    undoList->add(new GNEChange_DemandElement(vehicle->getDemandElementChilds().at(0), false), true);
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
                // creaste embedded route
                GNERoute* embeddedRoute = new GNERoute(myViewNet, vehicleOrRouteFlow, edges, myRouteColor, SVC_PASSENGER);
                // add both to net depending of myUndoDemandElements
                if (myUndoDemandElements) {
                    myViewNet->getUndoList()->p_begin("add vehicle and " + embeddedRoute->getTagStr());
                    // add both in net using undoList
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleOrRouteFlow, true), true);
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(embeddedRoute, true), true);
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
                    myViewNet->getNet()->insertDemandElement(embeddedRoute);
                    for (const auto& i : edges) {
                        i->addDemandElementChild(vehicleOrRouteFlow);
                    }
                    vehicleOrRouteFlow->addDemandElementChild(embeddedRoute);
                    embeddedRoute->incRef("buildVehicleAndRoute");
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
            // check if we're creating a vType or a pType
            SumoXMLTag vTypeTag = (myCurrentVType->vehicleClass == SVC_PEDESTRIAN)? SUMO_TAG_PTYPE : SUMO_TAG_VTYPE;
            // create VType using myCurrentVType
            GNEVehicleType* vType = new GNEVehicleType(myViewNet, *myCurrentVType, vTypeTag);
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
