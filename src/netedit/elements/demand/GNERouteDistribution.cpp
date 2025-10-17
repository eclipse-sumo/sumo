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
/// @file    GNERouteDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// Route distribution used in netedit
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNERouteDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteDistribution::GNERouteDistribution(GNENet* net) :
    GNEDemandElement("", net, "", SUMO_TAG_ROUTE_DISTRIBUTION, GNEPathElement::Options::DEMAND_ELEMENT) {
}


GNERouteDistribution::GNERouteDistribution(const std::string& ID, GNENet* net, const std::string& filename) :
    GNEDemandElement(ID, net, filename, SUMO_TAG_ROUTE_DISTRIBUTION, GNEPathElement::Options::DEMAND_ELEMENT) {
}


GNERouteDistribution::~GNERouteDistribution() {}


GNEMoveElement*
GNERouteDistribution::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERouteDistribution::getParameters() {
    return nullptr;
}


const Parameterised*
GNERouteDistribution::getParameters() const {
    return nullptr;
}


void
GNERouteDistribution::writeDemandElement(OutputDevice& device) const {
    // write attributes
    device.openTag(getTagProperty()->getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    // check if write route or refs)
    for (const auto& refChild : getChildDemandElements()) {
        if (refChild->getTagProperty()->getTag() == GNE_TAG_ROUTEREF) {
            int numReferences = 0;
            for (const auto& routeChild : refChild->getParentDemandElements().at(1)->getChildDemandElements()) {
                if (routeChild->getTagProperty()->getTag() == GNE_TAG_ROUTEREF) {
                    numReferences++;
                }
            }
            if (numReferences == 1) {
                refChild->getParentDemandElements().at(1)->writeDemandElement(device);
            } else {
                refChild->writeDemandElement(device);
            }
        }
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNERouteDistribution::isDemandElementValid() const {
    // currently distributions don't have problems
    return GNEDemandElement::Problem::OK;
}


std::string
GNERouteDistribution::getDemandElementProblem() const {
    return "";
}


void
GNERouteDistribution::fixDemandElementProblem() {
    // nothing to fix
}


SUMOVehicleClass
GNERouteDistribution::getVClass() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getVClass();
    } else {
        return SVC_IGNORING;
    }
}


const RGBColor&
GNERouteDistribution::getColor() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getColor();
    } else {
        return RGBColor::INVISIBLE;
    }
}


void
GNERouteDistribution::updateGeometry() {
    // nothing to update
}


Position
GNERouteDistribution::getPositionInView() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getPositionInView();
    } else {
        return Position();
    }
}


std::string
GNERouteDistribution::getParentName() const {
    return myNet->getMicrosimID();
}


Boundary
GNERouteDistribution::getCenteringBoundary() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getCenteringBoundary();
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNERouteDistribution::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNERouteDistribution::drawGL(const GUIVisualizationSettings&) const {
    // Vehicle Types aren't draw
}


void
GNERouteDistribution::computePathElement() {
    // nothing to compute
}


void
GNERouteDistribution::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


void
GNERouteDistribution::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


GNELane*
GNERouteDistribution::getFirstPathLane() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getFirstPathLane();
    } else {
        return nullptr;
    }
}


GNELane*
GNERouteDistribution::getLastPathLane() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getLastPathLane();
    } else {
        return nullptr;
    }
}


std::string
GNERouteDistribution::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNERouteDistribution::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNERouteDistribution::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


void
GNERouteDistribution::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNERouteDistribution::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(NamespaceIDs::routes, value);
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNERouteDistribution::getPopUpID() const {
    return getTagStr();
}


std::string
GNERouteDistribution::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// private
// ===========================================================================

void
GNERouteDistribution::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setDemandElementID(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
