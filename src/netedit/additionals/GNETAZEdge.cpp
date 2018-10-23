/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>

#include "GNETAZEdge.h"
#include "GNETAZ.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZEdge::GNETAZEdge(GNEAdditional* TAZParent, GNEEdge* edge, double departWeight, double arrivalWeight) :
    GNEAdditional(TAZParent, TAZParent->getViewNet(), GLO_TAZ, SUMO_TAG_TAZEDGE, "", false),
    myEdge(edge),
    myDepartWeight(departWeight),
    myArrivalWeight(arrivalWeight) {
}


GNETAZEdge::~GNETAZEdge() {}


void
GNETAZEdge::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNETAZEdge::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNETAZEdge::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNETAZEdge::getPositionInView() const {
    return myFirstAdditionalParent->getPositionInView();
}


std::string
GNETAZEdge::getParentName() const {
    return myFirstAdditionalParent->getID();
}


void
GNETAZEdge::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNETAZEdge::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case GNE_ATTR_TAZ_DEPARTWEIGHT:
            return toString(myDepartWeight);
        case GNE_ATTR_TAZ_ARRIVALWEIGHT:
            return toString(myArrivalWeight);
        case GNE_ATTR_PARENT:
            return myFirstAdditionalParent->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZEdge::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case GNE_ATTR_TAZ_DEPARTWEIGHT:
        case GNE_ATTR_TAZ_ARRIVALWEIGHT:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZEdge::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case GNE_ATTR_TAZ_DEPARTWEIGHT:
        case GNE_ATTR_TAZ_ARRIVALWEIGHT:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNETAZEdge::getPopUpID() const {
    return toString(getTag());
}


std::string
GNETAZEdge::getHierarchyName() const {
    return toString(getTag()) + ": " + getAttribute(GNE_ATTR_TAZ_DEPARTWEIGHT) + "-" + getAttribute(GNE_ATTR_TAZ_ARRIVALWEIGHT);
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZEdge::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case GNE_ATTR_TAZ_DEPARTWEIGHT:
            myDepartWeight = parse<double>(value);
            break;
        case GNE_ATTR_TAZ_ARRIVALWEIGHT:
            myArrivalWeight = parse<double>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
