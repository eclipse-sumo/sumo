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
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/router/DijkstraRouter.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>

#include "GNERouteHandler.h"
#include "GNERoute.h"
#include "GNEVehicleType.h"
#include "GNEVehicle.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteHandler::GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements) :
    SUMORouteHandler(file, ""),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements) {
}


GNERouteHandler::~GNERouteHandler() {}


void 
GNERouteHandler::buildVehicle(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* vehicleParameters) {
    // first check if vehicle parameter was sucesfulyl created
    if(vehicleParameters) {
        // now check if exist another vehicle with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VEHICLE, vehicleParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VEHICLE) + " with the same ID='" + vehicleParameters->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameters->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameters->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid vehicle Type '" + vehicleParameters->vtypeid + "' used in vehicle '" + vehicleParameters->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + vehicleParameters->routeid + "' used in vehicle '" + vehicleParameters->id + "'.");
            } else {
                // create vehicle using vehicleParameters 
                GNEVehicle* vehicle = new GNEVehicle(SUMO_TAG_VEHICLE, viewNet, *vehicleParameters, vType, route);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + vehicle->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(vehicle, true), true);
                    viewNet->getUndoList()->p_end();
                } else {
                    viewNet->getNet()->insertDemandElement(vehicle);
                    vehicle->incRef("buildVehicle");
                }
            }
        }
    }
}


void 
GNERouteHandler::buildFlow(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* flowParameters) {
    // first check if flow parameter was sucesfulyl created
    if(flowParameters) {
        // now check if exist another flow with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_FLOW, flowParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_FLOW) + " with the same ID='" + flowParameters->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, flowParameters->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, flowParameters->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid flow Type '" + flowParameters->vtypeid + "' used in flow '" + flowParameters->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + flowParameters->routeid + "' used in flow '" + flowParameters->id + "'.");
            } else {
                // create flow using flowParameters 
                GNEVehicle* flow = new GNEVehicle(SUMO_TAG_FLOW, viewNet, *flowParameters, vType, route);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(flow, true), true);
                    viewNet->getUndoList()->p_end();
                } else {
                    viewNet->getNet()->insertDemandElement(flow);
                    flow->incRef("buildFlow");
                }
            }
        }
    }
}


void 
GNERouteHandler::buildTrip(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* tripParameters, GNEEdge* from, GNEEdge* to, const std::vector<GNEEdge*> &via) {
    // first check if trip parameter was sucesfulyl created
    if(tripParameters) {
        // now check if exist another trip with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_TRIP, tripParameters->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_TRIP) + " with the same ID='" + tripParameters->id + "'.");
        } else {
            // obtain  vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, tripParameters->vtypeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid trip Type '" + tripParameters->vtypeid + "' used in trip '" + tripParameters->id + "'.");
            } else {
                // check if route exist
                std::vector<const NBEdge*> routeEdges;
                NBVehicle tmpVehicle("temporalNBVehicle", GNEAttributeCarrier::parse<SUMOVehicleClass>(vType->getAttribute(SUMO_ATTR_VCLASS)));
                SUMOAbstractRouter<NBEdge, NBVehicle>* route;
                // create Dijkstra route
                route = new DijkstraRouter<NBEdge, NBVehicle, SUMOAbstractRouter<NBEdge, NBVehicle> >(
                    viewNet->getNet()->getNetBuilder()->getEdgeCont().getAllEdges(), true, &NBEdge::getTravelTimeStatic, nullptr, true);
                // check if route is valid
                if (!route->compute(from->getNBEdge(), to->getNBEdge(), &tmpVehicle, 10, routeEdges)) {
                    WRITE_WARNING("There isn't a route bewteen edges '" + from->getID() + "' and '" + to->getID() + "'.");
                }
                // delete route
                delete route;
                // create trip using tripParameters 
                GNEVehicle* trip = new GNEVehicle(viewNet, *tripParameters, vType, from, to, via);
                if (undoDemandElements) {
                    viewNet->getUndoList()->p_begin("add " + trip->getTagStr());
                    viewNet->getUndoList()->add(new GNEChange_DemandElement(trip, true), true);
                    viewNet->getUndoList()->p_end();
                } else {
                    viewNet->getNet()->insertDemandElement(trip);
                    trip->incRef("buildTrip");
                }
            }
        }
    }
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
    // check that all elements are valid
    if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteID, false) != nullptr) {
        WRITE_WARNING("There is another " + toString(SUMO_TAG_ROUTE) + " with the same ID='" + myRouteID + "'.");
    } else if (edges.size() == 0) {
        WRITE_WARNING("Routes needs at least one edge.");
    } else {
        // creaste GNERoute
        GNERoute* route = new GNERoute(myViewNet, myRouteID, edges, myRouteColor);
        if (myUndoDemandElements) {
            myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
            myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            myViewNet->getUndoList()->p_end();
        } else {
            myViewNet->getNet()->insertDemandElement(route);
            for (const auto &i : edges) {
                i->addDemandElementChild(route);
            }
            route->incRef("buildRoute");
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
    // build vehicle
    buildVehicle(myViewNet, myUndoDemandElements, myVehicleParameter);
}


void 
GNERouteHandler::closeVType() {
    // first check that VType was sucesfully created
    if(myCurrentVType) {
        // now check if exist another VType with the same ID
        if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myCurrentVType->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VTYPE) + " with the same ID='" + myCurrentVType->id + "'.");
        } else {
            // create VType using myCurrentVType
            GNEVehicleType* vType = new GNEVehicleType(myViewNet, *myCurrentVType, false);
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
GNERouteHandler::closeContainer() {
    // currently unused
}


void 
GNERouteHandler::closeFlow() {
    // build flow
    buildFlow(myViewNet, myUndoDemandElements, myVehicleParameter);
}


void 
GNERouteHandler::closeTrip() {
    // force reroute
    myVehicleParameter->parametersSet |= VEHPARS_FORCE_REROUTE;
    // obtain from and to edges
    GNEEdge *from = myViewNet->getNet()->retrieveEdge(myFromID, false);
    GNEEdge *to = myViewNet->getNet()->retrieveEdge(myToID, false);
    // check if edges are valid
    if (from == nullptr) {
        WRITE_WARNING("Invalid 'from' edge used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else if (to == nullptr) { 
        WRITE_WARNING("Invalid 'to' edge used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else if (!GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs, false)) {
        WRITE_WARNING("Invalid 'via' edges used in trip '" + myVehicleParameter->vtypeid + "'.");
    } else {
        // build trips
        buildTrip(myViewNet, true, myVehicleParameter, from, to, GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), myEdgeIDs)); 
    }
}


void 
GNERouteHandler::addStop(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNERouteHandler::addPersonTrip(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNERouteHandler::addWalk(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}

/****************************************************************************/
