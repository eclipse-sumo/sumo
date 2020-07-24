/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/data/GNEGenericData.h>


#include "GNEHierarchicalElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEHierarchicalElement - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElement::GNEHierarchicalElement(const GNEAttributeCarrier* AC,
        const std::vector<GNEJunction*>& parentJunctions,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEShape*>& parentShapes,
        const std::vector<GNETAZElement*>& parentTAZElements,
        const std::vector<GNEDemandElement*>& ParentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas,
        const std::vector<GNEJunction*>& childJunctions,
        const std::vector<GNEEdge*>& childEdges,
        const std::vector<GNELane*>& childLanes,
        const std::vector<GNEAdditional*>& childAdditionals,
        const std::vector<GNEShape*>& childShapes,
        const std::vector<GNETAZElement*>& childTAZElements,
        const std::vector<GNEDemandElement*>& childDemandElements,
        const std::vector<GNEGenericData*>& childGenericDataElements) :
    myHierarchicalContainer(parentJunctions, parentEdges, parentLanes, parentAdditionals, parentShapes, parentTAZElements, ParentDemandElements, parentGenericDatas,
        childJunctions, childEdges, childLanes, childAdditionals, childShapes, childTAZElements, childDemandElements, childGenericDataElements),
    myHierarchicalConnections(this),
    myAC(AC) {
}


GNEHierarchicalElement::~GNEHierarchicalElement() {}


const GNEHierarchicalElementHelper::HierarchicalContainer&
GNEHierarchicalElement::getHierarchicalContainer() const {
    return myHierarchicalContainer;
}


void
GNEHierarchicalElement::restoreHierarchicalContainer(const GNEHierarchicalElementHelper::HierarchicalContainer &container) {
    myHierarchicalContainer = container;
}


std::vector<GNEHierarchicalElement*> 
GNEHierarchicalElement::getAllHierarchicalElements() const {
    // declare result
    std::vector<GNEHierarchicalElement*> result;
    // reserve
    result.reserve(myHierarchicalContainer.getContainerSize());
    // add parent elements
    for (const auto &element : myHierarchicalContainer.parentJunctions) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentEdges) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentLanes) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentAdditionals) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentShapes) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentTAZElements) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentDemandElements) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.parentGenericDatas) {
        result.push_back(element);
    }
    // add child elements
    for (const auto &element : myHierarchicalContainer.childJunctions) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childEdges) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childLanes) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childAdditionals) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childShapes) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childTAZElements) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childDemandElements) {
        result.push_back(element);
    }
    for (const auto &element : myHierarchicalContainer.childGenericDatas) {
        result.push_back(element);
    }
    return result;
}


const std::vector<GNEJunction*>&
GNEHierarchicalElement::getParentJunctions() const {
    return myHierarchicalContainer.parentJunctions;
}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getParentEdges() const {
    return myHierarchicalContainer.parentEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getParentLanes() const {
    return myHierarchicalContainer.parentLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getParentAdditionals() const {
    return myHierarchicalContainer.parentAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getParentShapes() const {
    return myHierarchicalContainer.parentShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getParentTAZElements() const {
    return myHierarchicalContainer.parentTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getParentDemandElements() const {
    return myHierarchicalContainer.parentDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getParentGenericDatas() const {
    return myHierarchicalContainer.parentGenericDatas;
}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getChildEdges() const {
    return myHierarchicalContainer.childEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getChildLanes() const {
    return myHierarchicalContainer.childLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getChildAdditionals() const {
    return myHierarchicalContainer.childAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getChildShapes() const {
    return myHierarchicalContainer.childShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getChildTAZElements() const {
    return myHierarchicalContainer.childTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElements() const {
    return myHierarchicalContainer.childDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getChildGenericDatas() const {
    return myHierarchicalContainer.childGenericDatas;
}


std::string
GNEHierarchicalElement::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(myHierarchicalContainer.parentEdges.size());
        // iterate over edges
        for (const auto& edge : myHierarchicalContainer.parentEdges) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(myHierarchicalContainer.parentLanes.size());
        // iterate over lanes
        for (const auto& lane : myHierarchicalContainer.parentLanes) {
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


void
GNEHierarchicalElement::updateHierarchicalConnections() {
    myHierarchicalConnections.update();
}


void
GNEHierarchicalElement::drawHierarchicalConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent, const double exaggeration) const {
    // first check if connections can be drawn
    if (!s.drawForRectangleSelection && (exaggeration > 0)) {
        myHierarchicalConnections.drawConnection(s, GLTypeParent, exaggeration);
    }
}


void 
GNEHierarchicalElement::drawChildDottedConnections(const GUIVisualizationSettings& s, const double exaggeration) const {
    // first check if connections can be drawn
    if (!s.drawForRectangleSelection && (exaggeration > 0)) {
        myHierarchicalConnections.drawDottedConnection(s, exaggeration);
    }
}


void
GNEHierarchicalElement::sortChildAdditionals() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (const auto& additional : myHierarchicalContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(additional);
            }
        }
        // obtain all exits
        for (const auto& additional : myHierarchicalContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(additional);
            }
        }
        // change myHierarchicalContainer.childAdditionals for sortedEntryExits
        if (sortedEntryExits.size() == myHierarchicalContainer.childAdditionals.size()) {
            myHierarchicalContainer.childAdditionals = sortedEntryExits;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (const auto& additional : myHierarchicalContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // obtain all TAZSinks
        for (const auto& additional : myHierarchicalContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // change myHierarchicalContainer.childAdditionals for sortedEntryExits
        if (sortedTAZSourceSink.size() == myHierarchicalContainer.childAdditionals.size()) {
            myHierarchicalContainer.childAdditionals = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted children
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
        // iterate over child additional
        for (const auto& additional : myHierarchicalContainer.childAdditionals) {
            sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), additional));
            // set begin/start attribute
            if (additional->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_TIME))) {
                sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_TIME);
            } else if (additional->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_BEGIN);
            }
            // set end attribute
            if (additional->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_END))) {
                sortedChildren.back().first.second = additional->getAttributeDouble(SUMO_ATTR_END);
            } else {
                sortedChildren.back().first.second = sortedChildren.back().first.first;
            }
        }
        // sort children
        std::sort(sortedChildren.begin(), sortedChildren.end());
        // make sure that number of sorted children is the same as the child additional
        if (sortedChildren.size() == myHierarchicalContainer.childAdditionals.size()) {
            myHierarchicalContainer.childAdditionals.clear();
            for (auto i : sortedChildren) {
                myHierarchicalContainer.childAdditionals.push_back(i.second);
            }
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    }
}


bool
GNEHierarchicalElement::checkChildAdditionalsOverlapping() const {
    // declare a vector to keep sorted children
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
    // iterate over child additional
    for (const auto& additional : myHierarchicalContainer.childAdditionals) {
        sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), additional));
        // set begin/start attribute
        if (additional->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_TIME))) {
            sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_TIME);
        } else if (additional->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_BEGIN);
        }
        // set end attribute
        if (additional->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_END))) {
            sortedChildren.back().first.second = additional->getAttributeDouble(SUMO_ATTR_END);
        } else {
            sortedChildren.back().first.second = sortedChildren.back().first.first;
        }
    }
    // sort children
    std::sort(sortedChildren.begin(), sortedChildren.end());
    // make sure that number of sorted children is the same as the child additional
    if (sortedChildren.size() == myHierarchicalContainer.childAdditionals.size()) {
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
        throw ProcessError("Some child additional were lost during sorting");
    }
}


void
GNEHierarchicalElement::sortChildDemandElements() {
    // by default empty
}


bool
GNEHierarchicalElement::checkChildDemandElementsOverlapping() const {
    return true;
}


void
GNEHierarchicalElement::updateParentAdditional() {
    // by default nothing to do
}

/****************************************************************************/
