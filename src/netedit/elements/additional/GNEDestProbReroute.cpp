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
/// @file    GNEDestProbReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>

#include "GNEDestProbReroute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDestProbReroute::GNEDestProbReroute(GNENet* net):
    GNEAdditional("", net, "", SUMO_TAG_DEST_PROB_REROUTE, ""),
    GNEAdditionalListed(this),
    myNewEdgeDestination(nullptr),
    myProbability(0) {
}


GNEDestProbReroute::GNEDestProbReroute(GNEAdditional* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability):
    GNEAdditional(rerouterIntervalParent, SUMO_TAG_DEST_PROB_REROUTE, ""),
    GNEAdditionalListed(this),
    myNewEdgeDestination(newEdgeDestination),
    myProbability(probability) {
    // set parents
    setParent<GNEAdditional*>(rerouterIntervalParent);
    setParent<GNEEdge*>(newEdgeDestination);
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEDestProbReroute::~GNEDestProbReroute() {}


GNEMoveElement*
GNEDestProbReroute::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEDestProbReroute::getParameters() {
    return nullptr;
}


const Parameterised*
GNEDestProbReroute::getParameters() const {
    return nullptr;
}


void
GNEDestProbReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_DEST_PROB_REROUTE);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_EDGE));
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    device.closeTag();
}


bool
GNEDestProbReroute::isAdditionalValid() const {
    return true;
}


std::string
GNEDestProbReroute::getAdditionalProblem() const {
    return "";
}


void
GNEDestProbReroute::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEDestProbReroute::checkDrawMoveContour() const {
    return false;
}


void
GNEDestProbReroute::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNEDestProbReroute::getPositionInView() const {
    return getListedPositionInView();
}


void
GNEDestProbReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNEDestProbReroute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEDestProbReroute::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEDestProbReroute::drawGL(const GUIVisualizationSettings& s) const {
    // draw dest prob reroute as listed attribute
    drawListedAdditional(s, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_DESTPROBREROUTE,
                         getAttribute(SUMO_ATTR_EDGE) + ": " + getAttribute(SUMO_ATTR_PROB));
}


std::string
GNEDestProbReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGE:
            return myNewEdgeDestination->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNEDestProbReroute::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEDestProbReroute::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEDestProbReroute::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEDestProbReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_PROB:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEDestProbReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            return (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNEDestProbReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNEDestProbReroute::getHierarchyName() const {
    return getTagStr() + ": " + myNewEdgeDestination->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDestProbReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            myNewEdgeDestination = myNet->getAttributeCarriers()->retrieveEdge(value);
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
