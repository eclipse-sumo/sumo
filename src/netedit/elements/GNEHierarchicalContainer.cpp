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
/// @file    GNEHierarchicalElementHelper.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Container for GNEHierarchicalElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalContainer::GNEHierarchicalContainer() {}


GNEHierarchicalContainer::GNEHierarchicalContainer(
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
    const std::vector<GNEGenericData*>& childGenericDatas):
    myParentJunctions(parentJunctions),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentAdditionals(parentAdditionals),
    myParentShapes(parentShapes),
    myParentTAZElements(parentTAZElements),
    myParentDemandElements(ParentDemandElements),
    myParentGenericDatas(parentGenericDatas),
    myChildJunctions(childJunctions),
    myChildEdges(childEdges),
    myChildLanes(childLanes),
    myChildAdditionals(childAdditionals),
    myChildShapes(childShapes),
    myChildTAZElements(childTAZElements),
    myChildDemandElements(childDemandElements),
    myChildGenericDatas(childGenericDatas) {
}


size_t 
GNEHierarchicalContainer::getContainerSize() const {
    return (
        myParentJunctions.size() + 
        myParentEdges.size() + 
        myParentLanes.size() + 
        myParentAdditionals.size() + 
        myParentShapes.size() + 
        myParentTAZElements.size() + 
        myParentDemandElements.size() + 
        myParentGenericDatas.size() + 
        myChildJunctions.size() + 
        myChildEdges.size() + 
        myChildLanes.size() + 
        myChildAdditionals.size() + 
        myChildShapes.size() + 
        myChildTAZElements.size() + 
        myChildDemandElements.size() + 
        myChildGenericDatas.size()
    );
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    if (std::find(myParentJunctions.begin(), myParentJunctions.end(), junction) != myParentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    if (std::find(myParentEdges.begin(), myParentEdges.end(), edge) != myParentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) != myParentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    if (std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional) != myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) != myParentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    if (std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement) != myParentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    if (std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement) != myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalContainer::addParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    if (std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData) != myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(myParentJunctions.begin(), myParentJunctions.end(), junction);
    if (it == myParentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
    if (it == myParentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(myParentLanes.begin(), myParentLanes.end(), lane);
    if (it == myParentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it == myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(myParentShapes.begin(), myParentShapes.end(), shape);
    if (it == myParentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentShapes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement);
    if (it == myParentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it == myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it == myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myParentGenericDatas.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    if (std::find(myChildJunctions.begin(), myChildJunctions.end(), junction) != myChildJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    if (std::find(myChildEdges.begin(), myChildEdges.end(), edge) != myChildEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    if (std::find(myChildLanes.begin(), myChildLanes.end(), lane) != myChildLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    if (std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional) != myChildAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    if (std::find(myChildShapes.begin(), myChildShapes.end(), shape) != myChildShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    if (std::find(myChildTAZElements.begin(), myChildTAZElements.end(), TAZElement) != myChildTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check demand element
    if (std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement) != myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalContainer::addChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    if (std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData) != myChildGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(myChildJunctions.begin(), myChildJunctions.end(), junction);
    if (it == myChildJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(myChildEdges.begin(), myChildEdges.end(), edge);
    if (it == myChildEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildEdges.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(myChildLanes.begin(), myChildLanes.end(), lane);
    if (it == myChildLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildLanes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it == myChildAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(myChildShapes.begin(), myChildShapes.end(), shape);
    if (it == myChildShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildShapes.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(myChildTAZElements.begin(), myChildTAZElements.end(), TAZElement);
    if (it == myChildTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    if (it == myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        myChildDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalContainer::removeChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(myChildGenericDatas.begin(), myChildGenericDatas.end(), genericData);
    if (it == myChildGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
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


template<> const std::vector<GNEShape*>&
GNEHierarchicalContainer::getParents() const {
    return myParentShapes;
}


template<> const std::vector<GNETAZElement*>&
GNEHierarchicalContainer::getParents() const {
    return myParentTAZElements;
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
GNEHierarchicalContainer::setParents(const std::vector<GNEShape*>& newParents) {
     myParentShapes = newParents;
}


template<> void
GNEHierarchicalContainer::setParents(const std::vector<GNETAZElement*>& newParents) {
     myParentTAZElements = newParents;
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


template<> const std::vector<GNEShape*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildShapes;
}


template<> const std::vector<GNETAZElement*>&
GNEHierarchicalContainer::getChildren() const {
    return myChildTAZElements;
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
GNEHierarchicalContainer::setChildren(const std::vector<GNEShape*>& newChildren) {
    myChildShapes = newChildren;
}


template<> void
GNEHierarchicalContainer::setChildren(const std::vector<GNETAZElement*>& newChildren) {
    myChildTAZElements = newChildren;
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
