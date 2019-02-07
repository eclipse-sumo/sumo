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
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/options/OptionsCont.h>

#include "GNERouteHandler.h"
#include "GNERoute.h"
#include "GNEVehicleType.h"
#include "GNEFlow.h"
#include "GNETrip.h"
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
GNERouteHandler::buildVehicle(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* vehicleParameter) {
    // first check if vehicle parameter was sucesfulyl created
    if(vehicleParameter) {
        // now check if exist another vehicle with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VEHICLE, vehicleParameter->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VEHICLE) + " with the same ID='" + vehicleParameter->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleParameter->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, vehicleParameter->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid vehicle Type '" + vehicleParameter->vtypeid + "' used in vehicle '" + vehicleParameter->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + vehicleParameter->routeid + "' used in vehicle '" + vehicleParameter->id + "'.");
            } else {
                // create vehicle using vehicleParameter 
                GNEVehicle* vehicle = new GNEVehicle(viewNet, *vehicleParameter, vType, route);
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
GNERouteHandler::buildFlow(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* flowParameter) {
    // first check if flow parameter was sucesfulyl created
    if(flowParameter) {
        // now check if exist another flow with the same ID
        if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_FLOW, flowParameter->id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_FLOW) + " with the same ID='" + flowParameter->id + "'.");
        } else {
            // obtain routes and vtypes
            GNEDemandElement* vType = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, flowParameter->vtypeid, false);
            GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, flowParameter->routeid, false);
            if (vType == nullptr) {
                WRITE_WARNING("Invalid flow Type '" + flowParameter->vtypeid + "' used in flow '" + flowParameter->id + "'.");
            } else if (route == nullptr) {
                WRITE_WARNING("Invalid route '" + flowParameter->routeid + "' used in flow '" + flowParameter->id + "'.");
            } else {
                // create flow using flowParameter 
                GNEVehicle* flow = new GNEVehicle(viewNet, *flowParameter, vType, route);
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
    // parse attribute of calibrator routes
    myRouteID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE, SUMO_ATTR_ID, myAbort);
    myEdgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_EDGES, myAbort);
    myRouteColor = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_COLOR, myAbort);
}


void 
GNERouteHandler::closeRoute(const bool mayBeDisconnected) {
    // Continue if all parameters were sucesfully loaded
    if (!myAbort) {
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
                route->incRef("buildRoute");
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
GNERouteHandler::closeContainer() {
    // currently unused
}


void 
GNERouteHandler::closeFlow() {
    // build vehicle
    buildFlow(myViewNet, myUndoDemandElements, myVehicleParameter);
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
