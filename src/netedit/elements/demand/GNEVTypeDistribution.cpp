/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    GNEVTypeDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// VehicleType distribution used in netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEVTypeDistribution.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVTypeDistribution::GNEVTypeDistribution(GNENet* net) :
    GNEDemandElement("", net, GLO_VTYPE, SUMO_TAG_VTYPE_DISTRIBUTION, GUIIconSubSys::getIcon(GUIIcon::VTYPEDISTRIBUTION),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEVTypeDistribution::GNEVTypeDistribution(GNENet* net, const std::string& vTypeID, const int deterministic) :
    GNEDemandElement(vTypeID, net, GLO_VTYPE, SUMO_TAG_VTYPE_DISTRIBUTION,  GUIIconSubSys::getIcon(GUIIcon::VTYPEDISTRIBUTION),
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
myDeterministic(deterministic) {
}


GNEVTypeDistribution::~GNEVTypeDistribution() {}


GNEMoveOperation*
GNEVTypeDistribution::getMoveOperation() {
    return nullptr;
}


void
GNEVTypeDistribution::writeDemandElement(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    // write all vTypes
    for (const auto& vType : getChildDemandElements()) {
        vType->writeDemandElement(device);
    }
    device.closeTag();
}


GNEDemandElement::Problem
GNEVTypeDistribution::isDemandElementValid() const {
    // currently vTypeDistributions don't have problems
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
    return SVC_IGNORING;
}


const RGBColor&
GNEVTypeDistribution::getColor() const {
    return RGBColor::BLACK;
}


void
GNEVTypeDistribution::updateGeometry() {
    // nothing to update
}


Position
GNEVTypeDistribution::getPositionInView() const {
    return Position();
}


std::string
GNEVTypeDistribution::getParentName() const {
    return myNet->getMicrosimID();
}


Boundary
GNEVTypeDistribution::getCenteringBoundary() const {
    // VehicleType distribution doesn't have boundaries
    return Boundary(-0.1, -0.1, 0.1, 0.1);
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
GNEVTypeDistribution::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // vehicleType distributions don't use drawPartialGL
}


void
GNEVTypeDistribution::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // vehicleType distributions don't use drawPartialGL
}


GNELane*
GNEVTypeDistribution::getFirstPathLane() const {
    // vehicle types don't use lanes
    return nullptr;
}


GNELane*
GNEVTypeDistribution::getLastPathLane() const {
    // vehicle types don't use lanes
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEVTypeDistribution::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


Position
GNEVTypeDistribution::getAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
}


void
GNEVTypeDistribution::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_DETERMINISTIC:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVTypeDistribution::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // Vtypes and PTypes shares namespace
            if (SUMOXMLDefinitions::isValidVehicleID(value) && (myNet->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_DETERMINISTIC:
            if (value == "-1" || value.empty()) {
                return true;
            } else {
                return canParse<int>(value) && (parse<int>(value) >= 0);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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


const Parameterised::Map&
GNEVTypeDistribution::getACParametersMap() const {
    throw InvalidArgument(getTagStr() + " doesn't have parameters");
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVTypeDistribution::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setMicrosimID(value);
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
GNEVTypeDistribution::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // vehicleType distributions cannot be moved
}


void
GNEVTypeDistribution::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // vehicleType distributions cannot be moved
}

/****************************************************************************/
