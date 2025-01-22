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
/// @file    GNEHierarchicalElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>


#include "GNEHierarchicalElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEHierarchicalElement - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElement::GNEHierarchicalElement(GNENet* net, SumoXMLTag tag,
        const std::vector<GNEJunction*>& parentJunctions,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEDemandElement*>& ParentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas) :
    GNEAttributeCarrier(tag, net),
    myHierarchicalStructure(parentJunctions, parentEdges, parentLanes, parentAdditionals, ParentDemandElements, parentGenericDatas) {
}


GNEHierarchicalElement::~GNEHierarchicalElement() {}


const GNEHierarchicalStructure&
GNEHierarchicalElement::getHierarchicalContainer() const {
    return myHierarchicalStructure;
}


void
GNEHierarchicalElement::restoreHierarchicalContainer(const GNEHierarchicalStructure& container) {
    myHierarchicalStructure = container;
}


std::vector<GNEHierarchicalElement*>
GNEHierarchicalElement::getAllHierarchicalElements() const {
    // declare result
    std::vector<GNEHierarchicalElement*> result;
    // reserve
    result.reserve(myHierarchicalStructure.getContainerSize());
    // add parent elements
    for (const auto& element : getParentJunctions()) {
        result.push_back(element);
    }
    for (const auto& element : getParentEdges()) {
        result.push_back(element);
    }
    for (const auto& element : getParentLanes()) {
        result.push_back(element);
    }
    for (const auto& element : getParentAdditionals()) {
        result.push_back(element);
    }
    for (const auto& element : getParentDemandElements()) {
        result.push_back(element);
    }
    for (const auto& element : getParentGenericDatas()) {
        result.push_back(element);
    }
    // add child elements
    for (const auto& element : getChildJunctions()) {
        result.push_back(element);
    }
    for (const auto& element : getChildEdges()) {
        result.push_back(element);
    }
    for (const auto& element : getChildLanes()) {
        result.push_back(element);
    }
    for (const auto& element : getChildAdditionals()) {
        result.push_back(element);
    }
    for (const auto& element : getChildTAZSourceSinks()) {
        result.push_back(element);
    }
    for (const auto& element : getChildDemandElements()) {
        result.push_back(element);
    }
    for (const auto& element : getChildGenericDatas()) {
        result.push_back(element);
    }
    return result;
}


const GNEHierarchicalContainerParents<GNEJunction*>&
GNEHierarchicalElement::getParentJunctions() const {
    return myHierarchicalStructure.getParents<GNEJunction*>();
}


const GNEHierarchicalContainerParents<GNEEdge*>&
GNEHierarchicalElement::getParentEdges() const {
    return myHierarchicalStructure.getParents<GNEEdge*>();
}


const GNEHierarchicalContainerParents<GNELane*>&
GNEHierarchicalElement::getParentLanes() const {
    return myHierarchicalStructure.getParents<GNELane*>();
}


const GNEHierarchicalContainerParents<GNEAdditional*>&
GNEHierarchicalElement::getParentAdditionals() const {
    return myHierarchicalStructure.getParents<GNEAdditional*>();
}


const GNEHierarchicalContainerParents<GNEAdditional*>
GNEHierarchicalElement::getParentStoppingPlaces() const {
    GNEHierarchicalContainerParents<GNEAdditional*> stoppingPlaces;
    for (const auto& additional : getParentAdditionals()) {
        if (additional->getTagProperty().isStoppingPlace()) {
            stoppingPlaces.push_back(additional);
        }
    }
    return stoppingPlaces;
}


const GNEHierarchicalContainerParents<GNEAdditional*>
GNEHierarchicalElement::getParentTAZs() const {
    GNEHierarchicalContainerParents<GNEAdditional*> TAZs;
    for (const auto& additional : getParentAdditionals()) {
        if (additional->getTagProperty().isTAZElement()) {
            TAZs.push_back(additional);
        }
    }
    return TAZs;
}


const GNEHierarchicalContainerParents<GNEDemandElement*>&
GNEHierarchicalElement::getParentDemandElements() const {
    return myHierarchicalStructure.getParents<GNEDemandElement*>();
}


const GNEHierarchicalContainerParents<GNEGenericData*>&
GNEHierarchicalElement::getParentGenericDatas() const {
    return myHierarchicalStructure.getParents<GNEGenericData*>();
}


const GNEHierarchicalContainerChildren<GNEJunction*>&
GNEHierarchicalElement::getChildJunctions() const {
    return myHierarchicalStructure.getChildren<GNEJunction*>();
}


const GNEHierarchicalContainerChildren<GNEEdge*>&
GNEHierarchicalElement::getChildEdges() const {
    return myHierarchicalStructure.getChildren<GNEEdge*>();
}


const GNEHierarchicalContainerChildren<GNELane*>&
GNEHierarchicalElement::getChildLanes() const {
    return myHierarchicalStructure.getChildren<GNELane*>();
}


const GNEHierarchicalContainerChildren<GNEAdditional*>&
GNEHierarchicalElement::getChildAdditionals() const {
    return myHierarchicalStructure.getChildren<GNEAdditional*>();
}


const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>&
GNEHierarchicalElement::getChildTAZSourceSinks() const {
    return myHierarchicalStructure.getChildrenSet<GNETAZSourceSink*>();
}


const GNEHierarchicalContainerChildren<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElements() const {
    return myHierarchicalStructure.getChildren<GNEDemandElement*>();
}


const GNEHierarchicalContainerChildren<GNEGenericData*>&
GNEHierarchicalElement::getChildGenericDatas() const {
    return myHierarchicalStructure.getChildren<GNEGenericData*>();
}


std::string
GNEHierarchicalElement::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
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
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
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


bool
GNEHierarchicalElement::checkChildAdditionalsOverlapping() const {
    // declare a vector to keep sorted children
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
    // iterate over child meanData
    for (const auto& meanData : getChildAdditionals()) {
        sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), meanData));
        // set begin/start attribute
        if (meanData->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(meanData->getAttribute(SUMO_ATTR_TIME))) {
            sortedChildren.back().first.first = meanData->getAttributeDouble(SUMO_ATTR_TIME);
        } else if (meanData->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(meanData->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChildren.back().first.first = meanData->getAttributeDouble(SUMO_ATTR_BEGIN);
        }
        // set end attribute
        if (meanData->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(meanData->getAttribute(SUMO_ATTR_END))) {
            sortedChildren.back().first.second = meanData->getAttributeDouble(SUMO_ATTR_END);
        } else {
            sortedChildren.back().first.second = sortedChildren.back().first.first;
        }
    }
    // sort children
    std::sort(sortedChildren.begin(), sortedChildren.end());
    // make sure that number of sorted children is the same as the child meanData
    if (sortedChildren.size() == getChildAdditionals().size()) {
        if (sortedChildren.size() <= 1) {
            return true;
        } else {
            // check overlapping
            for (int i = 0; i < (int)sortedChildren.size() - 1; i++) {
                if (sortedChildren.at(i).first.second > sortedChildren.at(i + 1).first.first) {
                    return false;
                }
            }
        }
        return true;
    } else {
        throw ProcessError(TL("Some child meanData were lost during sorting"));
    }
}


bool
GNEHierarchicalElement::checkChildDemandElementsOverlapping() const {
    return true;
}

/****************************************************************************/
