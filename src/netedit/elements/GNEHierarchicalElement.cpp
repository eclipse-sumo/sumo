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
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

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
    myContainer(parentJunctions, parentEdges, parentLanes, parentAdditionals, parentShapes, parentTAZElements, ParentDemandElements, parentGenericDatas,
        childJunctions, childEdges, childLanes, childAdditionals, childShapes, childTAZElements, childDemandElements, childGenericDataElements),
    myChildConnections(this),
    myParentConnections(this),
    myAC(AC) {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        myDemandElementsByType[tag] = {};
    }
}


GNEHierarchicalElement::~GNEHierarchicalElement() {}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getParentEdges() const {
    return myContainer.parentEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getParentLanes() const {
    return myContainer.parentLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getParentAdditionals() const {
    return myContainer.parentAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getParentShapes() const {
    return myContainer.parentShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getParentTAZElements() const {
    return myContainer.parentTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getParentDemandElements() const {
    return myContainer.parentDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getParentGenericDatas() const {
    return myContainer.parentGenericDatas;
}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getChildEdges() const {
    return myContainer.childEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getChildLanes() const {
    return myContainer.childLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getChildAdditionals() const {
    return myContainer.childAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getChildShapes() const {
    return myContainer.childShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getChildTAZElements() const {
    return myContainer.childTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElements() const {
    return myContainer.childDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getChildGenericDatas() const {
    return myContainer.childGenericDataElements;
}


template <> void
GNEHierarchicalElement::addParentElement(GNEJunction* junction) {
    myContainer.addParentElement(myAC, junction, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNEEdge* edge) {
    myContainer.addParentElement(myAC, nullptr, edge, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNELane* lane) {
    myContainer.addParentElement(myAC, nullptr, nullptr, lane, nullptr, nullptr, nullptr, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNEAdditional* additional) {
    myContainer.addParentElement(myAC, nullptr, nullptr, nullptr, additional, nullptr, nullptr, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNEShape* shape) {
    myContainer.addParentElement(myAC, nullptr, nullptr, nullptr, nullptr, shape, nullptr, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNETAZElement* TAZElement) {
    myContainer.addParentElement(myAC, nullptr, nullptr, nullptr, nullptr, nullptr, TAZElement, nullptr, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNEDemandElement* demandElement) {
    myContainer.addParentElement(myAC, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, demandElement, nullptr);
}


template <> void
GNEHierarchicalElement::addParentElement(GNEGenericData* genericData) {
    myContainer.addParentElement(myAC, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, genericData);
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        auto it = std::find(myContainer.parentEdges.begin(), myContainer.parentEdges.end(), edge);
        if (it == myContainer.parentEdges.end()) {
            throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
        } else {
            myContainer.parentEdges.erase(it);
        }
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.parentLanes.begin(), myContainer.parentLanes.end(), lane) == myContainer.parentLanes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentLanes.erase(std::find(myContainer.parentLanes.begin(), myContainer.parentLanes.end(), lane));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myContainer.parentAdditionals.begin(), myContainer.parentAdditionals.end(), additional);
    if (it == myContainer.parentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.parentShapes.begin(), myContainer.parentShapes.end(), shape) == myContainer.parentShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentShapes.erase(std::find(myContainer.parentShapes.begin(), myContainer.parentShapes.end(), shape));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.parentTAZElements.begin(), myContainer.parentTAZElements.end(), TAZElement) == myContainer.parentTAZElements.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentTAZElements.erase(std::find(myContainer.parentTAZElements.begin(), myContainer.parentTAZElements.end(), TAZElement));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myContainer.parentDemandElements.begin(), myContainer.parentDemandElements.end(), demandElement);
    if (it == myContainer.parentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEGenericData* genericData) {
    // First check that GenericData was already inserted
    auto it = std::find(myContainer.parentGenericDatas.begin(), myContainer.parentGenericDatas.end(), genericData);
    if (it == myContainer.parentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.parentGenericDatas.erase(it);
    }
}


std::string
GNEHierarchicalElement::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(myContainer.parentEdges.size());
        // iterate over edges
        for (const auto& edge : myContainer.parentEdges) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(myContainer.parentLanes.size());
        // iterate over lanes
        for (const auto& lane : myContainer.parentLanes) {
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


GNEJunction*
GNEHierarchicalElement::getFirstParentJunction() const {
    if (myContainer.parentJunctions.size() > 0) {
        return myContainer.parentJunctions.at(0);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


GNEJunction*
GNEHierarchicalElement::getSecondParentJunction()const {
    if (myContainer.parentJunctions.size() > 1) {
        return myContainer.parentJunctions.at(1);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
}


void 
GNEHierarchicalElement::updateFirstParentJunction(GNEJunction* junction) {
    if (myContainer.parentJunctions.size() > 0) {
        myContainer.parentJunctions.at(0) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


void 
GNEHierarchicalElement::updateSecondParentJunction(GNEJunction* junction) {
    if (myContainer.parentJunctions.size() > 1) {
        myContainer.parentJunctions.at(1) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
}


std::vector<GNEEdge*>
GNEHierarchicalElement::getMiddleParentEdges() const {
    std::vector<GNEEdge*> middleEdges;
    // there are only middle edges if there is more than two edges
    if (myContainer.parentEdges.size() > 2) {
        // reserve middleEdges
        middleEdges.reserve(myContainer.parentEdges.size() - 2);
        // iterate over second and previous last parent edge
        for (auto i = (myContainer.parentEdges.begin() + 1); i != (myContainer.parentEdges.end() - 1); i++) {
            middleEdges.push_back(*i);
        }
    }
    return middleEdges;
}



const Position&
GNEHierarchicalElement::getChildPosition(const GNELane* lane) {
    for (const auto& childConnection : myChildConnections.symbolsPositionAndRotation) {
        if (childConnection.getLane() == lane) {
            return childConnection.getPosition();
        }
    }
    throw ProcessError("Lane doesn't exist");
}


double
GNEHierarchicalElement::getChildRotation(const GNELane* lane) {
    for (const auto& childConnection : myChildConnections.symbolsPositionAndRotation) {
        if (childConnection.getLane() == lane) {
            return childConnection.getRotation();
        }
    }
    throw ProcessError("Lane doesn't exist");
}


void
GNEHierarchicalElement::updateChildConnections() {
    myChildConnections.update();
}


void
GNEHierarchicalElement::drawChildConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent, const double exaggeration) const {
    // first check if connections can be drawn
    if (!s.drawForRectangleSelection && (exaggeration > 0)) {
        myChildConnections.drawConnection(s, GLTypeParent, exaggeration);
    }
}


void 
GNEHierarchicalElement::drawChildDottedConnections(const GUIVisualizationSettings& s, const double exaggeration) const {
    // first check if connections can be drawn
    if (!s.drawForRectangleSelection && (exaggeration > 0)) {
        myChildConnections.drawDottedConnection(s, exaggeration);
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childLanes.push_back(lane);
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEAdditional* additional) {
    // Check if additional is valid
    if (additional == nullptr) {
        throw InvalidArgument("Trying to add an empty child additional in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in child additional container
        myContainer.childAdditionals.push_back(additional);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildAdditionals();
        }
        // update parent additional after add additional (note: by default non-implemented)
        updateParentAdditional();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.childShapes.begin(), myContainer.childShapes.end(), shape) != myContainer.childShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childShapes.push_back(shape);
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and doesn't exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.childTAZElements.begin(), myContainer.childTAZElements.end(), TAZElement) != myContainer.childTAZElements.end()) {
        throw InvalidArgument("Trying to add a duplicate child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childTAZElements.push_back(TAZElement);
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEDemandElement* demandElement) {
    // Check if demand element is valid
    if (demandElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child demand element in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in demandElement child container
        myContainer.childDemandElements.push_back(demandElement);
        // add it also in SortedChildDemandElementsByType container
        myDemandElementsByType.at(demandElement->getTagProperty().getTag()).push_back(demandElement);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }
}

template <> void
GNEHierarchicalElement::addChildElement(GNEGenericData* genericDataElement) {
    // Check if demand element is valid
    if (genericDataElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child generic data element in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in generic data element child container
        myContainer.childGenericDataElements.push_back(genericDataElement);
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.childEdges.begin(), myContainer.childEdges.end(), edge) == myContainer.childEdges.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childEdges.erase(std::find(myContainer.childEdges.begin(), myContainer.childEdges.end(), edge));
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childLanes.erase(std::find(myContainer.childLanes.begin(), myContainer.childLanes.end(), lane));
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myContainer.childAdditionals.begin(), myContainer.childAdditionals.end(), additional);
    if (it == myContainer.childAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childAdditionals.erase(it);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildAdditionals();
        }
        // update parent additional after add additional (note: by default non-implemented)
        updateParentAdditional();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.childShapes.begin(), myContainer.childShapes.end(), shape) == myContainer.childShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childShapes.erase(std::find(myContainer.childShapes.begin(), myContainer.childShapes.end(), shape));
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to remove an empty child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myContainer.childTAZElements.begin(), myContainer.childTAZElements.end(), TAZElement) == myContainer.childTAZElements.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myContainer.childTAZElements.erase(std::find(myContainer.childTAZElements.begin(), myContainer.childTAZElements.end(), TAZElement));
        // update connections geometry
        myParentConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myContainer.childDemandElements.begin(), myContainer.childDemandElements.end(), demandElement);
    auto itByType = std::find(myDemandElementsByType.at(demandElement->getTagProperty().getTag()).begin(), myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end(), demandElement);
    if (it == myContainer.childDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myContainer.childDemandElements.begin(), myContainer.childDemandElements.end(), demandElement) == 1;
        myContainer.childDemandElements.erase(it);
        // only remove it from mySortedChildDemandElementsByType if is a single element
        if (singleElement && (itByType != myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end())) {
            myDemandElementsByType.at(demandElement->getTagProperty().getTag()).erase(itByType);
        }
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }
}

template <> void
GNEHierarchicalElement::removeChildElement(GNEGenericData* genericDataElement) {
    // First check that genericDataElement was already inserted
    auto it = std::find(myContainer.childGenericDataElements.begin(), myContainer.childGenericDataElements.end(), genericDataElement);
    if (it == myContainer.childGenericDataElements.end()) {
        throw ProcessError(genericDataElement->getTagStr() + " with ID='" + genericDataElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // remove it from child demand elements
        myContainer.childGenericDataElements.erase(it);
    }
}


void
GNEHierarchicalElement::sortChildAdditionals() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (const auto& additional : myContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(additional);
            }
        }
        // obtain all exits
        for (const auto& additional : myContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(additional);
            }
        }
        // change myContainer.childAdditionals for sortedEntryExits
        if (sortedEntryExits.size() == myContainer.childAdditionals.size()) {
            myContainer.childAdditionals = sortedEntryExits;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (const auto& additional : myContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // obtain all TAZSinks
        for (const auto& additional : myContainer.childAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // change myContainer.childAdditionals for sortedEntryExits
        if (sortedTAZSourceSink.size() == myContainer.childAdditionals.size()) {
            myContainer.childAdditionals = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted children
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
        // iterate over child additional
        for (const auto& additional : myContainer.childAdditionals) {
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
        if (sortedChildren.size() == myContainer.childAdditionals.size()) {
            myContainer.childAdditionals.clear();
            for (auto i : sortedChildren) {
                myContainer.childAdditionals.push_back(i.second);
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
    for (const auto& additional : myContainer.childAdditionals) {
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
    if (sortedChildren.size() == myContainer.childAdditionals.size()) {
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


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElementsByType(SumoXMLTag tag) const {
    return myDemandElementsByType.at(tag);
}


void
GNEHierarchicalElement::sortChildDemandElements() {
    // by default empty
}


bool
GNEHierarchicalElement::checkChildDemandElementsOverlapping() const {
    return true;
}


GNEDemandElement*
GNEHierarchicalElement::getPreviousChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(myContainer.childDemandElements.begin(), myContainer.childDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myContainer.childDemandElements.end()) {
        return nullptr;
    } else if (it == myContainer.childDemandElements.begin()) {
        return nullptr;
    } else {
        return *(it - 1);
    }
}


GNEDemandElement*
GNEHierarchicalElement::getNextChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(myContainer.childDemandElements.begin(), myContainer.childDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myContainer.childDemandElements.end()) {
        return nullptr;
    } else if (it == (myContainer.childDemandElements.end() - 1)) {
        return nullptr;
    } else {
        return *(it + 1);
    }
}


void
GNEHierarchicalElement::updateParentAdditional() {
    // by default nothing to do
}

void
GNEHierarchicalElement::updateParentDemandElement() {
    // by default nothing to do
}


void
GNEHierarchicalElement::changeChildEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of child edges
    for (const auto& edge : myContainer.childEdges) {
        edge->removeParentElement(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myContainer.childEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // add demandElement into parent edges
    for (const auto& edge : myContainer.childEdges) {
        edge->addParentElement(elementChild);
    }
    // update connections geometry
    myParentConnections.update();
}


void
GNEHierarchicalElement::changeChildLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of child lanes
    for (const auto& lane : myContainer.childLanes) {
        lane->removeParentElement(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myContainer.childLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // add demandElement into parent lanes
    for (const auto& lane : myContainer.childLanes) {
        lane->addParentElement(elementChild);
    }
    // update connections geometry
    myParentConnections.update();
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElement - protected methods
// ---------------------------------------------------------------------------

void
GNEHierarchicalElement::replaceParentEdges(GNEShape* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myContainer.parentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myContainer.parentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myContainer.parentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myContainer.parentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEDemandElement* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of parent edges
    for (const auto& edge : myContainer.parentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myContainer.parentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myContainer.parentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myContainer.parentEdges = newEdges;
    // check that lane parets aren't empty
    if (myContainer.parentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEGenericData* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove genericData of parent edges
    for (const auto& edge : myContainer.parentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myContainer.parentEdges = newEdges;
    // check that lane parets aren't empty
    if (myContainer.parentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add genericData into parent edges
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceFirstParentEdge(GNEDemandElement* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myContainer.parentEdges.size() < 1) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myContainer.parentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myContainer.parentEdges[0] = newFirstEdge;
        // add demandElement into parent edges
        myContainer.parentEdges.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceFirstParentEdge(GNEGenericData* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myContainer.parentEdges.size() < 1) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myContainer.parentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myContainer.parentEdges[0] = newFirstEdge;
        // add generic data into parent edges
        myContainer.parentEdges.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceMiddleParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newMiddleEdges, const bool updateChildReferences) {
    // declare a vector for new parent edges
    std::vector<GNEEdge*> newEdges;
    // check if add first edge
    if (myContainer.parentEdges.size() > 0) {
        newEdges.push_back(myContainer.parentEdges.front());
    }
    // add newMiddleEdges
    for (const auto& edge : newMiddleEdges) {
        newEdges.push_back(edge);
    }
    // check if add last edge
    if (myContainer.parentEdges.size() > 1) {
        newEdges.push_back(myContainer.parentEdges.back());
    }
    // check if we have to update references in all childs, or simply update parent edges vector
    if (updateChildReferences) {
        replaceParentEdges(elementChild, newEdges);
    } else {
        myContainer.parentEdges = newEdges;
    }
}


void
GNEHierarchicalElement::replaceLastParentEdge(GNEDemandElement* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myContainer.parentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myContainer.parentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myContainer.parentEdges.pop_back();
        myContainer.parentEdges.push_back(newLastEdge);
        // add demandElement into parent edges
        myContainer.parentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceLastParentEdge(GNEGenericData* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myContainer.parentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myContainer.parentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myContainer.parentEdges.pop_back();
        myContainer.parentEdges.push_back(newLastEdge);
        // add generic data into parent edges
        myContainer.parentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove additional of parent edges
    for (const auto& lane : myContainer.parentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myContainer.parentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& lane : myContainer.parentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEDemandElement* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myContainer.parentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myContainer.parentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myContainer.parentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEShape* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myContainer.parentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myContainer.parentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myContainer.parentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myContainer.parentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEShape* shapeTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myContainer.parentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent additional
        myContainer.parentAdditionals.at(additionalParentIndex) = shapeTobeChanged->getNet()->retrieveAdditional(myContainer.parentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->addChildElement(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEAdditional* additionalTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myContainer.parentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent additional
        myContainer.parentAdditionals.at(additionalParentIndex) = additionalTobeChanged->getNet()->retrieveAdditional(myContainer.parentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->addChildElement(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEDemandElement* demandElementTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myContainer.parentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove demand element of the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent demand element
        myContainer.parentAdditionals.at(additionalParentIndex) = demandElementTobeChanged->getNet()->retrieveAdditional(myContainer.parentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add demand element int the children of parent additional
        myContainer.parentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceFirstParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newFirstTAZElement) {
    // first check that at least there is two TAZElements
    if (myContainer.parentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove generic data of parent TAZElements
        myContainer.parentTAZElements.front()->removeChildElement(elementChild);
        // replace first TAZElement
        myContainer.parentTAZElements[0] = newFirstTAZElement;
        // add generic data into parent TAZElements
        myContainer.parentTAZElements.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceLastParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newLastTAZElement) {
    // first check that at least there is two TAZElements
    if (myContainer.parentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove demandElement of parent TAZElements
        myContainer.parentTAZElements.back()->removeChildElement(elementChild);
        // replace last TAZElement
        myContainer.parentTAZElements.pop_back();
        myContainer.parentTAZElements.push_back(newLastTAZElement);
        // add demandElement into parent TAZElements
        myContainer.parentTAZElements.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEShape* shapeTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myContainer.parentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent demand element
        myContainer.parentDemandElements.at(demandElementParentIndex) = shapeTobeChanged->getNet()->retrieveDemandElement(myContainer.parentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->addChildElement(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEAdditional* additionalTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myContainer.parentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent demand element
        myContainer.parentDemandElements.at(demandElementParentIndex) = additionalTobeChanged->getNet()->retrieveDemandElement(myContainer.parentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->addChildElement(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEDemandElement* demandElementTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myContainer.parentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove additional of the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex) = demandElementTobeChanged->getNet()->retrieveDemandElement(myContainer.parentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add additional int the children of parent additional
        myContainer.parentDemandElements.at(demandElementParentIndex)->addChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}

/****************************************************************************/
