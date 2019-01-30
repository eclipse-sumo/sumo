/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElementHandler.cpp
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

#include "GNEDemandElementHandler.h"
#include "GNERoute.h"
#include "GNEVehicleType.h"
#include "GNEFlow.h"
#include "GNETrip.h"
#include "GNEVehicle.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElementHandler::GNEDemandElementHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements) :
    SUMORouteHandler(file, ""),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements) {
}


GNEDemandElementHandler::~GNEDemandElementHandler() {}


void 
GNEDemandElementHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNEDemandElementHandler::closeVehicleTypeDistribution() {

}


void 
GNEDemandElementHandler::openRoute(const SUMOSAXAttributes& attrs) {
    myAbort = false;
    // parse attribute of calibrator routes
    myRouteID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE, SUMO_ATTR_ID, myAbort);
    myEdgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_EDGES, myAbort);
    myRouteColor = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, myRouteID, SUMO_TAG_ROUTE, SUMO_ATTR_COLOR, myAbort);
}


void 
GNEDemandElementHandler::closeRoute(const bool mayBeDisconnected) {
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
            if (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteID, false) == nullptr) {
                // create route
                GNERoute* route = new GNERoute(myViewNet, myRouteID, edges, myRouteColor);
                if (myUndoDemandElements) {
                    myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
                    myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
                    myViewNet->getUndoList()->p_end();
                } else {
                    myViewNet->getNet()->insertDemandElement(route);
                    route->incRef("buildRoute");
                }
            } else {
                throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTE) + " with ID '" + myRouteID + "' in netedit; probably declared twice.");
            }
        }
    }
}


void 
GNEDemandElementHandler::openRouteDistribution(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNEDemandElementHandler::closeRouteDistribution() {
    // currently unused
}


void 
GNEDemandElementHandler::closeVehicle() {

}


void 
GNEDemandElementHandler::closePerson() {

}


void 
GNEDemandElementHandler::closeContainer() {

}


void 
GNEDemandElementHandler::closeFlow() {

}


void 
GNEDemandElementHandler::addStop(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNEDemandElementHandler::addPersonTrip(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}


void 
GNEDemandElementHandler::addWalk(const SUMOSAXAttributes& /*attrs*/) {
    // currently unused
}

/****************************************************************************/
