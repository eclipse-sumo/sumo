/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNERouteProbReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNERouteProbReroute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_ROUTE_PROB_REROUTE, ""),
    GNEAdditionalListed(this) {
}


GNERouteProbReroute::GNERouteProbReroute(GNEAdditional* rerouterIntervalParent, GNEDemandElement* route, double probability) :
    GNEAdditional(rerouterIntervalParent, SUMO_TAG_ROUTE_PROB_REROUTE, ""),
    GNEAdditionalListed(this),
    myProbability(probability) {
    // set parents
    setParent<GNEAdditional*>(rerouterIntervalParent);
    setParent<GNEDemandElement*>(route);
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNERouteProbReroute::~GNERouteProbReroute() {}


GNEMoveElement*
GNERouteProbReroute::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERouteProbReroute::getParameters() {
    return nullptr;
}


const Parameterised*
GNERouteProbReroute::getParameters() const {
    return nullptr;
}


void
GNERouteProbReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE_PROB_REROUTE);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_ROUTE));
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    device.closeTag();
}


bool
GNERouteProbReroute::isAdditionalValid() const {
    return true;
}


std::string
GNERouteProbReroute::getAdditionalProblem() const {
    return "";
}


void
GNERouteProbReroute::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNERouteProbReroute::checkDrawMoveContour() const {
    return false;
}


void
GNERouteProbReroute::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNERouteProbReroute::getPositionInView() const {
    return getListedPositionInView();
}


void
GNERouteProbReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNERouteProbReroute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNERouteProbReroute::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNERouteProbReroute::drawGL(const GUIVisualizationSettings& s) const {
    // draw dest prob reroute as listed attribute
    drawListedAdditional(s, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_ROUTEPROBREROUTE,
                         getAttribute(SUMO_ATTR_ROUTE) + ": " + getAttribute(SUMO_ATTR_PROB));
}


std::string
GNERouteProbReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_ROUTE:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNERouteProbReroute::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNERouteProbReroute::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNERouteProbReroute::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNERouteProbReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_ROUTE:
        case SUMO_ATTR_PROB:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNERouteProbReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_ROUTE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::routes, value, false) == nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value);
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNERouteProbReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNERouteProbReroute::getHierarchyName() const {
    return getTagStr() + ": " + getParentDemandElements().front()->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERouteProbReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_ROUTE:
            replaceDemandElementParent(SUMO_TAG_ROUTE, value, 0);
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
