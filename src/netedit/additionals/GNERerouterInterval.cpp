/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>

#include "GNERerouterInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNERerouterDialog* rerouterDialog) :
    GNEAdditional(rerouterDialog->getEditedAdditional(), rerouterDialog->getEditedAdditional()->getViewNet(), GLO_REROUTER, SUMO_TAG_INTERVAL, "", false,
{}, {}, {}, {rerouterDialog->getEditedAdditional()}, {}, {}, {}, {}, {}, {}) {
    // fill reroute interval with default values
    setDefaultValues();
}


GNERerouterInterval::GNERerouterInterval(GNEAdditional* rerouterParent, SUMOTime begin, SUMOTime end) :
    GNEAdditional(rerouterParent, rerouterParent->getViewNet(), GLO_REROUTER, SUMO_TAG_INTERVAL, "", false,
{}, {}, {}, {rerouterParent}, {}, {}, {}, {}, {}, {}),
myBegin(begin),
myEnd(end) {
}


GNERerouterInterval::~GNERerouterInterval() {}

void
GNERerouterInterval::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNERerouterInterval::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNERerouterInterval::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position
GNERerouterInterval::getPositionInView() const {
    return getAdditionalParents().at(0)->getPositionInView();
}


Boundary
GNERerouterInterval::getCenteringBoundary() const {
    return getAdditionalParents().at(0)->getCenteringBoundary();
}


std::string
GNERerouterInterval::getParentName() const {
    return getAdditionalParents().at(0)->getID();
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
            return time2string(myBegin);
        case SUMO_ATTR_END:
            return time2string(myEnd);
        case GNE_ATTR_PARENT:
            return getAdditionalParents().at(0)->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            // Change Ids of all Rerouter children
            for (auto i : getAdditionalChildren()) {
                i->setAttribute(SUMO_ATTR_ID, generateChildID(i->getTagProperty().getTag()), undoList);
            }
            break;
        }
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouterInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_BEGIN:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) < myEnd);
        case SUMO_ATTR_END:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) > myBegin);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERerouterInterval::getPopUpID() const {
    return getTagStr();
}


std::string
GNERerouterInterval::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
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
        case SUMO_ATTR_BEGIN:
            myBegin = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<SUMOTime>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
