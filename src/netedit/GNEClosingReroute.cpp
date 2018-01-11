/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEClosingReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
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

#include <utils/common/ToString.h>

#include "GNEClosingReroute.h"
#include "GNEEdge.h"
#include "GNEUndoList.h"
#include "GNEChange_Attribute.h"
#include "GNERerouter.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNERerouterInterval.h"
#include "GNERerouterIntervalDialog.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingReroute::GNEClosingReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAttributeCarrier(SUMO_TAG_CLOSING_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalDialog->getEditedRerouterInterval()),
    myClosedEdge(rerouterIntervalDialog->getEditedRerouterInterval()->getRerouterParent()->getEdgeChilds().at(0)),
    myAllowedVehicles(parseVehicleClasses(getDefaultValue<std::string>(SUMO_TAG_CLOSING_LANE_REROUTE, SUMO_ATTR_ALLOW))),
    myDisallowedVehicles(parseVehicleClasses(getDefaultValue<std::string>(SUMO_TAG_CLOSING_LANE_REROUTE, SUMO_ATTR_DISALLOW))) {
}


GNEClosingReroute::GNEClosingReroute(GNERerouterInterval* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) :
    GNEAttributeCarrier(SUMO_TAG_CLOSING_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalParent),
    myClosedEdge(closedEdge),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles) {
}


GNEClosingReroute::~GNEClosingReroute() {}


void
GNEClosingReroute::writeClosingReroute(OutputDevice& device) const {
    // open closing reroute tag
    device.openTag(getTag());
    // write Lane ID
    device.writeAttr(SUMO_ATTR_ID, myClosedEdge->getID());
    // write Allowed vehicles
    device.writeAttr(SUMO_ATTR_ALLOW, getVehicleClassNames(myAllowedVehicles));
    // write disallowed vehicles
    device.writeAttr(SUMO_ATTR_DISALLOW, getVehicleClassNames(myDisallowedVehicles));
    // close closing reroute tag
    device.closeTag();
}


GNERerouterInterval*
GNEClosingReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}


std::string
GNEClosingReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myClosedEdge->getID();
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myAllowedVehicles);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(myDisallowedVehicles);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEClosingReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEClosingReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return (myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(value, false) != NULL);
        case SUMO_ATTR_ALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEClosingReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            myClosedEdge = myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(value);
            break;
        }
        case SUMO_ATTR_ALLOW: {
            myAllowedVehicles = parseVehicleClasses(value);
            break;
        }
        case SUMO_ATTR_DISALLOW: {
            myAllowedVehicles = parseVehicleClasses(value);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
