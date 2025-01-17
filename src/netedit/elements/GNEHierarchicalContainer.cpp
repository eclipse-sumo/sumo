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
/// @file    GNEHierarchicalContainer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Container for GNEHierarchicalElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>

#define CHECK_DUPLICATES

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalContainer::GNEHierarchicalContainer() {}


GNEHierarchicalContainer::GNEHierarchicalContainer(
    const std::vector<GNEJunction*>& parentJunctions,
    const std::vector<GNEEdge*>& parentEdges,
    const std::vector<GNELane*>& parentLanes,
    const std::vector<GNEAdditional*>& parentAdditionals,
    const std::vector<GNEDemandElement*>& ParentDemandElements,
    const std::vector<GNEGenericData*>& parentGenericDatas):
    myParentJunctions(parentJunctions),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentAdditionals(parentAdditionals),
    myParentDemandElements(ParentDemandElements),
    myParentGenericDatas(parentGenericDatas) {
}


size_t
GNEHierarchicalContainer::getContainerSize() const {
    return (
               myParentJunctions.size() +
               myParentEdges.size() +
               myParentLanes.size() +
               myParentAdditionals.size() +
               myParentDemandElements.size() +
               myParentGenericDatas.size() +
               myChildJunctions.size() +
               myChildEdges.size() +
               myChildLanes.size() +
               myChildAdditionals.size() +
               myChildDemandElements.size() +
               myChildGenericDatas.size()
           );
}


template <> void
GNEHierarchicalContainer::addParentElement(GNEJunction* junction) {
    myParentJunctions.push_back(junction);
}


template <> void
GNEHierarchicalContainer::addParentElement(GNEEdge* edge) {
    myParentEdges.push_back(edge);
}


template <> void
GNEHierarchicalContainer::addParentElement(GNELane* lane) {
    myParentLanes.push_back(lane);
}


template <> void
GNEHierarchicalContainer::addParentElement(GNEAdditional* additional) {
    myParentAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalContainer::addParentElement(GNEDemandElement* demandElement) {
    myParentDemandElements.push_back(demandElement);
}


template <> void
GNEHierarchicalContainer::addParentElement(GNEGenericData* genericData) {
    myParentGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNEJunction* junction) {
    // check junction
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it == myParentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' is not a parent element");
    } else {
        myParentJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNEEdge* edge) {
    // check edge
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it == myParentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' is not a parent element");
    } else {
        myParentEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNELane* lane) {
    // check lane
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it == myParentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' is not a parent element");
    } else {
        myParentLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNEAdditional* additional) {
    // check additional
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it == myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' is not a parent element");
    } else {
        myParentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it == myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " is not a parent element");
    } else {
        myParentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it == myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " is not a parent element");
    } else {
        myParentGenericDatas.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(GNEJunction* junction) {
    myChildJunctions.push_back(junction);
}


template <> void
GNEHierarchicalContainer::addChildElement(GNEEdge* edge) {
    myChildEdges.push_back(edge);
}


template <> void
GNEHierarchicalContainer::addChildElement(GNELane* lane) {
    myChildLanes.push_back(lane);
}


template <> void
GNEHierarchicalContainer::addChildElement(GNEAdditional* additional) {
    myChildAdditionals.push_back(additional);
}


template <> void
GNEHierarchicalContainer::addChildElement(GNEDemandElement* demandElement) {
    myChildDemandElements.push_back(demandElement);

}


template <> void
GNEHierarchicalContainer::addChildElement(GNEGenericData* genericData) {
    myChildGenericDatas.push_back(genericData);
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNEJunction* junction) {
    // check junction
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (it == myChildJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " is not a child element");
    } else {
        myChildJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNEEdge* edge) {
    // check edge
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (it == myChildEdges.end()) {
        throw ProcessError(edge->getTagStr() + " is not a child element");
    } else {
        myChildEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNELane* lane) {
    // check lane
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (it == myChildLanes.end()) {
        throw ProcessError(lane->getTagStr() + " is not a child element");
    } else {
        myChildLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNEAdditional* additional) {
    // check additional
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it == myChildAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " is not a child element");
    } else {
        myChildAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNEDemandElement* demandElement) {
    // check demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (it == myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " is not a child element");
    } else {
        myChildDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (it == myChildGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " is not a child element");
    } else {
        myChildGenericDatas.erase(it);
    }
}


template<> const std::vector<GNEJunction*>&
GNEHierarchicalContainer::getParents() const {
    return myParentJunctions;
}


template<> const std::vector<GNEEdge*>&
GNEHierarchicalContainer::getParents() const {
    return myParentEdges;
}


template<> const std::vector<GNELane*>&
GNEHierarchicalContainer::getParents() const {
    return myParentLanes;
}


template<> const std::vector<GNEAdditional*>&
GNEHierarchicalContainer::getParents() const {
    return myParentAdditionals;
}


template<> const std::vector<GNEDemandElement*>&
GNEHierarchicalContainer::getParents() const {
    return myParentDemandElements;
}


template<> const std::vector<GNEGenericData*>&
GNEHierarchicalContainer::getParents() const {
    return myParentGenericDatas;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNEJunction*>& newParents) {
    myParentJunctions = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNEEdge*>& newParents) {
    myParentEdges = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNELane*>& newParents) {
    myParentLanes = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNEAdditional*>& newParents) {
    myParentAdditionals = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNEDemandElement*>& newParents) {
    myParentDemandElements = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNEGenericData*>& newParents) {
    myParentGenericDatas = newParents;
}


template<> const std::vector<GNEJunction*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildJunctions;
}


template<> const std::vector<GNEEdge*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildEdges;
}


template<> const std::vector<GNELane*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildLanes;
}


template<> const std::vector<GNEAdditional*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildAdditionals;
}


template<> const std::vector<GNEDemandElement*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildDemandElements;
}


template<> const std::vector<GNEGenericData*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildGenericDatas;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNEJunction*>& newChildren) {
    myChildJunctions = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNEEdge*>& newChildren) {
    myChildEdges = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNELane*>& newChildren) {
    myChildLanes = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNEAdditional*>& newChildren) {
    myChildAdditionals = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNEDemandElement*>& newChildren) {
    myChildDemandElements = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNEGenericData*>& newChildren) {
    myChildGenericDatas = newChildren;
}

/****************************************************************************/
