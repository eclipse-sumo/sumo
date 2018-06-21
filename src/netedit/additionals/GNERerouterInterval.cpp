/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouterInterval.cpp
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
#include <utils/common/MsgHandler.h>
#include "GNERerouterInterval.h"
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>

#include "GNEClosingLaneReroute.h"
#include "GNEParkingAreaReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"
#include "GNERerouter.h"
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNERerouterDialog* rerouterDialog) :
    GNEAdditional(rerouterDialog->getEditedRerouter(), rerouterDialog->getEditedRerouter()->getViewNet(), GLO_REROUTER, SUMO_TAG_INTERVAL, false, false),
    myRerouterParent(rerouterDialog->getEditedRerouter()),
    myBegin(parse<double>(getTagProperties(SUMO_TAG_INTERVAL).getDefaultValue(SUMO_ATTR_BEGIN))),
    myEnd(parse<double>(getTagProperties(SUMO_TAG_INTERVAL).getDefaultValue(SUMO_ATTR_END))) {
}


GNERerouterInterval::GNERerouterInterval(GNERerouter* rerouterParent, double begin, double end) :
    GNEAdditional(rerouterParent, rerouterParent->getViewNet(), GLO_REROUTER, SUMO_TAG_INTERVAL, false, false),
    myRerouterParent(rerouterParent),
    myBegin(begin),
    myEnd(end) {
}


GNERerouterInterval::~GNERerouterInterval() {}


GNERerouter*
GNERerouterInterval::getRerouterParent() const {
    return myRerouterParent;
}


double
GNERerouterInterval::getBegin() const {
    return myBegin;
}


double
GNERerouterInterval::getEnd() const {
    return myEnd;
}


void 
GNERerouterInterval::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNERerouterInterval::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void 
GNERerouterInterval::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNERerouterInterval::getPositionInView() const {
    return Position();
}


std::string 
GNERerouterInterval::getParentName() const {
    return myRerouterParent->getID();
}


void 
GNERerouterInterval::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNERerouterInterval::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        case SUMO_ATTR_END:
            return toString(myEnd);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            // change ID of Rerouter Interval
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            // Change Ids of all Rerouter childs
            for (auto i : myAdditionalChilds) {
                i->setAttribute(SUMO_ATTR_ID, generateAdditionalChildID(i->getTag()), undoList);
            }
            break;
        }
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouterInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) < myEnd);
        case SUMO_ATTR_END:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) > myBegin);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_BEGIN: {
            myBegin = parse<double>(value);
            break;
        }
        case SUMO_ATTR_END: {
            myEnd = parse<double>(value);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
