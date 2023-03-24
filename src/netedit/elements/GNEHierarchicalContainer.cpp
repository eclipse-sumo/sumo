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
/// @file    GNEHierarchicalContainer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Container for GNEHierarchicalElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>

// ===========================================================================
// static member definitions
// ===========================================================================

const bool GNEHierarchicalContainer::checkContainer = true;

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
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEJunction* junction) {
    // check junction
    if (checkContainer && (std::find(myParentJunctions.begin(), myParentJunctions.end(), junction) != myParentJunctions.end())) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEEdge* edge) {
    // check edge
    if (checkContainer && (std::find(myParentEdges.begin(), myParentEdges.end(), edge) != myParentEdges.end())) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNELane* lane) {
    // check lane
    if (checkContainer && (std::find(myParentLanes.begin(), myParentLanes.end(), lane) != myParentLanes.end())) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEAdditional* additional) {
    // check additional
    if (checkContainer && (std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional) != myParentAdditionals.end())) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEDemandElement* demandElement) {
    // check TAZElement
    if (checkContainer && (std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement) != myParentDemandElements.end())) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEGenericData* genericData) {
    // check generic data
    if (checkContainer && (std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData) != myParentGenericDatas.end())) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEJunction* junction) {
    // check junction
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (checkContainer && (it == myParentJunctions.end())) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEEdge* edge) {
    // check edge
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (checkContainer && (it == myParentEdges.end())) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNELane* lane) {
    // check lane
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (checkContainer && (it == myParentLanes.end())) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEAdditional* additional) {
    // check additional
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (checkContainer && (it == myParentAdditionals.end())) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (checkContainer && (it == myParentDemandElements.end())) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEHierarchicalElement* hierarchicalElement, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (checkContainer && (it == myParentGenericDatas.end())) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myParentGenericDatas.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEJunction* junction) {
    // check junction
    if (checkContainer && (std::find(myChildJunctions.begin(), myChildJunctions.end(), junction) != myChildJunctions.end())) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEEdge* edge) {
    // check edge
    if (checkContainer && (std::find(myChildEdges.begin(), myChildEdges.end(), edge) != myChildEdges.end())) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* hierarchicalElement, GNELane* lane) {
    // check lane
    if (checkContainer && (std::find(myChildLanes.begin(), myChildLanes.end(), lane) != myChildLanes.end())) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEAdditional* additional) {
    // check additional
    if (checkContainer && (std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional) != myChildAdditionals.end())) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* /*hierarchicalElement*/, GNEDemandElement* demandElement) {
    /*
    // check demand element
    if (checkContainer && (std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement) != myChildDemandElements.end())) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + demandElement->getTagStr() + " with ID='" + demandElement->getID() + "'");
    } else {
        myChildDemandElements.push_back(demandElement);
    }
    */
    myChildDemandElements.push_back(demandElement);

}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEGenericData* genericData) {
    // check generic data
    if (checkContainer && (std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData) != myChildGenericDatas.end())) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEJunction* junction) {
    // check junction
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (checkContainer && (it == myChildJunctions.end())) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEEdge* edge) {
    // check edge
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (checkContainer && (it == myChildEdges.end())) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNELane* lane) {
    // check lane
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (checkContainer && (it == myChildLanes.end())) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEAdditional* additional) {
    // check additional
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (checkContainer && (it == myChildAdditionals.end())) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEDemandElement* demandElement) {
    // check demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (checkContainer && (it == myChildDemandElements.end())) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
    } else {
        myChildDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEHierarchicalElement* hierarchicalElement, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (checkContainer && (it == myChildGenericDatas.end())) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + hierarchicalElement->getTagStr() + " with ID='" + hierarchicalElement->getID() + "'");
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
