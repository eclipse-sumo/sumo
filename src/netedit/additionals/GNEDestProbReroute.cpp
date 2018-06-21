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
#include <config.h>

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
    GNEAdditional("", rerouterIntervalDialog->getEditedRerouterInterval()->getViewNet(), GLO_REROUTER, SUMO_TAG_DEST_PROB_REROUTE, false, false),
    myRerouterIntervalParent(rerouterIntervalDialog->getEditedRerouterInterval()),
    myNewEdgeDestination(rerouterIntervalDialog->getEditedRerouterInterval()->getRerouterParent()->getEdgeChilds().at(0)),
    myProbability(parse<double>(getTagProperties(SUMO_TAG_ROUTE_PROB_REROUTE).getDefaultValue(SUMO_ATTR_PROB))) {
}


GNEDestProbReroute::GNEDestProbReroute(GNERerouterInterval* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability):
    GNEAdditional("", rerouterIntervalParent->getViewNet(), GLO_REROUTER, SUMO_TAG_DEST_PROB_REROUTE, false, false),
    myRerouterIntervalParent(rerouterIntervalParent),
    myNewEdgeDestination(newEdgeDestination),
    myProbability(probability) {
}


GNEDestProbReroute::~GNEDestProbReroute() {}


GNERerouterInterval*
GNEDestProbReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}


void 
GNEDestProbReroute::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNEDestProbReroute::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}

void 
GNEDestProbReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNEDestProbReroute::getPositionInView() const {
    return Position();
}


std::string 
GNEDestProbReroute::getParentName() const {
    return myRerouterIntervalParent->getID();
}


void 
GNEDestProbReroute::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
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
