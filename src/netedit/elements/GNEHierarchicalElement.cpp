/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEHierarchicalElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>

#include "GNEHierarchicalElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEHierarchicalElement - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElement::GNEHierarchicalElement() {}


GNEHierarchicalElement::~GNEHierarchicalElement() {}


const GNEHierarchicalStructureParents&
GNEHierarchicalElement::getParents() const {
    return myHierarchicalStructureParents;
}


void
GNEHierarchicalElement::clearParents() {
    myHierarchicalStructureParents.clear();
}


const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalElement::getParentJunctions() const {
    return myHierarchicalStructureParents.get<GNEJunction*>();
}


const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalElement::getParentEdges() const {
    return myHierarchicalStructureParents.get<GNEEdge*>();
}


const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalElement::getParentLanes() const {
    return myHierarchicalStructureParents.get<GNELane*>();
}


const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalElement::getParentAdditionals() const {
    return myHierarchicalStructureParents.get<GNEAdditional*>();
}


const GNEHierarchicalContainerParents<GNEAdditional*>
GNEHierarchicalElement::getParentStoppingPlaces() const {
    GNEHierarchicalContainerParents<GNEAdditional*> stoppingPlaces;
    for (const auto& additional : getParentAdditionals()) {
        if (additional->getTagProperty()->isStoppingPlace()) {
            stoppingPlaces.push_back(additional);
        }
    }
    return stoppingPlaces;
}


const GNEHierarchicalContainerParents<GNEAdditional*>
GNEHierarchicalElement::getParentTAZs() const {
    GNEHierarchicalContainerParents<GNEAdditional*> TAZs;
    for (const auto& additional : getParentAdditionals()) {
        if (additional->getTagProperty()->isTAZElement()) {
            TAZs.push_back(additional);
        }
    }
    return TAZs;
}


const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalElement::getParentDemandElements() const {
    return myHierarchicalStructureParents.get<GNEDemandElement*>();
}


const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalElement::getParentGenericDatas() const {
    return myHierarchicalStructureParents.get<GNEGenericData*>();
}


const GNEHierarchicalStructureChildren&
GNEHierarchicalElement::getChildren() const {
    return myHierarchicalStructureChildren;
}


const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalElement::getChildJunctions() const {
    return myHierarchicalStructureChildren.get<GNEJunction*>();
}


const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalElement::getChildEdges() const {
    return myHierarchicalStructureChildren.get<GNEEdge*>();
}


const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalElement::getChildLanes() const {
    return myHierarchicalStructureChildren.get<GNELane*>();
}


const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalElement::getChildAdditionals() const {
    return myHierarchicalStructureChildren.get<GNEAdditional*>();
}


const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElements() const {
    return myHierarchicalStructureChildren.get<GNEDemandElement*>();
}


const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalElement::getChildGenericDatas() const {
    return myHierarchicalStructureChildren.get<GNEGenericData*>();
}


const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalElement::getChildTAZSourceSinks() const {
    return myHierarchicalStructureChildren.getSet<GNETAZSourceSink*>();
}


std::string
GNEHierarchicalElement::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty()->getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty()->getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(getParentEdges().size());
        // iterate over edges
        for (const auto& edge : getParentEdges()) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty()->getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty()->getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(getParentLanes().size());
        // iterate over lanes
        for (const auto& lane : getParentLanes()) {
            // add lane ID
            solution.push_back(lane->getID());
            // if current lane is the current element, then insert newNextElement ID
            if (lane == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    }
    // remove consecutive (adjacent) duplicates
    solution.erase(std::unique(solution.begin(), solution.end()), solution.end());
    // return solution
    return toString(solution);
}

/****************************************************************************/
