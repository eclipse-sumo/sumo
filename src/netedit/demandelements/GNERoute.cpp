/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A class for visualizing routes in Netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/netelements/GNELane.h>
#include <utils/options/OptionsCont.h>

#include "GNERoute.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNERoute::GNERoute(GNEViewNet* viewNet, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) :
    GNEDemandElement(routeID, viewNet, GLO_ROUTE, SUMO_TAG_ROUTE),
    myEdges(edges),
    myColor(color) {
}


GNERoute::~GNERoute() {}


const std::vector<GNEEdge*>&
GNERoute::getGNEEdges() const {
    return myEdges;
}


void
GNERoute::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNERoute::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNERoute::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNERoute::getPositionInView() const {
    return Position();
}


std::string
GNERoute::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNERoute::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNERoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_EDGES:
            return parseIDs(myEdges);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_COLOR:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERoute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNEDemandElement::isRouteValid(parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value), false);
            } else {
                return false;
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERoute::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNERoute::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_EDGES:
            myEdges = parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/