/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteProbReroute.cpp
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

#include "GNERouteProbReroute.h"
#include "GNEEdge.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNERerouter.h"
#include "GNERerouterIntervalDialog.h"
#include "GNEViewNet.h"
#include "GNENet.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE_PROB_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalDialog->getEditedRerouterInterval()),
    myNewRouteId(rerouterIntervalDialog->getEditedRerouterInterval()->getRerouterParent()->getViewNet()->getNet()->generateCalibratorRouteID()),
    myProbability(getDefaultValue<double>(SUMO_TAG_ROUTE_PROB_REROUTE, SUMO_ATTR_PROB)) {
}


GNERouteProbReroute::GNERouteProbReroute(GNERerouterInterval* rerouterIntervalParent, const std::string& newRouteId, double probability) :
    GNEAttributeCarrier(SUMO_TAG_ROUTE_PROB_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalParent),
    myNewRouteId(newRouteId),
    myProbability(probability) {
}


GNERouteProbReroute::~GNERouteProbReroute() {}


void
GNERouteProbReroute::writeRouteProbReroute(OutputDevice& device) const {
    // open Tag
    device.openTag(getTag());
    // write Route ID
    device.writeAttr(SUMO_ATTR_ID, myNewRouteId);
    // write Probability
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    // close tag
    device.closeTag();
}


GNERerouterInterval*
GNERouteProbReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}


std::string
GNERouteProbReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myNewRouteId;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNERouteProbReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value);
        case SUMO_ATTR_PROB:
            return canParse<double>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNERouteProbReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            myNewRouteId = value;
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
