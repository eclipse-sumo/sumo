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
    myParentConnections(this),
    myParentJunctions(parentJunctions),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentAdditionals(parentAdditionals),
    myParentShapes(parentShapes),
    myParentTAZElements(parentTAZElements),
    myParentDemandElements(ParentDemandElements),
    myParentGenericDatas(parentGenericDatas),
    myChildConnections(this),
    myChildJunctions(childJunctions),
    myChildEdges(childEdges),
    myChildLanes(childLanes),
    myChildAdditionals(childAdditionals),
    myChildShapes(childShapes),
    myChildTAZElements(childTAZElements),
    myChildDemandElements(childDemandElements),
    myChildGenericDataElements(childGenericDataElements),
    myAC(AC) {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        myDemandElementsByType[tag] = {};
    }
}


GNEHierarchicalElement::~GNEHierarchicalElement() {}


template <> void
GNEHierarchicalElement::addParentElement(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentEdges.begin(), myParentEdges.end(), edge) != myParentEdges.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) != myParentLanes.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional) != myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) != myParentShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and doesn't exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement) != myParentTAZElements.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNEDemandElement* demandElement) {
    // First check that demandElement wasn't already inserted
    if (std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement) != myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalElement::addParentElement(GNEGenericData* genericData) {
    // First check that GenericData wasn't already inserted
    if (std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData) != myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
        if (it == myParentEdges.end()) {
            throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
        } else {
            myParentEdges.erase(it);
        }
    }
}



template <> void
GNEHierarchicalElement::removeParentElement(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) == myParentLanes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.erase(std::find(myParentLanes.begin(), myParentLanes.end(), lane));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it == myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) == myParentShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.erase(std::find(myParentShapes.begin(), myParentShapes.end(), shape));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement) == myParentTAZElements.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentTAZElements.erase(std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement));
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it == myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalElement::removeParentElement(GNEGenericData* genericData) {
    // First check that GenericData was already inserted
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it == myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.erase(it);
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getParentEdges() const {
    return myParentEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getParentLanes() const {
    return myParentLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getParentAdditionals() const {
    return myParentAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getParentShapes() const {
    return myParentShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getParentTAZElements() const {
    return myParentTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getParentDemandElements() const {
    return myParentDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getParentGenericDatas() const {
    return myParentGenericDatas;
}


std::string
GNEHierarchicalElement::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(myParentEdges.size());
        // iterate over edges
        for (const auto& edge : myParentEdges) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(myParentLanes.size());
        // iterate over lanes
        for (const auto& lane : myParentLanes) {
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
    if (myParentJunctions.size() > 0) {
        return myParentJunctions.at(0);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


GNEJunction*
GNEHierarchicalElement::getSecondParentJunction()const {
    if (myParentJunctions.size() > 1) {
        return myParentJunctions.at(1);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
}


void 
GNEHierarchicalElement::updateFirstParentJunction(GNEJunction* junction) {
    if (myParentJunctions.size() > 0) {
        myParentJunctions.at(0) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


void 
GNEHierarchicalElement::updateSecondParentJunction(GNEJunction* junction) {
    if (myParentJunctions.size() > 1) {
        myParentJunctions.at(1) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
}


std::vector<GNEEdge*>
GNEHierarchicalElement::getMiddleParentEdges() const {
    std::vector<GNEEdge*> middleEdges;
    // there are only middle edges if there is more than two edges
    if (myParentEdges.size() > 2) {
        // reserve middleEdges
        middleEdges.reserve(myParentEdges.size() - 2);
        // iterate over second and previous last parent edge
        for (auto i = (myParentEdges.begin() + 1); i != (myParentEdges.end() - 1); i++) {
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
        myChildEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildLanes.push_back(lane);
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEAdditional* additional) {
    // Check if additional is valid
    if (additional == nullptr) {
        throw InvalidArgument("Trying to add an empty child additional in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in child additional container
        myChildAdditionals.push_back(additional);
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
    } else if (std::find(myChildShapes.begin(), myChildShapes.end(), shape) != myChildShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildShapes.push_back(shape);
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and doesn't exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildTAZElements.begin(), myChildTAZElements.end(), TAZElement) != myChildTAZElements.end()) {
        throw InvalidArgument("Trying to add a duplicate child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildTAZElements.push_back(TAZElement);
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::addChildElement(GNEDemandElement* demandElement) {
    // Check if demand element is valid
    if (demandElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child demand element in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in demandElement child container
        myChildDemandElements.push_back(demandElement);
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
        myChildGenericDataElements.push_back(genericDataElement);
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildEdges.begin(), myChildEdges.end(), edge) == myChildEdges.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildEdges.erase(std::find(myChildEdges.begin(), myChildEdges.end(), edge));
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildLanes.erase(std::find(myChildLanes.begin(), myChildLanes.end(), lane));
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it == myChildAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildAdditionals.erase(it);
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
    } else if (std::find(myChildShapes.begin(), myChildShapes.end(), shape) == myChildShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildShapes.erase(std::find(myChildShapes.begin(), myChildShapes.end(), shape));
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to remove an empty child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildTAZElements.begin(), myChildTAZElements.end(), TAZElement) == myChildTAZElements.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child TAZElement in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildTAZElements.erase(std::find(myChildTAZElements.begin(), myChildTAZElements.end(), TAZElement));
        // update connections geometry
        myChildConnections.update();
    }
}


template <> void
GNEHierarchicalElement::removeChildElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    auto itByType = std::find(myDemandElementsByType.at(demandElement->getTagProperty().getTag()).begin(), myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end(), demandElement);
    if (it == myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement) == 1;
        myChildDemandElements.erase(it);
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
    auto it = std::find(myChildGenericDataElements.begin(), myChildGenericDataElements.end(), genericDataElement);
    if (it == myChildGenericDataElements.end()) {
        throw ProcessError(genericDataElement->getTagStr() + " with ID='" + genericDataElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // remove it from child demand elements
        myChildGenericDataElements.erase(it);
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElement::getChildEdges() const {
    return myChildEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalElement::getChildLanes() const {
    return myChildLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElement::getChildAdditionals() const {
    return myChildAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalElement::getChildShapes() const {
    return myChildShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalElement::getChildTAZElements() const {
    return myChildTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElement::getChildDemandElements() const {
    return myChildDemandElements;
}


const std::vector<GNEGenericData*>&
GNEHierarchicalElement::getChildGenericDatas() const {
    return myChildGenericDataElements;
}


void
GNEHierarchicalElement::sortChildAdditionals() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(additional);
            }
        }
        // obtain all exits
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(additional);
            }
        }
        // change myChildAdditionals for sortedEntryExits
        if (sortedEntryExits.size() == myChildAdditionals.size()) {
            myChildAdditionals = sortedEntryExits;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // obtain all TAZSinks
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // change myChildAdditionals for sortedEntryExits
        if (sortedTAZSourceSink.size() == myChildAdditionals.size()) {
            myChildAdditionals = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted children
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
        // iterate over child additional
        for (const auto& additional : myChildAdditionals) {
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
        if (sortedChildren.size() == myChildAdditionals.size()) {
            myChildAdditionals.clear();
            for (auto i : sortedChildren) {
                myChildAdditionals.push_back(i.second);
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
    for (const auto& additional : myChildAdditionals) {
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
    if (sortedChildren.size() == myChildAdditionals.size()) {
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
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myChildDemandElements.end()) {
        return nullptr;
    } else if (it == myChildDemandElements.begin()) {
        return nullptr;
    } else {
        return *(it - 1);
    }
}


GNEDemandElement*
GNEHierarchicalElement::getNextChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myChildDemandElements.end()) {
        return nullptr;
    } else if (it == (myChildDemandElements.end() - 1)) {
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
    for (const auto& edge : myChildEdges) {
        edge->removeParentElement(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myChildEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // add demandElement into parent edges
    for (const auto& edge : myChildEdges) {
        edge->addParentElement(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}


void
GNEHierarchicalElement::changeChildLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of child lanes
    for (const auto& lane : myChildLanes) {
        lane->removeParentElement(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myChildLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // add demandElement into parent lanes
    for (const auto& lane : myChildLanes) {
        lane->addParentElement(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElement - protected methods
// ---------------------------------------------------------------------------

void
GNEHierarchicalElement::replaceParentEdges(GNEShape* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEDemandElement* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentEdges(GNEGenericData* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove genericData of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add genericData into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceFirstParentEdge(GNEDemandElement* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 1) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myParentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add demandElement into parent edges
        myParentEdges.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceFirstParentEdge(GNEGenericData* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 1) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add generic data into parent edges
        myParentEdges.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceMiddleParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newMiddleEdges, const bool updateChildReferences) {
    // declare a vector for new parent edges
    std::vector<GNEEdge*> newEdges;
    // check if add first edge
    if (myParentEdges.size() > 0) {
        newEdges.push_back(myParentEdges.front());
    }
    // add newMiddleEdges
    for (const auto& edge : newMiddleEdges) {
        newEdges.push_back(edge);
    }
    // check if add last edge
    if (myParentEdges.size() > 1) {
        newEdges.push_back(myParentEdges.back());
    }
    // check if we have to update references in all childs, or simply update parent edges vector
    if (updateChildReferences) {
        replaceParentEdges(elementChild, newEdges);
    } else {
        myParentEdges = newEdges;
    }
}


void
GNEHierarchicalElement::replaceLastParentEdge(GNEDemandElement* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myParentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add demandElement into parent edges
        myParentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceLastParentEdge(GNEGenericData* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add generic data into parent edges
        myParentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove additional of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEDemandElement* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentLanes(GNEShape* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEShape* shapeTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = shapeTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildElement(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEAdditional* additionalTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = additionalTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildElement(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentAdditional(GNEDemandElement* demandElementTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove demand element of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent demand element
        myParentAdditionals.at(additionalParentIndex) = demandElementTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add demand element int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceFirstParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newFirstTAZElement) {
    // first check that at least there is two TAZElements
    if (myParentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove generic data of parent TAZElements
        myParentTAZElements.front()->removeChildElement(elementChild);
        // replace first TAZElement
        myParentTAZElements[0] = newFirstTAZElement;
        // add generic data into parent TAZElements
        myParentTAZElements.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceLastParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newLastTAZElement) {
    // first check that at least there is two TAZElements
    if (myParentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove demandElement of parent TAZElements
        myParentTAZElements.back()->removeChildElement(elementChild);
        // replace last TAZElement
        myParentTAZElements.pop_back();
        myParentTAZElements.push_back(newLastTAZElement);
        // add demandElement into parent TAZElements
        myParentTAZElements.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEShape* shapeTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = shapeTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEAdditional* additionalTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = additionalTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElement::replaceParentDemandElement(GNEDemandElement* demandElementTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove additional of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent additional
        myParentDemandElements.at(demandElementParentIndex) = demandElementTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add additional int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


// ---------------------------------------------------------------------------
// GNEHierarchicalElement::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElement::ChildConnections::ConnectionGeometry::ConnectionGeometry(GNELane* lane) :
    myLane(lane),
    myRotation(0) {
    // set position and length depending of shape's lengt
    if (lane->getLaneShape().length() - 6 > 0) {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
    } else {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
    }
}


const GNELane*
GNEHierarchicalElement::ChildConnections::ConnectionGeometry::getLane() const {
    return myLane;
}

const Position&
GNEHierarchicalElement::ChildConnections::ConnectionGeometry::getPosition() const {
    return myPosition;
}


double
GNEHierarchicalElement::ChildConnections::ConnectionGeometry::getRotation() const {
    return myRotation;
}


GNEHierarchicalElement::ChildConnections::ConnectionGeometry::ConnectionGeometry() :
    myLane(nullptr),
    myRotation(0) {
}


GNEHierarchicalElement::ChildConnections::ChildConnections(GNEHierarchicalElement* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElement::ChildConnections::update() {
    // first clear containers
    connectionsGeometries.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& edge : myHierarchicalElement->myChildEdges) {
        for (const auto& lane : edge->getLanes()) {
            symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& lane : myHierarchicalElement->myChildLanes) {
        symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
    }
    // calculate position for every child additional
    for (const auto& additional : myHierarchicalElement->myChildAdditionals) {
        // check that additional position is different of parent position
        if (additional->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            // create connection shape
            std::vector<Position> connectionShape;
            const double A = std::abs(additional->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            const double B = std::abs(additional->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            connectionShape.push_back(additional->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > additional->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x() + A, additional->getPositionInView().y()));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() + B));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x() - A, additional->getPositionInView().y()));
                }
            }
            connectionShape.push_back(myHierarchicalElement->getPositionInView());
            // declare Geometry
            GNEGeometry::Geometry geometry;
            // update geometry with connectino shape
            geometry.updateGeometry(connectionShape);
            // add geometry in connectionsGeometry
            connectionsGeometries.push_back(geometry);
        }
    }
    // calculate geometry for connections between parent and children
    for (const auto& symbol : symbolsPositionAndRotation) {
        // create connection shape
        std::vector<Position> connectionShape;
        const double A = std::abs(symbol.getPosition().x() - myHierarchicalElement->getPositionInView().x());
        const double B = std::abs(symbol.getPosition().y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        connectionShape.push_back(symbol.getPosition());
        if (myHierarchicalElement->getPositionInView().x() > symbol.getPosition().x()) {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x() + A, symbol.getPosition().y()));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() + B));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x() - A, symbol.getPosition().y()));
            }
        }
        connectionShape.push_back(myHierarchicalElement->getPositionInView());
        // declare Geometry
        GNEGeometry::Geometry geometry;
        // update geometry with connectino shape
        geometry.updateGeometry(connectionShape);
        // add geometry in connectionsGeometry
        connectionsGeometries.push_back(geometry);
    }
}


void
GNEHierarchicalElement::ChildConnections::drawConnection(const GUIVisualizationSettings& s, const GUIGlObjectType parentType, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
        // Set color of the base
        GLHelper::setColor(s.colorSettings.childConnections);
        // Draw box lines
        GLHelper::drawBoxLines(connectionGeometry.getShape(), connectionGeometry.getShapeRotations(), connectionGeometry.getShapeLengths(), exaggeration * 0.1);
        // Pop draw matrix
        glPopMatrix();
    }
}


void
GNEHierarchicalElement::ChildConnections::drawDottedConnection(const GUIVisualizationSettings& s, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // calculate dotted geometry
        GNEGeometry::DottedGeometry dottedGeometry(s, connectionGeometry.getShape(), false);
        // change default width
        dottedGeometry.setWidth(0.1);
        // use drawDottedContourLane to draw it
        GNEGeometry::drawDottedContourLane(true, s, dottedGeometry, exaggeration * 0.1, false, false);
    }
}


/****************************************************************************/
