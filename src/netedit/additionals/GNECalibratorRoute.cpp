/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNECalibratorRoute.h"
#include "GNECalibrator.h"
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorRoute::GNECalibratorRoute(GNECalibratorDialog* calibratorDialog) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE),
    myCalibratorParent(calibratorDialog->getEditedCalibrator()),
    myRouteID(calibratorDialog->getEditedCalibrator()->getViewNet()->getNet()->generateCalibratorRouteID()),
    myColor(RGBColor::YELLOW) {
    // add the Edge in which Calibrator is placed as default Edge
    if (GNEAttributeCarrier::hasAttribute(myCalibratorParent->getTag(), SUMO_ATTR_EDGE)) {
        myEdges.push_back(myCalibratorParent->getViewNet()->getNet()->retrieveEdge(myCalibratorParent->getAttribute(SUMO_ATTR_EDGE)));
    } else {
        GNELane* lane = myCalibratorParent->getViewNet()->getNet()->retrieveLane(myCalibratorParent->getAttribute(SUMO_ATTR_LANE));
        myEdges.push_back(myCalibratorParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID()));
    }
}


GNECalibratorRoute::GNECalibratorRoute(GNECalibrator* calibratorParent, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE),
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
    writeAttribute(device, SUMO_ATTR_ID);
    // Write edge IDs
    writeAttribute(device, SUMO_ATTR_EDGES);
    // Write Color
    writeAttribute(device, SUMO_ATTR_COLOR);
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


void 
GNECalibratorRoute::selectAttributeCarrier(bool) {
    // this AC cannot be selected
}


void 
GNECalibratorRoute::unselectAttributeCarrier(bool) {
    // this AC cannot be unselected
}


bool 
GNECalibratorRoute::isAttributeCarrierSelected() const {
    // this AC doesn't own a select flag
    return false;
}


std::string
GNECalibratorRoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myRouteID;
        case SUMO_ATTR_EDGES:
            return parseIDs(myEdges);
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
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorRoute(value, false) == nullptr);
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myCalibratorParent->getViewNet()->getNet(), value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNEAdditional::isRouteValid(parse<std::vector<GNEEdge*> >(myCalibratorParent->getViewNet()->getNet(), value), false);
            } else {
                return false;
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
        case SUMO_ATTR_ID: {
            std::string oldID = myRouteID;
            myRouteID = value;
            myCalibratorParent->getViewNet()->getNet()->changeCalibratorRouteID(this, oldID);
            break;
        }
        case SUMO_ATTR_EDGES:
            myEdges = parse<std::vector<GNEEdge*> >(myCalibratorParent->getViewNet()->getNet(), value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
