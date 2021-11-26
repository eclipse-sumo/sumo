/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>

#include "GNERouteProbReroute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNENet* net) :
    GNEAdditional("", net, GLO_REROUTER_ROUTEPROBREROUTE, SUMO_TAG_ROUTE_PROB_REROUTE, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myProbability(0) {
    // reset default values
    resetDefaultValues();
}


GNERouteProbReroute::GNERouteProbReroute(GNEAdditional* rerouterIntervalParent, GNEDemandElement* route, double probability) :
    GNEAdditional(rerouterIntervalParent->getNet(), GLO_REROUTER_ROUTEPROBREROUTE, SUMO_TAG_ROUTE_PROB_REROUTE, "",
        {}, {}, {}, {rerouterIntervalParent}, {}, {}, {route}, {},
    std::map<std::string, std::string>()),
    myProbability(probability) {
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNERouteProbReroute::~GNERouteProbReroute() {}


void
GNERouteProbReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE_PROB_REROUTE);
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_ROUTE));
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    device.closeTag();
}


GNEMoveOperation*
GNERouteProbReroute::getMoveOperation() {
    // GNERouteProbReroutes cannot be moved
    return nullptr;
}


void
GNERouteProbReroute::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


void
GNERouteProbReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    myAdditionalBoundary.add(getPositionInView());
    myAdditionalBoundary.grow(5);
}


Position
GNERouteProbReroute::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5 + 6.25, (getDrawPositionIndex() * -1) - getParentAdditionals().front()->getDrawPositionIndex() + 1, 0);
    // return signPosition
    return signPosition;
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
    // draw route prob reroute as listed attribute
    drawListedAddtional(s, getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                        1, getParentAdditionals().front()->getDrawPositionIndex(),
                        RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_ROUTEPROBREROUTE,
                        getAttribute(SUMO_ATTR_ROUTE) + ": " + getAttribute(SUMO_ATTR_PROB));
}


std::string
GNERouteProbReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_ROUTE:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERouteProbReroute::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERouteProbReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_ROUTE:
            return (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERouteProbReroute::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
            setMicrosimID(value);
            break;
        case SUMO_ATTR_ROUTE:
            replaceDemandElementParent(SUMO_TAG_ROUTE, value, 0);
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbReroute::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void GNERouteProbReroute::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
