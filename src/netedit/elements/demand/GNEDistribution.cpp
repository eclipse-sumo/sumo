/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// abstract distribution used in netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDistribution::GNEDistribution(GNENet* net, GUIGlObjectType type, SumoXMLTag elementTag, GUIIcon icon) :
    GNEDemandElement("", net, type, elementTag, GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEDistribution::GNEDistribution(GNENet* net, GUIGlObjectType type, SumoXMLTag elementTag, GUIIcon icon,
                                 const std::string& ID, const int deterministic) :
    GNEDemandElement(ID, net, type, elementTag,  GUIIconSubSys::getIcon(icon),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
myDeterministic(deterministic) {
}


GNEDistribution::~GNEDistribution() {}


GNEMoveOperation*
GNEDistribution::getMoveOperation() {
    // distributions cannot be moved
    return nullptr;
}


GNEDemandElement::Problem
GNEDistribution::isDemandElementValid() const {
    // currently distributions don't have problems
    return GNEDemandElement::Problem::OK;
}


std::string
GNEDistribution::getDemandElementProblem() const {
    return "";
}


void
GNEDistribution::fixDemandElementProblem() {
    // nothing to fix
}


SUMOVehicleClass
GNEDistribution::getVClass() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getVClass();
    } else {
        return SVC_IGNORING;
    }
}


const RGBColor&
GNEDistribution::getColor() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getColor();
    } else {
        return RGBColor::BLACK;
    }
}


void
GNEDistribution::updateGeometry() {
    // nothing to update
}


Position
GNEDistribution::getPositionInView() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getPositionInView();
    } else {
        return Position();
    }
}


std::string
GNEDistribution::getParentName() const {
    return myNet->getMicrosimID();
}


Boundary
GNEDistribution::getCenteringBoundary() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getCenteringBoundary();
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEDistribution::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEDistribution::drawGL(const GUIVisualizationSettings&) const {
    // Vehicle Types aren't draw
}


void
GNEDistribution::computePathElement() {
    // nothing to compute
}


void
GNEDistribution::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


void
GNEDistribution::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


GNELane*
GNEDistribution::getFirstPathLane() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getFirstPathLane();
    } else {
        return nullptr;
    }
}


GNELane*
GNEDistribution::getLastPathLane() const {
    if (getDistributionKeyValues().size() > 0) {
        return getDistributionKeyValues().begin()->first->getLastPathLane();
    } else {
        return nullptr;
    }
}


std::string
GNEDistribution::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_DETERMINISTIC:
            if (myDeterministic == -1) {
                return "";
            } else {
                return toString(myDeterministic);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEDistribution::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_ADDITIONALCHILDREN: {
            // count number of additional placed in distribution keys (needed for save distribution either in route or in additional file)
            double counter = 0;
            for (const auto& distributionKey : getDistributionKeyValues()) {
                counter += (double)distributionKey.first->getChildAdditionals().size();
            }
            return counter;
        }
        default:
            if (getDistributionKeyValues().size() > 0) {
                return getDistributionKeyValues().begin()->first->getAttributeDouble(key);
            } else {
                return 0;
            }
    }
}


Position
GNEDistribution::getAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
}


void
GNEDistribution::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_DETERMINISTIC:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDistribution::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(NamespaceIDs::routes, value);
        case SUMO_ATTR_DETERMINISTIC:
            if ((value == "-1") || value.empty()) {
                return true;
            } else {
                return canParse<int>(value) && (parse<int>(value) >= 0);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEDistribution::getPopUpID() const {
    return getTagStr();
}


std::string
GNEDistribution::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}


const Parameterised::Map&
GNEDistribution::getACParametersMap() const {
    throw InvalidArgument(getTagStr() + " doesn't have parameters");
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDistribution::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setDemandElementID(value);
            break;
        case SUMO_ATTR_DETERMINISTIC:
            if (value.empty()) {
                myDeterministic = -1;
            } else {
                myDeterministic = parse<int>(value);
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDistribution::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // distributions cannot be moved
}


void
GNEDistribution::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // distributions cannot be moved
}

/****************************************************************************/
