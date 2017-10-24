/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNECalibratorRoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNECalibratorRoute.h"
#include "GNECalibrator.h"
#include "GNECalibratorDialog.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"



// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorRoute::GNECalibratorRoute(GNECalibratorDialog* calibratorDialog) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE, ICON_EMPTY),
    myCalibratorParent(calibratorDialog->getEditedCalibrator()), 
    myRouteID(calibratorDialog->generateRouteID()), 
    myColor(RGBColor::BLACK) {
    // add the Edge in which Calibrator is placed as default Edge
    if(GNEAttributeCarrier::hasAttribute(myCalibratorParent->getTag(), SUMO_ATTR_EDGE)) {
        myEdges.push_back(myCalibratorParent->getViewNet()->getNet()->retrieveEdge(myCalibratorParent->getAttribute(SUMO_ATTR_EDGE)));
    } else {
        GNELane *lane = myCalibratorParent->getViewNet()->getNet()->retrieveLane(myCalibratorParent->getAttribute(SUMO_ATTR_LANE));
        myEdges.push_back(myCalibratorParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID()));
    }
}


GNECalibratorRoute::GNECalibratorRoute(GNECalibrator* calibratorParent, const std::string &routeID, const std::vector<GNEEdge*> &edges, const RGBColor& color) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE, ICON_EMPTY),
    myCalibratorParent(calibratorParent), 
    myRouteID(routeID), 
    myEdges(edges),
    myColor(color) {
}


GNECalibratorRoute::~GNECalibratorRoute() {}


void 
GNECalibratorRoute::writeRoute(OutputDevice& device) {
    // Open route tag
    device.openTag(getTag());
    // Write route ID
    device.writeAttr(SUMO_ATTR_BEGIN, myRouteID);
    // Write edge IDs
    device.writeAttr(SUMO_ATTR_BEGIN, getAttribute(SUMO_ATTR_EDGES));
    // Write Color
    device.writeAttr(SUMO_ATTR_BEGIN, myColor);
    // Close flow tag
    device.closeTag();
}


GNECalibrator*
GNECalibratorRoute::getCalibratorParent() const {
    return myCalibratorParent;
}


const std::vector<GNEEdge*>&
GNECalibratorRoute::getGNEEdges() const {
    return myEdges;
}


std::string 
GNECalibratorRoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
    case SUMO_ATTR_ID:
        return myRouteID;
    case SUMO_ATTR_EDGES: {
        // obtain ID's of Edges
        std::vector<std::string> edgeIDs;
        for (auto i : myEdges) {
            edgeIDs.push_back(i->getID());
        }
        return joinToString(edgeIDs, " ");
    }
    case SUMO_ATTR_COLOR:
        return toString(myColor);
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void 
GNECalibratorRoute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
    case SUMO_ATTR_ID:
    case SUMO_ATTR_EDGES:
    case SUMO_ATTR_COLOR:
        undoList->p_add(new GNEChange_Attribute(this, key, value));
        break;
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNECalibratorRoute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
        return isValidID(value);
    case SUMO_ATTR_EDGES: {
        std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
        std::vector<GNEEdge*> edges;
        // Empty Edges aren't valid
        if (edgeIds.empty()) {
            return false;
        }
        // Iterate over parsed edges and check that exists
        for (auto i : edgeIds) {
            GNEEdge* retrievedEdge = myCalibratorParent->getViewNet()->getNet()->retrieveEdge(i, false);
            if (retrievedEdge == NULL) {
                return false;
            } else {
                edges.push_back(retrievedEdge);
            }
        }
        // all edges exist, then check if compounds a valid route
        return isRouteValid(edges);
    }
    case SUMO_ATTR_COLOR:
        return canParse<RGBColor>(value);
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void 
GNECalibratorRoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
        myRouteID = value;
        // changeID(value);
        break;
    case SUMO_ATTR_EDGES: {
        // clear old edges
        myEdges.clear();
        std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
        // Iterate over parsed edges and check that exists
        for (auto i : edgeIds) {
            myEdges.push_back(myCalibratorParent->getViewNet()->getNet()->retrieveEdge(i));
        }
        break;
    }
    case SUMO_ATTR_COLOR:
        myColor = parse<RGBColor>(value);
        break;
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNECalibratorRoute::isRouteValid(const std::vector<GNEEdge*> &edges) const {
    if(edges.size() == 0) {
        // routes cannot be empty
        return false;
    } else if(edges.size() == 1) {
        // routes with a single edge are valid
        return true;
    } else {
        // iterate over edges to check that compounds a chain
        auto it = edges.begin();
        while (it != edges.end() - 1) {
            GNEEdge *currentEdge = *it;
            GNEEdge *nextEdge = *(it+1);
            // consecutive edges aren't allowed
            if(currentEdge->getID() == nextEdge->getID()) {
                return false;
            }
            // make sure that edges are consecutives
            if(std::find(currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().begin(),
                         currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end(),
                         nextEdge) == currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end()) {
                return false;
            }
            it++;
        }
    }
    return true;
}

/****************************************************************************/
