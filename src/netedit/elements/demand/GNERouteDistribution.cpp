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
/// @file    GNERouteDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// Route distribution used in netedit
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/common/StringTokenizer.h>

#include "GNERouteDistribution.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteDistribution::GNERouteDistribution(GNENet* net) :
    GNEDemandElement("", net, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION, GUIIconSubSys::getIcon(GUIIcon::ROUTEDISTRIBUTION),
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNERouteDistribution::GNERouteDistribution(GNENet* net, const std::string& vTypeID, const int deterministic) :
    GNEDemandElement(vTypeID, net, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION,  GUIIconSubSys::getIcon(GUIIcon::ROUTEDISTRIBUTION),
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT, {}, {}, {}, {}, {}, {}),
    myDeterministic(deterministic) {
}


GNERouteDistribution::~GNERouteDistribution() {}


GNEMoveOperation*
GNERouteDistribution::getMoveOperation() {
    return nullptr;
}


void
GNERouteDistribution::writeDemandElement(OutputDevice& device) const {
    // get vtypes that has this vType distribution
    std::vector<std::string> vTypes;
    std::vector<std::string> probabilities;
    // first obtain vTypes sorted by ID
    std::map<std::string, GNEDemandElement*> vTypesSorted;
    for (const auto& vType : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
        vTypesSorted[vType->getID()] = vType;
    }
    // nowe get type distributions and probabilities
    for (const auto& vType : vTypesSorted) {
        const auto typeDistributionIDs = StringTokenizer(vType.second->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION)).getVector();
        const auto distributionProbabilities = StringTokenizer(vType.second->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION_PROBABILITY)).getVector();
        for (int i = 0; i < (int)typeDistributionIDs.size(); i++) {
            if (typeDistributionIDs.at(i) == getID()) {
                vTypes.push_back(vType.second->getID());
                probabilities.push_back(distributionProbabilities.at(i));
            }
        }
    }
    // only save if there is vTypes to save
    if (vTypes.size() > 0) {
        device.openTag(getTagProperty().getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        device.writeAttr(SUMO_ATTR_ROUTES, vTypes);
        device.writeAttr(SUMO_ATTR_PROBS, probabilities);
        device.closeTag();
    }
}


GNEDemandElement::Problem
GNERouteDistribution::isDemandElementValid() const {
    // currently vTypeDistributions don't have problems
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
    return SVC_IGNORING;
}


const RGBColor&
GNERouteDistribution::getColor() const {
    return RGBColor::BLACK;
}


void
GNERouteDistribution::updateGeometry() {
    // nothing to update
}


Position
GNERouteDistribution::getPositionInView() const {
    return Position();
}


std::string
GNERouteDistribution::getParentName() const {
    return myNet->getMicrosimID();
}


Boundary
GNERouteDistribution::getCenteringBoundary() const {
    // Route distribution doesn't have boundaries
    return Boundary(-0.1, -0.1, 0.1, 0.1);
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
GNERouteDistribution::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawPartialGL
}


void
GNERouteDistribution::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // route distributions don't use drawPartialGL
}


GNELane*
GNERouteDistribution::getFirstPathLane() const {
    // vehicle types don't use lanes
    return nullptr;
}


GNELane*
GNERouteDistribution::getLastPathLane() const {
    // vehicle types don't use lanes
    return nullptr;
}


std::string
GNERouteDistribution::getAttribute(SumoXMLAttr key) const {
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
GNERouteDistribution::getAttributeDouble(SumoXMLAttr key) const {
    if (key == GNE_ATTR_ADDITIONALCHILDREN) {
        double counter = 0;
        for (const auto& vType : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
            // only write if appear in this distribution
            if (vType->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION) == getID() && vType->getChildAdditionals().size() > 0) {
                counter++;
            }
        }
        return counter;
    } else {
        // obtain all types with the given typeDistribution sorted by ID
        std::map<std::string, GNEDemandElement*> sortedTypes;
        for (const auto &type : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
            if (type->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION) == getID()) {
                sortedTypes[type->getID()] = type;
            }
        }
        // return first type, or default vType
        if (sortedTypes.size() > 0) {
            return sortedTypes.begin()->second->getAttributeDouble(key);
        } else {
            return 0;
        }
    }
}


Position
GNERouteDistribution::getAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a Position attribute of type '" + toString(key) + "'");
}


void
GNERouteDistribution::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNERouteDistribution::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (value == getID()) {
                return true;
            } else if (SUMOXMLDefinitions::isValidVehicleID(value)) {
                return (demandElementExist(value, {SUMO_TAG_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION}) == false);
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
GNERouteDistribution::getPopUpID() const {
    return getTagStr();
}


std::string
GNERouteDistribution::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}


const Parameterised::Map&
GNERouteDistribution::getACParametersMap() const {
    throw InvalidArgument(getTagStr() + " doesn't have parameters");
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
GNERouteDistribution::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // route distributions cannot be moved
}


void
GNERouteDistribution::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // route distributions cannot be moved
}

/****************************************************************************/
