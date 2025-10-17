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
/// @file    GNEVTypeDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// VehicleType distribution used in netedit
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEVTypeDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVTypeDistribution::GNEVTypeDistribution(GNENet* net) :
    GNEDemandElement("", net, "", SUMO_TAG_VTYPE_DISTRIBUTION, GNEPathElement::Options::DEMAND_ELEMENT) {
}


GNEVTypeDistribution::GNEVTypeDistribution(const std::string& ID, GNENet* net, const std::string& filename, const int deterministic) :
    GNEDemandElement(ID, net, filename, SUMO_TAG_VTYPE_DISTRIBUTION, GNEPathElement::Options::DEMAND_ELEMENT),
    myDeterministic(deterministic) {
}


GNEVTypeDistribution::~GNEVTypeDistribution() {}


GNEMoveElement*
GNEVTypeDistribution::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEVTypeDistribution::getParameters() {
    return nullptr;
}


const Parameterised*
GNEVTypeDistribution::getParameters() const {
    return nullptr;
}


void
GNEVTypeDistribution::writeDemandElement(OutputDevice& device) const {
    // now write attributes
    device.openTag(getTagProperty()->getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (myDeterministic != myTagProperty->getDefaultIntValue(SUMO_ATTR_DETERMINISTIC)) {
        device.writeAttr(SUMO_ATTR_DETERMINISTIC, myDeterministic);
    }
    // check if write vType or refs)
    for (const auto& refChild : getChildDemandElements()) {
        if (refChild->getTagProperty()->getTag() == GNE_TAG_VTYPEREF) {
            int numReferences = 0;
            for (const auto& vTypeChild : refChild->getParentDemandElements().at(1)->getChildDemandElements()) {
                if (vTypeChild->getTagProperty()->getTag() == GNE_TAG_VTYPEREF) {
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
GNEVTypeDistribution::isDemandElementValid() const {
    // currently distributions don't have problems
    return GNEDemandElement::Problem::OK;
}


std::string
GNEVTypeDistribution::getDemandElementProblem() const {
    return "";
}


void
GNEVTypeDistribution::fixDemandElementProblem() {
    // nothing to fix
}


SUMOVehicleClass
GNEVTypeDistribution::getVClass() const {
    for (const auto& childDemandElement : getChildDemandElements()) {
        if (childDemandElement->getTagProperty()->hasAttribute(SUMO_ATTR_REFID)) {
            return childDemandElement->getVClass();
        }
    }
    return SVC_IGNORING;
}


const RGBColor&
GNEVTypeDistribution::getColor() const {
    for (const auto& childDemandElement : getChildDemandElements()) {
        if (childDemandElement->getTagProperty()->hasAttribute(SUMO_ATTR_REFID)) {
            return childDemandElement->getColor();
        }
    }
    return RGBColor::YELLOW;
}


void
GNEVTypeDistribution::updateGeometry() {
    // nothing to update
}


Position
GNEVTypeDistribution::getPositionInView() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getPositionInView();
    } else {
        return Position();
    }
}


std::string
GNEVTypeDistribution::getParentName() const {
    return myNet->getMicrosimID();
}


Boundary
GNEVTypeDistribution::getCenteringBoundary() const {
    if (getChildDemandElements().size() > 0) {
        return getChildDemandElements().front()->getCenteringBoundary();
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEVTypeDistribution::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEVTypeDistribution::drawGL(const GUIVisualizationSettings&) const {
    // Vehicle Types aren't draw
}


void
GNEVTypeDistribution::computePathElement() {
    // nothing to compute
}


void
GNEVTypeDistribution::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


void
GNEVTypeDistribution::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawJunctionPartialGL
}


GNELane*
GNEVTypeDistribution::getFirstPathLane() const {
    return nullptr;
}


GNELane*
GNEVTypeDistribution::getLastPathLane() const {
    return nullptr;
}


std::string
GNEVTypeDistribution::getAttribute(SumoXMLAttr key) const {
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
            // check if the given attribute is part of
            for (const auto& childDemandElement : getChildDemandElements()) {
                if (childDemandElement->getTagProperty()->hasAttribute(SUMO_ATTR_REFID)) {
                    return childDemandElement->getParentDemandElements().at(1)->getAttribute(key);
                }
            }
            return "";
    }
}


double
GNEVTypeDistribution::getAttributeDouble(SumoXMLAttr key) const {
    // first try to get value from type child
    for (const auto& childDemandElement : getChildDemandElements()) {
        if (childDemandElement->getTagProperty()->hasAttribute(SUMO_ATTR_REFID)) {
            return childDemandElement->getParentDemandElements().at(1)->getAttributeDouble(key);
        }
    }
    // special cases for vehicles
    switch (key) {
        case SUMO_ATTR_WIDTH:
            return 1;
        case SUMO_ATTR_LENGTH:
            return 3;
        default:
            return 0;
    }
}


Position
GNEVTypeDistribution::getAttributePosition(SumoXMLAttr key) const {
    for (const auto& childDemandElement : getChildDemandElements()) {
        if (childDemandElement->getTagProperty()->hasAttribute(SUMO_ATTR_REFID)) {
            return childDemandElement->getAttributePosition(key);
        }
    }
    return Position();
}


void
GNEVTypeDistribution::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_DETERMINISTIC:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEVTypeDistribution::isValid(SumoXMLAttr key, const std::string& value) {
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
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNEVTypeDistribution::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVTypeDistribution::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVTypeDistribution::setAttribute(SumoXMLAttr key, const std::string& value) {
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
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
