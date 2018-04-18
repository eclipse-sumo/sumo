/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDestProbReroute.cpp
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
#include <netedit/dialogs/GNERerouterIntervalDialog.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEDestProbReroute.h"
#include <netedit/GNEUndoList.h>
#include "GNERerouter.h"
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include "GNERerouterInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDestProbReroute::GNEDestProbReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAttributeCarrier(SUMO_TAG_DEST_PROB_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalDialog->getEditedRerouterInterval()),
    myNewEdgeDestination(rerouterIntervalDialog->getEditedRerouterInterval()->getRerouterParent()->getEdgeChilds().at(0)),
    myProbability(getDefaultValue<double>(SUMO_TAG_ROUTE_PROB_REROUTE, SUMO_ATTR_PROB)) {
}


GNEDestProbReroute::GNEDestProbReroute(GNERerouterInterval* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability):
    GNEAttributeCarrier(SUMO_TAG_DEST_PROB_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalParent),
    myNewEdgeDestination(newEdgeDestination),
    myProbability(probability) {
}


GNEDestProbReroute::~GNEDestProbReroute() {}


void
GNEDestProbReroute::writeDestProbReroute(OutputDevice& device) const {
    // open tag
    device.openTag(getTag());
    // write edge ID
    device.writeAttr(SUMO_ATTR_ID, myNewEdgeDestination->getID());
    // write probability
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    // close tag
    device.closeTag();
}


GNERerouterInterval*
GNEDestProbReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}


void 
GNEDestProbReroute::selectAttributeCarrier() {
    // this AC cannot be selected
}


void 
GNEDestProbReroute::unselectAttributeCarrier() {
    // this AC cannot be unselected
}


bool 
GNEDestProbReroute::isAttributeCarrierSelected() const {
    // this AC doesn't own a select flag
    return false;
}


std::string
GNEDestProbReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myNewEdgeDestination->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDestProbReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PROB:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDestProbReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return (myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDestProbReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            myNewEdgeDestination = myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(value);
            break;
        }
        case SUMO_ATTR_PROB: {
            myProbability = parse<double>(value);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
